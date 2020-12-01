#pragma once

#include <QMap>

#include "effects/backends/builtin/builtinbackend.h"
#include "effects/backends/effectprocessor.h"
#include "engine/effects/engineeffect.h"
#include "engine/effects/engineeffectparameter.h"
#include "util/class.h"
#include "util/types.h"

struct BitCrusherGroupState : public EffectState {
    // Default accumulator to 1 so we immediately pick an input value.
    BitCrusherGroupState(const mixxx::EngineParameters& bufferParameters)
            : EffectState(bufferParameters),
              hold_l(0),
              hold_r(0),
              accumulator(1) {
    }
    CSAMPLE hold_l, hold_r;
    // Accumulated fractions of a samplerate period.
    CSAMPLE accumulator;
};

class BitCrusherEffect : public EffectProcessorImpl<BitCrusherGroupState> {
  public:
    BitCrusherEffect() = default;
    virtual ~BitCrusherEffect();

    static QString getId();
    static EffectManifestPointer getManifest();

    void loadEngineEffectParameters(
            const QMap<QString, EngineEffectParameterPointer>& parameters) override;

    void processChannel(
            BitCrusherGroupState* pState,
            const CSAMPLE* pInput,
            CSAMPLE* pOutput,
            const mixxx::EngineParameters& bufferParameters,
            const EffectEnableState enableState,
            const GroupFeatureState& groupFeatureState) override;

  private:
    QString debugString() const {
        return getId();
    }

    EngineEffectParameterPointer m_pBitDepthParameter;
    EngineEffectParameterPointer m_pDownsampleParameter;

    DISALLOW_COPY_AND_ASSIGN(BitCrusherEffect);
};

namespace {
bool whydoesthisneedtoexist = BuiltInBackend::registerEffect<BitCrusherEffect>();
}
