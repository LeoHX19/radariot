#include <Servo.h>

// Pin definitions
const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 11;
const int buzzerPin = 12;

// Constants - tune these
const int SWEEP_START = 0;
const int SWEEP_END = 180;
const int STEP_DELAY = 30;        // ms per step during sweep
const int ANGLE_STEP = 1;         // degrees between samples
const int DETECTION_THRESHOLD = 500; // cm, objects closer than this
const int TRACKING_TIME_MS = 1000;   // 3 seconds tracking per object
const int NUM_READINGS = 8;          // readings during tracking
const int BEEP_DURATION = 2000;

Servo myServo;

struct DetectedObject {
  int angle;
  float distance;
};

DetectedObject objects[20];  // max 20 objects per sweep
int objectCount = 0;

unsigned long trackingStartTime;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  myServo.attach(servoPin);
  Serial.begin(9600);
  myServo.write(90); // neutral start
  delay(500);
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH,20000);
  float distance = duration * 0.034 / 2; // cm
  return distance;
}

void sweepAndDetect() {
  objectCount = 0;
  Serial.println("Starting Sweep...");
  
  for (int angle = SWEEP_START; angle <= SWEEP_END; angle += ANGLE_STEP) {
    myServo.write(angle);
    delay(STEP_DELAY);
    
    float dist = getDistance();
    if (dist > 2 && dist < DETECTION_THRESHOLD) {  // valid range
      objects[objectCount].angle = angle;
      objects[objectCount].distance = dist;
      objectCount++;
      Serial.print("Object at "); Serial.print(angle);
      Serial.print("° dist: "); Serial.println(dist);
    }
  }
}

void trackObjects() {
  if (objectCount == 0) return;
  
  Serial.println("Starting tracking...");
  trackingStartTime = millis();
  
  for (int i = 0; i < objectCount; i++) {
    if (millis() - trackingStartTime > 10000) break; // 10s max tracking phase
    
    int ang = objects[i].angle;
    float prevDist = objects[i].distance;
    
    myServo.write(ang);
    delay(400); // settle time
    
    // Take multiple readings
    float total = 0;
    int validReads = 0;
    
    for (int r = 0; r < NUM_READINGS; r++) {
      float d = getDistance();
      if (d > 2 && d < 400) {
        total += d;
        validReads++;
      }
      delay(TRACKING_TIME_MS / NUM_READINGS);
    }
    
    float currentDist = (validReads > 0) ? total / validReads : prevDist;
    
    Serial.print("Object at "); Serial.print(ang);
    Serial.print("° | Prev: "); Serial.print(prevDist);
    Serial.print(" | Now: "); Serial.println(currentDist);
    
    if (currentDist < prevDist - 3) {  // approaching (with some tolerance)
      tone(buzzerPin, 1000, BEEP_DURATION);
      delay(BEEP_DURATION);           // stare while beeping
      // keep staring - servo already at position
      return;                         // or continue based on your preference
    } 
    // else discard (do nothing - it will be dropped on next sweep)
  }
  
  objectCount = 0; // clear list after round
}

void loop() {
  sweepAndDetect();
  trackObjects();
  
  delay(500); // small pause before next full sweep
}