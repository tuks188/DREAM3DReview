

#include "TransformationPhaseGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TransformationPhaseGuiPlugin::TransformationPhaseGuiPlugin()
: TransformationPhasePlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TransformationPhaseGuiPlugin::~TransformationPhaseGuiPlugin() = default;

#include "TransformationPhase/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
