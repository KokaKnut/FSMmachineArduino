#include <LightChrono.h>
#include "limits.h"
#define MAXARRAY 30

// set up refference constants for pins
#define VALVEPINONE 5
#define VALVEPINTWO 6
#define VALVEPITHREE 7
#define SENSORPIN 4

// set up the valve delay constants
int valveDelay1 = 0;
int valveDelay2 = 1000;
int valveDelay3 = 2000;

// set up time keeping capability
LightChrono myChrono;
unsigned long t = 0;
unsigned long start[ MAXARRAY ];
unsigned long finish[ MAXARRAY ];
bool active[ MAXARRAY ];
byte index = 0;

// set up tracking whether we are timing a stick or not
bool tracking = false;
unsigned long trackingThreshold = 0;
unsigned long blip = 25;

void setup() {
  // set up pins
  pinMode(VALVEPINONE, OUTPUT);
  pinMode(VALVEPINTWO, OUTPUT);
  pinMode(VALVEPITHREE, OUTPUT);
  pinMode(SENSORPIN, INPUT);
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup

  Serial.begin(9600); // sets communication rate

  // initialize arrays
  for(int i = 0; i < MAXARRAY; i++){
    start[i] = 0;
    finish[i] = 0;
    active[i] = false;
  }
}

void loop() {
  // code for sensing and timing sticks
  t = myChrono.elapsed();

if(t%500 == 0)
{
  Serial.println("------------------");
  Serial.println(t);
  Serial.println(digitalRead(SENSORPIN));
  Serial.println(tracking);
  Serial.println(trackingThreshold);
}
  
  if(digitalRead(SENSORPIN) == LOW){   // did the sensor see something?
    if(!tracking){   // is it a new stick?
      tracking = true;
      trackingThreshold = t;
    }
    if(trackingThreshold < t - blip) {   // is it more than just a blip?
      trackingThreshold = ULONG_MAX;
      start[index] = t;
      finish[index] = 0;
      active[index] = true;
    }
    
  } else {
    if(tracking){   // are we still tracking when its empty?
      tracking = false;
      trackingThreshold = 0;
      finish[index] = t;
      Serial.println(finish[index]);
      index = (index + 1) % MAXARRAY;
    }
  }

  // let's assume we are turning off all of the valves unless we find reason not to
  bool turnOffValve1 = true;
  bool turnOffValve2 = true;
  bool turnOffValve3 = true;
  
  // code for controlling valves and tracking sticks passing through
  for(int i = 0; i < MAXARRAY; i++) {   // iterate through the sticks
    if(active[i]) {   // is this one in the machine?
      if(t >= start[i] + valveDelay1) {  // is the front of the stick past the valve?
        if(finish[i] == 0 || t <= finish[i] + valveDelay1) {  // is the back of the stick not all the way in the machine yet or past the valve?
          turnOffValve1 = false;
        }
      }
      // valve2 check
      if(t >= start[i] + valveDelay2) {  // is the front of the stick past the valve?
        if(finish[i] == 0 || t <= finish[i] + valveDelay2) {  // is the back of the stick not all the way in the machine yet or past the valve?
          turnOffValve2 = false;
        }
      }
      // valve3 check
      if(t >= start[i] + valveDelay3) {  // is the front of the stick past the valve?
        if(finish[i] == 0 || t <= finish[i] + valveDelay3) {  // is the back of the stick not all the way in the machine yet or past the valve?
          turnOffValve3 = false;
        } else {
          active[i] = false;
        }
      }
    }
  }

  //turn off the valves we dicided should be off
  if(turnOffValve1) {
    digitalWrite(VALVEPINONE, HIGH);
  } else {
    digitalWrite(VALVEPINONE, LOW);
  }
  
  if(turnOffValve2) {
    digitalWrite(VALVEPINTWO, HIGH);
  } else {
    digitalWrite(VALVEPINTWO, LOW);
  }
  
  if(turnOffValve3) {
    digitalWrite(VALVEPITHREE, HIGH);
  } else {
    digitalWrite(VALVEPITHREE, LOW);
  }
}









