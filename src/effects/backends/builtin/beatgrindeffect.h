#pragma once

#include <QMap>

#include "effects/backends/effectprocessor.h"
#include "engine/engine.h"
#include "engine/effects/engineeffect.h"
#include "engine/effects/engineeffectparameter.h"
#include "util/class.h"
#include "util/defs.h"
#include "util/sample.h"
#include "util/samplebuffer.h"

class BeatGrindState : public EffectState {
  public:
    // 3 seconds max. This supports the full range of 2 beats for tempos down to
    // 40 BPM.
    static constexpr int kMaxDelaySeconds = 3;

    BeatGrindState(const mixxx::EngineParameters bufferParameters)
           : EffectState(bufferParameters) {
        audioParametersChanged(bufferParameters);
       clear();
       mixPrev = 0;
       startPos = 0;
       selectedLoop = loop.data();
    }

    void audioParametersChanged(const mixxx::EngineParameters bufferParameters) {
        loop = mixxx::SampleBuffer(kMaxDelaySeconds
                * bufferParameters.sampleRate() * bufferParameters.channelCount());
        reverseLoop = mixxx::SampleBuffer(kMaxDelaySeconds
                * bufferParameters.sampleRate() * bufferParameters.channelCount());
    };

    void clear() {
        writeSamplePos = 0;
        readSamplePos = 0;
        isRecording = true;
    };

    mixxx::SampleBuffer loop;
    mixxx::SampleBuffer reverseLoop;
    CSAMPLE* selectedLoop;
    int readSamplePos;
    int writeSamplePos;
    int startPos;
    int endPos;
    bool isRecording;
    double mixPrev;
};

class BeatGrindEffect : public EffectProcessorImpl<BeatGrindState> {
  public:
    BeatGrindEffect() {};

    static QString getId();
    static EffectManifestPointer getManifest();

    void loadEngineEffectParameters(
            const QMap<QString, EngineEffectParameterPointer>& parameters) override;

    void processChannel(
            BeatGrindState* pState,
            const CSAMPLE* pInput, CSAMPLE* pOutput,
            const mixxx::EngineParameters& bufferParameters,
            const EffectEnableState enableState,
            const GroupFeatureState& groupFeatures) override;

  private:
    QString debugString() const {
        return getId();
    }
    EngineEffectParameterPointer m_pLengthParameter;
    EngineEffectParameterPointer m_pMixParameter;
    EngineEffectParameterPointer m_pTripletParameter;
    EngineEffectParameterPointer m_pQuantizeParameter;
    EngineEffectParameterPointer m_pReverseParameter;
    DISALLOW_COPY_AND_ASSIGN(BeatGrindEffect);
};
