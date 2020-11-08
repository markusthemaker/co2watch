#ifndef _DUOLED_H_
#define _DUOLED_H_

//Dual color LED with common cathode (green / red)
//Connected to _pinGreen, _pinRed, and GND (use R=470 Ohm)
class DuoLED {
    byte _pinGreen;
    byte _pinRed;
    byte _brightness; // 0 (off) - 254 (full brightness)

  public:

    DuoLED(byte pinGreen, byte pinRed) {
      _pinGreen = pinGreen;
      _pinRed = pinRed;
      pinMode(_pinGreen, OUTPUT);
      pinMode(_pinRed, OUTPUT);
    }

    void red(byte brightness) {
      _brightness = brightness;
      red();
    }

    void green(byte brightness) {
      _brightness = brightness;
      green();
    }

    void red() {
      analogWrite(_pinRed, _brightness);
      analogWrite(_pinGreen, 0);
    }

    void green() {
      //adjust green brightness to match red
      analogWrite(_pinGreen, _brightness / 4);
      analogWrite(_pinRed, 0);
    }

    //empirical settings for day, evening, night
    void setBrightness(DayPeriod period) {
      switch (period) {
        case (DayPeriod::NIGHT): _brightness = 10; break;
        case (DayPeriod::EVENING): _brightness = 100; break;
        case (DayPeriod::DAY): _brightness = 200;
      }
    }
};

#endif //_DUOLED_H_
