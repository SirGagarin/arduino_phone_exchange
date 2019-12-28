/*
 * Experiments with rotary dial, Siemens type.
 * Wiring (Polish standard):
 * - red - 5V
 * - green - pulses (pulse in LOW)
 * - yellow - dial in use (standby in LOW)
 *
 * Pawel Adamowicz, 2016
 */

byte pulsePin = 2;
byte standByPin = 3;
byte digit = 0;

int debounceDelay = 100;
long lastPulse;
bool isPrinted;
bool pulseRegistered;


void setup() {
  pinMode(pulsePin,INPUT_PULLUP);
  pinMode(standByPin,INPUT_PULLUP);
  lastPulse = 0;
  isPrinted = true;
  pulseRegistered = false;
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(standByPin) == HIGH)
  {
    isPrinted = false;
    if (digitalRead(pulsePin) == LOW)
    {
      if ((millis() - lastPulse) > debounceDelay)
      {
        pulseRegistered = true;
        digit++;
        lastPulse = millis();
      }
    }
  }
  else
  {
    if (!isPrinted && pulseRegistered)
    {
      if (digit == 10)
        digit = 0;
      Serial.print("Dialed ");
      Serial.println(digit,DEC);
      isPrinted = true;
      pulseRegistered = false;
      digit = 0;
    }
  }
}
