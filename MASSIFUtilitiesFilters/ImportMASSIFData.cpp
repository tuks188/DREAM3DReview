/*
 * Your License or Copyright can go here
 */

#include "ImportMASSIFData.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Utilities/StringOperations.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "MASSIFUtilities/MASSIFUtilitiesConstants.h"
#include "MASSIFUtilities/MASSIFUtilitiesVersion.h"

namespace Detail
{
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> IDataArray::Pointer readH5Dataset(hid_t locId, const QString& datasetPath, const QVector<size_t>& tDims, const QVector<size_t>& cDims)
{
  herr_t err = -1;
  IDataArray::Pointer ptr;

  ptr = DataArray<T>::CreateArray(tDims, cDims, datasetPath);

  T* data = (T*)(ptr->getVoidPointer(0));
  err = QH5Lite::readPointerDataset(locId, datasetPath, data);
  if(err < 0)
  {
    qDebug() << "readH5Data read error: " << __FILE__ << "(" << __LINE__ << ")";
    ptr = IDataArray::NullPointer();
  }
  return ptr;
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportMASSIFData::ImportMASSIFData() :
  AbstractFilter(),
  m_FilePrefix("Step-"),
  m_StepNumber(2)
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportMASSIFData::~ImportMASSIFData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", MassifInputFilePath, FilterParameter::Parameter, ImportMASSIFData, "*.h5;*.hdf5;*.dream3d"));
  }

  {
    parameters.push_back(SIMPL_NEW_STRING_FP("File Prefix", FilePrefix, FilterParameter::Parameter, ImportMASSIFData));
  }

