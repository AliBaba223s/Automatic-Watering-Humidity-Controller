#pragma once

void setupComms(); // first connection attempt to Wi-Fi and Adafruit IO
void updateComms(); // handles dashboard messages and status, call every loop
void publishReadings (float moisture, float humidity, float level); // sends readings every publishInterval, call after each new reading