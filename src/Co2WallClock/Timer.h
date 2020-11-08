#ifndef _TIMER_H_
#define _TIMER_H_

class Timer {
    unsigned long _timerStartMs = 0;
    unsigned long _timerTargetMs = 0;

    //reset flag resets timer
    bool _reset = true;

  public:

    void startTimer(unsigned long msec) {
      if (_reset) {
        _timerStartMs = millis();
        _reset = false;
      }
      _timerTargetMs = msec;
    }

    bool isTimerDone() {
      unsigned long timeSinceStart = millis() - _timerStartMs; //time since start
      return timeSinceStart > _timerTargetMs;
    }

    void stopTimer() {
      _reset = true;
    }
};

#endif //_TIMER_H_
