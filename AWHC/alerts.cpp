#include "alerts.h"
#include "config.h"
#include "control.h"

bool alertActive = false; // used to separate 1st alert from subsequent alerts
long lastAlert = 0;

void setupAlerts()
{
  pinMode (ledRedPin, OUTPUT);
  pinMode (ledGreenPin, OUTPUT);
  pinMode (ledBluePin, OUTPUT);
  pinMode (buzzerPin, OUTPUT);

  digitalWrite (ledRedPin, LOW);
  digitalWrite (ledGreenPin, LOW);
  digitalWrite (ledBluePin, LOW);
  digitalWrite (buzzerPin, LOW);
}

void redLED()
{
  digitalWrite (ledRedPin, HIGH);
}

void buzzerAlert()
{
  digitalWrite (buzzerPin, HIGH);
  delay (beepTime);
  digitalWrite (buzzerPin, LOW);
}

void updateAlerts (float level)
{
  bool needsAlert = false;
  if (inFault || level < reservoirLowPercent)
    needsAlert = true;

  if (!needsAlert)
  {
    alertActive = false;
    digitalWrite (ledRedPin, LOW); // problem cleared, LED off
    return;
  }

  if (!alertActive || millis() - lastAlert >= alertInterval) // alerts on entry, then every alertInterval (10 mins)
  {
    alertActive = true;
    lastAlert = millis();
    redLED();
    buzzerAlert();
  }
}