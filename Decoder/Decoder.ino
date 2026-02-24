#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// --- 1. INCLURE TON DÉCODEUR FAUST ---
// Assure-toi que c'est bien le code du DÉCODEUR et pas de l'encodeur !
#include "SteganoDecoder.h" 

// --- 2. PINS SPÉCIFIQUES TEENSY 4.0 ---
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11   // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN   13  // Teensy 4 ignores this, uses pin 13

// --- 3. OBJETS AUDIO ---
//AudioPlaySdWav           playStego;      // Lecteur du fichier crypté
//AudioRecordQueue         queueOutput;    // Enregistreur du signal décodé
AudioOutputI2S           i2s_out;        // Monitoring casque
AudioInputI2S in;
AudioControlSGTL5000     audioShield;

// Ton objet Faust (Vérifie le nom de la classe dans ton fichier decoder.h)
// Si tu as gardé "mydsp", remplace "decoder" par "mydsp".
SteganoDecoder monDecodeur;    

// --- 4. CÂBLAGE (PATCHING) ---

// A. Lecture -> Décodeur
// On envoie le fichier STEGO dans le décodeur
AudioConnection          patch1(in, 0, monDecodeur, 0);

// B. Décodeur -> Enregistreur
// On enregistre le résultat (la voix extraite)
AudioConnection          patch2(monDecodeur, 0, i2s_out, 0);
AudioConnection          patch3(monDecodeur, 0, i2s_out, 1);

// --- VARIABLES ---
File fileDecoded;
bool isDecoding = false;

void setup() {
  Serial.begin(9600);
  AudioMemory(60); // Mémoire tampon confortable

  // Init Audio Shield
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_MIC);
  audioShield.micGain(30);
  audioShield.volume(0.93); // On met fort car le signal décodé est souvent faible

  Serial.println("Hello");

  // Init SD Card
  /*SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while(1) { Serial.println("Erreur Carte SD !"); delay(1000); }
  }
  
  Serial.println("Prêt au décodage.");
  delay(100);
  
  startDecoding();*/
}

void loop() {
  /*if (isDecoding) {
    // 1. Écriture continue sur la SD
    if (queueOutput.available() >= 2) {
      byte buffer[512];
      memcpy(buffer, queueOutput.readBuffer(), 256);
      queueOutput.freeBuffer();
      memcpy(buffer + 256, queueOutput.readBuffer(), 256);
      queueOutput.freeBuffer();
      AudioNoInterrupts();
      fileDecoded.write(buffer, 512);
      AudioInterrupts();
    }

    // 2. Arrêt automatique à la fin du fichier source
    if (!playStego.isPlaying()) {
      stopDecoding();
    }
  }*/
}

// --- FONCTIONS DE PILOTAGE ---

/*void startDecoding() {
  Serial.println("Début du décodage vers DECODED.WAV...");
  
  // Nettoyage ancien fichier
  if (SD.exists("DECODED.WAV")) SD.remove("DECODED.WAV");
  
  fileDecoded = SD.open("DECODED.WAV", FILE_WRITE);
  
  if (fileDecoded) {
    // Réserver l'espace pour le header WAV
    for (int i = 0; i < 44; i++) fileDecoded.write(0);
    
    queueOutput.begin();
    
    // Lancer la lecture du fichier source
    playStego.play("STEGO.WAV");
    delay(10);
    isDecoding = true;
  } else {
    Serial.println("Impossible de créer le fichier DECODED.WAV");
  }
}

void stopDecoding() {
  isDecoding = false;
  queueOutput.end();
  
  // Vider les tampons restants
  while (queueOutput.available() > 0) {
    fileDecoded.write((byte*)queueOutput.readBuffer(), 256);
    queueOutput.freeBuffer();
  }
  
  // Écrire l'en-tête WAV final (Taille réelle)
  unsigned long fileSize = fileDecoded.size();
  writeWavHeader(fileDecoded, fileSize);
  
  fileDecoded.close();
  Serial.println("Décodage terminé !");
  Serial.println("Le secret est dans DECODED.WAV");
}

// Fonction utilitaire pour créer un WAV valide (Mono 44.1kHz 16bit)
void writeWavHeader(File file, unsigned long fileSize) {
  unsigned long sampleRate = 44100;
  unsigned short numChannels = 1; // Mono
  unsigned short bitsPerSample = 16;
  unsigned long byteRate = sampleRate * numChannels * (bitsPerSample / 8);
  unsigned short blockAlign = numChannels * (bitsPerSample / 8);
  unsigned long subChunk2Size = fileSize - 44;
  unsigned long chunkSize = 36 + subChunk2Size;

  file.seek(0);
  file.print("RIFF");
  file.write((byte*)&chunkSize, 4);
  file.print("WAVE");
  file.print("fmt ");
  
  unsigned long subChunk1Size = 16;
  unsigned short audioFormat = 1; 
  
  file.write((byte*)&subChunk1Size, 4);
  file.write((byte*)&audioFormat, 2);
  file.write((byte*)&numChannels, 2);
  file.write((byte*)&sampleRate, 4);
  file.write((byte*)&byteRate, 4);
  file.write((byte*)&blockAlign, 2);
  file.write((byte*)&bitsPerSample, 2);
  file.print("data");
  file.write((byte*)&subChunk2Size, 4);
}*/