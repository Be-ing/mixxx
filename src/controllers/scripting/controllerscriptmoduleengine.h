#pragma once

#include "controllers/scripting/controllerscriptenginebase.h"

/// ControllerScriptModuleEngine loads and executes script module files for controller mappings.
class ControllerScriptModuleEngine : public ControllerScriptEngineBase {
    Q_OBJECT
  public:
    ControllerScriptModuleEngine(Controller* controller);

    bool initialize() override;

    void setModuleFileInfo(QFileInfo moduleFileInfo) {
        m_moduleFileInfo = moduleFileInfo;
    }

    void handleInput(const QVector<uint8_t>& data, mixxx::Duration timestamp);

  private:
    void shutdown() override;

    QJSValue m_handleInputFunction;
    QJSValue m_shutdownFunction;

    QFileInfo m_moduleFileInfo;
};
