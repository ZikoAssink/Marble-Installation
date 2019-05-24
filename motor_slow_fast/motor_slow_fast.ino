#define SERVOMIN  110 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  2000 // this is the 'maximum' pulse length count (out of 4096)
#define SERVO_NUM 2 // number of servo motors

#include <Wire.h>

#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
uint8_t servos[SERVO_NUM];

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  // Assign servo numbers
  for (int i = 0; i < SERVO_NUM; i += 1) {
    servos[i] = i;
  }
}

void loop() {
  // SLOW
  sweep(1, 1);
  sweep(2, 1);

//  // FAST
//  sweep(1, 3);
//  sweep(2, 3);
//
//  // FASTER
//  sweep(1, 5);
//  sweep(2, 5);
}

void sweep(int which, int motorSpeed){
  float pos;
  for (pos = 0; pos <= 360; pos += motorSpeed) { // goes from 0 degrees to 360 degrees
    pwm.setPWM(servos[which-1], 0, map(pos, 0, 360, SERVOMIN, SERVOMAX));              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
  }
}
