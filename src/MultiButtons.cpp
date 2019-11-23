/**
 * MultiButtons Library
 * Copyright (c) 2019 Mickey Chan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Developed for Arduino-ESP32
 * Created by Mickey Chan (developer AT comicparty.com)
 * 
 */
#include "Arduino.h"
#include "MultiButtons.h"

MultiButtons::MultiButtons(int pin, int buttonCount, int arrVoltageRanges[][2], 
  callback_t callback, int adcMax, int triggerEdge) {
  this->_pin = pin;
  
  this->_arrVoltageRanges = new int* [buttonCount];
  for (int i = 0; i < buttonCount; i++) {
    this->_arrVoltageRanges[i] = arrVoltageRanges[i];
  }
  
  this->_buttonCount = buttonCount;
  this->_callback = callback;
  this->_adcMax = adcMax;
  this->_triggerEdge = triggerEdge;
}

MultiButtons::~MultiButtons() {
  // release allocated memories
  for (int i = 0; i < this->_buttonCount; i++) {
    delete [] this->_arrVoltageRanges[i];
  }
  delete [] this->_arrVoltageRanges;
}

void MultiButtons::begin() {
  analogRead(this->_pin);
  delay(1);
}

void MultiButtons::loop() {
  unsigned long _curTime = millis();
  if (_curTime - this->_lastLoop >= this->_loopInterval) {
    this->_lastLoop = _curTime;
    this->_getButton();
  }
}

void MultiButtons::_getButton() {
  int _button;

  _button = this->_readButton();
  if (this->_readPhase == BTN_READ_PHASE_1ST_PASS && _button != this->_lastPressedBtn) {
    this->_prevBtn = _button;
    this->_readPhase = BTN_READ_PHASE_2ND_PASS;
  } else if (this->_readPhase == BTN_READ_PHASE_2ND_PASS) {
    if (_button == this->_prevBtn) {
      if (this->_triggerEdge == BTN_TRIGGER_EDGE_RELEASE && _button == -1) { // handle on release
        this->_callback(this, this->_lastPressedBtn);
      } else if (this->_triggerEdge == BTN_TRIGGER_EDGE_PRESS && _button > -1) {
        this->_callback(this, _button);
      }
      this->_lastPressedBtn = _button;
    }
    this->_readPhase = BTN_READ_PHASE_1ST_PASS; // anyway, reset read state
  }
}

int MultiButtons::_readButton () {
  int z, _sum;
  int _button = -1;
  
  _sum = 0;
  for (int i=0; i < 4; i++) {
    _sum += analogRead(this->_pin);
  }
  z = _sum / 4;

  // TODO: check ADC value with an array of buttons with value ranges
  if (z > this->_adcMax) _button = -1;
  else {
    for (int i = 0; i < this->_buttonCount; i++) {
      if (z > this->_arrVoltageRanges[i][0] && z < this->_arrVoltageRanges[i][1]) {
        _button = i;
        break;
      }
    }
  }

  return _button;
}

int MultiButtons::printReading(int pin) {
  int z;
  z = analogRead(pin);
  if (z > 100) Serial.println(z);
  return z;
}

bool MultiButtons::isPressingAny(int pin) {
  return (bool)analogRead(pin);
}

bool MultiButtons::isPressing() {
  int z;
  z = analogRead(this->_pin);
  if (z > 100) {
    return true;
  }
  else return false;
}

bool MultiButtons::isPressing(int btnIndex) {
  int z;
  z = analogRead(this->_pin);
  if (z > this->_arrVoltageRanges[btnIndex][0] && z < this->_arrVoltageRanges[btnIndex][1]) {
    return true;
  }
  else return false;
}

int MultiButtons::getTriggerEdge() {
  return this->_triggerEdge;
}

bool MultiButtons::setTriggerEdge(int edge) {
  if (edge < BTN_TRIGGER_EDGE_PRESS || edge > BTN_TRIGGER_EDGE_RELEASE) {
    return false;
  }
  this->_triggerEdge = edge;
  return true;
}
