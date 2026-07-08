#include <Arduino.h>
#include <string.h>
// #include <Mouse.h>

const bool solenoidOn = true; // HIGH

//Phisical output pins
const byte H2BreakBeamPin = 3;
const byte solenoidH2Pin = 2;
const byte ArduinoLEDPin = 13;
const byte NMRPin = 4;
bool OldbreakBeamState = false;
bool OldNMRBeamState = false;
bool printonce = true;

bool printLogMSG = false; // If true provides H2 on/off logs and bream beam states if false gives user only necessary information

const unsigned long NMRClickWaitTimer = 1500; // in milliseconds
const long NMRClickWaitTimer_Micro = NMRClickWaitTimer*1000;
const bool MRClickWaitTimer_Enable = false; // Turn to false to have NMR Break beam trigger click as soon as possible.


unsigned long timerStart = 0;

bool solenoidBreakBeamFunc(byte BreakBeamPin, bool defualtState);
bool NMRBreakBeamFunc(byte __NMRPin, bool __solenoidBreakBeamState, bool __MRClickWaitTimer_Enable);

void setup() {
  Serial.begin(115200);
  
  pinMode(H2BreakBeamPin, INPUT_PULLUP);
  pinMode(solenoidH2Pin, OUTPUT);
  pinMode(ArduinoLEDPin, OUTPUT);
  digitalWrite(solenoidH2Pin, LOW);
  digitalWrite(ArduinoLEDPin, LOW);

  Serial.println("Log: Setup Done!");

}

//timer varables
bool solenoidBreankBeamStateOldtimer = true;
bool NMRBreankBeamStateOldTimer = true;
unsigned long starttimerValue;
bool timerRunning = false;

void loop() {

  bool solenoidBreankBeamState = solenoidBreakBeamFunc(H2BreakBeamPin, solenoidOn);
  bool NMRBreankBeamState = NMRBreakBeamFunc(NMRPin, solenoidBreankBeamState, MRClickWaitTimer_Enable);

  // impementing time feture
  if (solenoidBreankBeamState != solenoidBreankBeamStateOldtimer) { // change of state
    if (solenoidBreankBeamState) { // beam connected start timer 
        timerRunning = true;
        starttimerValue = micros(); //un Fact: Timer overflows at 49 days XD

        if (!NMRBreankBeamState) {
          Serial.println("Error: NMR should be empty but is not - Timer unable to record");
          timerRunning = false;
        } else {
          Serial.println("Log: Timer Started");
          printonce = true;
        }
        
        NMRBreankBeamStateOldTimer = NMRBreankBeamState;
      }
    solenoidBreankBeamStateOldtimer = solenoidBreankBeamState;
  } else {
    if (timerRunning) {
      if (NMRBreankBeamStateOldTimer != NMRBreankBeamState && !NMRBreankBeamState) { //chnge of state
        unsigned long curentTimerValue = micros();
        timerRunning = false;

        long deltaTimer = curentTimerValue - starttimerValue;
        
        if (deltaTimer < 0) {
          Serial.println("Error: Timer overflow");
        }
      
        long deltaTimerSeconds = deltaTimer/1000000;
        long deltaTimeMiliSeconds = (deltaTimer - (deltaTimerSeconds*1000000) )/1000;

        String outputMessage = "Log: SolenoidBreankBeam to NMRBreankBeamS timer value ";
        outputMessage += deltaTimerSeconds;
        outputMessage += " s  ";
        outputMessage += deltaTimeMiliSeconds;
        outputMessage += " ms";
        Serial.println(outputMessage);

        NMRBreankBeamStateOldTimer = NMRBreankBeamState;
      }
    }
  }
}

bool solenoidBreakBeamFunc(byte BreakBeamPin, bool defualtState) {
  bool breakBeamState = !digitalRead(BreakBeamPin);

  if (breakBeamState) { //if beam is broken
    digitalWrite(solenoidH2Pin, defualtState); //turn on flow for H2
    
    // digitalWrite(ArduinoLEDPin, defualtState);

    if (OldbreakBeamState != breakBeamState) { //only prints message once
      if (printLogMSG) {
         Serial.println("Log: Beam Broken, H2 on");
        }
      OldbreakBeamState = breakBeamState;
    }
    return false;
  
  } else {
    digitalWrite(solenoidH2Pin, !defualtState); //turn off flow for H2
    digitalWrite(ArduinoLEDPin, !defualtState);

    if (OldbreakBeamState != breakBeamState) { //only prints message once
      if (printLogMSG) { Serial.println("Log: Beam not Broken, H2 off"); }
      OldbreakBeamState = breakBeamState;
    }

    return true;
  }
}

bool NMRBreakBeamFunc(byte __NMRPin, bool __solenoidBreakBeamState, bool __MRClickWaitTimer_Enable) {
  bool NMRBeamState = !digitalRead(__NMRPin);

  if (NMRBeamState) { //if beam is broken click
    if (OldNMRBeamState != NMRBeamState) { //only prints message once
      if (printLogMSG) { Serial.println("Log: NMR Beam Broken"); }
      OldNMRBeamState = NMRBeamState;
      digitalWrite(ArduinoLEDPin, HIGH);

      if (__solenoidBreakBeamState) { // removed edje case, both beams broken
        
        if (__MRClickWaitTimer_Enable) { 
          unsigned long __currentTime = 0;
          unsigned long __deltaTime = 0;
          do
          {
            __currentTime = micros();
            __deltaTime = __currentTime - starttimerValue;
          } while ( NMRClickWaitTimer_Micro > __deltaTime );
        }
        if (printonce) {
          Serial.println("Click: Trigger NMR");
          printonce = false;
        }
      }
    }

    return false;
  
  } else { // does nothing other then loging
    if (OldNMRBeamState != NMRBeamState) { //only prints message once
      if (printLogMSG) { Serial.println("Log: NMR Beam not Broken"); }
      OldNMRBeamState = NMRBeamState;
    }
    digitalWrite(ArduinoLEDPin, LOW);
    return true;
  }
}