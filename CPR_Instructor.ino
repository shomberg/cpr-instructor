#include <Wire.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_LSM303_U.h>

#define arduinoPower 13
#define pressurePin A0
#define speakerPin
#define buzzerPin 8
#define nextPin 9

#define buzzer_period 600
#define step_period 10
#define instruct_msg_time 2000

enum state_type {
  call_help,
  instruct_call_help,
  instruct_lay_back,
  instruct_place_hands,
  instruct_push,
  instruct_continue,
  instruct_beat,
  pump_chest
};
state_type cpr_step = call_help;
unsigned long state_start_time = millis();
unsigned long step_start_time;

//int sound = 250;
//float pressureStrength;

//////////////////////////////////////
//
//void displaySensorDetails(void)
//{
//  sensor_t sensor;
//  accel.getSensor(&sensor);
//  Serial.println("------------------------------------");
//  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
//  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
//  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
//  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
//  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
//  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
//  Serial.println("------------------------------------");
//  Serial.println("");
//  delay(500);
//}
//////////////////////////////////////////////


void setup() {
  //Start serial at 9600 boud
  Serial.begin(9600);
  Serial.println();
  //set up pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(nextPin, INPUT);
}




void loop() {
  //loop iteration starts
  step_start_time = millis();

  //State transitions
  bool state_switched = false;
  switch (cpr_step) {
    case call_help:
      if (digitalRead(nextPin) == HIGH) {
        cpr_step = instruct_call_help;
        state_switched = true;
      }
      break;
    case instruct_call_help:
      if (millis() - state_start_time > instruct_msg_time) {
        cpr_step = instruct_lay_back;
        state_switched = true;
      }
      break;
    case instruct_lay_back:
      if (millis() - state_start_time > instruct_msg_time) {
        cpr_step = instruct_place_hands;
        state_switched = true;
      }
      break;
    case instruct_place_hands:
      if (millis() - state_start_time > instruct_msg_time) {
        cpr_step = instruct_push;
        state_switched = true;
      }
      break;
    case instruct_push:
      if (millis() - state_start_time > instruct_msg_time) {
        cpr_step = instruct_continue;
        state_switched = true;
      }
      break;
    case instruct_continue:
      if (millis() - state_start_time > instruct_msg_time) {
        cpr_step = instruct_beat;
        state_switched = true;
      }
      break;
    case instruct_beat:
      if (millis() - state_start_time > instruct_msg_time) {
        cpr_step = pump_chest;
        state_switched = true;
      }
      break;
  }
  if (state_switched) {
    state_start_time = millis();
  }

  //state actions
  switch (cpr_step) {
    case call_help:
      //speak
      Serial.println("Help! The person is having a heart attack.");
      Serial.println("Please come over and press the button for CPR instruction.");
      Serial.println();
      break;
    case instruct_call_help:
      Serial.println("Call 991 before beginning CPR.");
      Serial.println();
      break;
    case instruct_lay_back:
      //lay patient on the back
      Serial.println("Lay the person on their back.");
      Serial.println();
      break;
    case instruct_place_hands:
      Serial.println("Place both hands on the center of the person's chest.");
      Serial.println();
      break;
    case instruct_push:
      Serial.println("Push hard as hard as you can. Use all of your body weight.");
      Serial.println();
      break;
    case instruct_continue:
      Serial.println("Continue to push a rate of 100 compressions a minute.");
      Serial.println();
      break;
    case instruct_beat:
      Serial.println("A beat will now begin playing. Make sure to push to the beat.");
      Serial.println();
      break;
    case pump_chest:
      //buzzer starts
      if ((millis() - state_start_time) % buzzer_period < 50) {
        digitalWrite(buzzerPin, HIGH);
      } else {
        digitalWrite(buzzerPin, LOW);
      }
      //buzzer beat 100/min
      //     tone(buzzerPin, sound);
      //      delay(50);
      //      noTone(buzzerPin);
      //      delay(550);
  }
  delay(step_period - (millis() - step_start_time));
}
