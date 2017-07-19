/*
 * Your License or Copyright can go here
 */

#include "ImportMASSIFData.h"

#include <QtCore/QFileInfo>

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

  if (m_MassifInputFilePath.isEmpty())
  {
    QString ss = "No HDF5 file has been selected";
    setErrorCondition(-3001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFileInfo hdf5FileInfo(m_MassifInputFilePath);
  QString ext = hdf5FileInfo.suffix();
  if (hdf5FileInfo.exists() && hdf5FileInfo.isFile() && ext != "h5" && ext != "hdf5" && ext != "dream3d")
  {
    QString ss = tr("The selected file '%1' is not an HDF5 file.").arg(hdf5FileInfo.fileName());
    setErrorCondition(-3002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int err = 0;
  AttributeMatrix::Pointer am = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, m_SelectedAttributeMatrix, err);
  if (getErrorCondition() < 0) { return; }

  hid_t fileId = H5Utilities::openFile(m_MassifInputFilePath.toStdString(), true);
  if (fileId < 0)
  {
    QString ss = tr("Error Reading HDF5 file: %1\n").arg(m_MassifInputFilePath);
    setErrorCondition(-3003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

//  QVector<DataArrayPath> dataArrayPaths;

//  DataArrayPath dPath = m_SelectedAttributeMatrix;
//  dPath.setDataArrayName("D11");

//  for (int i=0; i<m_SelectedHDF5Paths.size(); i++)
//  {
//    QString parentPath = QH5Utilities::getParentPath(m_SelectedHDF5Paths[i]);

//    hid_t parentId = QH5Utilities::openHDF5Object(fileId, parentPath);

//    // Read dataset into DREAM.3D structure
//    QString objectName = QH5Utilities::getObjectNameFromPath(m_SelectedHDF5Paths[i]);

//    QVector<hsize_t> dims;
//    H5T_class_t type_class;
//    size_t type_size;
//    herr_t err = QH5Lite::getDatasetInfo(parentId, objectName, dims, type_class, type_size);
//    if (err < 0)
//    {
//      QString ss = tr("Error reading type info from dataset with path '%1'").arg(m_SelectedHDF5Paths[i]);
//      setErrorCondition(-20004);
//      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
//      return;
//    }

//    IDataArray::Pointer dPtr = H5DataArrayReader::ReadIDataArray(parentId, objectName, getInPreflight());
  
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

