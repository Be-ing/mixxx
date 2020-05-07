#include "widget/weffectchainpresetselector.h"

#include <QtDebug>

#include "effects/effectsmanager.h"
#include "widget/effectwidgetutils.h"

WEffectChainPresetSelector::WEffectChainPresetSelector(
        QWidget* pParent, EffectsManager* pEffectsManager)
        : QComboBox(pParent),
          WBaseWidget(this),
          m_bQuickEffectChain(false),
          m_pChainPresetManager(pEffectsManager->getChainPresetManager()),
          m_pEffectsManager(pEffectsManager) {
    // Prevent this widget from getting focused to avoid
    // interfering with using the library via keyboard.
    setFocusPolicy(Qt::NoFocus);
}

void WEffectChainPresetSelector::setup(const QDomNode& node, const SkinContext& context) {
    m_pChainSlot = EffectWidgetUtils::getEffectChainSlotFromNode(
            node, context, m_pEffectsManager);

    VERIFY_OR_DEBUG_ASSERT(m_pChainSlot != nullptr) {
        SKIN_WARNING(node, context)
                << "EffectChainPresetSelector node could not attach to EffectChainSlot";
        return;
    }

    auto chainPresetListUpdateSignal = &EffectChainPresetManager::effectChainPresetListUpdated;
    auto pQuickEffectChainSlot = dynamic_cast<QuickEffectChainSlot*>(m_pChainSlot.get());
    if (pQuickEffectChainSlot) {
        chainPresetListUpdateSignal = &EffectChainPresetManager::quickEffectChainPresetListUpdated;
        m_bQuickEffectChain = true;
    }
    connect(m_pChainPresetManager.get(),
            chainPresetListUpdateSignal,
            this,
            &WEffectChainPresetSelector::populate);
    connect(m_pChainSlot.data(),
            &EffectChainSlot::nameChanged,
            this,
            &WEffectChainPresetSelector::slotEffectChainNameChanged);
    connect(this,
            QOverload<int>::of(&QComboBox::activated),
            this,
            &WEffectChainPresetSelector::slotEffectChainPresetSelected);

    populate();
}

void WEffectChainPresetSelector::populate() {
    blockSignals(true);
    clear();

    QFontMetrics metrics(font());

    QList<EffectChainPresetPointer> presetList;
    if (m_bQuickEffectChain) {
        presetList = m_pEffectsManager->getChainPresetManager()->getQuickEffectPresetsSorted();
    } else {
        presetList = m_pEffectsManager->getChainPresetManager()->getPresetsSorted();
    }
    for (const auto& pChainPreset : presetList) {
        QString elidedDisplayName = metrics.elidedText(pChainPreset->name(),
                Qt::ElideMiddle,
                width() - 2);
        addItem(elidedDisplayName, QVariant(pChainPreset->name()));
    }

    slotEffectChainNameChanged(m_pChainSlot->presetName());
    blockSignals(false);
}

void WEffectChainPresetSelector::slotEffectChainPresetSelected(int index) {
    Q_UNUSED(index);
    m_pChainSlot->loadChainPreset(
            m_pChainPresetManager->getPreset(currentData().toString()));
}

void WEffectChainPresetSelector::slotEffectChainNameChanged(const QString& name) {
    setCurrentIndex(findData(name));
}