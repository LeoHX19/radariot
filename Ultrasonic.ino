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

void activateBuzzer() {
  digitalWrite(Buzzer, HIGH);
  delay(1000); // Buzzer on for 1 second
  digitalWrite(Buzzer, LOW);
}

void sweepServo() {
  for (int pos = 0; pos <= 180; pos += 1) {
    servo.write(pos);
    checkDistance();
    Serial.println(pos);
    delay(15);
  }
  for (int pos = 180; pos >= 0; pos -= 1) {
    servo.write(pos);
    checkDistance();
    Serial.println(pos);
    delay(15);
  }
}

void checkDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000); // 30 ms timeout

  float distance = duration * 0.034 / 2;
  Serial.println(distance);

}

void loop() {
  sweepServo();
}
