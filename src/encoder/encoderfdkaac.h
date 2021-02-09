#pragma once

#include <fdk-aac/aacenc_lib.h>

#include <QString>
#include <memory>

#include "encoder/encoder.h"
#include "util/fifo.h"

class EncoderFdkAac : public Encoder {
  public:
    EncoderFdkAac(EncoderCallback* pCallback);
    virtual ~EncoderFdkAac();

    int initEncoder(int samplerate, QString* pUserErrorMessage) override;
    void encodeBuffer(const CSAMPLE* samples, const int sampleCount) override;
    void updateMetaData(const QString& artist, const QString& title, const QString& album) override;
    void flush() override;
    void setEncoderSettings(const EncoderSettings& settings) override;

  private:
    void processFIFO();

    int m_aacAot;
    int m_bitrate;
    int m_channels;
    int m_samplerate;
    EncoderCallback* m_pCallback;
    FIFO<SAMPLE>* m_pInputFifo;
    SAMPLE* m_pFifoChunkBuffer;
    int m_readRequired;
    HANDLE_AACENCODER m_aacEnc;
    unsigned char* m_pAacDataBuffer;
    AACENC_InfoStruct m_aacInfo;
    bool m_hasSbr;
};
