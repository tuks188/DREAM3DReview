/*
 * Your License or Copyright can go here
 */

#include "ImportMASSIFData.h"

#include "SIMPLib/Common/Constants.h"



#include "MASSIFUtilities/MASSIFUtilitiesConstants.h"
#include "MASSIFUtilities/MASSIFUtilitiesVersion.h"

// Include the MOC generated file for this class
#include "moc_ImportMASSIFData.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportMASSIFData::ImportMASSIFData() :
  AbstractFilter()
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportMASSIFData::~ImportMASSIFData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::initialize()
{
  setErrorCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::dataCheck()
{
  setErrorCondition(0);
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  if (getErrorCondition() < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportMASSIFData::newFilterInstance(bool copyFilterParameters)
{
  ImportMASSIFData::Pointer filter = ImportMASSIFData::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getCompiledLibraryName()
{ return MASSIFUtilitiesConstants::MASSIFUtilitiesBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getBrandingString()
{
  return "MASSIFUtilities";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  MASSIFUtilities::Version::Major() << "." << MASSIFUtilities::Version::Minor() << "." << MASSIFUtilities::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getGroupName()
{ return SIMPL::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getSubGroupName()
{ return "MASSIFUtilities"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getHumanLabel()
{ return "ImportMASSIFData"; }

