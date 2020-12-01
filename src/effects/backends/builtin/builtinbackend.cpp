#include "effects/backends/builtin/builtinbackend.h"

#include <QtDebug>

#include "effects/backends/effectmanifest.h"
#include "effects/backends/effectprocessor.h"

BuiltInBackend* BuiltInBackend::get() {
    static BuiltInBackend instance;
    return &instance;
}

std::unique_ptr<EffectProcessor> BuiltInBackend::createProcessor(
        const EffectManifestPointer pManifest) const {
    VERIFY_OR_DEBUG_ASSERT(m_registeredEffects.contains(pManifest->id())) {
        return nullptr;
    }
    return m_registeredEffects[pManifest->id()].instantiator();
}

BuiltInBackend::~BuiltInBackend() {
    //qDebug() << debugString() << "destroyed";
    m_registeredEffects.clear();
    m_effectIds.clear();
}

void BuiltInBackend::registerEffect(
        const QString& id,
        EffectManifestPointer pManifest,
        EffectProcessorInstantiator instantiator) {
    VERIFY_OR_DEBUG_ASSERT(!m_registeredEffects.contains(id)) {
        return;
    }

    pManifest->setBackendType(getType());

    m_registeredEffects[id] = RegisteredEffect{pManifest, instantiator};
    m_effectIds.append(id);

    qDebug() << "================================================" << this
             << "registered effect" << id << pManifest << instantiator;
}

const QList<QString> BuiltInBackend::getEffectIds() const {
    return m_effectIds;
}

EffectManifestPointer BuiltInBackend::getManifest(const QString& effectId) const {
    VERIFY_OR_DEBUG_ASSERT(m_registeredEffects.contains(effectId)) {
        return EffectManifestPointer();
    }
    return m_registeredEffects.value(effectId).pManifest;
}

const QList<EffectManifestPointer> BuiltInBackend::getManifests() const {
    QList<EffectManifestPointer> list;
    for (const auto& registeredEffect : m_registeredEffects) {
        list.append(registeredEffect.pManifest);
    }
    return list;
}

bool BuiltInBackend::canInstantiateEffect(const QString& effectId) const {
    return m_registeredEffects.contains(effectId);
}
