#include <Arduino.h>

class SomeClass{
  public:
    String Foo;
    int SomeInteger;
};

enum SomeEnum {
  EnumValueA,
  EnumValueB,
  EnumValueC
};

struct SomeStruct{
  double FieldA;
  bool FieldB;
  SomeEnum FieldC;
};

unsigned long nextTime = 5000;
SomeClass someClass;
SomeStruct someStruct;

void setup() {
  Serial.begin(115200);
  // Initialise someClass
  someClass.Foo = "Bar";
  someClass.SomeInteger = 1337;
  // Initialise someStruct
  someStruct.FieldA = 123.456;
  someStruct.FieldB = true;
  someStruct.FieldC = EnumValueA;
  Serial.println("Initialised"); // Set a Breakpoint on this line to interrogate someClass and someStruct
}

void loop() {
  if (millis() < nextTime) { return; } // Code won't proceed further until we reach the nextTime. A Breakpoint here will hit every time loop() is called
  // Breakpoints below this line will only hit every time the device counts 5 seconds!
  someClass.SomeInteger = someClass.SomeInteger * 2; // We double the value every 5 seconds
  someStruct.FieldA = someStruct.FieldA * 3; // We triple the value every 5 seconds
  nextTime = millis() + 5000; // nextTime = Now + 5 Seconds. Place a Breakpoint here to look at the updated values above!
}