

#include "HEDMAnalysisGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HEDMAnalysisGuiPlugin::HEDMAnalysisGuiPlugin()
: HEDMAnalysisPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HEDMAnalysisGuiPlugin::~HEDMAnalysisGuiPlugin() = default;

#include "HEDMAnalysis/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
