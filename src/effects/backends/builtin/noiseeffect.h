#pragma once

#include "effects/backends/effectprocessor.h"
#include "engine/effects/engineeffectparameter.h"
#include "util/types.h"

// This effect requires no state.
struct NoiseGroupState : public EffectState {
    NoiseGroupState(const mixxx::EngineParameters& bufferParameters)
            : EffectState(bufferParameters) {
    }
};

class NoiseEffect : public EffectProcessorImpl<NoiseGroupState> {
  public:
    NoiseEffect(){};
    ~NoiseEffect(){};

    static QString getId();
    static EffectManifestPointer getManifest();

    void loadEngineEffectParameters(
            const QMap<QString, EngineEffectParameterPointer>& parameters) override;

    void processChannel(
            NoiseGroupState* pState,
            const CSAMPLE* pInput,
            CSAMPLE* pOutput,
            const mixxx::EngineParameters& bufferParameters,
            const EffectEnableState enableState,
            const GroupFeatureState& groupFeatureState) override;

  private:
    QString debugString() const {
        return getId();
    }

    EngineEffectParameterPointer m_pDepthParameter;

    DISALLOW_COPY_AND_ASSIGN(NoiseEffect);
};
