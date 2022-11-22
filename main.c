/*
The MIT License (MIT)

Smart Lighting Management for Arduino
Copyright (c) 2013 Clément Villanueva

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


*/


/*
 * On Arduino boards, there's analog and digital pins and they have different designation.
 * Analog pins are designated by an 'A' before a pin number, example : A0, A1, A2, etc...
 * Digital pins are designated by just a pin number, example : 0, 1, 2, etc...
 * Here, we associate a pin to a sensor, depending on which type of input it uses.
 */

/*
 * The LDR (Light Dependent Resistor) determine how much light there's in the room.
 */
const int LDR_SENSOR = A0;

/*
 * The POTENTIOMETER is used to manually manage the light available from the LEDs.
 * Warning : when ON, it disables smart lighting management, and turn it on when OFF.
 */
const int POTENTIOMETER = A2;

/*
 * The power switch (a switch button) simply enables or disables the smart light management system.
 */
const int POWER_SWITCH = 1;

/*
 * The PIR (Passive InfraRed) sensor determines if there's someone in the room.
 */
const int PIR_SENSOR = 3;

/*
 * The power state indicator (a LED) tells the user if the system is ON or OFF.
 */
const int POWER_STATE_INDICATOR = 9;

/*
 * The LEDS light up the room.
 */
const int LEDS = 10;

/*
 * Constant defining the time (in milliseconds) the light should be turned on when the PIR sensor detects someone.
 * If no motion is detected by the PIR sensor within this amount of time, the light is turned off when it ends.
 * Can be adjusted, depending on the behaviour the user wants.
 * Warning : low values will be frustrating.
 */
const int PIR_MAX_TIME = 60000;

/*
 * Constant defining the time (in milliseconds) power switch state can't change if the button is pushed.
 * It is meant to avoid weird behaviours.
 * Warning : it's not recommended to change this value, it was determined by testing and it works.
 */
const int POWER_SWITCH_TIMER = 200;

/*
 * Used to turn the system on if LDR value is lower than this value.
 */
const int ACTIVE_VALUE = 400;

/*
 * Power state initialization to HIGH.
 */
int power_switch_state = HIGH;

/*
 * Stores the previous state of the power switch (could be HIGH or LOW).
 * Power switch previous state is initialized to LOW.
 */
int power_switch_previous_state = LOW;

/*
 * Stores the current power switch state (could be HIGH or LOW).
 */
int current_power_switch_state;

/*
 * Power switch timer initialization.
 */
int power_switch_time = 0;

/*
 * Stores the value measured by the LDR sensor.
 */
int ldr_sensor_value;

/*
 * PIR sensor timer initialization.
 */
int pir_sensor_time = 0;

/*
 * Stores the PIR sensor state (could be HIGH or LOW).
 */
int pir_sensor_state;

/*
 * Stores the POTENTIOMETER value (between 0 and 255).
 */
int potentiometer_value;

/*
 * This method sets the pin mode for output sensor.
 * As you can see, not all sensor pins are set.
 * A pin is set as an INPUT by default.
 * So, we just define OUTPUT sensors.
 */
void setup(void) {
    pinMode(LEDS, OUTPUT);
    pinMode(POWER_STATE_INDICATOR, OUTPUT);
}

/*
 * Main method, loops.
 */
void loop(void) {
    current_power_switch_state = digitalRead(POWER_SWITCH);
    if (current_power_switch_state == HIGH && power_switch_previous_state == LOW) {
      int power_time = millis() - power_switch_time;
      if (power_time > POWER_SWITCH_TIMER) {
        if (power_switch_state == HIGH) {
          power_switch_state = LOW;
        }
        power_switch_time = millis();
      }
    }
    
    power_switch_previous_state = current_power_switch_state;
    pir_sensor_state = digitalRead(PIR_SENSOR);
    potentiometer_value = map(POTENTIOMETER, 0, 1023, 0, 255);

    // Reverted mapping (more external light, less LEDs power involved)
    ldr_sensor_value = map(LDR_SENSOR, 0, 1023, 255, 0);

    if (power_switch_state == HIGH) {
        analogWrite(POWER_STATE_INDICATOR, 1);
        int pir_time = millis() - pir_sensor_time;
        if (potentiometer_value > 0) {
            analogWrite(LEDS, potentiometer_value);
        } else if (pir_sensor_state == HIGH && pir_sensor_time == 0) {
            pir_sensor_time = millis();
        } else if (pir_sensor_time > 0 && pir_time < PIR_MAX_TIME && ldr_sensor_value < ACTIVE_VALUE) {
            analogWrite(LEDS, ldr_sensor_value);
        } else {
            analogWrite(LEDS, 0);
            pir_sensor_time = 0; // Reset PIR sensor timer
        }
    } else {
        analogWrite(LEDS, 0);
        analogWrite(POWER_STATE_INDICATOR, 0);
    }
}
