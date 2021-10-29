/* Arudino script for controlling LEDS in LED stim board, v1
 *  Stim board has single control input, which controls all LEDs through simple transistor switch
 *  Two parameters: stimRate [Hz] and pulseDuration [ms], can be set below
 *  Keep Serial communications on for visual confirmation of pulses on Arduino board (Tx LEDs on board will flash when transmitting information)
 *  
 *  BUGS: Cannot set stimRate below 1 Hz.  NEED TO FIX.
 * 
 * 
 */

const int LEDpins = 7;

/////////////////////////////////// SET PARAMETERS BELOW ///////////////////////////////////////
unsigned long stimRate = 1; // Hz
unsigned long pulseDuration = 200; // ms
////////////////////////////////////////////////////////////////////////////////////////////////


const int stimPeriod = 1000 / stimRate; // ms
void setup() {
  pinMode(LEDpins, OUTPUT);
  digitalWrite(LEDpins, LOW);
  // Uncomment below if serial comm needed (debugging)
  Serial.begin(9600);
  Serial.println(stimPeriod);
}

void loop() {

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
  