  {
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Step Value", StepNumber, FilterParameter::Parameter, ImportMASSIFData));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportMASSIFData::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

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

  if (m_StepNumber < 0 || m_StepNumber > MASSIFUtilitiesConstants::ImportMassifData::MaxStepNumber)
  {
    QString ss = tr("The selected step number is greater than %1, the maximum allowable step number.").arg(MASSIFUtilitiesConstants::ImportMassifData::MaxStepNumber);
    setErrorCondition(-3003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_FilePrefix.isEmpty())
  {
    QString ss = tr("The file prefix is empty.  Are you sure you meant to do this?");
    setWarningCondition(-3004);
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  // Get the geometry info of the arrays
  QVector<size_t> geoDims;
  QVector<float> origin;
  QVector<float> res;
  getDataContainerGeometry(geoDims, origin, res);
  if (getErrorCondition() < 0) { return; }
  if (geoDims.size() != 3)
  {
    QString ss = tr("The dimensions are not of size 3");
    setErrorCondition(-3003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if (origin.size() != 3)
  {
    QString ss = tr("The origin is not of size 3");
    setErrorCondition(-3003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if (res.size() != 3)
  {
    QString ss = tr("The resolution does not have a size of 3");
    setErrorCondition(-3003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, MASSIFUtilitiesConstants::ImportMassifData::MassifDC);
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  image->setResolution(res[0], res[1], res[2]);
  image->setOrigin(origin[0], origin[1], origin[2]);
  image->setDimensions(geoDims[0], geoDims[1], geoDims[2]);
  dc->setGeometry(image);
  if (getErrorCondition() < 0) { return; }

  int err = 0;
  AttributeMatrix::Pointer am = dc->createNonPrereqAttributeMatrix<AbstractFilter>(this, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, geoDims, AttributeMatrix::Type::Cell);
  if (getErrorCondition() < 0 || err < 0) { return; }

  hid_t fileId = H5Utilities::openFile(m_MassifInputFilePath.toStdString(), true);
  if (fileId < 0)
  {
    QString ss = tr("Error Reading HDF5 file: %1\n").arg(m_MassifInputFilePath);
    setErrorCondition(-3003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<QString> hdf5ArrayPaths = createHDF5DatasetPaths();

  for (int i=0; i<hdf5ArrayPaths.size(); i++)
  {
    QString parentPath = QH5Utilities::getParentPath(hdf5ArrayPaths[i]);

    hid_t parentId = QH5Utilities::openHDF5Object(fileId, parentPath);
    HDF5ScopedGroupSentinel sentinel(&parentId, false);
    // Read dataset into DREAM.3D structure
    QString objectName = QH5Utilities::getObjectNameFromPath(hdf5ArrayPaths[i]);

    IDataArray::Pointer dPtr = readIDataArray(parentId, objectName, geoDims, getInPreflight());
    if (dPtr == IDataArray::NullPointer())
    {
      QString ss = tr("Could not read dataset '%1' at path '%2'").arg(objectName).arg(parentPath);
      setErrorCondition(-3003);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    am->addAttributeArray(dPtr->getName(), dPtr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> ImportMASSIFData::createDataArrayPaths()
{
  QVector<DataArrayPath> dataArrayPaths;
  dataArrayPaths.push_back(DataArrayPath(MASSIFUtilitiesConstants::ImportMassifData::MassifDC, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, MASSIFUtilitiesConstants::ImportMassifData::DField));
  dataArrayPaths.push_back(DataArrayPath(MASSIFUtilitiesConstants::ImportMassifData::MassifDC, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, MASSIFUtilitiesConstants::ImportMassifData::EField));
  dataArrayPaths.push_back(DataArrayPath(MASSIFUtilitiesConstants::ImportMassifData::MassifDC, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, MASSIFUtilitiesConstants::ImportMassifData::SField));

  dataArrayPaths.push_back(DataArrayPath(MASSIFUtilitiesConstants::ImportMassifData::MassifDC, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, MASSIFUtilitiesConstants::ImportMassifData::EVM));
  dataArrayPaths.push_back(DataArrayPath(MASSIFUtilitiesConstants::ImportMassifData::MassifDC, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, MASSIFUtilitiesConstants::ImportMassifData::SVM));
  dataArrayPaths.push_back(DataArrayPath(MASSIFUtilitiesConstants::ImportMassifData::MassifDC, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, MASSIFUtilitiesConstants::ImportMassifData::GrainID));
  dataArrayPaths.push_back(DataArrayPath(MASSIFUtilitiesConstants::ImportMassifData::MassifDC, MASSIFUtilitiesConstants::ImportMassifData::MassifAM, MASSIFUtilitiesConstants::ImportMassifData::Phase));

  return dataArrayPaths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> ImportMASSIFData::createHDF5DatasetPaths()
{
  QVector<QString> arrayPaths;
  QString parentPath = "/" + MASSIFUtilitiesConstants::ImportMassifData::DCGrpName + "/" +
      m_PaddedStep + "/" + MASSIFUtilitiesConstants::ImportMassifData::Datapoint;

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::DFieldsGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::DField);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::EFieldsGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::EField);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::SFieldsGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::SField);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::DFieldsGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::EVM);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::SFieldsGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::SVM);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::EulerAngleGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::Phi1);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::EulerAngleGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::Phi);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::EulerAngleGrpName + "/" +  MASSIFUtilitiesConstants::ImportMassifData::Phi2);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::GrainID);

  arrayPaths.push_back(parentPath + "/" + MASSIFUtilitiesConstants::ImportMassifData::Phase);

  return arrayPaths;
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
void ImportMASSIFData::getDataContainerGeometry(QVector<size_t> &tDims, QVector<float> &origin, QVector<float> &res)
{
  hid_t fileId = QH5Utilities::openFile(m_MassifInputFilePath, true);
  if (fileId < 0)
  {
    QString ss = tr("Error Reading HDF5 file: %1\n").arg(m_MassifInputFilePath);
    setErrorCondition(-3005);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  HDF5ScopedFileSentinel sentinel(&fileId, true);

  QString totalPath = MASSIFUtilitiesConstants::ImportMassifData::DCGrpName;
  hid_t gid = QH5Utilities::openHDF5Object(fileId, MASSIFUtilitiesConstants::ImportMassifData::DCGrpName);
  if (gid < 0)
  {
    QString ss = tr("Could not open path: %1\n")
        .arg(totalPath);
    setErrorCondition(-3006);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  sentinel.addGroupId(&gid);

  // Build up the step
  m_PaddedStep = m_FilePrefix;
  m_PaddedStep.append(StringOperations::GenerateIndexString(m_StepNumber, MASSIFUtilitiesConstants::ImportMassifData::MaxStepNumber));
  totalPath.append("/" + m_PaddedStep);

  hid_t stepGid = QH5Utilities::openHDF5Object(gid, m_PaddedStep);
  if (stepGid < 0)
  {
    QString ss = tr("Could not open path: %1\n").arg(totalPath);
    setErrorCondition(-3007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  sentinel.addGroupId(&stepGid);

  totalPath.append("/" + MASSIFUtilitiesConstants::ImportMassifData::GeometryGrpName);
  hid_t geoGid = QH5Utilities::openHDF5Object(stepGid, MASSIFUtilitiesConstants::ImportMassifData::GeometryGrpName);
  if (geoGid < 0)
  {
    QString ss = tr("Could not open path: %1\n").arg(totalPath);
    setErrorCondition(-3007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  sentinel.addGroupId(&geoGid);

  std::vector<size_t> dims;
  herr_t err = QH5Lite::readVectorDataset(geoGid, MASSIFUtilitiesConstants::ImportMassifData::DimGrpName, dims);
  if (err < 0)
  {
    QString ss = tr("Could not read dataset %1 at path '%2'\n")
        .arg(MASSIFUtilitiesConstants::ImportMassifData::DimGrpName)
        .arg(totalPath);
    setErrorCondition(-3008);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  tDims = QVector<size_t>::fromStdVector(dims);

  std::vector<float> stdOrigin;
  err = QH5Lite::readVectorDataset(geoGid, MASSIFUtilitiesConstants::ImportMassifData::OriginGrpName, stdOrigin);
  if (err < 0)
  {
    QString ss = tr("Could not read dataset %1 at path '%2'\n")
        .arg(MASSIFUtilitiesConstants::ImportMassifData::OriginGrpName)
        .arg(totalPath);
    setErrorCondition(-3009);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  origin = QVector<float>::fromStdVector(stdOrigin);

  std::vector<float> stdRes;
  err = QH5Lite::readVectorDataset(geoGid, MASSIFUtilitiesConstants::ImportMassifData::SpacingGrpName, stdRes);
  if (err < 0)
  {
    QString ss = tr("Could not read dataset %1 at path '%2'\n")
        .arg(MASSIFUtilitiesConstants::ImportMassifData::SpacingGrpName)
        .arg(totalPath);
    setErrorCondition(-3010);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  res = QVector<float>::fromStdVector(stdRes);
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
const QUuid ImportMASSIFData::getUuid()
{
  return QUuid("{85654f78-04a8-50ed-9ae1-25dfbd0539b3}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getSubGroupName()
{ return "MASSIFUtilities"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportMASSIFData::getHumanLabel()
{ return "Import MASSIF Data"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer ImportMASSIFData::readIDataArray(hid_t gid, const QString& name, QVector<size_t> geoDims, bool metaDataOnly)
{

  herr_t err = -1;
  // herr_t retErr = 1;
  hid_t typeId = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  QString res;

  QVector<hsize_t> dims; // Reusable for the loop
  IDataArray::Pointer ptr = IDataArray::NullPointer();
  // qDebug() << "Reading Attribute " << *iter ;
  typeId = QH5Lite::getDatasetType(gid, name);
  if(typeId < 0)
  {
    return ptr;
  }
  // Get the HDF5 DataSet information. the dimensions will be the combined Tuple Dims and the Data Array Componenet dimes
  err = QH5Lite::getDatasetInfo(gid, name, dims, attr_type, attr_size);
  if(err < 0)
  {
    qDebug() << "Error in getAttributeInfo method in readUserMetaData.";
  }
  else
  {
    QString classType;
    //int version = 0;
    QVector<size_t> tDims = geoDims;
    QVector<size_t> cDims;

    if (geoDims.size() == dims.size())
    {
      cDims.push_back(1);
    }
    else
    {
      for (int i = geoDims.size(); i < dims.size(); i++)
      {
        cDims.push_back(dims[i]);
      }
    }

//    err = ReadRequiredAttributes(gid, name, classType, version, tDims, cDims);
//    if(err < 0)
//    {
//      return ptr;
//    }

    // Check to see if we are reading a bool array and if so read it and return
    if(classType.compare("DataArray<bool>") == 0)
    {
      if(metaDataOnly == false)
      {
        ptr = Detail::readH5Dataset<bool>(gid, name, tDims, cDims);
      }
      else
      {
        ptr = DataArray<bool>::CreateArray(tDims, cDims, name, false);
      }
      err = H5Tclose(typeId);
      return ptr; // <== Note early return here.
    }
    switch(attr_type)
    {
    case H5T_STRING:
      res.clear(); // Clear the string out first
      err = QH5Lite::readStringDataset(gid, name, res);
      //        if(err >= 0)
      //        {
      //          IDataArray::Pointer attr = MXAAsciiStringData::Create(res);
      //          attr->setName(name);
      //          attributes[*iter] = attr;
      //        }
      break;
    case H5T_INTEGER:
      // qDebug() << "User Meta Data Type is Integer" ;
      if(H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId, H5T_STD_U8LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<uint8_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint8_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId, H5T_STD_U16LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<uint16_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint16_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId, H5T_STD_U32LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<uint32_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint32_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId, H5T_STD_U64LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<uint64_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<uint64_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId, H5T_STD_I8LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int8_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int8_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId, H5T_STD_I16LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int16_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int16_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId, H5T_STD_I32LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int32_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int32_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId, H5T_STD_I64LE))
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<int64_t>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<int64_t>::CreateArray(tDims, cDims, name, false);
        }
      }
      else
      {
        qDebug() << "Unknown Type: " << typeId << " at " << name;
        err = -1;
      }
      break;
    case H5T_FLOAT:
      if(attr_size == 4)
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<float>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<float>::CreateArray(tDims, cDims, name, false);
        }
      }
      else if(attr_size == 8)
      {
        if(metaDataOnly == false)
        {
          ptr = Detail::readH5Dataset<double>(gid, name, tDims, cDims);
        }
        else
        {
          ptr = DataArray<double>::CreateArray(tDims, cDims, name, false);
        }
      }
      else
      {
        qDebug() << "Unknown Floating point type";
        err = -1;
      }
      break;
    default:
      qDebug() << "Error: readUserMetaData() Unknown attribute type: " << attr_type;
      QH5Utilities::printHDFClassType(attr_type);
    }

    err = H5Tclose(typeId);
    // Close the H5A type Id that was retrieved during the loop
  }

  return ptr;
}

