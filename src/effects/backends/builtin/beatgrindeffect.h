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
    }

    void audioParametersChanged(const mixxx::EngineParameters bufferParameters) {
        delay_buf = mixxx::SampleBuffer(kMaxDelaySeconds
                * bufferParameters.sampleRate() * bufferParameters.channelCount());
        bufferedSample.reserve(bufferParameters.channelCount());
    };

    void clear() {
        delay_buf.clear();
        prev_send = 0.0f;
        prev_feedback= 0.0f;
        write_position = 0;
    };

    mixxx::SampleBuffer delay_buf;
    std::vector<CSAMPLE> bufferedSample;
    CSAMPLE_GAIN prev_send;
    CSAMPLE_GAIN prev_feedback;
    int prev_delay_samples;
    int read_position;
    int write_position;
    bool isRecording;
    unsigned int currentFrame;
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
    EngineEffectParameterPointer m_pQuantizeParameter;
    EngineEffectParameterPointer m_pDelayParameter;
    EngineEffectParameterPointer m_pSendParameter;
    EngineEffectParameterPointer m_pFeedbackParameter;
    EngineEffectParameterPointer m_pTripletParameter;

    DISALLOW_COPY_AND_ASSIGN(BeatGrindEffect);
};
