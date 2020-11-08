#ifndef _LDR_H_
#define _LDR_H_

//Enum to track day period: day, evening, and night
enum class DayPeriod { DAY, EVENING, NIGHT };

//Reads LDR sensor to determine if it is day, evening, or night
class LDR {
    byte _pin;

    //Voltage divider R1 V1
    const unsigned int _v1 = 1023; //5V at 10bit
    const unsigned int _r1 = 10; //kOhm

    //Voltage divider R2 (empirical values)
    //LDR Type GL5528
    //Datasheet https://cdn.sparkfun.com/datasheets/Sensors/LightImaging/SEN-09088.pdf
    const unsigned int _LDR_day = 2; //kOhm
    const unsigned int _LDR_evening = 10; //kOhm
    const unsigned int _LDR_night = 50; //kOhm

  public:

    LDR(byte pin) {
      _pin = pin;
      pinMode(_pin, INPUT);
    }

    DayPeriod getState() {
      unsigned int r2 = findR2();
      if (r2 < _LDR_day) return DayPeriod::DAY;
      if (r2 < _LDR_evening) return DayPeriod::EVENING;
      return DayPeriod::NIGHT;
    }

  private:

    unsigned int readVoltage() {
      return analogRead(_pin);
    }

    unsigned int findR2() {
      //Voltage Divider: R2 = (V2 x R1) / (V1 - V2)
      unsigned int v2 = readVoltage();
      return (v2 * _r1) / (_v1 - v2); //return R2 in kOhm (as int)
    }
};

#endif // _LDR_H_
