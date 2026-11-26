#pragma once

void setupSensors();
float readMoisture(); // average % WBV of both soil sensors
float readHumidity(); // % RH
float readLevel(); // % capacity (water reservoir)