#include <Arduino.h>

uint8_t counter;

void setup() {
    counter = 0;    // if you break here, counter may be 0 or any other uint8_t value
}   // if you break here, counter will definitely be 0

void loop() {
    counter++;  // every time you break here, counter will increment by one and jump from 255 to 0
}