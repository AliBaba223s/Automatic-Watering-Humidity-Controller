# Automatic Watering and Humidity Controller (AWHC)

Arduino firmware for a plant care system that keeps a houseplant's soil moisture and air humidity within target ranges on its own, and streams live readings to a web dashboard. Built by Team 18 for ELEC 290 at Queen's University (Fall 2026).

## Results

- Built and tested a working prototype that has kept a plant healthy for its entire run.
- Collects sensor data throughout testing, with readings published to an Adafruit IO dashboard every 15 minutes.

## How it works

- **Sensing:** every 3 seconds, reads two capacitive soil moisture sensors (averaged), a DHT11 humidity sensor and an ultrasonic sensor that measures the water reservoir level.
- **Control:** two state machines drive the pump (monitoring, watering, soaking) and the humidifier (monitoring, humidifying) through a relay module. Relays are set off before their pins become outputs, so nothing switches on at power-up.
- **Fault detection:** shuts everything off if a sensor stops responding, the two soil sensors disagree, the reservoir runs empty, or watering or humidifying fails to raise the reading within a time limit.
- **Alerts:** a red LED and buzzer warn of a low reservoir or a fault, repeating every 10 minutes until the problem is cleared.
- **Dashboard:** connects over Wi-Fi to Adafruit IO using MQTT over TLS. Users can change the target ranges with dashboard sliders, see the system status and reset faults remotely. New targets are checked (low must stay below high) and saved to EEPROM, so they survive a power loss.
- **Offline handling:** keeps controlling the plant without Wi-Fi and retries the connection every minute, but never while the pump is running.

## Hardware

| Part | Model |
| --- | --- |
| MCU | Arduino UNO WiFi Rev2 |
| Adapter | Dual-Port 5 V USB Wall Adapter (Port 1: Relay + Loads, Port 2: Arduino) |
| Relay | 2-Channel 5 V Relay Module |
| Humidity sensor | DHT11 Temperature and Humidity Sensor |
| Ultrasonic sensor | HC-SR04 Ultrasonic Distance Sensor |
| Soil sensors | 2x Capacitive Soil Moisture Sensor v1.2 |
| RGB LED | 5 mm Common-Cathode RGB LED |
| Buzzer | Active Piezo Buzzer |
| Humidifier | Micro-USB Ultrasonic Mist Maker Module |
| Pump | 3-6 V DC Micro Submersible Water Pump |

## Pin Map

| Pin | Connection |
| --- | --- |
| D2 | Humidity sensor DATA |
| D4 | Buzzer + |
| D6 | RGB LED B |
| D7 | Ultrasonic sensor Trig |
| D8 | Ultrasonic sensor Echo |
| D9 | RGB LED R |
| D10 | RGB LED G |
| D11 | Relay IN1 (pump) |
| D12 | Relay IN2 (humidifier) |
| A0 | Soil sensor 1 AOUT |
| A1 | Soil sensor 2 AOUT |

`AWHC/config.h` is the only file where pin numbers are assigned, along with calibration values, target ranges and timing.

## Code Layout

| File | Purpose |
| --- | --- |
| `AWHC/AWHC.ino` | Main loop: reads sensors every 3 s, then runs control, alerts and publishing |
| `AWHC/sensing` | Soil, humidity and reservoir readings, with error checks |
| `AWHC/control` | Pump and humidifier state machines and fault handling |
| `AWHC/alerts` | LED and buzzer warnings |
| `AWHC/comms` | Wi-Fi, MQTT over TLS and Adafruit IO dashboard messages |
| `AWHC/settings` | Target ranges saved to and loaded from EEPROM |
| `AWHC/config.h` | Pins, calibration, targets, timing and feed names |

## Setup

1. Install the **Arduino megaAVR Boards** package.
2. Install these libraries:

   | Library | Author | Used for |
   | --- | --- | --- |
   | WiFiNINA | Arduino | Wi-Fi and TLS connection |
   | ArduinoMqttClient | Arduino | MQTT messages to Adafruit IO |
   | DHT sensor library | Adafruit | Humidity sensor |
   | Adafruit Unified Sensor | Adafruit | Required by the DHT library |

3. Copy `AWHC/arduino_secrets.example.h` to `AWHC/arduino_secrets.h` and fill in your Wi-Fi and Adafruit IO details. `arduino_secrets.h` is gitignored, so it is never committed.
4. Open `AWHC/AWHC.ino`, select the UNO WiFi Rev2, and upload.
