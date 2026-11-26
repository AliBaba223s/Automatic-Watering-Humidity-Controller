#pragma once
#include <Arduino.h>

// Pins
const int humidityPin = 2;
const int buzzerPin = 4;
const int ledBluePin = 6;
const int trigPin = 7; // ultrasonic trig pin
const int echoPin = 8; // ultrasonic echo pin
const int ledRedPin = 9;
const int ledGreenPin = 10;
const int pumpRelayPin = 11;
const int humidifierRelayPin = 12;
const int soil1Pin = A0;
const int soil2Pin = A1;

// Relay Configuration (Active-Low)
const int relayOn = LOW;
const int relayOff = HIGH;

// Default Target Ranges
const int defaultSoilLow = 40; // % WBV, update later
const int defaultSoilHigh = 60; // % WBV
const int defaultHumidityLow = 50; // % RH
const int defaultHumidityHigh = 70; // % RH

// Soil Sensor Calibration
const int soil1Air = 518; // i.e. 0% WBV
const int soil1Water = 222; // i.e. 100% WBV
const int soil2Air = 472; // i.e. 0% WBV
const int soil2Water = 0; // i.e. 100% WBV

// Reservoir
const int reservoirLowPercent = 20; // indicator threshold
const int reservoirEmptyPercent = 0;
const int ultrasonicEmpty = 1166; // update later
const int ultrasonicFull = 175; // update later

// Timing
const long sensorInterval = 3000; // 3 s
const long publishInterval = 900000; // 15 min
const long alertInterval = 600000; // 10 min
const long soakTime = 300000; // 5 min, update later
const long maxPumpTime = 10000; // 10 s, update later
const long maxRunTime = 600000; // 10 min humidifier limit, update later
const long beepTime = 500; // 0.5 s buzzer beep
const long reconnectInterval = 60000; // 1 min between reconnect attempts while offline

// Fault Checks
const int minMoistureRise = 2; // % WBV counted as "moisture rose", update later
const int minHumidityRise = 2; // % RH counted as "humidity rose", update later
const int soilMargin = 50; // raw reading above the air value counted as an error, update later
const int maxSoilDifference = 20; // % WBV the two sensors can differ before an error, update later

// Adafruit IO
const char aioServer[] = "io.adafruit.com";
const int aioPort = 8883; // MQTT over TLS
const char feedSoilMoisture[] = "soil-moisture";
const char feedHumidity[] = "humidity";
const char feedReservoirLevel[] = "reservoir-level";
const char feedSoilLow[] = "soil-low";
const char feedSoilHigh[] = "soil-high";
const char feedHumidityLow[] = "humidity-low";
const char feedHumidityHigh[] = "humidity-high";
const char feedSystemStatus[] = "system-status";
const char feedFaultReset[] = "fault-reset";

// System Status
const int statusNormal = 1;
const int statusFault = 0;