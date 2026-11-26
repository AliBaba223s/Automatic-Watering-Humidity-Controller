#pragma once

extern bool inFault; // true = in Fault state / a system error currently exists

void setupControl(); // sets up the relay pins with both relays off
void updateControl (float moisture, float humidity, float level); // runs the state machine, call after each new reading
void resetFault(); // clears any fault, unless the reservoir is still empty