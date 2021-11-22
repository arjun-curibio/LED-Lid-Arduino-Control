/* Arudino script for controlling LEDS in LED stim board, v1
 *  Stim board has single control input, which controls all LEDs through simple transistor switch
 *  Two parameters: stimRate [Hz] and pulseDuration [ms], can be set below
 *  Keep Serial communications on for visual confirmation of pulses on Arduino board (Tx LEDs on board will flash when transmitting information)
 * 
 *  
 *  Digital Pin 2:  Button or switch for tetanic stimulation.
 *                    Interrupt to trigger a flag which starts tetanus routine.  This is needed for correct timing.
 *                    Turns LEDs on for 2 seconds (or user-defined)
 *  
 *  Digital Pin 6:  Switch for turning pulsing on and off.  
 *                    HIGH to begin pulsing, LOW to stop pulsing.
 *  
 *  Digital Pin 3:  Control signal for all LEDS through transistor - PWM signal
 *  
 *  
 *  
 *  Eventually, integrate with Processing or Python to create a GUI that Alec can control: 
 *      1) Pulse Duration
 *      2) Pulse Frequency
 *      3) Tetanic stimulation duration
 *      4) Light Intensity
 *      
 */

/////////////////////////////////// SET PARAMETERS BELOW ///////////////////////////////////////
const float   FREQUENCY         = 1; // Hz
unsigned long PULSE_WIDTH       = 100; // ms
unsigned long TETANUS_DURATION  = 2000; // ms

////////////////////////////////////////////////////////////////////////////////////////////////


// PIN Definitions
const int LEDpins = 3;
const int buttonPin = 2;
const int pulsingModePin = 6;
const int AnalogInputPin = A7;

volatile boolean flag = false; // ISR flag

const int period = 1000 / FREQUENCY; // ms
void setup() {
  pinMode(LEDpins,        OUTPUT); // pin output goes to transistor, turns all LEDs on and off
  pinMode(buttonPin,      INPUT); // Pin to trigger interrupt on HIGH
  pinMode(pulsingModePin, INPUT); // toggle for pulsing mode on and off
  pinMode(AnalogInputPin, INPUT); // to be translated to PWM
  pinMode(13,             OUTPUT);
  pinMode(A4,             OUTPUT);
  digitalWrite(LEDpins,     LOW); // starts LOW
  digitalWrite(buttonPin, HIGH);
  digitalWrite(13, LOW);
  //attachInterrupt(digitalPinToInterrupt(buttonPin), myISR, RISING);
  // Uncomment below if serial comm needed (debugging)
  
  Serial.begin(9600);
  Serial.println(period);
}

void loop() {
  int AnalogVal = analogRead(AnalogInputPin);
  //Serial.println(map(AnalogVal, 0, 1023, 0, 100));
  
  if (digitalRead(pulsingModePin) != 0) {
    digitalWrite(LEDpins, LOW);
    digitalWrite(13, LOW);
    analogWrite(A4, 0);
    single_pulse(PULSE_WIDTH, period, AnalogVal);
  }
  else if (digitalRead(pulsingModePin) == 0) {
    digitalWrite(LEDpins, LOW);
    digitalWrite(13, LOW);
    if (digitalRead(buttonPin) == LOW) {
      //tetanus();
      analogWrite(LEDpins, AnalogVal / 4);
      Serial.print(" ");
      digitalWrite(13, HIGH);
      analogWrite(A4, 2.5);
    }
    
  }
}



void single_pulse(int PULSE_WIDTH, int PERIOD, int AnalogVal) {
  unsigned long loopStartTime = millis();
    
  // Set all to high (turn LEDs on)
  analogWrite(LEDpins, AnalogVal / 4);
  digitalWrite(13, HIGH);
  analogWrite(A4, 2.5);
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
  digitalWrite(13, LOW);
  analogWrite(A4, 0);
  Serial.print("OFF: ");
  Serial.println(t);
  // pause for rest of stimulation frequency 
  t = millis();
  while (t - loopStartTime < period){
    t = millis();
  }
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
  flag = false;
}
  
