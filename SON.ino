#include <Audio.h>
#include "capture.h"

capture capture;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(capture,0,out,0);
AudioConnection patchCord1(capture,0,out,1);

void setup() {
  AudioMemory(2);
  audioShield.enable();
  audioShield.volume(0.5);
  capture.setParamValue("gain", 1);
}

void loop() {

}
