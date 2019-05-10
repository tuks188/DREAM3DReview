/*
 * Your License should go here
 */
#pragma once

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace HEDMAnalysisConstants
{
  const QString HEDMAnalysisPluginFile("HEDMAnalysisPlugin");
  const QString HEDMAnalysisPluginDisplayName("HEDMAnalysis");
  const QString HEDMAnalysisBaseName("HEDMAnalysis");

  namespace FilterGroups
  {
    const QString HEDMAnalysisFilters("HEDM Analysis");
  }
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{

/*  const QString SomeCustomWidget("SomeCustomWidget"); */

}
