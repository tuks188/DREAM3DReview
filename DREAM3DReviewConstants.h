/*
 * Your License should go here
 */
#ifndef _dream3dreviewconstants_h_
#define _dream3dreviewconstants_h_

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace DREAM3DReviewConstants
{
  const QString DREAM3DReviewPluginFile("DREAM3DReviewPlugin");
  const QString DREAM3DReviewPluginDisplayName("DREAM3DReviewPlugin");
  const QString DREAM3DReviewBaseName("DREAM3DReviewPlugin");

  namespace FilterGroups
  {
  	const QString DREAM3DReviewFilters("DREAM3DReview");
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
#endif
