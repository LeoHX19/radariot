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
  servo.attach(ServoPin);
  Serial.begin(9600);
  int pos = 0;
}

void activateBuzzer() {
  digitalWrite(Buzzer, HIGH);
  delay(1000); // Buzzer on for 1 second
  digitalWrite(Buzzer, LOW);
}

void sweepServo() {
  if (pos == 0) {
    for (; pos <= 180; pos += 1) {
      servo.write(pos);
      delay(15);
      checkDistance();
      Serial.println(",",pos);
    }
  }else {
    for (; pos > 0; pos -= 1) {
      servo.write(pos);
      delay(15);
      checkDistance();
      Serial.println(",",pos);
    }
  }
}

void checkDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000); // 30 ms timeout
  if (duration == 0) {
    Serial.print("NULL");
  }else {
    float distance = duration * 0.034 / 2;
    Serial.print(distance);
  }
}

void 

void loop() {
  sweepServo();
}
