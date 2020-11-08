#ifndef _BUZZER_H_
#define _BUZZER_H_

//Drives classic piezo buzzer connected to _pin (+) and GND (-)
class Buzzer {
    byte _pin;
    byte _numberOfBeeps = 1;

    const byte _freq = 1000;   //Hz
    const byte _duration = 50; //ms

  public:

    Buzzer(byte pin) {
      _pin = pin;
      pinMode(_pin, OUTPUT);
    }

    void buzz() {
      tone(_pin, _freq);
      delay(_duration);
      noTone(_pin);
    }

    void buzz(byte numberOfBeeps) {
      _numberOfBeeps = numberOfBeeps;
      for (byte i = 0; i < _numberOfBeeps; i++) {
        buzz();
        delay(500);
      }
    }
};
#endif //_BUZZER_H_
