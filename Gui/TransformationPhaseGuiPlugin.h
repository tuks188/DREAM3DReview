#pragma once

#include "TransformationPhase/TransformationPhasePlugin.h"

class TransformationPhaseGuiPlugin : public TransformationPhasePlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.TransformationPhaseGuiPlugin")

public:
  TransformationPhaseGuiPlugin();
   ~TransformationPhaseGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  TransformationPhaseGuiPlugin(const TransformationPhaseGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  TransformationPhaseGuiPlugin(TransformationPhaseGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  TransformationPhaseGuiPlugin& operator=(const TransformationPhaseGuiPlugin&) = delete; // Copy Assignment Not Implemented
  TransformationPhaseGuiPlugin& operator=(TransformationPhaseGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
