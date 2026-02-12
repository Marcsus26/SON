#ifndef SDRECREAD_H_INCLUDED
#define SDRECREAD_H_INCLUDED

#include <SdFat.h>
#include <Audio.h>

class SDRecRead{
  public:
    SDRecRead();
    AudioAnalyzePeak peak1;
    AudioRecordQueue queue1, queueOutput;       
    AudioPlaySdWav playWav1;
    AudioPlaySdWav playWav2;
    File frec, fileStego;
    int mode;
    bool isEncoding;
    void startRecording();
    void continueRecording();
    void stopRecording();
    void startPlaying();
    void continuePlaying();
    void writeWavHeader(File file, int fileSize);
    void startEncoding();
    void stopEncoding();
};
#endif