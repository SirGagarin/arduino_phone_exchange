/*
 * Simulation of 8 lines of telephone traffic comming to a pulse exchange.
 * Calls are made at random intervals
 *
 * Pawel Adamowicz, 2016
 */

// used pins
byte dataPin = 4;
byte latchPin = 5;
byte clockPin = 6;

// time data for pulse sequences
const int pulseTime = 100;      // single pulse time
const int gapTime = 100;        // interval between pulses
const int dialBreakTime = 300;  // interval between digit pulse sequences */

const long minTimeBetweenCalls = 30000; // minimum interval between calls (maximum is twice this value)
const long minCallDuration = 20000;     // minimum call duration (maximum is twice this value)

const byte linesCount = 1;  // maximum is 8 for 8 shift regisetr outputs
const byte occupiedLinesLimit = 4;
byte occupiedLinesCnt;

char* phoneNumbers[] = {"012","423","121","816","256","513","922","786","427","020","576","137","631","595","711","800"};
//char* phoneNumbers[] = {"2114213","2114254","6331212","1234567","8901234","5563451","0801922","4392211"};
const byte availableNumbers = 16;

// Number of pulses per digit
const byte dialPattern[10] = {10,1,2,3,4,5,6,7,8,9}; // Generic Siemens model
//byte dialPattern[10] = {1,2,3,4,5,6,7,8,9,10}; // Swedish model
//byte dialPattern[10] = {10,9,8,7,6,5,4,3,2,1}; // Norwegian model

byte outData = 0; // output for shift register

//long lastCallTime;


enum LineState {FREE, DIALING_INIT, DIALING_PULSE, DIALING_GAP, DIALING_BREAK, OCCUPIED};

struct PhoneLine {
    byte lineId;
    long lastEventTime;
    LineState state;
    char currentDial[8];
    byte currentPulseCnt;
    byte digitId;
} lines[8];


void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  randomSeed(analogRead(0));
  outData = 0;
  occupiedLinesCnt = 0;
  for (int i = 0; i < linesCount; i++)
  {
    lines[i].lineId = i;
    lines[i].lastEventTime = 0;
    lines[i].state = FREE;
    lines[i].currentDial[0] = 0;
  }
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < linesCount; i++)
  {
    checkLineState(i,millis());
  }
  updateShiftRegister(outData);
}

void checkLineState(byte lineId, long currTime)
{
  //enum LineState {FREE, DIALING_INIT, DIALING_PULSE, DIALING_GAP, DIALING_BREAK, OCCUPIED};
  switch (lines[lineId].state)
  {
    case FREE:  // line is free and can be set HIGH so exchange can start connection
      if ((currTime - lines[lineId].lastEventTime) > (minTimeBetweenCalls * random(1,10000)) && occupiedLinesCnt < occupiedLinesLimit) //random(minTimeBetweenCalls, minTimeBetweenCalls * 2) &&
          //(currTime - lastCallTime) > (minTimeBetweenCalls * random(1,10)))//random(minTimeBetweenCalls, minTimeBetweenCalls * 2))
      {
        int phoneId = chooseUnusedPhone();
        if (phoneId != -1) {
          lines[lineId].state = DIALING_INIT;
          lines[lineId].lastEventTime = currTime;
          strcpy(lines[lineId].currentDial, phoneNumbers[phoneId]);
          //lastCallTime = currTime;
          bitSet(outData,lineId);
          lines[lineId].digitId = 0;
          occupiedLinesCnt++;
          Serial.print("Connection initiated on line ");
          Serial.println(lineId,DEC);
        }
      }
    break;
    case DIALING_INIT: // line initiated connection and is ready to start dialing
      if ((currTime - lines[lineId].lastEventTime) > dialBreakTime)
      {
        lines[lineId].currentPulseCnt = dialPattern[lines[lineId].currentDial[lines[lineId].digitId] - '0'];
        lines[lineId].state = DIALING_PULSE;
        lines[lineId].lastEventTime = currTime;
        bitClear(outData,lineId);
        Serial.print("Started dialing ");
        Serial.print(lines[lineId].currentDial);
        Serial.print(" on line ");
        Serial.println(lineId,DEC);
      }
    break;
    case DIALING_PULSE:
      if ((currTime - lines[lineId].lastEventTime) > pulseTime)
      {
        lines[lineId].currentPulseCnt --;
        if (lines[lineId].currentPulseCnt == 0) {
          lines[lineId].digitId++;
          if (lines[lineId].currentDial[lines[lineId].digitId] == 0)
          {
            lines[lineId].state = OCCUPIED;
            lines[lineId].lastEventTime = currTime;
            bitSet(outData,lineId);
            Serial.print("Dialing finished, occupied line ");
            Serial.println(lineId,DEC);
          }
          else
          {
            lines[lineId].currentPulseCnt = dialPattern[lines[lineId].currentDial[lines[lineId].digitId] - '0'];
            lines[lineId].state = DIALING_BREAK;
            lines[lineId].lastEventTime = currTime;
            bitSet(outData,lineId);
          }
        }
        else
        {
          lines[lineId].state = DIALING_GAP;
          lines[lineId].lastEventTime = currTime;
          bitSet(outData,lineId);
        }
      }
    break;
    case DIALING_GAP:
      if ((currTime - lines[lineId].lastEventTime) > gapTime)
      {
          lines[lineId].state = DIALING_PULSE;
          lines[lineId].lastEventTime = currTime;
          bitClear(outData,lineId);
      }
    break;
    case DIALING_BREAK:
      if ((currTime - lines[lineId].lastEventTime) > dialBreakTime)
      {
          lines[lineId].state = DIALING_PULSE;
          lines[lineId].lastEventTime = currTime;
          bitClear(outData,lineId);
      }
    break;
    case OCCUPIED:
      if ((currTime - lines[lineId].lastEventTime) > random(minCallDuration, minCallDuration * 4))
      {
        lines[lineId].state = FREE;
        lines[lineId].lastEventTime = currTime;
        lines[lineId].currentDial[0] = 0;
        occupiedLinesCnt--;
        bitClear(outData,lineId);
        Serial.print("Disconnecting line ");
        Serial.println(lineId,DEC);
      }
    break;
  }
}


int chooseUnusedPhone()
{
    int phoneId = -1;
    bool occupied;
    for (byte i = 0; i < availableNumbers; i++)
    {
      phoneId = random(availableNumbers);
      occupied = false;
      for (byte j = 0; j < linesCount; j++)
      {
        if (!strcmp(lines[j].currentDial, phoneNumbers[phoneId]))
        {
          occupied = true;
          break;
        }
      }
      if (!occupied && phoneNumbers[phoneId][0] != 0)
        return phoneId;
    }
    return -1;
}


void updateShiftRegister(byte value)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}
