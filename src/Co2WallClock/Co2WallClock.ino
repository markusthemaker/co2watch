#include <SparkFun_SCD30_Arduino_Library.h>
#include <Wire.h>

#include "Buzzer.h"
#include "Timer.h"
#include "LDR.h"
#include "LEDRing.h"
#include "DuoLED.h"

//PINS
const byte PIN_BUZZER = 13;
const byte PIN_LDR = A7;
const byte PIN_LEDRING = 4;
const byte PIN_BUTTON = 2;  //Use a pin that works for interrupts
const byte PIN_RED_LED = 10;
const byte PIN_GREEN_LED = 9;

//CO2 concentration is affected by altitude
//Altitude compensation for CO2 measurement - update for your setting
const word altitude = 250;
//Force calibration based on a reliable and known reference; only use if better than factory default
const word refCO2Value = 0; //0:No calibration; 400-2000: use trusted reference value

//Setting a flag when inerrupt is detected is an effective way to debounce a button
volatile bool buttonInterrupt = false;

//Buzzer is active if true
bool buzzerOn = true;

//Keep track of last CO2 readout
//Not every loop cylce updates CO2 - sensor delivery new data only every 2s
word co2 = 0;

Timer timer;
SCD30 sensor;
LDR ldr(PIN_LDR);
Buzzer buzzer(PIN_BUZZER);
DuoLED duoLed(PIN_RED_LED, PIN_GREEN_LED);
LEDRing ledRing(24, PIN_LEDRING, 65536 * 2 / 3); //Hue is a 16 bit number; value = blue

//Arduino specific - initializes and sets the initial values
void setup() {
  Serial.begin(9600);

  //Show LED0 for 1s
  ledRing.setBrightness(DayPeriod::DAY);
  ledRing.setLed(0);
  delay(1000);

  //Button as interrupt
  digitalWrite(PIN_BUTTON, HIGH); //pullup
  //Button pulls to GND - detect falling edge as interrupt
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), interruptDetected, LOW);

  //CO2 sensor - I2C bus begin
  Wire.begin();
  //Sensor begin
  if (!sensor.begin(Wire, false)) {
    Serial.println("SCD30 init failed. Freezing...");
    while (true);
  }

  //Relevant when used at alitude
  sensor.setAltitudeCompensation(altitude); //Set altitude of the sensor in m
  
  //Force Calibration based on a known reference value; only use if better than factory default
  if(refCO2Value >= 400) {
    delay(120000); // 2 min CO2 sensor warm-up
    sensor.setForcedRecalibrationFactor(refCO2Value);
    Serial.println("Calibration completed; disable calibration with refCO2Value=0 and re-flash Arduino");
    while (true);
  }
}

//Used by attachInterrupt() to track status
void interruptDetected() {
  Serial.println("button interrupt");
  buttonInterrupt = true;
}

void loop() {

  //Due, Zero and MKR boards have 12-bit ADC capabilities. Set to 10-bits to work with LDR class
  //analogReadResolution(10); //default to 10 bit resolution

  //Get ambient light set LEDs to corresponding brightness (day/evening/night)
  DayPeriod dp = ldr.getState();
  ledRing.setBrightness(dp);
  duoLed.setBrightness(dp);

  //Change status if button was pressed and update status buzzer LED
  if (buttonInterrupt) {
    buzzerOn = !buzzerOn; //change status if button pressed
    delay(100); //debounce button for at least 50-100ms, otherwise it might trigger several times
    buttonInterrupt = false; //reset interrupt
    buzzer.buzz();
    Serial.print("Buzzer: "); Serial.println(buzzerOn);
  }
  //Update LED status; outside so respective LED active even without any interrupts
  (buzzerOn == true) ? duoLed.green() : duoLed.red();

  //get co2 sensor data, range ristrict it, active corresponding LED
  if (sensor.dataAvailable()) {
    co2 = sensor.getCO2();
    Serial.print("CO2(ppm):"); Serial.println(co2);

    //limit co2 to available range on watch face
    if (co2 > 1750) co2 = 1750;
    if (co2 < 600) co2 = 600;

    //Map co2 onto 24 LED range
    byte activeLed = map(co2, 600, 1750, 0, 23);

    //Update led brightness
    ledRing.setLed(activeLed);
  }

  //Set buzzer timer and set number of beeps
  if (buzzerOn) {
    byte numberBeeps = 0;
    if (co2 < 1000) {
      //If CO2 concentration falls below alarm value, we reset the timer
      timer.stopTimer();
    } else if (co2 < 1250) {
      timer.startTimer(60000); //warn every 60 seconds
      numberBeeps = 1;
    } else if (co2 < 1500) {
      timer.startTimer(30000); //warn every 30 seconds
      numberBeeps = 2;
    } else {
      timer.startTimer(15000); //earn every 15 seconds
      numberBeeps = 3;
    }

    //Activate buzzer and reset timer
    if (timer.isTimerDone()) {
      buzzer.buzz(numberBeeps);
      timer.stopTimer(); //start counting from scratch
    }
  }

  delay(1000);
}
