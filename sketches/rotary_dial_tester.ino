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
byte latchPin = 5;
byte clockPin = 6;
byte dataPin = 4;

byte digit = 0;
int debounceDelay = 100;
long lastPulse;
bool isPrinted;
bool pulseRegistered;


void setup() {
  pinMode(pulsePin,INPUT_PULLUP);
  pinMode(standByPin,INPUT_PULLUP);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  lastPulse = 0;
  isPrinted = true;
  pulseRegistered = false;
  updateShiftRegister(0);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(standByPin) == HIGH)
  {
    if (isPrinted) {
      updateShiftRegister(0);
      isPrinted = false;
      Serial.println("Dialing...");
    }
    if (digitalRead(pulsePin) == LOW)
    {
      if ((millis() - lastPulse) > debounceDelay)
      {
        pulseRegistered = true;
        digit++;
        updateShiftRegister(leadPattern(digit));
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

// set bits in order, if highest bit is reached clear bits starting from lowest
byte leadPattern(byte value)
{
  byte pattern = 0;
  for (byte i = 0; i < value; i++) {
    if ( bitRead(pattern,i % 8) )
      bitClear(pattern,i % 8);
    else
      bitSet(pattern,i % 8);
  }
  return pattern;
}

void updateShiftRegister(byte value)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}
