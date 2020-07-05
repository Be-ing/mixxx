#include "effects/backends/builtin/noiseeffect.h"

#include "effects/backends/effectmanifest.h"

// static
QString NoiseEffect::getId() {
    return "org.mixxx.effects.noise";
}

// static
EffectManifestPointer NoiseEffect::getManifest() {
    EffectManifestPointer pManifest(new EffectManifest());
    pManifest->setId(getId());
    pManifest->setName(QObject::tr("Noise"));
    pManifest->setShortName(QObject::tr("Noise"));
    pManifest->setAuthor("The Mixxx Team");
    pManifest->setVersion("1.0");
    pManifest->setDescription(QObject::tr(
            "Adds noise"));
    pManifest->setEffectRampsFromDry(true);
    pManifest->setMetaknobDefault(0.0);

    EffectManifestParameterPointer depth = pManifest->addParameter();
    depth->setId("depth");
    depth->setName(QObject::tr("Depth"));
    depth->setShortName(QObject::tr("Depth"));
    depth->setDescription(QObject::tr(
            "Depth"));
    depth->setValueScaler(EffectManifestParameter::ValueScaler::LOGARITHMIC);
    depth->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    depth->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    depth->setDefaultLinkType(EffectManifestParameter::LinkType::LINKED);
    depth->setRange(0, 0, 1);

    return pManifest;
}

void NoiseEffect::loadEngineEffectParameters(
        const QMap<QString, EngineEffectParameterPointer>& parameters) {
    m_pDepthParameter = parameters.value("depth");
}

void NoiseEffect::processChannel(
        NoiseGroupState* pState,
        const CSAMPLE* pInput,
        CSAMPLE* pOutput,
        const mixxx::EngineParameters& bufferParameters,
        const EffectEnableState enableState,
        const GroupFeatureState& groupFeatures) {
    Q_UNUSED(pState);
    Q_UNUSED(groupFeatures);
    Q_UNUSED(enableState);

    const CSAMPLE depth = m_pDepthParameter->value();

    for (unsigned int i = 0; i < bufferParameters.samplesPerBuffer(); i++) {
        int sign = (pInput[i] >= 0) ? 1 : -1;
        pOutput[i] = sign * depth - pInput[i];
    }
}
