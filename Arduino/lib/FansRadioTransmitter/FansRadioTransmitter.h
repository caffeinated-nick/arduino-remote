#ifndef FANS_RADIO_TRANSMITTER_H
#define FANS_RADIO_TRANSMITTER_H

#include <RCSwitch.h>
#include <Arduino.h>

class FansRadioTransmitter
{
private:
  RCSwitch rcSwitch = RCSwitch();
  bool debugMode = false;

  /** TV Light States, Lounge Light States (see "Fan RF Signals.csv" for more info)
   * TV:
   *  Light off, fan 0-3: 229389, 229472, 229455, 229422
   *  Light on, fan 0-3: 229505, 229604, 229571, 229538
   * Lounge:
   *  Light off, fan 0-3: 245775, 245858, 245825, 245792
   *  Light on, fan 0-3: 245891, 245990, 245957, 245924
   */
  const int states[2][8] = {{229389, 229472, 229455, 229422, 229505, 229604, 229571, 229538}, {245775, 245858, 245825, 245792, 245891, 245990, 245957, 245924}};
  const int tvIndex = 0;
  const int loungeIndex = 1;

  // This int represents which index in states[][] is currently set.
  // e.g. if tvSwitchState is 4 then we send signal[0][4], or 229505
  int tvSwitchState = 0;
  int loungeSwitchState = 0;

  void log(char message[])
  {
    if (debugMode)
    {
      Serial.println(message);
    }
  }

  /**
   *  Send a generic (decimal number) signal to the fan.
   */
  void sendSignal(int signal)
  {
    log("Sending one signal!");
    // Serial.printf("Sending signal %d...", signal);
    for (int i = 0; i < 3; i++)
    {
      // Serial.printf("Attempt %d...", i);
      rcSwitch.send(signal, 21);
      delay(10);
    }
    log("");
  }

  /**
   * Update the light state for a switch, return the new state.
   * If the state is high (4-7) then the light is on already, so turn it off.
   */
  int updateLightState(int switchIndex, int switchState)
  {
    const int newState = getNewLightState(switchState);
    log("");
    // Serial.printf("Setting switch %d to %d...", switchIndex, newState);
    log("");
    updateSingleState(switchIndex, newState);
    return newState;
  }

  int getNewLightState(int currentLightState)
  {
    if (currentLightState < 4)
    {
      currentLightState = currentLightState + 4;
    }
    else
    {
      currentLightState = currentLightState - 4;
    }
    return currentLightState;
  }

  /**
   * Update the fan state for a switch, return the new state.
   * If the state is 3 or 7 then the fan is at max speed, so set back down to 0/off.
   */
  int updateFanState(int switchIndex, int switchState)
  {
    const int newState = getNewFanState(switchState);
    log("");
    // Serial.printf("Setting switch %d to %d", switchIndex, switchState);
    log("");
    updateSingleState(switchIndex, switchState);
    return switchState;
  }

  int getNewFanState(int currentFanState)
  {
    if (currentFanState == 3 || currentFanState == 7)
    {
      currentFanState = currentFanState - 3;
    }
    else
    {
      currentFanState = currentFanState + 1;
    }
    return currentFanState;
  }

  /**
   *  Update the state of the FAN itself (i.e. send the updated state to the fan)
   */
  void updateSingleState(int index, int switchState)
  {
    sendSignal(states[index][switchState]);
  }

public:
  FansRadioTransmitter(int dataPin, bool debug = false)
  {
    debugMode = debug;
    // Transmitter is connected to Arduino Pin ${dataPin}
    rcSwitch.enableTransmit(dataPin);
    // Optional set protocol (default is 1, will work for most outlets)
    rcSwitch.setProtocol(6);
    // Optional set number of transmission repetitions.
    rcSwitch.setRepeatTransmit(15);
  }

  void incrementLoungeFan()
  {
    loungeSwitchState = updateFanState(loungeIndex, loungeSwitchState);
  }

  void toggleLoungeLight()
  {
    loungeSwitchState = updateLightState(loungeIndex, loungeSwitchState);
  }

  int getLoungeState()
  {
    return loungeSwitchState;
  }

  void incrementTVFan()
  {
    tvSwitchState = updateFanState(tvIndex, tvSwitchState);
  }

  void toggleTVLight()
  {
    tvSwitchState = updateLightState(tvIndex, tvSwitchState);
  }

  int getTVState()
  {
    return tvSwitchState;
  }
};

#endif