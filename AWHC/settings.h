#pragma once

// extern means the variables are shared by every file that includes this header
extern int soilLow;
extern int soilHigh;
extern int humidityLow;
extern int humidityHigh;

void loadTargets(); // loads saved targets from EEPROM (local Arduino storage), or keeps the defaults if none are saved
void storeTargets(); // saves the current targets to EEPROM