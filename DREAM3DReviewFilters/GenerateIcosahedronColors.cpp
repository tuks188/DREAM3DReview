/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <memory>

#include "GenerateIcosahedronColors.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/Common/SIMPLRange.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Utilities/ColorTable.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "DREAM3DReview/DREAM3DReviewConstants.h"
#include "DREAM3DReview/DREAM3DReviewVersion.h"

#include "EbsdLib/EbsdConstants.h"

enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID31 = 31
};

/**
 * @brief The GenerateIcosahedronColorsImpl class implements a threaded algorithm that computes the Icosahedron
 * colors for each element in a geometry
 */
class GenerateIcosahedronColorsImpl
{
public:
  GenerateIcosahedronColorsImpl(GenerateIcosahedronColors* filter, float* cAxisLocations, int32_t* phases, uint32_t* crystalStructures, int32_t numPhases, bool* goodVoxels, uint8_t* colors)
  : m_Filter(filter)
  , m_CellCAxisLocations(cAxisLocations)
  , m_CellPhases(phases)
  , m_CrystalStructures(crystalStructures)
  , m_NumPhases(numPhases)
  , m_GoodVoxels(goodVoxels)
  , m_CellIcosahedronColors(colors)
  {
  }

  virtual ~GenerateIcosahedronColorsImpl() = default;

  void convert(size_t start, size_t end) const
  {
    QVector<LaueOps::Pointer> ops = LaueOps::getOrientationOpsQVector();
    double refDir[3] = {0.0f, 0.0f, 1.0f};
    double dCAxis[3] = {0.0, 0.0, 0.0};
    SIMPL::Rgb argb = 0x00000000;
    int32_t phase = 0;
    bool calcIcosahedron = false;
    size_t index = 0;
    for(size_t i = start; i < end; i++)
    {
      phase = m_CellPhases[i];
      index = i * 3;
      m_CellIcosahedronColors[index] = 0;
      m_CellIcosahedronColors[index + 1] = 0;
      m_CellIcosahedronColors[index + 2] = 0;
      dCAxis[0] = m_CellCAxisLocations[index];
      dCAxis[1] = m_CellCAxisLocations[index + 1];
      dCAxis[2] = m_CellCAxisLocations[index + 2];

      // Make sure we are using a valid C-Axis Locations with valid crystal symmetry
      calcIcosahedron = true;
      if(nullptr != m_GoodVoxels)
      {
        calcIcosahedron = m_GoodVoxels[i];
      }
      // Sanity check the phase data to make sure we do not walk off the end of the array
      if(phase >= m_NumPhases)
      {
        m_Filter->incrementPhaseWarningCount();
      }

      if(phase < m_NumPhases && calcIcosahedron && m_CrystalStructures[phase] < Ebsd::CrystalStructure::LaueGroupEnd)
      {
        argb = ops[m_CrystalStructures[phase]]->generateIPFColor(dCAxis, refDir, false);
        m_CellIcosahedronColors[index] = static_cast<uint8_t>(RgbColor::dRed(argb));
        m_CellIcosahedronColors[index + 1] = static_cast<uint8_t>(RgbColor::dGreen(argb));
        m_CellIcosahedronColors[index + 2] = static_cast<uint8_t>(RgbColor::dBlue(argb));
      }
    }
  }

