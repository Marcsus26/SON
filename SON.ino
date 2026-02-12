#include <Audio.h>
#include <Bounce.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "SDRecRead.h"
#include "steganophone.h"

SDRecRead sDRecRead;
steganophone steganophone; 
AudioInputI2S in;
AudioOutputI2S out;
AudioControlSGTL5000 audioShield;    
AudioConnection patchCord1(in, 0, sDRecRead.queue1, 0);
AudioConnection patchCord2(in, 0, sDRecRead.peak1, 0);
AudioConnection patchCord3(sDRecRead.playWav2, 0, steganophone, 0);
//AudioConnection patchCord4(sDRecRead.playWav2, 1, steganophone, 0);
AudioConnection patchCord5(sDRecRead.playWav1,   0, steganophone, 1);
//AudioConnection patchCord6(sDRecRead.playWav1,   1, steganophone, 1);
AudioConnection patchCord7(steganophone, 0, sDRecRead.queueOutput, 0);

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11   // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN   13  // Teensy 4 ignores this, uses pin 13

Bounce boutonCapture = Bounce(0, 10);

void setup() {
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP);
  Serial.println("Hello");
  AudioMemory(200);
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.micGain(10); // in dB
  audioShield.volume(1);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void loop() {
  // A. On met à jour l'état du bouton physique
  boutonCapture.update();

  // Respond to button presses
  if (boutonCapture.fallingEdge()) {
    if (sDRecRead.mode == 0) {
      sDRecRead.playWav2.stop();
      Serial.println("Started recording");
      sDRecRead.startRecording();
    }
    else { 
      if (sDRecRead.mode == 1) {
        Serial.println("Stopped recording");
        sDRecRead.stopRecording();
        delay(100);
        sDRecRead.startEncoding();
    }
    }
    }
    if (sDRecRead.mode == 1) {
      sDRecRead.continueRecording();
  }
  if (sDRecRead.isEncoding) {

    if (sDRecRead.queueOutput.available() >= 4) {
        byte tempBuffer[1024]; 

        for (int i = 0; i < 4; i++) {
            memcpy(tempBuffer + (i * 256), sDRecRead.queueOutput.readBuffer(), 256);
            sDRecRead.queueOutput.freeBuffer();
        }

        // On écrit et on vérifie l'usage mémoire
        sDRecRead.fileStego.write(tempBuffer, 1024);
        }
      // 2. Vérification de la fin (DÉPLACÉ ICI)
      // On vérifie si les fichiers ont fini de jouer UNIQUEMENT si on est en train d'encoder
      if (!sDRecRead.playWav2.isPlaying() || !sDRecRead.playWav1.isPlaying()) {
          sDRecRead.playWav1.stop();
          delay(100);
          sDRecRead.playWav2.stop();
        Serial.println("Fin de lecture détectée, fermeture du fichier.");
        AudioNoInterrupts();
        sDRecRead.stopEncoding(); // Ici isEncoding repasse à false, arrêtant ce bloc
        AudioInterrupts();
        // PETIT DELAI DE SECURITE
        delay(200); 
        
        // RÉOUVERTURE POUR HEADER
        File f = SD.open("STEGO.WAV", FILE_WRITE);
        if (f) {
            Serial.println("Mise à jour du header WAV...");
            unsigned long fileSize = f.size();
            sDRecRead.writeWavHeader(f, fileSize); 
            f.close();
            Serial.println("Encodage terminé proprement !");
        }
      }
    }
  }