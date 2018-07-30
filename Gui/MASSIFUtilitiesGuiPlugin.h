#pragma once

#include "MASSIFUtilities/MASSIFUtilitiesPlugin.h"

class MASSIFUtilitiesGuiPlugin : public MASSIFUtilitiesPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.MASSIFUtilitiesGuiPlugin")

public:
  MASSIFUtilitiesGuiPlugin();
   ~MASSIFUtilitiesGuiPlugin() override;
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  MASSIFUtilitiesGuiPlugin(const MASSIFUtilitiesGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  MASSIFUtilitiesGuiPlugin(MASSIFUtilitiesGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  MASSIFUtilitiesGuiPlugin& operator=(const MASSIFUtilitiesGuiPlugin&) = delete; // Copy Assignment Not Implemented
  MASSIFUtilitiesGuiPlugin& operator=(MASSIFUtilitiesGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
