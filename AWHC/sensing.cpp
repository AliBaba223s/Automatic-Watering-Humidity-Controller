#include "sensing.h"
#include "config.h"
#include <DHT.h>

DHT dht (humidityPin, DHT11);

void setupSensors()
{
  pinMode (trigPin, OUTPUT);
  pinMode (echoPin, INPUT);
  dht.begin();
}

// sensor reading to % capacity converter
float toPercent (long reading, long zeroValue, long fullValue)
{
  float percent = (float)(zeroValue - reading) / (zeroValue - fullValue) * 100;
  if (percent < 0)
    percent = 0;

  if (percent > 100)
    percent = 100;

  return percent;
}

// if any of the following 3 functions returns -1 it is considered a sensor error
float readMoisture()
{
  int raw1 = analogRead (soil1Pin);
  int raw2 = analogRead (soil2Pin);
  if (raw1 > soil1Air + soilMargin || raw2 > soil2Air + soilMargin)
    return -1; // reading above the max value (air value) by more than margin

  float soil1 = toPercent (raw1, soil1Air, soil1Water);
  float soil2 = toPercent (raw2, soil2Air, soil2Water);
  if (soil1 - soil2 > maxSoilDifference || soil2 - soil1 > maxSoilDifference)
    return -1; // sensors disagree

  return (soil1 + soil2) / 2;
}

float readHumidity()
{
  float humidity = dht.readHumidity();

  if (humidity >= 0 && humidity <= 100)
    return humidity;

  return -1; // no valid reading
}

float readLevel()
{
  digitalWrite (trigPin, LOW);
  delayMicroseconds (2);

  digitalWrite (trigPin, HIGH);
  delayMicroseconds (10);

  digitalWrite (trigPin, LOW);
  long echoTime = pulseIn (echoPin, HIGH, 30000); // gives up after 30 ms

  if (echoTime == 0)
    return -1; // no echo

  return toPercent (echoTime, ultrasonicEmpty, ultrasonicFull);
}