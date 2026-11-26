#include "comms.h"
#include "config.h"
#include "settings.h"
#include "control.h"
#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

WiFiSSLClient wifiClient; // encrypted (TLS) connection
MqttClient mqttClient (wifiClient);

long lastConnectAttempt = 0;
long lastPublish = 0;
bool publishNow = true; // sends readings straight after connecting
bool statusSent = false; // sends the status straight after connecting
bool lastFault = false;

// builds a full Adafruit IO topic
String feedTopic (const char feed[])
{
  return String (SECRET_AIO_USERNAME) + "/feeds/" + feed;
}

void publishValue (const char feed[], String value)
{
  mqttClient.beginMessage (feedTopic (feed)); // starts a message to topic
  mqttClient.print (value); // writes the contents
  mqttClient.endMessage(); // finishes/sends the message
}

void requestLatest (const char feed[])
{
  mqttClient.beginMessage (feedTopic (feed) + "/get"); // Adafruit IO replies with the feed's last value
  mqttClient.print (""); // empty contents, the /get topic is the request
  mqttClient.endMessage(); // sends the request
}

void connectToAdafruit()
{
  if (WiFi.status() != WL_CONNECTED) // checks if the board is on Wi-Fi
  {
    WiFi.begin (SECRET_SSID, SECRET_PASS); // joins the Wi-Fi network, waits a few seconds
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println ("Wi-Fi connection failed");
      return;
    }
  }

  if (!mqttClient.connect (aioServer, aioPort)) // logs in to Adafruit IO over TLS
  {
    Serial.println ("Adafruit IO connection failed");
    return;
  }

  mqttClient.subscribe (feedTopic (feedSoilLow)); // Adafruit IO forwards new values from this feed
  mqttClient.subscribe (feedTopic (feedSoilHigh));
  mqttClient.subscribe (feedTopic (feedHumidityLow));
  mqttClient.subscribe (feedTopic (feedHumidityHigh));
  mqttClient.subscribe (feedTopic (feedFaultReset));

  // catch up on slider changes made while offline
  requestLatest (feedSoilLow); // asks for this slider's last value
  requestLatest (feedSoilHigh);
  requestLatest (feedHumidityLow);
  requestLatest (feedHumidityHigh);

  publishNow = true;
  statusSent = false;
  Serial.println ("Connected to Adafruit IO");
}

void handleMessage()
{
  String topic = mqttClient.messageTopic(); // which feed the message came from
  String payload = "";
  while (mqttClient.available())
    payload += (char) mqttClient.read(); // reads the contents one char at a time

  int value = payload.toInt(); // string to int converter

  if (topic == feedTopic (feedFaultReset))
  {
    if (value == 1)
      resetFault(); // 0 is the button release, ignored
    return;
  }

  // min < max threshold checks
  if (topic == feedTopic (feedSoilLow) && value < soilHigh)
    soilLow = value;
  else if (topic == feedTopic (feedSoilHigh) && value > soilLow)
    soilHigh = value;
  else if (topic == feedTopic (feedHumidityLow) && value < humidityHigh)
    humidityLow = value;
  else if (topic == feedTopic (feedHumidityHigh) && value > humidityLow)
    humidityHigh = value;
  else
    return; // invalid value, nothing changed or saved

  storeTargets(); // saves the new targets to EEPROM
  Serial.println ("Targets updated from dashboard");
}

void setupComms()
{
  mqttClient.setUsernamePassword (SECRET_AIO_USERNAME, SECRET_AIO_KEY); // Adafruit IO login, used on every connect
  lastConnectAttempt = millis(); // starts the reconnect timer
  connectToAdafruit();
}

void updateComms()
{
  if (WiFi.status() != WL_CONNECTED || !mqttClient.connected()) // offline check
  {
    if (millis() - lastConnectAttempt >= reconnectInterval && digitalRead (pumpRelayPin) == relayOff) // won't retry connecting if pump is on because of reconnecting delays
    {
      lastConnectAttempt = millis();
      connectToAdafruit();
    }
    return;
  }

  if (mqttClient.parseMessage() > 0) // checks for a new message and keeps connection alive
    handleMessage();

  if (!statusSent || inFault != lastFault) // just connected / fault state changed
  {
    int status = statusNormal;
    if (inFault)
      status = statusFault;

    publishValue (feedSystemStatus, String (status));
    statusSent = true;
    lastFault = inFault; // used to detect a potential future change
  }
}

void publishReadings (float moisture, float humidity, float level)
{
  if (!mqttClient.connected())
    return; // offline, readings are skipped

  if (!publishNow && millis() - lastPublish < publishInterval) // less than 15 min since the last publish check, unless just connected
    return;

  publishNow = false;
  lastPublish = millis();

  // readings of -1 are sensor errors; not sent to dashboard
  if (moisture >= 0)
    publishValue (feedSoilMoisture, String (moisture, 1)); // 1 decimal place, e.g. "45.2"
  if (humidity >= 0)
    publishValue (feedHumidity, String (humidity, 1));
  if (level >= 0)
    publishValue (feedReservoirLevel, String (level, 1));
}