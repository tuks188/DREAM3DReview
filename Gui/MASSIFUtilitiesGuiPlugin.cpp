

#include "MASSIFUtilitiesGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MASSIFUtilitiesGuiPlugin::MASSIFUtilitiesGuiPlugin()
: MASSIFUtilitiesPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MASSIFUtilitiesGuiPlugin::~MASSIFUtilitiesGuiPlugin() = default;

#include "MASSIFUtilities/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
