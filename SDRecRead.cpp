#include "SDRecRead.h"

SDRecRead::SDRecRead() :
mode(0) {  isEncoding = false;}

void SDRecRead::writeWavHeader(File file, int fileSize) {
  // Un en-tête WAV fait 44 octets. 
  // Il faut le remplir byte par byte ou via un buffer.
  
  int sampleRate = 44100;
  int numChannels = 1; // Mono
  int bitsPerSample = 16;
  int byteRate = sampleRate * numChannels * (bitsPerSample / 8);
  int blockAlign = numChannels * (bitsPerSample / 8);
  int subChunk2Size = fileSize - 44; // Taille des données audio pure
  int chunkSize = 36 + subChunk2Size;

  file.seek(0); // On revient au début du fichier
  
  file.print("RIFF");
  file.write((byte*)&chunkSize, 4);
  file.print("WAVE");
  file.print("fmt ");
  
  long subChunk1Size = 16;
  short audioFormat = 1; // PCM
  
  file.write((byte*)&subChunk1Size, 4);
  file.write((byte*)&audioFormat, 2);
  file.write((byte*)&numChannels, 2);
  file.write((byte*)&sampleRate, 4);
  file.write((byte*)&byteRate, 4);
  file.write((byte*)&blockAlign, 2);
  file.write((byte*)&bitsPerSample, 2);
  file.print("data");
  file.write((byte*)&subChunk2Size, 4);
  
  // Pas besoin de refermer ici, on referme dans stopRecording
}

void SDRecRead::startRecording() {
  if (SD.exists("RECORD.WAV")) {
    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written.
    SD.remove("RECORD.WAV");
  }
  frec = SD.open("RECORD.WAV", FILE_WRITE);
  if (frec) {
    for(int i=0; i<44; i++) frec.write(0);
    queue1.begin();
    mode = 1;
  }
}

void SDRecRead::continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    //elapsedMicros usec = 0;
    frec.write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    //Serial.print("SD write, us=");
    //Serial.println(usec);
  }
}

void SDRecRead::stopRecording() {
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    unsigned long totalFileSize = frec.size();
    writeWavHeader(frec, totalFileSize);
    frec.close();
  }
  mode = 0;
}

void SDRecRead::continuePlaying() {
  if (!playWav2.isPlaying()) {
    playWav2.stop();
    Serial.println("Stopped Playing");
    mode = 0;
  }
}

void SDRecRead::startEncoding() {
  Serial.println("Tentative d'ouverture de STEGO.WAV...");
  
  if (SD.exists("STEGO.WAV")) SD.remove("STEGO.WAV");
  
  fileStego = SD.open("STEGO.WAV", FILE_WRITE);
  if (fileStego) {
    for (int i = 0; i < 44; i++) fileStego.write(0);
    
    // On attend un peu avant de lancer l'audio
    delay(50);
    queueOutput.begin();
    
    // On lance les fichiers un par un avec un petit délai
    if (playWav2.play("MARIO.WAV")) {
      Serial.println("Mario OK");
    }
    delay(100); // Laisse le temps au premier fichier de bufferiser
    
    if (playWav1.play("RECORD.WAV")) {
      Serial.println("Record OK");
    }
    delay(10);
    isEncoding = true;
  } else {
    Serial.println("Erreur fatale : Impossible d'ouvrir le fichier d'écriture !");
  }
}

void SDRecRead::stopEncoding() {
  isEncoding = false;
  queueOutput.end();
  
  // 1. On vide le reste
  while (queueOutput.available() > 0) {
    fileStego.write((byte*)queueOutput.readBuffer(), 256);
    queueOutput.freeBuffer();
  }
  playWav1.stop();
  delay(100);
  playWav2.stop();
  // 2. On ferme direct sans header pour l'instant
  fileStego.close();
  
  // 3. ON NE FAIT PAS LE HEADER ICI ! 
  // On le fera dans la loop après un petit délai
  Serial.println("Fichier brut fermé.");
}