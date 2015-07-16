/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2015 Dr. Joseph C. Tucker (UES, Inc.)
 * All rights reserved.
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "TiDwellFatigueCrystallographicAnalysis.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/IntFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DoubleFilterParameter.h"
#include "DREAM3DLib/FilterParameters/FloatVec3FilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/SeparatorFilterParameter.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "OrientationLib/OrientationMath/OrientationMath.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

#include "Plugins/Statistics/StatisticsFilters/FindNeighbors.h"

#include "TransformationPhase/TransformationPhaseConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TiDwellFatigueCrystallographicAnalysis::TiDwellFatigueCrystallographicAnalysis() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::SyntheticVolumeDataContainerName),
  m_AlphaGlobPhase(1),
  m_MTRPhase(2),
  m_LatticeParameterA(2.9131f),
  m_LatticeParameterC(4.6572f),
  m_SubsurfaceDistance(0),
  m_ConsiderationFraction(1.0f),
  m_DoNotAssumeInitiatorPresence(true),
  m_InitiatorLowerThreshold(40.0f),
  m_InitiatorUpperThreshold(50.0f),
  m_HardFeatureLowerThreshold(0.0f),
  m_HardFeatureUpperThreshold(25.0f),
  m_SoftFeatureLowerThreshold(70.0f),
  m_SoftFeatureUpperThreshold(90.0f),
  m_SelectedFeaturesArrayName(TransformationPhaseConstants::SelectedFeatures),
  m_InitiatorsArrayName(TransformationPhaseConstants::Initiators),
  m_HardFeaturesArrayName(TransformationPhaseConstants::HardFeatures),
  m_SoftFeaturesArrayName(TransformationPhaseConstants::SoftFeatures),
  m_HardSoftGroupsArrayName(TransformationPhaseConstants::HardSoftGroups),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellFeatureAttributeMatrixPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellParentIdsArrayName(DREAM3D::CellData::ParentIds),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_NeighborListArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::NeighborList),
  m_CentroidsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_SelectedFeatures(NULL),
  m_Initiators(NULL),
  m_HardFeatures(NULL),
  m_SoftFeatures(NULL),
  m_HardSoftGroups(NULL),
  m_FeatureIds(NULL),
  m_CellParentIds(NULL),
  m_FeatureParentIds(NULL),
  m_Active(NULL),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhases(NULL),
  m_Centroids(NULL),
  m_CrystalStructures(NULL)
{
  m_StressAxis.x = 0.0f;
  m_StressAxis.y = 0.0f;
  m_StressAxis.z = 1.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TiDwellFatigueCrystallographicAnalysis::~TiDwellFatigueCrystallographicAnalysis()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::setupFilterParameters()
{
  FilterParameterVector parameters;
  QStringList linkedProps1("AlphaGlobPhase");
  parameters.push_back(LinkedBooleanFilterParameter::New("Alpha Glob Phase Present", "AlphaGlobPhasePresent", getAlphaGlobPhasePresent(), linkedProps1, FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Alpha Glob Phase Number", "AlphaGlobPhase", getAlphaGlobPhase(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Microtextured Region Phase Number", "MTRPhase", getMTRPhase(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Lattice Parameter A", "LatticeParameterA", getLatticeParameterA(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Lattice Parameter C", "LatticeParameterC", getLatticeParameterC(), FilterParameter::Parameter));
  parameters.push_back(FloatVec3FilterParameter::New("Stress Axis", "StressAxis", getStressAxis(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Subsurface Feature Distance to Consider (Microns)", "SubsurfaceDistance", getSubsurfaceDistance(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Fraction of Features to Consider", "ConsiderationFraction", getConsiderationFraction(), FilterParameter::Parameter));
  QStringList linkedProps2;
  linkedProps2 << "InitiatorLowerThreshold" << "InitiatorUpperThreshold";
  parameters.push_back(LinkedBooleanFilterParameter::New("Do Not Assume Initiator Presence", "DoNotAssumeInitiatorPresence", getDoNotAssumeInitiatorPresence(), linkedProps2, FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Initiator Lower Threshold (Degrees)", "InitiatorLowerThreshold", getInitiatorLowerThreshold(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Initiator Upper Threshold (Degrees)", "InitiatorUpperThreshold", getInitiatorUpperThreshold(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Hard Feature Lower Threshold (Degrees)", "HardFeatureLowerThreshold", getHardFeatureLowerThreshold(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Hard Feature Upper Threshold (Degrees)", "HardFeatureUpperThreshold", getHardFeatureUpperThreshold(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Soft Feature Lower Threshold (Degrees)", "SoftFeatureLowerThreshold", getSoftFeatureLowerThreshold(), FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Soft Feature Upper Threshold (Degrees)", "SoftFeatureUpperThreshold", getSoftFeatureUpperThreshold(), FilterParameter::Parameter));


  parameters.push_back(DataContainerSelectionFilterParameter::New("Data Container", "DataContainerName", getDataContainerName(), FilterParameter::RequiredArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));

  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    parameters.push_back(DataArraySelectionFilterParameter::New("FeatureIds", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Cell Feature Attribute Matrix", "CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    QVector<unsigned int> amTypes;
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
    req.amTypes = amTypes;
    req.componentDimensions = QVector< QVector<size_t> >(1, QVector<size_t>(1, 3));
    parameters.push_back(DataArraySelectionFilterParameter::New("Average Euler Angles", "FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    QVector<unsigned int> amTypes;
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
    req.amTypes = amTypes;
    req.componentDimensions = QVector< QVector<size_t> >(1, QVector<size_t>(1, 1));
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    QVector<unsigned int> amTypes;
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
    req.amTypes = amTypes;
    req.componentDimensions = QVector< QVector<size_t> >(1, QVector<size_t>(1, 1));
    parameters.push_back(DataArraySelectionFilterParameter::New("Neighbor List", "NeighborListArrayPath", getNeighborListArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    QVector<unsigned int> amTypes;
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeFeature);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexFeature);
    req.amTypes = amTypes;
    req.componentDimensions = QVector< QVector<size_t> >(1, QVector<size_t>(1, 3));
    parameters.push_back(DataArraySelectionFilterParameter::New("Centroids", "CentroidsArrayPath", getCentroidsArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::DataStructureRequirements req;
    QVector<unsigned int> amTypes;
    amTypes.push_back(DREAM3D::AttributeMatrixType::CellEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::FaceEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::EdgeEnsemble);
    amTypes.push_back(DREAM3D::AttributeMatrixType::VertexEnsemble);
    req.amTypes = amTypes;
    req.componentDimensions = QVector< QVector<size_t> >(1, QVector<size_t>(1, 1));
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Parent Ids", "CellParentIdsArrayName", getCellParentIdsArrayName(), FilterParameter::CreatedArray));

  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Feature Attribute Matrix", "NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Selected Features", "SelectedFeaturesArrayName", getSelectedFeaturesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Initiators", "InitiatorsArrayName", getInitiatorsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Hard Features", "HardFeaturesArrayName", getHardFeaturesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Soft Features", "SoftFeaturesArrayName", getSoftFeaturesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Hard-Soft Groups", "HardSoftGroupsArrayName", getHardSoftGroupsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Parent Ids", "FeatureParentIdsArrayName", getFeatureParentIdsArrayName(), FilterParameter::CreatedArray));

  //parameters.push_back(StringFilterParameter::New("Active", "ActiveArrayName", getActiveArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setAlphaGlobPhasePresent(reader->readValue("AlphaGlobPhasePresent", getAlphaGlobPhasePresent()) );
  setAlphaGlobPhase(reader->readValue("AlphaGlobPhase", getAlphaGlobPhase()) );
  setMTRPhase(reader->readValue("MTRPhase", getMTRPhase()) );
  setLatticeParameterA(reader->readValue("LatticeParameterA", getLatticeParameterA()) );
  setLatticeParameterC(reader->readValue("LatticeParameterC", getLatticeParameterC()) );
  setStressAxis(reader->readFloatVec3("Stress Axis", getStressAxis() ) );
  setSubsurfaceDistance(reader->readValue("SubsurfaceDistance", getSubsurfaceDistance()) );
  setConsiderationFraction(reader->readValue("ConsiderationFraction", getConsiderationFraction()) );
  setDoNotAssumeInitiatorPresence(reader->readValue("DoNotAssumeInitiatorPresence", getDoNotAssumeInitiatorPresence()) );
  setInitiatorLowerThreshold(reader->readValue("InitiatorLowerThreshold", getInitiatorLowerThreshold()) );
  setInitiatorUpperThreshold(reader->readValue("InitiatorUpperThreshold", getInitiatorUpperThreshold()) );
  setHardFeatureLowerThreshold(reader->readValue("HardFeatureLowerThreshold", getHardFeatureLowerThreshold()) );
  setHardFeatureUpperThreshold(reader->readValue("HardFeatureUpperThreshold", getHardFeatureUpperThreshold()) );
  setSoftFeatureLowerThreshold(reader->readValue("SoftFeatureLowerThreshold", getSoftFeatureLowerThreshold()) );
  setSoftFeatureUpperThreshold(reader->readValue("SoftFeatureUpperThreshold", getSoftFeatureUpperThreshold()) );
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName() ) );
  setSelectedFeaturesArrayName(reader->readString("SelectedFeaturesArrayName", getSelectedFeaturesArrayName() ) );
  setInitiatorsArrayName(reader->readString("InitiatorsArrayName", getInitiatorsArrayName() ) );
  setHardFeaturesArrayName(reader->readString("HardFeaturesArrayName", getHardFeaturesArrayName() ) );
  setSoftFeaturesArrayName(reader->readString("SoftFeaturesArrayName", getSoftFeaturesArrayName() ) );
  setHardSoftGroupsArrayName(reader->readString("HardSoftGroupsArrayName", getHardSoftGroupsArrayName() ) );
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setCellFeatureAttributeMatrixPath(reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath()));
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName() ) );
  setCellParentIdsArrayName(reader->readString("CellParentIdsArrayName", getCellParentIdsArrayName() ) );
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TiDwellFatigueCrystallographicAnalysis::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(AlphaGlobPhasePresent)
  DREAM3D_FILTER_WRITE_PARAMETER(AlphaGlobPhase)
  DREAM3D_FILTER_WRITE_PARAMETER(MTRPhase)
  DREAM3D_FILTER_WRITE_PARAMETER(LatticeParameterA)
  DREAM3D_FILTER_WRITE_PARAMETER(LatticeParameterC)
  DREAM3D_FILTER_WRITE_PARAMETER(StressAxis)
  DREAM3D_FILTER_WRITE_PARAMETER(SubsurfaceDistance)
  DREAM3D_FILTER_WRITE_PARAMETER(ConsiderationFraction)
  DREAM3D_FILTER_WRITE_PARAMETER(DoNotAssumeInitiatorPresence)
  DREAM3D_FILTER_WRITE_PARAMETER(InitiatorLowerThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(InitiatorUpperThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(HardFeatureLowerThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(HardFeatureUpperThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(SoftFeatureLowerThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(SoftFeatureUpperThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedFeaturesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(InitiatorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(HardFeaturesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SoftFeaturesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(HardSoftGroupsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(ActiveArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NeighborListArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m.get()) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer newCellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = m->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }

  // Feature Data
  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSelectedFeaturesArrayName() );
  m_SelectedFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedFeatures = m_SelectedFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getInitiatorsArrayName() );
  m_InitiatorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_InitiatorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Initiators = m_InitiatorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getHardFeaturesArrayName() );
  m_HardFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_HardFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_HardFeatures = m_HardFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getSoftFeaturesArrayName() );
  m_SoftFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SoftFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SoftFeatures = m_SoftFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getHardSoftGroupsArrayName() );
  m_HardSoftGroupsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_HardSoftGroupsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_HardSoftGroups = m_HardSoftGroupsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getCellParentIdsArrayName() );
  m_CellParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixPath().getDataContainerName(), getCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getFeatureParentIdsArrayName() );
  m_FeatureParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNeighborListArrayPath(), dims);

  dims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // New Feature Data

  // Ensemble Data
  dims[0] = 1;
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::preflight()
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
void TiDwellFatigueCrystallographicAnalysis::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DREAM3D_RANDOMNG_NEW()

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t totalPoints = static_cast<size_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());

  bool subsurfaceFlag = false;
  bool hardfeatureFlag = false;
//  bool initiatorFlag = false;
//  bool softfeatureFlag = false;

  float random = 0.0f;

  // Normalize input stress axis
  MatrixMath::Normalize3x1(m_StressAxis.x, m_StressAxis.y, m_StressAxis.z);

  for (size_t i = 1; i < totalFeatures; ++i)
  {
    //  int stop = 0;
    random = static_cast<float>(rg.genrand_res53());
    if (random < m_ConsiderationFraction)
    {
      m_SelectedFeatures[i] = true;
      subsurfaceFlag = determine_subsurfacefeatures(i);
    }
    if (subsurfaceFlag == true)
    {
      // Determine if it's a hard feature
      if (m_FeaturePhases[i] == m_MTRPhase) { hardfeatureFlag = determine_hardfeatures(i); }
      // Determine if it's a soft feature only if it's not a hard feature
      if (hardfeatureFlag == false && m_FeaturePhases[i] == m_MTRPhase) { determine_softfeatures(i); }
      // Determine if it's an initiator only if we're assuming initiators are not necessarily present
      if (m_DoNotAssumeInitiatorPresence == true && m_FeaturePhases[i] == m_AlphaGlobPhase ) { determine_initiators(i); }
      subsurfaceFlag = false;
    }
  }

  for (size_t i = 1; i < totalFeatures; ++i)
  {
    // Group neighboring hard features and soft features
    if (m_HardFeatures[i] == true || m_SoftFeatures[i] == true)
    {
      group_flaggedfeatures(i);
    }
  }

  size_t fid = 0;

  // map grouped features to the cells
  for (size_t i = 0; i < totalPoints; ++i)
  {
    fid = m_FeatureParentIds[m_FeatureIds[i]];
    if (fid != -1)
    {
      m_CellParentIds[i] = fid;
    }
    else
    {
      m_CellParentIds[i] = m_FeatureIds[i];
    }
  }

  QString filtName = "FindNeighbors";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer findNeighborFactory = fm->getFactoryForFilter(filtName);

  DataArrayPath tempPath;

  if (NULL != findNeighborFactory.get() )
  {
    // If we get this far, the Factory is good so creating the filter should not fail unless something has
    // horribly gone wrong in which case the system is going to come down quickly after this.
    AbstractFilter::Pointer find_Neighbor = findNeighborFactory->create();

    // Connect up the Error/Warning/Progress object so the filter can report those things
    connect(find_Neighbor.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
            this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
    find_Neighbor->setDataContainerArray(getDataContainerArray()); // AbstractFilter implements this so no problem
    // Now set the filter parameters for the filter using QProperty System since we can not directly
    // instantiate the filter since it resides in a plugin. These calls are SLOW. DO NOT EVER do this in a
    // tight loop. Your filter will slow down by 10X.
    tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), "");
    QVariant v;
    v.setValue(tempPath);
    bool propWasSet = find_Neighbor->setProperty("CellFeatureAttributeMatrixPath", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("CellFeatureAttributeMatrixPath").arg(filtName);
      setErrorCondition(-109872);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getCellParentIdsArrayName() );
    v.setValue(tempPath);
    propWasSet = find_Neighbor->setProperty("FeatureIdsArrayPath", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("FeatureIdsArrayPath").arg(filtName);
      setErrorCondition(-109873);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    QString tempString("BoundaryCells");
    v.setValue(tempString);
    propWasSet = find_Neighbor->setProperty("BoundaryCellsArrayName", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("BoundaryCellsArrayName").arg(filtName);
      setErrorCondition(-109874);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    tempString = "SurfaceFeatures";
    v.setValue(tempString);
    propWasSet = find_Neighbor->setProperty("SurfaceFeaturesArrayName", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("SurfaceFeaturesArrayName").arg(filtName);
      setErrorCondition(-109875);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    v.setValue(false);
    propWasSet = find_Neighbor->setProperty("StoreBoundaryCells", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("StoreBoundaryCellsArrayName").arg(filtName);
      setErrorCondition(-109876);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    v.setValue(false);
    propWasSet = find_Neighbor->setProperty("StoreSurfaceFeatures", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("StoreSurfaceFeaturesArrayName").arg(filtName);
      setErrorCondition(-109877);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    tempString = "ParentNumNeighbors";
    v.setValue(tempString);
    propWasSet = find_Neighbor->setProperty("NumNeighborsArrayName", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("ParentNumNeighbors").arg(filtName);
      setErrorCondition(-109878);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    tempString = "ParentNeighborList";
    v.setValue(tempString);
    propWasSet = find_Neighbor->setProperty("NeighborListArrayName", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("ParentNeighborList").arg(filtName);
      setErrorCondition(-109879);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

    tempString = "ParentSharedSurfaceAreaList";
    v.setValue(tempString);
    propWasSet = find_Neighbor->setProperty("SharedSurfaceAreaListArrayName", v);
    if(false == propWasSet)
    {
      QString ss = QObject::tr("Ti Dwell Fatigue Error Setting Property '%1' into filter '%2' which is a subfilter called by Ti Dwell Fatigue Error. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers.").arg("ParentSharedSurfaceAreaList").arg(filtName);
      setErrorCondition(-109880);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    find_Neighbor->execute();
  }

  QVector<size_t> cDims(1, 1);
  tempPath.update(getFeatureEulerAnglesArrayPath().getDataContainerName(), getFeatureEulerAnglesArrayPath().getAttributeMatrixName(), "ParentNeighborList");
  m_ParentNeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, tempPath, cDims);

  for (size_t i = 1; i < totalFeatures; ++i)
  {
    // only proceed if it's either an MTR or alpha glob
    if (m_FeaturePhases[i] == m_MTRPhase || (m_FeaturePhases[i] == m_AlphaGlobPhase && m_DoNotAssumeInitiatorPresence == true))
    {
      // Determine if it's a hard-soft pair only if there the current ID is either a hardfeature, initiator or soft feature
      if (m_Initiators[i] == true || m_HardFeatures[i] == true || m_SoftFeatures[i] == true) { assign_HardSoftGroups(i); }
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TiDwellFatigueCrystallographicAnalysis::determine_subsurfacefeatures(int index)
{
  // using feature euler angles simply because it's available
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureEulerAnglesArrayPath.getDataContainerName());

  bool subsurfaceFlag = false;

  int xPoints = static_cast<int>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int yPoints = static_cast<int>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int zPoints = static_cast<int>(m->getGeometryAs<ImageGeom>()->getZPoints());
  float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
  float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
  float zRes = m->getGeometryAs<ImageGeom>()->getZRes();
  float xyzScaledDimension[3] = {xPoints * xRes, yPoints * yRes, zPoints * zRes};

  // check if current feature centroid is within the subsurface defined centroid
  if ( m_Centroids[3 * index + 0] >= m_SubsurfaceDistance && m_Centroids[3 * index + 0] <= (xyzScaledDimension[0] - m_SubsurfaceDistance)
       && m_Centroids[3 * index + 1] >= m_SubsurfaceDistance && m_Centroids[3 * index + 1] <= (xyzScaledDimension[1] - m_SubsurfaceDistance)
       && m_Centroids[3 * index + 2] >= m_SubsurfaceDistance && m_Centroids[3 * index + 2] <= (xyzScaledDimension[2] - m_SubsurfaceDistance))
  {
    subsurfaceFlag = true;
  }
  if (subsurfaceFlag == true) { return true; }
  else { return false; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TiDwellFatigueCrystallographicAnalysis::determine_hardfeatures(int index)
{
  float g[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float w = 0.0f;
  const int hardfeaturePlane[12][4] =
  {
    { -1, 0, 1, 7},
    { -1, 1, 0, -7},
    {0, -1, 1, 7},
    {1, 0, -1, 7},
    {1, -1, 0, -7},
    {0, 1, -1, 7},
    {0, -1, 1, -7},
    {0, 1, -1, -7},
    {1, 0, -1, -7},
    { -1, 0, 1, -7},
    { -1, 1, 0, 7},
    {1, -1, 0, 7}
  };
  float hardfeaturePlaneNormal[12][3];
  for(size_t i = 0; i < 12; i++) { for(size_t j = 0; j < 3; j++) { hardfeaturePlaneNormal[i][j] = 0.0f; } }
  const float m_OneOverA = 1 / m_LatticeParameterA;
  const float m_OneOverAxSqrtThree = 1 / (m_LatticeParameterA * sqrtf(3.0f));
  const float m_OneOverC = 1 / m_LatticeParameterC;
  bool hardfeatureFlag = false;

  // convert Miller-Bravais to unit normal
  for (int j = 0; j < 12; ++j)
  {
    hardfeaturePlaneNormal[j][0] = hardfeaturePlane[j][0] * m_OneOverA;
    hardfeaturePlaneNormal[j][1] = (2.0f * hardfeaturePlane[j][1] + hardfeaturePlane[j][0]) * m_OneOverAxSqrtThree;
    hardfeaturePlaneNormal[j][2] = hardfeaturePlane[j][3] * m_OneOverC;
  }

  FOrientArrayType om(9, 0.0);
  FOrientTransformsType::eu2om(FOrientArrayType( &(m_FeatureEulerAngles[3 * index + 0]), 3), om);
  om.toGMatrix(g);
  if (m_FeaturePhases[index] == m_MTRPhase)
  {
    for (int j = 0; j < 12; ++j)
    {
      w = find_angle(g, hardfeaturePlaneNormal[j][0], hardfeaturePlaneNormal[j][1], hardfeaturePlaneNormal[j][2]);
      if (w >= m_HardFeatureLowerThreshold && w <= m_HardFeatureUpperThreshold)
      {
        m_HardFeatures[index] = true;
        hardfeatureFlag = true;
        break;
      }
    }
  }
  if (hardfeatureFlag == true) { return true; }
  else { return false; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::determine_initiators(int index)
{
  const float caxis[3] = {0.0f, 0.0f, 1.0f};
  bool initiatorFlag = false;
  float w = 0.0f;
  float g[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  if (m_FeaturePhases[index] == m_AlphaGlobPhase)
  {
    FOrientArrayType om(9, 0.0);
    FOrientTransformsType::eu2om(FOrientArrayType(&(m_FeatureEulerAngles[3 * index + 0]), 3), om);
    om.toGMatrix(g);

    w = find_angle(g, caxis[0], caxis[1], caxis[2]);
    if (w >= m_InitiatorLowerThreshold && w <= m_InitiatorUpperThreshold)
    {
      m_Initiators[index] = true;
      initiatorFlag = true;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::determine_softfeatures(int index)
{
  const float caxis[3] = {0.0f, 0.0f, 1.0f};
  float w = 0.0f;
  float g[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  //bool softfeatureFlag = false;

  if (m_FeaturePhases[index] == m_MTRPhase)
  {
    FOrientArrayType om(9, 0.0);
    FOrientTransformsType::eu2om(FOrientArrayType(&(m_FeatureEulerAngles[3 * index + 0]), 3), om);
    om.toGMatrix(g);

    w = find_angle(g, caxis[0], caxis[1], caxis[2]);
    if (w >= m_SoftFeatureLowerThreshold && w <= m_SoftFeatureUpperThreshold)
    {
      m_SoftFeatures[index] = true;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::group_flaggedfeatures(int index)
{
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *(m_NeighborList.lock());

  for (int j = 0; j < neighborlist[index].size(); ++j)
  {
    if (m_FeatureParentIds[index] != neighborlist[index][j] && ((m_HardFeatures[index] == true && m_HardFeatures[neighborlist[index][j]] == true) || (m_SoftFeatures[index] == true && m_SoftFeatures[neighborlist[index][j]])))
    {
      if (m_FeatureParentIds[neighborlist[index][j]] == -1)
      {
        if (m_FeatureParentIds[index] == -1) { m_FeatureParentIds[neighborlist[index][j]] = index; }
        if (m_FeatureParentIds[index] != -1) { m_FeatureParentIds[neighborlist[index][j]] = m_FeatureParentIds[index]; }
        group_flaggedfeatures(neighborlist[index][j]);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::assign_HardSoftGroups(int index)
{
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& parentneighborlist = *(m_ParentNeighborList.lock());

  bool hardfeatureFlag = false;
  bool initiatorFlag = false;
  bool softfeatureFlag = false;
  int hardfeatureIndex = 0;
  int softfeatureIndex = 0;

  for (int j = 0; j < parentneighborlist[index].size(); ++j)
  {
    if ((m_DoNotAssumeInitiatorPresence == true && m_FeaturePhases[index] == m_AlphaGlobPhase) || m_FeaturePhases[parentneighborlist[index][j]] == m_MTRPhase)
    {
      if (m_Initiators[index] == true && initiatorFlag == false && m_DoNotAssumeInitiatorPresence == true && m_FeaturePhases[index] == m_AlphaGlobPhase)
      {
        initiatorFlag = true;
      }
      if (m_Initiators[parentneighborlist[index][j]] == true && initiatorFlag == false && m_DoNotAssumeInitiatorPresence == true && m_FeaturePhases[parentneighborlist[index][j]] == m_AlphaGlobPhase)
      {
        initiatorFlag = true;
      }
      if (m_HardFeatures[index] == true && hardfeatureFlag == false && m_FeaturePhases[index] == m_MTRPhase)
      {
        hardfeatureFlag = true;
        hardfeatureIndex = index;
      }
      if (m_HardFeatures[parentneighborlist[index][j]] == true && hardfeatureFlag == false && m_FeaturePhases[parentneighborlist[index][j]] == m_MTRPhase)
      {
        hardfeatureFlag = true;
        hardfeatureIndex = parentneighborlist[index][j];
      }
      if (m_SoftFeatures[index] == true && softfeatureFlag == false && m_FeaturePhases[index] == m_MTRPhase)
      {
        softfeatureFlag = true;
        softfeatureIndex = index;
      }
      if (m_SoftFeatures[parentneighborlist[index][j]] == true && softfeatureFlag == false && m_FeaturePhases[parentneighborlist[index][j]] == m_MTRPhase)
      {
        softfeatureFlag = true;
        softfeatureIndex = parentneighborlist[index][j];
      }
      // only flag as a hard-soft pair if there's a neighboring group of initiator - hardfeature - soft feature and either the current index of the hardfeature or
      // soft feature have not already been flagged as a bad acting pair
      if ((m_DoNotAssumeInitiatorPresence == false || initiatorFlag == true) && hardfeatureFlag == true && softfeatureFlag == true && (m_HardSoftGroups[hardfeatureIndex] == false || m_HardSoftGroups[softfeatureIndex] == false))
      {
        m_HardSoftGroups[hardfeatureIndex] = true;
        m_HardSoftGroups[softfeatureIndex] = true;
        break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float TiDwellFatigueCrystallographicAnalysis::find_angle(float g[3][3], float planeNormalU, float planeNormalV, float planeNormalW)
{
  float gt[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float v[3] = {0};
  float sampleLoading[3] = {m_StressAxis.x, m_StressAxis.y, m_StressAxis.z};
  float w = 0.0f;

  MatrixMath::Normalize3x1(planeNormalU, planeNormalV, planeNormalW);
  float planeNormal[3] = {planeNormalU, planeNormalV, planeNormalW};
  MatrixMath::Transpose3x3(g, gt);
  MatrixMath::Multiply3x3with3x1(gt, planeNormal, v);
  // Normalize so that the magnitude is 1
  MatrixMath::Normalize3x1(v);
  if(v[2] < 0) { MatrixMath::Multiply3x1withConstant(v, -1); }
  w = GeometryMath::CosThetaBetweenVectors(v, sampleLoading);
  w = acos(w);
  // Convert from radian to degrees
  return w *= DREAM3D::Constants::k_180OverPi;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer TiDwellFatigueCrystallographicAnalysis::newFilterInstance(bool copyFilterParameters)
{
  TiDwellFatigueCrystallographicAnalysis::Pointer filter = TiDwellFatigueCrystallographicAnalysis::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TiDwellFatigueCrystallographicAnalysis::getCompiledLibraryName()
{
  return TransformationPhaseConstants::TransformationPhaseBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TiDwellFatigueCrystallographicAnalysis::getGroupName()
{
  return DREAM3D::FilterGroups::Unsupported;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TiDwellFatigueCrystallographicAnalysis::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::CrystallographicFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString TiDwellFatigueCrystallographicAnalysis::getHumanLabel()
{
  return "Ti Dwell Fatigue Crystallographic Analysis";
}
