#pragma once

#include "control/controlproxy.h"
#include "effects/backends/effectprocessor.h"
#include "engine/effects/engineeffect.h"
#include "engine/effects/engineeffectparameter.h"
#include "engine/filters/enginefilterbiquad1.h"
#include "engine/filters/enginefilterbessel4.h"
#include "effects/backends/builtin/lvmixeqbase.h"
#include "engine/filters/enginefilterdelay.h"
#include "util/class.h"
#include "util/defs.h"
#include "util/sample.h"
#include "util/types.h"
#include "util/memory.h"
#include "util/samplebuffer.h"

static const int kMaxDelay2 = 3300; // allows a 30 Hz filter at 97346;

class BiquadFullKillEQEffectGroupState : public EffectState {
  public:
    BiquadFullKillEQEffectGroupState(const mixxx::EngineParameters& bufferParameters);

    void setFilters(
            int sampleRate, double lowFreqCorner, double highFreqCorner);

    std::unique_ptr<EngineFilterBiquad1Peaking> m_lowBoost;
    std::unique_ptr<EngineFilterBiquad1Peaking> m_midBoost;
    std::unique_ptr<EngineFilterBiquad1Peaking> m_highBoost;
    std::unique_ptr<EngineFilterBiquad1LowShelving> m_lowKill;
    std::unique_ptr<EngineFilterBiquad1Peaking> m_midKill;
    std::unique_ptr<EngineFilterBiquad1HighShelving> m_highKill;
    std::unique_ptr<LVMixEQEffectGroupState<EngineFilterBessel4Low>> m_lvMixIso;

    mixxx::SampleBuffer m_pLowBuf;
    mixxx::SampleBuffer m_pBandBuf;
    mixxx::SampleBuffer m_pHighBuf;
    mixxx::SampleBuffer m_tempBuf;

    double m_oldLowBoost;
    double m_oldMidBoost;
    double m_oldHighBoost;
    double m_oldLowKill;
    double m_oldMidKill;
    double m_oldHighKill;
    double m_oldLow;
    double m_oldMid;
    double m_oldHigh;

    double m_loFreqCorner;
    double m_highFreqCorner;

    int m_rampHoldOff;
    int m_groupDelay;

    unsigned int m_oldSampleRate;
};

class BiquadFullKillEQEffect : public EffectProcessorImpl<BiquadFullKillEQEffectGroupState> {
  public:
    BiquadFullKillEQEffect();

    static QString getId();
    static EffectManifestPointer getManifest();

    void loadEngineEffectParameters(
            const QMap<QString, EngineEffectParameterPointer>& parameters) override;

    void processChannel(
            BiquadFullKillEQEffectGroupState* pState,
            const CSAMPLE* pInput, CSAMPLE *pOutput,
            const mixxx::EngineParameters& bufferParameters,
            const EffectEnableState enableState,
            const GroupFeatureState& groupFeatureState) override;

    void setFilters(int sampleRate, double lowFreqCorner, double highFreqCorner);

  private:
    BiquadFullKillEQEffect(const BiquadFullKillEQEffect&) = delete;
    void operator=(const BiquadFullKillEQEffect&) = delete;

    QString debugString() const {
        return getId();
    }

    EngineEffectParameterPointer m_pPotLow;
    EngineEffectParameterPointer m_pPotMid;
    EngineEffectParameterPointer m_pPotHigh;

    EngineEffectParameterPointer m_pKillLow;
    EngineEffectParameterPointer m_pKillMid;
    EngineEffectParameterPointer m_pKillHigh;

    std::unique_ptr<ControlProxy> m_pLoFreqCorner;
    std::unique_ptr<ControlProxy> m_pHiFreqCorner;
};