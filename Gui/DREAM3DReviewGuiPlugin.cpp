

#include "DREAM3DReviewGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DReviewGuiPlugin::DREAM3DReviewGuiPlugin()
: DREAM3DReviewPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DReviewGuiPlugin::~DREAM3DReviewGuiPlugin() = default;

#include "DREAM3DReview/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
