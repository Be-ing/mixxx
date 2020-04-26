#pragma once
#include "control/controlproxy.h"
#include "effects/backends/effectprocessor.h"
#include "engine/effects/engineeffect.h"
#include "engine/effects/engineeffectparameter.h"
#include "engine/filters/enginefilterbiquad1.h"
#include "util/class.h"
#include "util/defs.h"
#include "util/sample.h"
#include "util/types.h"
#include "util/memory.h"

class LoudnessContourEffectGroupState final : public EffectState {
  public:
    LoudnessContourEffectGroupState(const mixxx::EngineParameters& bufferParameters);
    ~LoudnessContourEffectGroupState();

    void setFilters(int sampleRate, double gain);

    std::unique_ptr<EngineFilterBiquad1Peaking> m_low;
    std::unique_ptr<EngineFilterBiquad1HighShelving> m_high;
    CSAMPLE* m_pBuf;
    double m_oldGainKnob;
    double m_oldLoudness;
    double m_oldGain;
    double m_oldFilterGainDb;
    bool m_oldUseGain;
    unsigned int m_oldSampleRate;
};

class LoudnessContourEffect
        : public EffectProcessorImpl<LoudnessContourEffectGroupState> {
  public:
    LoudnessContourEffect() {};
    ~LoudnessContourEffect() override;

    static QString getId();
    static EffectManifestPointer getManifest();

    void loadEngineEffectParameters(
            const QMap<QString, EngineEffectParameterPointer>& parameters) override;

    void processChannel(
            LoudnessContourEffectGroupState* pState,
            const CSAMPLE* pInput, CSAMPLE *pOutput,
            const mixxx::EngineParameters& bufferParameters,
            const EffectEnableState enableState,
            const GroupFeatureState& groupFeatureState) override;

    void setFilters(int sampleRate);

  private:
    LoudnessContourEffect(const LoudnessContourEffect&) = delete;
    void operator=(const LoudnessContourEffect&) = delete;

    QString debugString() const {
        return getId();
    }

    EngineEffectParameterPointer m_pLoudness;
    EngineEffectParameterPointer m_pUseGain;
};