/*
 * Your License or Copyright Information can go here
 */
#include "TransformationPhasePlugin.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_TransformationPhasePlugin.cxx"

Q_EXPORT_PLUGIN2(TransformationPhasePlugin, TransformationPhasePlugin)

namespace Detail
{
   const std::string TransformationPhasePluginFile("TransformationPhasePlugin");
   const std::string TransformationPhasePluginDisplayName("TransformationPhasePlugin");
   const std::string TransformationPhasePluginBaseName("TransformationPhasePlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TransformationPhasePlugin::TransformationPhasePlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TransformationPhasePlugin::~TransformationPhasePlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString TransformationPhasePlugin::getPluginName()
{
  return QString::fromStdString(Detail::TransformationPhasePluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhasePlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhasePlugin::readSettings(QSettings &prefs)
{

}

#include "TransformationPhaseFilters/RegisterKnownFilterWidgets.cpp"
#include "TransformationPhaseFilters/RegisterKnownFilters.cpp"
