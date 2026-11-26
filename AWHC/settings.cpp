#include "settings.h"
#include "config.h"
#include <EEPROM.h>

// EEPROM layout
const int markerAddress = 0;
const int savedMarker = 1; // stored at markerAddress; changing it resets saved targets to the defaults

const int soilLowAddress = 1;
const int soilHighAddress = 2;

const int humidityLowAddress = 3;
const int humidityHighAddress = 4;


int soilLow = defaultSoilLow;
int soilHigh = defaultSoilHigh;

int humidityLow = defaultHumidityLow;
int humidityHigh = defaultHumidityHigh;

void loadTargets()
{
  if (EEPROM.read (markerAddress) != savedMarker)
    return; // nothing saved yet, keep the defaults

  soilLow = EEPROM.read (soilLowAddress);
  soilHigh = EEPROM.read (soilHighAddress);

  humidityLow = EEPROM.read (humidityLowAddress);
  humidityHigh = EEPROM.read (humidityHighAddress);
}

void storeTargets()
{
  EEPROM.update (soilLowAddress, soilLow);
  EEPROM.update (soilHighAddress, soilHigh);

  EEPROM.update (humidityLowAddress, humidityLow);
  EEPROM.update (humidityHighAddress, humidityHigh);

  EEPROM.update (markerAddress, savedMarker);
}