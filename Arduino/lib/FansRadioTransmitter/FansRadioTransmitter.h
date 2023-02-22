#ifndef FANS_RADIO_TRANSMITTER_H
#define FANS_RADIO_TRANSMITTER_H

#include <RCSwitch.h>
#include <Arduino.h>

class FansRadioTransmitter
{
private:
  RCSwitch rcSwitch;
  bool debugMode;
  const int states[2][8];
  const int tvIndex;
  const int loungeIndex;
  int tvSwitchState;
  int loungeSwitchState;

  void log(char message[]);
  void sendSignal(int signal);
  void updateLightState(int switchIndex, int switchState);
  void updateFanState(int switchIndex, int switchState);
  int getNewLightState(int switchState);
  void updateSingleState(int index, int switchState);

public:
  FansRadioTransmitter(int dataPin);
  FansRadioTransmitter(int dataPin, bool debug);
  void incrementLoungeFan();
  void toggleLoungeLight();
  void incrementTVFan();
  void toggleTVLight();
};

#endif