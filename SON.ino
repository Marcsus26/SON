#include <Audio.h>
#include <Bounce.h>
#include "capture.h"

capture capture;
AudioInputI2S in;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;
AudioConnection patchCord0(capture,0,out,0);
AudioConnection patchCord1(capture,0,out,1);

Bounce boutonCapture = Bounce(0, 10);

void setup() {
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP);
  AudioMemory(6);
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.micGain(10); // in dB
  audioShield.volume(1);
}

void loop() {
  // A. On met à jour l'état du bouton physique
  boutonCapture.update();

  // B. Si le bouton a changé d'état (Appuyé ou Relâché)
  if (boutonCapture.fallingEdge()) {
    // Falling Edge = Passage de 3.3V à 0V = APPUI
    // On envoie "1.0" au paramètre Faust
    capture.setParamValue("Capture", 1.0);
    Serial.println("Capture ON");
  }
  
  if (boutonCapture.risingEdge()) {
    // Rising Edge = Passage de 0V à 3.3V = RELACHÉ
    // On envoie "0.0" au paramètre Faust
    capture.setParamValue("Capture", 0.0);
    Serial.println("Capture OFF");
  }
  
  // Note: setParamValue prend en 1er argument le "Chemin" du widget Faust

}
