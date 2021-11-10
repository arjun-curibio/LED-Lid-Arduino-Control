/* Arudino script for controlling LEDS in LED stim board, v1
 *  Stim board has single control input, which controls all LEDs through simple transistor switch
 *  Two parameters: stimRate [Hz] and pulseDuration [ms], can be set below
 *  Keep Serial communications on for visual confirmation of pulses on Arduino board (Tx LEDs on board will flash when transmitting information)
 *  
 *  BUGS: Cannot set stimRate below 1 Hz.  NEED TO FIX.
 *        11/4/21 - FIXED
 * 
 * 
 */

const int LEDpins = 7;
const int switchingPin = 2;
const int outputPin5V = 24;

/////////////////////////////////// SET PARAMETERS BELOW ///////////////////////////////////////
const float stimRate = 1; // Hz
unsigned long pulseDuration = 100; // ms
////////////////////////////////////////////////////////////////////////////////////////////////

volatile boolean flag = false;
const int stimPeriod = 1000 / stimRate; // ms
void setup() {
  pinMode(LEDpins, OUTPUT);
  pinMode(switchingPin, INPUT);
  pinMode(outputPin5V, OUTPUT);
  
  digitalWrite(LEDpins, LOW);
  digitalWrite(outputPin5V, HIGH);

  attachInterrupt(digitalPinToInterrupt(switchingPin), tetanus, RISING);
  // Uncomment below if serial comm needed (debugging)
  Serial.begin(9600);
  Serial.println(stimPeriod);
}

void loop() {
  
  if (flag == true) {
    tetanus();
    flag = false;
  }
  int currentTime = millis();
  
  // Set all to high (turn LEDs on)
  digitalWrite(LEDpins,HIGH);
  Serial.print("ON: ");
  Serial.print(millis());
  Serial.print(',');
  // pause for pulse duration
  int t = millis();
  while (t - currentTime < pulseDuration) {
    t = millis();
    
  }
  
  // Set all to low (turn LEDs off)
  digitalWrite(LEDpins,LOW);
  Serial.print("OFF: ");
  Serial.println(t);
  // pause for rest of stimulation frequency 
  t = millis();
  while (t - currentTime < stimPeriod){
    t = millis();
  }

}

void myISR() {
  flag = true;
}
void tetanus() {
  flag = true;
  digitalWrite(LEDpins, LOW);
  
  Serial.print("Delay before Tetanus: ");
  Serial.print(millis());
  delay(3000);
  // Turn them on for two seconds
  digitalWrite(LEDpins, HIGH);
  Serial.print("; In Tetanus; ");
  delay(2000);
  digitalWrite(LEDpins, LOW);
  Serial.print("End Tetanus: ");
  Serial.println(millis());
  delay(10000);
}
  
