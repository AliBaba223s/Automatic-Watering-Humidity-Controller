#pragma once

void setupAlerts(); // sets up the LED and buzzer pins, all off
void updateAlerts (float level); // alerts on a low reservoir or a fault, call after each new reading