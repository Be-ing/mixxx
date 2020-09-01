#include "widget/weffectchainpresetbutton.h"

#include <QCheckBox>
#include <QWidgetAction>

#include "effects/presets/effectpresetmanager.h"
#include "widget/effectwidgetutils.h"

WEffectChainPresetButton::WEffectChainPresetButton(QWidget* parent, EffectsManager* pEffectsManager)
        : QPushButton(parent),
          WBaseWidget(this),
          m_pEffectsManager(pEffectsManager),
          m_pChainPresetManager(pEffectsManager->getChainPresetManager()),
          m_pMenu(make_parented<QMenu>(new QMenu(this))) {
    setMenu(m_pMenu.get());
    connect(this,
            &QPushButton::pressed,
            this,
            &QPushButton::showMenu);
    connect(m_pChainPresetManager.get(),
            &EffectChainPresetManager::effectChainPresetListUpdated,
            this,
            &WEffectChainPresetButton::populateMenu);
}

void WEffectChainPresetButton::setup(const QDomNode& node, const SkinContext& context) {
    m_iChainNumber = EffectWidgetUtils::getEffectUnitNumberFromNode(node, context);
    m_pChainSlot = EffectWidgetUtils::getEffectChainSlotFromNode(
            node, context, m_pEffectsManager);
    for (const auto& pEffectSlot : m_pChainSlot->getEffectSlots()) {
        connect(pEffectSlot.get(),
                &EffectSlot::effectChanged,
                this,
                &WEffectChainPresetButton::populateMenu);
        connect(pEffectSlot.get(),
                &EffectSlot::parametersChanged,
                this,
                &WEffectChainPresetButton::populateMenu);
    }
    populateMenu();
    // TODO: set icon
}

void WEffectChainPresetButton::populateMenu() {
    m_pMenu->clear();

    // Chain preset items
    for (const auto pChainPreset : m_pChainPresetManager->getPresetsSorted()) {
        m_pMenu->addAction(pChainPreset->name(), [=]() {
            m_pChainSlot->loadChainPreset(pChainPreset);
        });
    }
    m_pMenu->addSeparator();
    m_pMenu->addAction(tr("Save preset"), this, [this]() {
        m_pChainPresetManager->savePreset(m_pChainSlot);
    });

    m_pMenu->addSeparator();

    // Effect parameter hiding/showing and saving snapshots
    int effectSlotIndex = 0;
    for (const auto pEffectSlot : m_pChainSlot->getEffectSlots()) {
        const ParameterMap loadedParameters = pEffectSlot->getLoadedParameters();
        const ParameterMap hiddenParameters = pEffectSlot->getHiddenParameters();

        auto pManifest = pEffectSlot->getManifest();
        if (pManifest == nullptr) {
            m_pMenu->addAction(tr("Empty Effect Slot %1").arg(effectSlotIndex));
            effectSlotIndex++;
            continue;
        }

        auto pEffectMenu = make_parented<QMenu>(m_pMenu);
        pEffectMenu->setTitle(pEffectSlot->getManifest()->displayName());

        int numTypes = static_cast<int>(EffectManifestParameter::ParameterType::NUM_TYPES);
        for (int parameterTypeId = 0; parameterTypeId < numTypes; ++parameterTypeId) {
            const EffectManifestParameter::ParameterType parameterType =
                    static_cast<EffectManifestParameter::ParameterType>(parameterTypeId);
            for (const auto pParameter : loadedParameters.value(parameterType)) {
                auto pCheckbox = make_parented<QCheckBox>(pEffectMenu);
                pCheckbox->setChecked(true);
                pCheckbox->setText(pParameter->manifest()->name());
                auto handler = [pCheckbox{pCheckbox.get()}, pEffectSlot, pParameter] {
                    if (pCheckbox->isChecked()) {
                        pEffectSlot->showParameter(pParameter);
                    } else {
                        pEffectSlot->hideParameter(pParameter);
                    }
                };
                connect(pCheckbox.get(), &QCheckBox::stateChanged, this, handler);

                auto pAction = make_parented<QWidgetAction>(pEffectMenu);
                pAction->setDefaultWidget(pCheckbox.get());
                connect(pAction.get(), &QAction::triggered, this, handler);

                pEffectMenu->addAction(pAction.get());
            }

            for (const auto pParameter : hiddenParameters.value(parameterType)) {
                auto pCheckbox = make_parented<QCheckBox>(pEffectMenu);
                pCheckbox->setChecked(false);
                pCheckbox->setText(pParameter->manifest()->name());
                auto handler = [pCheckbox{pCheckbox.get()}, pEffectSlot, pParameter] {
                    if (pCheckbox->isChecked()) {
                        pEffectSlot->showParameter(pParameter);
                    } else {
                        pEffectSlot->hideParameter(pParameter);
                    }
                };
                connect(pCheckbox.get(), &QCheckBox::stateChanged, this, handler);

                auto pAction = make_parented<QWidgetAction>(pEffectMenu);
                pAction->setDefaultWidget(pCheckbox.get());
                connect(pAction.get(), &QAction::triggered, this, handler);

                pEffectMenu->addAction(pAction.get());
            }
            pEffectMenu->addSeparator();
        }
        pEffectMenu->addAction(tr("Save snapshot"), [this, pEffectSlot] {
            m_pEffectsManager->getEffectPresetManager()->saveDefaultForEffect(pEffectSlot);
        });
        m_pMenu->addMenu(pEffectMenu);
        effectSlotIndex++;
    }
}