  void operator()(const SIMPLRange& range) const
  {
    convert(range.min(), range.max());
  }

private:
  GenerateIcosahedronColors* m_Filter = nullptr;
  float* m_CellCAxisLocations;
  int32_t* m_CellPhases;
  unsigned int* m_CrystalStructures;
  int32_t m_NumPhases = 0;
  bool* m_GoodVoxels;
  uint8_t* m_CellIcosahedronColors;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIcosahedronColors::GenerateIcosahedronColors()
: m_CellPhasesArrayPath("", "", "")
, m_CellCAxisLocationsArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_UseGoodVoxels(false)
, m_GoodVoxelsArrayPath("", "", "")
, m_CellIcosahedronColorsArrayName(SIMPL::CellData::IcosahedronColor)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIcosahedronColors::~GenerateIcosahedronColors() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Apply to Good Elements Only (Bad Elements Will Be Black)", UseGoodVoxels, FilterParameter::Parameter, GenerateIcosahedronColors, linkedProps));
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("C-Axis Locations", CellCAxisLocationsArrayPath, FilterParameter::RequiredArray, GenerateIcosahedronColors, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, GenerateIcosahedronColors, req));
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Bool, 1, AttributeMatrix::Category::Any);
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Mask", GoodVoxelsArrayPath, FilterParameter::RequiredArray, GenerateIcosahedronColors, req));

  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, GenerateIcosahedronColors, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Icosahedron Colors", CellIcosahedronColorsArrayName, CellCAxisLocationsArrayPath, CellCAxisLocationsArrayPath, FilterParameter::CreatedArray, GenerateIcosahedronColors));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels()));
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setCellCAxisLocationsArrayPath(reader->readDataArrayPath("CellCAxisLocationsArrayPath", getCellCAxisLocationsArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setCellIcosahedronColorsArrayName(reader->readString("CellIcosahedronColorsArrayName", getCellIcosahedronColorsArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::initialize()
{
  m_PhaseWarningCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  QVector<DataArrayPath> dataArraypaths;

  std::vector<size_t> cDims(1, 1);
  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArraypaths.push_back(getCellPhasesArrayPath());
  }

  cDims[0] = 3;
  m_CellCAxisLocationsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellCAxisLocationsArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellCAxisLocationsPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellCAxisLocations = m_CellCAxisLocationsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArraypaths.push_back(getCellCAxisLocationsArrayPath());
  }

  cDims[0] = 1;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(m_CellCAxisLocationsArrayPath.getDataContainerName(), getCellCAxisLocationsArrayPath().getAttributeMatrixName(), getCellIcosahedronColorsArrayName());
  m_CellIcosahedronColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(
      this, tempPath, 0, cDims, "", DataArrayID31); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellIcosahedronColorsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellIcosahedronColors = m_CellIcosahedronColorsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getUseGoodVoxels())
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    cDims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(),
                                                                                                       cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_GoodVoxelsPtr.lock())                                                                      /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCode() >= 0)
    {
      dataArraypaths.push_back(getGoodVoxelsArrayPath());
    }
  }
  else
  {
    m_GoodVoxels = nullptr;
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArraypaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::execute()
{
  initialize();
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  m_PhaseWarningCount = 0;
  size_t totalPoints = m_CellCAxisLocationsPtr.lock()->getNumberOfTuples();

  int32_t numPhases = static_cast<int32_t>(m_CrystalStructuresPtr.lock()->getNumberOfTuples());

  // Allow data-based parallelization
  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0, totalPoints);
  dataAlg.execute(GenerateIcosahedronColorsImpl(this, m_CellCAxisLocations, m_CellPhases, m_CrystalStructures, numPhases, m_GoodVoxels, m_CellIcosahedronColors));

  if(m_PhaseWarningCount > 0)
  {
    QString ss = QObject::tr("The Ensemble Phase information only references %2 phase(s) but %1 cell(s) had a phase value greater than %2. \
This indicates a problem with the input cell phase data. DREAM.3D will give INCORRECT RESULTS.")
                     .arg(m_PhaseWarningCount)
                     .arg(numPhases - 1);
    setErrorCondition(-48000, ss);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::incrementPhaseWarningCount()
{
  ++m_PhaseWarningCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateIcosahedronColors::newFilterInstance(bool copyFilterParameters) const
{
  GenerateIcosahedronColors::Pointer filter = GenerateIcosahedronColors::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getCompiledLibraryName() const
{
  return DREAM3DReviewConstants::DREAM3DReviewBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << DREAM3DReview::Version::Major() << "." << DREAM3DReview::Version::Minor() << "." << DREAM3DReview::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateIcosahedronColors::getUuid() const
{
  return QUuid("{1448f2a8-c8c7-498b-bedf-7a538c8dc8d2}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getHumanLabel() const
{
  return "Generate Icosahedron Colors";
}

// -----------------------------------------------------------------------------
GenerateIcosahedronColors::Pointer GenerateIcosahedronColors::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateIcosahedronColors> GenerateIcosahedronColors::New()
{
  struct make_shared_enabler : public GenerateIcosahedronColors
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getNameOfClass() const
{
  return QString("GenerateIcosahedronColors");
}

// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::ClassName()
{
  return QString("GenerateIcosahedronColors");
}

// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::setCellPhasesArrayPath(const DataArrayPath& value)
{
  m_CellPhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIcosahedronColors::getCellPhasesArrayPath() const
{
  return m_CellPhasesArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::setCellCAxisLocationsArrayPath(const DataArrayPath& value)
{
  m_CellCAxisLocationsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIcosahedronColors::getCellCAxisLocationsArrayPath() const
{
  return m_CellCAxisLocationsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIcosahedronColors::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::setUseGoodVoxels(bool value)
{
  m_UseGoodVoxels = value;
}

// -----------------------------------------------------------------------------
bool GenerateIcosahedronColors::getUseGoodVoxels() const
{
  return m_UseGoodVoxels;
}

// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::setGoodVoxelsArrayPath(const DataArrayPath& value)
{
  m_GoodVoxelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateIcosahedronColors::getGoodVoxelsArrayPath() const
{
  return m_GoodVoxelsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateIcosahedronColors::setCellIcosahedronColorsArrayName(const QString& value)
{
  m_CellIcosahedronColorsArrayName = value;
}

// -----------------------------------------------------------------------------
QString GenerateIcosahedronColors::getCellIcosahedronColorsArrayName() const
{
  return m_CellIcosahedronColorsArrayName;
}
