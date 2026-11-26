#include "control.h"
#include "config.h"
#include "settings.h"

enum SoilState { monitoringSoil, watering, soaking };
enum HumidityState { monitoringHumidity, humidifying };

SoilState soilState = monitoringSoil;
HumidityState humidityState = monitoringHumidity;
bool inFault = false;

long wateringStart = 0;
long soakingStart = 0;
long humidifyingStart = 0;
float moistureAtStart = 0;
float humidityAtStart = 0;
float lastLevel = 0;

void setupControl()
{
  // relays off before the pins become outputs to prevent pump/humidifier from starting at power-up
  digitalWrite (pumpRelayPin, relayOff);
  digitalWrite (humidifierRelayPin, relayOff);

  pinMode (pumpRelayPin, OUTPUT);
  pinMode (humidifierRelayPin, OUTPUT);
}

void pumpOn()
{
  digitalWrite (pumpRelayPin, relayOn);
}

void pumpOff()
{
  digitalWrite (pumpRelayPin, relayOff);
}

void humidifierOn()
{
  digitalWrite (humidifierRelayPin, relayOn);
}

void humidifierOff()
{
  digitalWrite (humidifierRelayPin, relayOff);
}

void allOff()
{
  pumpOff();
  humidifierOff();
}

void enterFault (const char reason[])
{
  allOff();
  inFault = true;
  soilState = monitoringSoil;
  humidityState = monitoringHumidity;

  Serial.print ("Fault: ");
  Serial.println (reason);
}

void updateSoil (float moisture)
{
  if (soilState == monitoringSoil)
  {
    if (moisture < soilLow) // below moisture threshold check
    {
      pumpOn();
      wateringStart = millis();
      moistureAtStart = moisture;
      soilState = watering;
      Serial.println ("Watering");
    }
  }
  else if (soilState == watering)
  {
    if (moisture >= soilHigh) // above moisture threshold check
    {
      pumpOff();
      soakingStart = millis();
      soilState = soaking;
      Serial.println ("Soaking");
    }
    else if (millis() - wateringStart >= maxPumpTime) // above max watering time check
    {
      pumpOff();

      if (moisture - moistureAtStart >= minMoistureRise) // watering failure check
      {
        soakingStart = millis();
        soilState = soaking;
        Serial.println ("Soaking (max pump time reached)"); // max watering time; switches to soaking
      }
      else
        enterFault ("no moisture rise"); // water failure occurred
    }
  }
  else if (soilState == soaking)
  {
    if (millis() - soakingStart >= soakTime) // waits until the configured soak time has passed
    {
      soilState = monitoringSoil;
      Serial.println ("Monitoring soil");
    }
  }
}

void updateHumidity (float humidity)
{
  if (humidityState == monitoringHumidity)
  {
    if (humidity < humidityLow) // below humidity threshold check
    {
      humidifierOn();
      humidifyingStart = millis();
      humidityAtStart = humidity;
      humidityState = humidifying;
      Serial.println ("Humidifying");
    }
  }
  else if (humidityState == humidifying)
  {
    if (humidity >= humidityHigh) // above humidity threshold check
    {
      humidifierOff();
      humidityState = monitoringHumidity;
      Serial.println ("Monitoring humidity");
    }
    else if (millis() - humidifyingStart >= maxRunTime) // above max humidifying time check
    {
      humidifierOff();

      if (humidity - humidityAtStart >= minHumidityRise) // humidifying failure check
      {
        humidityState = monitoringHumidity;
        Serial.println ("Monitoring humidity (max run time reached)"); // max humidifying time; stops humidifier
      }
      else
        enterFault ("no humidity rise"); // humidifying error occurred
    }
  }
}

void updateControl (float moisture, float humidity, float level)
{
  lastLevel = level; // kept so resetFault() can check the reservoir

  if (inFault)
    return; // everything stays off until resetFault() occurs

  if (moisture < 0 || humidity < 0 || level < 0) // sensor error checks, since invalid readings = -1
  {
    enterFault ("sensor error");
    return;
  }

  if (level <= reservoirEmptyPercent) // empty reservoir check (separate from an ultrasonic sensor error)
  {
    enterFault ("reservoir empty");
    return;
  }

  updateSoil (moisture);

  if (!inFault)
    updateHumidity (humidity); // skipped if the soil check just caused a fault
}

void resetFault()
{
  if (!inFault || lastLevel <= reservoirEmptyPercent)
    return; // no fault, or the reservoir is empty or ultrasonic sensor has an error

  inFault = false;
  Serial.println ("Fault cleared");
}