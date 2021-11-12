/* Arudino script for controlling LEDS in LED stim board, v1
 *  Stim board has single control input, which controls all LEDs through simple transistor switch
 *  Two parameters: stimRate [Hz] and pulseDuration [ms], can be set below
 *  Keep Serial communications on for visual confirmation of pulses on Arduino board (Tx LEDs on board will flash when transmitting information)
 *  
 *  BUGS: Cannot set stimRate below 1 Hz.  NEED TO FIX.
 *        11/4/21 - FIXED
 * 
 *  
 *  Digital Pin 2:  Button or switch for tetanic stimulation.
 *                    Interrupt to trigger a flag which starts tetanus routine.  This is needed for correct timing.
 *                    Turns LEDs on for 2 seconds (or user-defined)
 *  
 *  Digital Pin 6:  Switch for turning pulsing on and off.  
 *                    HIGH to begin pulsing, LOW to stop pulsing.
 *  
 *  Digital Pin 7:  Control signal for all LEDS through transistor
 *  
 *  Digital Pin 24: Constant 5V that feeds into Pin 2.  Used to avoid using 5V pin, in case that's used for the LEDs
 *  
 *  
 *  Eventually, integrate with Processing or Python to create a GUI that Alec can control: 
 *      1) Pulse Duration
 *      2) Pulse Frequency
 *      3) Tetanic stimulation duration
 *      4) Light Intensity
 */

/////////////////////////////////// SET PARAMETERS BELOW ///////////////////////////////////////
const float FREQUENCY = 1; // Hz
unsigned long PULSE_WIDTH = 100; // ms
unsigned long TETANUS_DURATION = 2000; // ms
////////////////////////////////////////////////////////////////////////////////////////////////


// PIN Definitions
const int LEDpins = 7;
const int interruptPin = 2;
const int const5VPin = 24;
const int pulsingModePin = 6;

volatile boolean flag = false; // ISR flag

const int period = 1000 / FREQUENCY; // ms
void setup() {
  pinMode(LEDpins,        OUTPUT); // pin output goes to transistor, turns all LEDs on and off
  pinMode(interruptPin,   INPUT); // Pin to trigger interrupt on HIGH
  pinMode(const5VPin,     OUTPUT); // Pin that's always HIGH
  pinMode(pulsingModePin, INPUT); // toggle for pulsing mode on and off

  digitalWrite(LEDpins,     LOW); // starts LOW
  digitalWrite(const5VPin,  HIGH); // starts (and stays) HIGH

  attachInterrupt(digitalPinToInterrupt(interruptPin), myISR, RISING);
  // Uncomment below if serial comm needed (debugging)
  Serial.begin(9600);
  Serial.println(period);
}

void loop() {
  unsigned long loopStartTime = millis(); // Get 
  if (flag == true) {
    if (digitalRead(pulsingModePin) == 0) {
      tetanus();
    }
    flag = false;
  }

  if (digitalRead(pulsingModePin) != 0) {
    
    // Set all to high (turn LEDs on)
    digitalWrite(LEDpins,HIGH);
    Serial.print("ON: ");
    Serial.print(millis());
    Serial.print(',');
    // pause for pulse duration
    unsigned long t = millis();
    while (t - loopStartTime < PULSE_WIDTH) {
      t = millis();
      
    }
    
    // Set all to low (turn LEDs off)
    digitalWrite(LEDpins,LOW);
    Serial.print("OFF: ");
    Serial.println(t);
    // pause for rest of stimulation frequency 
    t = millis();
    while (t - loopStartTime < period){
      t = millis();
    }
  }
}

void myISR() {
  flag = true;
}
void tetanus() {
  flag = true;
  digitalWrite(LEDpins, LOW);
  unsigned long currentTime = millis();
  
  // Turn them on for two seconds
  digitalWrite(LEDpins, HIGH);
  Serial.print("; In Tetanus ");
  Serial.print(millis());
  currentTime = millis();
  while (millis() - currentTime < TETANUS_DURATION){
    // Leave blank
  }
  // delay(2000);
  digitalWrite(LEDpins, LOW);
  Serial.print("; End Tetanus: ");
  Serial.println(millis());
  currentTime = millis();
  while (millis() - currentTime < 1000){ // To prevent accidental double pushes

  }
}
  
