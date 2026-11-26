#include "config.h"
#include "sensing.h"
#include "settings.h"
#include "control.h"
#include "alerts.h"
#include "comms.h"

long lastSensorRead = 0;

void setup()
{
  setupControl(); // relays off first
  setupAlerts();
  Serial.begin (9600);
  setupSensors();
  loadTargets();
  setupComms();
}

void loop()
{
  updateComms();

  long duration = millis() - lastSensorRead; // duration since last reading; millis is a "stopwatch"

  if (duration >= sensorInterval)
  {
    lastSensorRead = millis(); // duration restarts from 0 on the next loop

    float moisture = readMoisture();
    float humidity = readHumidity();
    float level = readLevel();

    Serial.print ("Moisture: ");
    Serial.print (moisture);
    Serial.print (" % WBV | Humidity: ");
    Serial.print (humidity);
    Serial.print (" % RH | Reservoir: ");
    Serial.print (level);
    Serial.println (" %");

    updateControl (moisture, humidity, level);
    updateAlerts (level);
    publishReadings (moisture, humidity, level);
  }
}