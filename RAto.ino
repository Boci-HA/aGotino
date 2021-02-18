/*
 * RAto v 0.5  Arduino code to move telescope eq mount Right Ascension at
 *             1x and detect button press to cycle among speeds 1x->8x->-6x
 *
 *  by gspeed @ astronomia.com / qde / cloudynights.com forum           
 *  Wirings: https://mappite.org/stars/RAto-drv8825.png
 *  This code or newer versions at https://mappite.org/stars/RAto.ino 
 *  
 *  This code is free software under GPL v3 License use at your risk and fun ;)
 *
 * UPDATE: new project aGotino can be used for just RA tracking as well
 *         check for updates on
 *         https://github.com/mappite/aGotino
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * How to calculate STEP_DELAY to drive motor at right speed to follow stars
 * 
 * Worm Ratio  144		// 144 eq5/exos2, 135 heq5, 130 eq3-2
 * Pulley/Gear Ratio 2.5	// depends on your pulley setup e.w. 40T/16T = 2.5 
 * Steps per revolution  400	// or 200, usually
 * Microstep 32			// make sure to enable pins in driver
 *
 * Steps per degree  12800 	// WormRatio*OtherRatio*StepsPerRevolution*Microsteps/360
 *                         	// = number of microsteps to rotate the scope by 1 degree
 *
 * STEP_DELAY  18699  // 1000000*((23*60*60+56*60+4)/360)/stepsPerDegree
 *                    // = microseconds to advance a microstep
 *                    // 23*60*60+56*60+4 is the number of secs for earth 360deg rotation
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 */

// in micros, step duration to follow stars
const unsigned long STEP_DELAY = 18699; 

// pin connections
const int dirPin = 4;
const int stepPin = 3;
const int buttonPin = 2;     
const int ledPin = LED_BUILTIN;     

// buffer time in micros: when we are at STEP_DELAY_MIN micros from having
// to change pulse status let's call (blocking) delayMicroseconds to allow a smooth pulse signal
// this must be less than 16383 and more than what logic does at each loop to honor STEP_DELAY
unsigned long STEP_DELAY_MIN = 100; 

unsigned long lastTime; // micros, last time pulse has changed status 
boolean stepPinStatus = false;

int velocity = 1; // speed is a reserved keyword ;)
unsigned long i = 0; // for debug

void setup() {
  Serial.begin(19200);
  Serial.print("RAto: READY");
  // init Driver Motor pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  // Set motor direction clockwise (HIGH)
  digitalWrite(dirPin, HIGH);
  // make sure no signal is sent
  digitalWrite(stepPin, LOW); // no signal
  // init Button pin as Input Pullup so no need resistor
  pinMode(buttonPin, INPUT_PULLUP);
  // init led and turn on for 0.5 sec
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print(" and RUNNING with pulse length: ");
  Serial.println(STEP_DELAY);
  // Resolution is 4microsecs on 16Mhz board
  lastTime = micros();
}


void runStep(long stepDelay) {
    unsigned long halfStepDelay = stepDelay/2; // duration of each pulse
    unsigned long dt  = micros() - lastTime;   // delta time elapsed since previus stepPin status change
    // micros() overflow after approximately 70 minutes but "-" logic above will work anyway to calculate dt 
    
    if ( dt > halfStepDelay)  { // no luck, too fast, we can't honor halfStepDelay.
      Serial.print("Can't honor the speed, dt: ");
      Serial.println(dt);
    } else  if ( dt > halfStepDelay-STEP_DELAY_MIN) { // need to switch pulse in halfStepDelay-dt micros, let's do it:
      // remaining delay to keep current pin status to honor halfStepDelay
      delayMicroseconds(halfStepDelay-dt);// which is less than STEP_DELAY_MIN
      //change pin status 
      stepPinStatus = !stepPinStatus;
      digitalWrite(stepPin, (stepPinStatus?HIGH:LOW));
      /* debug 
      i++;
      if (i == 1000) { 
        unsigned long pulseDuration2 = (micros()-lastTime);
        Serial.println("logB:");
        Serial.print("stepDelay: "); Serial.println(stepDelay);
        Serial.print("halfStepDelay: "); Serial.println(halfStepDelay);
        Serial.print("lastTime: ");Serial.println(lastTime);
        Serial.print("dt: "); Serial.println(dt);
        Serial.print("delayMicroseconds:"); Serial.println((halfStepDelay-dt));    
        Serial.print("pulseDuration2: "); Serial.println(pulseDuration2);    
        delay(5000);  
        i= 0;
      }
      */
      lastTime = micros(); // last time we updated the pin status
    }
}

void loop() {
  runStep(STEP_DELAY/velocity);
  /* 
   * logic executed at each loop must take less than (STEP_DELAY/2-STEP_DELAY_MIN) 
   * and less than STEP_DELAY_MIN. This is just a an IF for now...
   */
  // cycle velocity on button press
  if ( digitalRead(buttonPin) == LOW ) {
     digitalWrite(LED_BUILTIN, HIGH);
     Serial.print("New Velocity: ");
     // 1x -> 8x -> -6x
     if (velocity == 1) { 
        velocity = 8;
        digitalWrite(dirPin, HIGH);
     } else if  (velocity == 8) {
        velocity = 6;
        digitalWrite(dirPin, LOW); // change direction
     } else if  (velocity == 6) {
        velocity = 1;
        digitalWrite(dirPin, HIGH);
     }
     Serial.println(velocity);
     delay(1000); // wait to release button... this is blocking
     digitalWrite(LED_BUILTIN, LOW);
     lastTime = micros(); // reset time 
  }
 
}
