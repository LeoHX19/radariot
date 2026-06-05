import <Servo.h>

Servo servo;

#define TRIG 9
#define ECHO 10
#define Buzzer 3
#define ServoPin 6

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(ServoPin, OUTPUT);
  servo.attach(ServoPin);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000); // 30 ms timeout

  if (duration == 0) {
    Serial.println("No echo received");
  } else {
    float distance = duration * 0.034 / 2;
    Serial.print("Distance: ");
    Serial.println(distance);
  }

  delay(500);
}
