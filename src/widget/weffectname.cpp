#include <QtDebug>

#include "widget/weffectname.h"

#include "effects/effectsmanager.h"
#include "widget/effectwidgetutils.h"

WEffectName::WEffectName(QWidget* pParent, EffectsManager* pEffectsManager)
        : WLabel(pParent),
          m_pEffectsManager(pEffectsManager) {
    effectUpdated();
}

void WEffectName::setup(const QDomNode& node, const SkinContext& context) {
    WLabel::setup(node, context);
    // EffectWidgetUtils propagates NULLs so this is all safe.
    EffectChainSlotPointer pChainSlot = EffectWidgetUtils::getEffectChainSlotFromNode(
            node, context, m_pEffectsManager);
    EffectSlotPointer pEffectSlot = EffectWidgetUtils::getEffectSlotFromNode(
            node, context, pChainSlot);
    if (pEffectSlot) {
        setEffectSlot(pEffectSlot);
    } else {
        SKIN_WARNING(node, context)
                << "EffectName node could not attach to effect slot.";
    }
}

void WEffectName::setEffectSlot(EffectSlotPointer pEffectSlot) {
    if (pEffectSlot) {
        m_pEffectSlot = pEffectSlot;
        connect(pEffectSlot.data(), SIGNAL(effectChanged()),
                this, SLOT(effectUpdated()));
        effectUpdated();
    }
}

void WEffectName::effectUpdated() {
    QString name;
    QString description;
    if (m_pEffectSlot && m_pEffectSlot->isLoaded()) {
        EffectManifestPointer pManifest = m_pEffectSlot->getManifest();
        name = pManifest->displayName();
        //: %1 = effect name; %2 = effect description
        description = tr("%1: %2").arg(pManifest->name(), pManifest->description());
    } else {
        name = tr("None");
        description = tr("No effect loaded.");
    }
    setText(name);
    setBaseTooltip(description);
}