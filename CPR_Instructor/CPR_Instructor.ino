#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_VS1053.h>

#define arduinoPower 13
#define pressurePin A0
#define speakerPin
#define buzzerPin 8
#define nextPin 11

// defines for mp3 breakout
#define MP3_RESET 9
#define MP3_CS 10
#define MP3_DCS 8
#define MP3_CARDCS 4
#define MP3_DREQ 3

#define buzzer_period 600
#define step_period 10
#define instruct_msg_time 2000

enum state_type {
  initial_state,
  call_help,
  instruct_all,
  instruct_call_help,
  instruct_lay_back,
  instruct_place_hands,
  instruct_push,
  instruct_continue,
  instruct_beat,
  pump_chest
};
state_type cpr_step = initial_state;
unsigned long state_start_time = millis();
unsigned long step_start_time;

Adafruit_VS1053_FilePlayer mp3_player =
  Adafruit_VS1053_FilePlayer(MP3_RESET, MP3_CS, MP3_DCS, MP3_DREQ, MP3_CARDCS);

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
  //Start serial at 9600 baud
  Serial.begin(9600);
  Serial.println();
  //set up pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(nextPin, INPUT);

  if (!mp3_player.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  SD.begin(MP3_CARDCS);
  mp3_player.setVolume(0, 0);
  mp3_player.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  mp3_player.dumpRegs();
}

bool first = true;

void loop() {
  //loop iteration starts
  step_start_time = millis();
  //State transitions
  bool state_switched = false;
  switch (cpr_step) {
    case initial_state:
      cpr_step = call_help;
      state_switched = true;
      break;
    case call_help:
      if (digitalRead(nextPin) == HIGH 
      || (millis() - state_start_time > 1000 && mp3_player.stopped())) {
        cpr_step = instruct_all;
        state_switched = true;
      }
      break;
    case instruct_all:
      if (millis() - state_start_time > 1000 && mp3_player.stopped()) {
        cpr_step = instruct_beat;
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
    first = true;
  }
  else {
    first = false;
  }

  //state actions
  switch (cpr_step) {
    case call_help:
      if (!first) break;
      //speak
      Serial.println("Help! The person is having a heart attack.");
      Serial.println("Please come over and press the button for CPR instruction.");
      Serial.println();
      mp3_player.startPlayingFile("1.mp3");
      break;
    case instruct_all:
      if (!first) break;
      Serial.println("instruct_all");
      mp3_player.startPlayingFile("2.mp3");
      break;
    case instruct_call_help:
      Serial.println("Call 911 before beginning CPR.");
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
  // Serial.println(step_period - (millis() - step_start_time));
  // delay(step_period - (millis() - step_start_time));
  delay(step_period);
}
