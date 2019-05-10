/*
 * Your License should go here
 */
#pragma once

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace MASSIFUtilitiesConstants
{
  const QString MASSIFUtilitiesPluginFile("MASSIFUtilitiesPlugin");
  const QString MASSIFUtilitiesPluginDisplayName("MASSIF Utilities Plugin");
  const QString MASSIFUtilitiesBaseName("MASSIFUtilities");

  namespace FilterGroups
  {
  	const QString MASSIFUtilitiesFilters("MASSIFUtilities");
  }

  namespace ImportMassifData
  {
    const QString MassifDC = "MassifDataContainer";
    const QString MassifAM = "MassifAttributeMatrix";
    const QString DField = "D11";
    const QString EField = "El11";
    const QString SField = "S11";
    const QString DimGrpName = "Dimension";
    const QString OriginGrpName = "Origin";
    const QString SpacingGrpName = "Spacing";
    const QString DCGrpName = "3Ddatacontainer";
    const QString GeometryGrpName = "Geometry";
    const QString EVM = "EVM";
    const QString SVM = "SVM";
    const QString GrainID = "Grainid";
    const QString Phase = "Phase";
    const QString Datapoint = "Datapoint";
    const QString DFieldsGrpName = "Dfields";
    const QString EFieldsGrpName = "Elastic strain";
    const QString SFieldsGrpName = "Sfields";
    const QString EulerAngleGrpName = "Eulerangle";
    const QString Phi1 = "Phi1";
    const QString Phi = "Phi";
    const QString Phi2 = "phi2";

    const int MaxStepNumber = 999999;
  }
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}
