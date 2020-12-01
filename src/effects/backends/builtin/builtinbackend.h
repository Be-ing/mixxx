#pragma once

#include "effects/backends/effectsbackend.h"
#include "effects/defs.h"

/// Refer to EffectsBackend for documentation
class BuiltInBackend : public EffectsBackend {
  public:
    static BuiltInBackend* get();
    virtual ~BuiltInBackend();

    EffectBackendType getType() const {
        return EffectBackendType::BuiltIn;
    };

    const QList<QString> getEffectIds() const;
    EffectManifestPointer getManifest(const QString& effectId) const;
    const QList<EffectManifestPointer> getManifests() const;
    std::unique_ptr<EffectProcessor> createProcessor(
            const EffectManifestPointer pManifest) const;
    bool canInstantiateEffect(const QString& effectId) const;

    typedef std::unique_ptr<EffectProcessor> (*EffectProcessorInstantiator)();

    template<typename EffectProcessorImpl>
    static bool registerEffect() {
        BuiltInBackend::get()->registerEffectInner(
                EffectProcessorImpl::getId(),
                EffectProcessorImpl::getManifest(),
                []() {
                    return static_cast<std::unique_ptr<EffectProcessor>>(
                            std::make_unique<EffectProcessorImpl>());
                });
        return true;
    };

  private:
    BuiltInBackend() = default;

    QString debugString() const {
        return "BuiltInBackend";
    }

    struct RegisteredEffect {
        EffectManifestPointer pManifest;
        EffectProcessorInstantiator instantiator;
    };

    void registerEffectInner(const QString& id,
            EffectManifestPointer pManifest,
            EffectProcessorInstantiator instantiator);

    QMap<QString, RegisteredEffect> m_registeredEffects;
    QList<QString> m_effectIds;
};
