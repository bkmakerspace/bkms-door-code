#ifndef door_lock_h
#define door_lock_h

class DoorLock {
public:
  DoorLock(int pin);
  void reset();
  void unlockFor(long milliseconds);
private:
  int pin;
};

#endif
