#include "effects/chains/quickeffectchain.h"

#include "effects/effectslot.h"
#include "effects/presets/effectchainpresetmanager.h"

QuickEffectChain::QuickEffectChain(const QString& group,
        EffectsManager* pEffectsManager,
        EffectsMessengerPointer pEffectsMessenger)
        : PerGroupEffectChain(group,
                  formatEffectChainGroup(group),
                  SignalProcessingStage::Postfader,
                  pEffectsManager,
                  pEffectsMessenger) {
    for (int i = 0; i < kNumEffectsPerUnit; ++i) {
        addEffectSlot(formatEffectSlotGroup(group, i));
        m_effectSlots.at(i)->setEnabled(true);
    }
    disconnect(m_pChainPresetManager.data(),
            &EffectChainPresetManager::effectChainPresetListUpdated,
            this,
            &QuickEffectChain::slotPresetListUpdated);
    m_pControlNumPresetsAvailable->forceSet(m_pChainPresetManager->numQuickEffectPresets());
    connect(m_pChainPresetManager.data(),
            &EffectChainPresetManager::quickEffectChainPresetListUpdated,
            this,
            &QuickEffectChain::slotPresetListUpdated);
}

QString QuickEffectChain::formatEffectChainGroup(const QString& group) {
    return QString("[QuickEffectRack1_%1]").arg(group);
}

QString QuickEffectChain::formatEffectSlotGroup(
        const QString& group, const int iEffectSlotNumber) {
    return QString("[QuickEffectRack1_%1_Effect%2]")
            .arg(group,
                    QString::number(iEffectSlotNumber + 1));
}

int QuickEffectChain::presetIndex() const {
    return m_pChainPresetManager->quickEffectPresetIndex(m_presetName);
}

EffectChainPresetPointer QuickEffectChain::presetAtIndex(int index) const {
    return m_pChainPresetManager->quickEffectPresetAtIndex(index);
}

void QuickEffectChain::loadChainPreset(EffectChainPresetPointer pPreset) {
    EffectChain::loadChainPreset(pPreset);
    setSuperParameter(pPreset->superKnob(), true);
}

int QuickEffectChain::numPresets() const {
    VERIFY_OR_DEBUG_ASSERT(m_pChainPresetManager) {
        return 0;
    }
    return m_pChainPresetManager->numQuickEffectPresets();
}