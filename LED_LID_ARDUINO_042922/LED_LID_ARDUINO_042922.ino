#include "elapsedMillis.h"

String val;
// DEFAULT VALUES
float FREQUENCY = 1; // Hz
uint32_t PULSEWIDTH = 50 * 1000; // us
int AMPLITUDE = 50; // % of max

const int BASE = 3;
const int INDICATOR = 2;
float period = 1000000 / FREQUENCY; // us
boolean firstCycle = HIGH;
boolean enableState = LOW;
boolean ledState = LOW;
int waitTime = 0;
elapsedMicros timer;
elapsedMicros timerSerial;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(0);
  EstablishConnection();
  pinMode(BASE, OUTPUT);
  pinMode(INDICATOR, OUTPUT);
  
  analogWrite(BASE, 0);
  analogWrite(INDICATOR, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    val = Serial.readString();
    if (val.equals("Python Connection Established!\n")) {
      Serial.println("Arduino Connection Established!");
      FREQUENCY = 1;
      period = 1000000 / FREQUENCY;
      PULSEWIDTH = 50*1000;
      AMPLITUDE = 50;
      timer = 0;
      enableState = LOW;
    }
    
    else if (val.substring(0, 1) == "F") {
      // changing frequency
      float newFREQUENCY = val.substring(1, val.length() - 1).toFloat();

      float newperiod = 1000000 / newFREQUENCY;
      if (newperiod <= PULSEWIDTH) {
        newperiod = period; // do not do anything
        newFREQUENCY = FREQUENCY;
      }
      period = newperiod;
      FREQUENCY = newFREQUENCY;
    }
    else if (val.substring(0, 1) == "P") {
      // changing pulse width
      int newPULSEWIDTH = val.substring(1, val.length() - 1).toInt();

      newPULSEWIDTH = newPULSEWIDTH * 1000;
      if (newPULSEWIDTH >= period) {
        newPULSEWIDTH = PULSEWIDTH; // do not do anything
      }
      else if (newPULSEWIDTH < 10*1000) {
        newPULSEWIDTH = PULSEWIDTH;
      }
      PULSEWIDTH = newPULSEWIDTH;
    }
    else if (val.substring(0, 1) == "A") {
      // changing amplitude
      int newAMPLITUDE = val.substring(1, val.length() - 1).toInt();

      if (newAMPLITUDE > 100) {
        newAMPLITUDE = 100;
      }
      else if (newAMPLITUDE < 0) {
        newAMPLITUDE = 0;
      }
      AMPLITUDE = newAMPLITUDE;
    }
    else if (val.substring(0, 1) == "E") {
      // changing enable state
      enableState = !enableState;
      waitTime = timer;
    }
    else if (val.substring(0, 1) == "S") {
      // STOP
      enableState = LOW;
      waitTime = timer;
    }
  }
  // END SERIAL COMMANDS

  if (firstCycle) {
    timer = 0;
    firstCycle = LOW;
  }

  if (timer >= uint32_t(period)) {
    timer = 0;
  }

  if (enableState) {
    if (timer < PULSEWIDTH) {
      analogWrite(BASE, map(AMPLITUDE, 0, 100, 0, 255));
      analogWrite(INDICATOR, map(AMPLITUDE, 0, 100, 0, 255));
      
      ledState = HIGH;
      //Serial.println(timer);


    }
    else if (timer >= PULSEWIDTH) {
      analogWrite(BASE, LOW);
      analogWrite(INDICATOR, LOW);
      ledState = LOW;

    }
  }
  else {
    analogWrite(BASE, LOW);
    analogWrite(INDICATOR, LOW);
    ledState = LOW;
    timer = 0;
  }

  if (timerSerial >= 10 * 1000) {
    Serial.print("VALUES: ");
    Serial.print(enableState);
    Serial.print(",");
    Serial.print(FREQUENCY);
    Serial.print(",");
    Serial.print(PULSEWIDTH);
    Serial.print(",");
    Serial.print(AMPLITUDE);
    Serial.print(",");
    if (ledState) {
      Serial.println(AMPLITUDE);
    }
    else {
      Serial.println(0);
    }

    timerSerial = 0;
  }

}

void EstablishConnection() { // stuck in here until connection is made
  int waiting = HIGH;
  while (waiting) {
    if (Serial.available() > 0) {
      val = Serial.readString();
      if (val.equals("Python Connection Established!\n")) {
        Serial.println("Arduino Connection Established!");
        waiting = LOW;
      }
    }
  }
}
