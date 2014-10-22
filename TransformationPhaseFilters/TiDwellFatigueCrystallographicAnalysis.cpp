/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
#include "TransformationPhase/TransformationPhaseConstants.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Math/GeometryMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TiDwellFatigueCrystallographicAnalysis::TiDwellFatigueCrystallographicAnalysis() :
  AbstractFilter(),
  m_AlphaGlobPhasePresent(true),
  m_AlphaGlobPhase(1),
  m_MTRPhase(2),
  m_LatticeParameterA(2.9131f),
  m_LatticeParameterC(4.6572f),
  m_SubsurfaceDistance(0),
  m_DoNotAssumeInitiatorPresence(true),
  m_InitiatorLowerThreshold(40.0f),
  m_InitiatorUpperThreshold(50.0f),
  m_PropagatorLowerThreshold(0.0f),
  m_PropagatorUpperThreshold(25.0f),
  m_SoftFeatureLowerThreshold(70.0f),
  m_SoftFeatureUpperThreshold(90.0f),
  m_InitiatorsArrayName(TransformationPhase::Initiators),
  m_PropagatorsArrayName(TransformationPhase::Propagators),
  m_SoftFeaturesArrayName(TransformationPhase::SoftFeatures),
  m_BadActorsArrayName(TransformationPhase::BadActors),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_NeighborListArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::NeighborList),
  m_NeighborhoodListArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::NeighborhoodList),
  m_CentroidsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_Initiators(NULL),
  m_Propagators(NULL),
  m_SoftFeatures(NULL),
  m_BadActors(NULL),
  m_FeatureParentIds(NULL),
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
  parameters.push_back(LinkedBooleanFilterParameter::New("Alpha Glob Phase Present", "AlphaGlobPhasePresent", getAlphaGlobPhasePresent(), linkedProps1, false));
  parameters.push_back(FilterParameter::New("Alpha Glob Phase Number", "AlphaGlobPhase", FilterParameterWidgetType::IntWidget, getAlphaGlobPhase(), false, ""));
  parameters.push_back(FilterParameter::New("Microtextured Region Phase Number", "MTRPhase", FilterParameterWidgetType::IntWidget, getMTRPhase(), false, ""));
  parameters.push_back(FilterParameter::New("Lattice Parameter A", "LatticeParameterA", FilterParameterWidgetType::DoubleWidget, getLatticeParameterA(), false, ""));
  parameters.push_back(FilterParameter::New("Lattice Parameter C", "LatticeParameterC", FilterParameterWidgetType::DoubleWidget, getLatticeParameterC(), false, ""));
  parameters.push_back(FilterParameter::New("Stress Axis", "StressAxis", FilterParameterWidgetType::FloatVec3Widget, getStressAxis(), false));
  parameters.push_back(FilterParameter::New("Subsurface Feature Distance To Consider", "SubsurfaceDistance", FilterParameterWidgetType::IntWidget, getSubsurfaceDistance(), false, "Microns"));
  QStringList linkedProps2;
  linkedProps2 << "InitiatorLowerThreshold" << "InitiatorUpperThreshold";
  parameters.push_back(LinkedBooleanFilterParameter::New("Do Not Assume Initiator Presence", "DoNotAssumeInitiatorPresence", getDoNotAssumeInitiatorPresence(), linkedProps2, false));
  parameters.push_back(FilterParameter::New("Initiator Lower Threshold", "InitiatorLowerThreshold", FilterParameterWidgetType::DoubleWidget, getInitiatorLowerThreshold(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Initiator Upper Threshold", "InitiatorUpperThreshold", FilterParameterWidgetType::DoubleWidget, getInitiatorUpperThreshold(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Propagator Lower Threshold", "PropagatorLowerThreshold", FilterParameterWidgetType::DoubleWidget, getPropagatorLowerThreshold(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Propagator Upper Threshold", "PropagatorUpperThreshold", FilterParameterWidgetType::DoubleWidget, getPropagatorUpperThreshold(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Soft Feature Lower Threshold", "SoftFeatureLowerThreshold", FilterParameterWidgetType::DoubleWidget, getSoftFeatureLowerThreshold(), false, "Degrees"));
  parameters.push_back(FilterParameter::New("Soft Feature Upper Threshold", "SoftFeatureUpperThreshold", FilterParameterWidgetType::DoubleWidget, getSoftFeatureUpperThreshold(), false, "Degrees"));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Euler Angles", "FeatureEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Feature FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("NeighborList Array", "NeighborListArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getNeighborListArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("NeighborhoodList Array", "NeighborhoodListArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getNeighborhoodListArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Centroids", "CentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCentroidsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));

  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Initiators Array Name", "InitiatorsArrayName", FilterParameterWidgetType::StringWidget, getInitiatorsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Propagators Array Name", "PropagatorsArrayName", FilterParameterWidgetType::StringWidget, getPropagatorsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Soft Features Array Name", "SoftFeaturesArrayName", FilterParameterWidgetType::StringWidget, getSoftFeaturesArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Bad Actors Array Name", "BadActorsArrayName", FilterParameterWidgetType::StringWidget, getBadActorsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Parent Ids", "FeatureParentIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureParentIdsArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAlphaGlobPhasePresent(reader->readValue("AlphaGlobPhasePresent", getAlphaGlobPhasePresent()) );
  setAlphaGlobPhase(reader->readValue("AlphaGlobPhase", getAlphaGlobPhase()) );
  setMTRPhase(reader->readValue("MTRPhase", getMTRPhase()) );
  setLatticeParameterA(reader->readValue("LatticeParameterA", getLatticeParameterA()) );
  setLatticeParameterC(reader->readValue("LatticeParameterC", getLatticeParameterC()) );
  setStressAxis(reader->readFloatVec3("Stress Axis", getStressAxis() ) );
  setSubsurfaceDistance(reader->readValue("SubsurfaceDistance", getSubsurfaceDistance()) );
  setDoNotAssumeInitiatorPresence(reader->readValue("DoNotAssumeInitiatorPresence", getDoNotAssumeInitiatorPresence()) );
  setInitiatorLowerThreshold(reader->readValue("InitiatorLowerThreshold", getInitiatorLowerThreshold()) );
  setInitiatorUpperThreshold(reader->readValue("InitiatorUpperThreshold", getInitiatorUpperThreshold()) );
  setPropagatorLowerThreshold(reader->readValue("PropagatorLowerThreshold", getPropagatorLowerThreshold()) );
  setPropagatorUpperThreshold(reader->readValue("PropagatorUpperThreshold", getPropagatorUpperThreshold()) );
  setSoftFeatureLowerThreshold(reader->readValue("SoftFeatureLowerThreshold", getSoftFeatureLowerThreshold()) );
  setSoftFeatureUpperThreshold(reader->readValue("SoftFeatureUpperThreshold", getSoftFeatureUpperThreshold()) );
  setInitiatorsArrayName(reader->readString("InitiatorsArrayName", getInitiatorsArrayName() ) );
  setPropagatorsArrayName(reader->readString("PropagatorsArrayName", getPropagatorsArrayName() ) );
  setSoftFeaturesArrayName(reader->readString("SoftFeaturesArrayName", getSoftFeaturesArrayName() ) );
  setBadActorsArrayName(reader->readString("BadActorsArrayName", getBadActorsArrayName() ) );
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName() ) );
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setNeighborhoodListArrayPath(reader->readDataArrayPath("NeighborhoodListArrayPath", getNeighborhoodListArrayPath()));
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
  DREAM3D_FILTER_WRITE_PARAMETER(AlphaGlobPhasePresent)
  DREAM3D_FILTER_WRITE_PARAMETER(AlphaGlobPhase)
  DREAM3D_FILTER_WRITE_PARAMETER(MTRPhase)
  DREAM3D_FILTER_WRITE_PARAMETER(LatticeParameterA)
  DREAM3D_FILTER_WRITE_PARAMETER(LatticeParameterC)
  DREAM3D_FILTER_WRITE_PARAMETER(StressAxis)
  DREAM3D_FILTER_WRITE_PARAMETER(SubsurfaceDistance)
  DREAM3D_FILTER_WRITE_PARAMETER(DoNotAssumeInitiatorPresence)
  DREAM3D_FILTER_WRITE_PARAMETER(InitiatorLowerThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(InitiatorUpperThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(PropagatorLowerThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(PropagatorUpperThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(SoftFeatureLowerThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(SoftFeatureUpperThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(InitiatorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(PropagatorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SoftFeaturesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(BadActorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NeighborListArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NeighborhoodListArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  // Feature Data
  QVector<size_t> dims(1, 1);
  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getInitiatorsArrayName() );
  m_InitiatorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_InitiatorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Initiators = m_InitiatorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getPropagatorsArrayName() );
  m_PropagatorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PropagatorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Propagators = m_PropagatorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getSoftFeaturesArrayName() );
  m_SoftFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SoftFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SoftFeatures = m_SoftFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getBadActorsArrayName() );
  m_BadActorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BadActorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_BadActors = m_BadActorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getFeatureParentIdsArrayName() );
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

  m_NeighborhoodList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNeighborhoodListArrayPath(), dims);

  dims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

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

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  bool subsurfaceFlag = false;
  bool propagatorFlag = false;
  bool initiatorFlag = false;
  bool softfeatureFlag = false;

  // Normalize input stress axis
  MatrixMath::Normalize3x1(m_StressAxis.x, m_StressAxis.y, m_StressAxis.z);

  for (int i = 1; i < totalFeatures; ++i)
  {
	subsurfaceFlag = determine_subsurfacefeatures(i);
	if (subsurfaceFlag == true) 
	{ 
	  // Determine if it's a propagator
	  if (m_FeaturePhases[i] == m_MTRPhase) { propagatorFlag = determine_propagators(i); }
	  // Determine if it's a soft feature only if it's not a propagator
	  if (propagatorFlag == false && m_FeaturePhases[i] == m_MTRPhase) { determine_softfeatures(i); }
	  // Determine if it's an initiator only if we're assuming initiators are not necessarily present
	  if (m_DoNotAssumeInitiatorPresence == false && m_FeaturePhases[i] == m_AlphaGlobPhase ) { determine_initiators(i); }
	}
  }

  for (int i = 1; i < totalFeatures; ++i)
  {
	// Determine if it's a hard-soft pair only if there the current ID is either a propagator, initiator or soft feature
	if (m_Initiators[i] == true || m_Propagators[i] == true || m_SoftFeatures[i] == true) { assign_badactors(i); }
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_FeatureEulerAnglesArrayPath.getDataContainerName());

  bool subsurfaceFlag = false;

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  float xyzScaledDimension[3] = {xPoints*xRes, yPoints*yRes, zPoints*zRes};

  // check if current feature centroid is within the subsurface defined centroid
  if ( m_Centroids[3*index+0] > m_SubsurfaceDistance && m_Centroids[3*index+0] < (xyzScaledDimension[0] - m_SubsurfaceDistance)
  && m_Centroids[3*index+1] > m_SubsurfaceDistance && m_Centroids[3*index+1] < (xyzScaledDimension[1] - m_SubsurfaceDistance)
  && m_Centroids[3*index+2] > m_SubsurfaceDistance && m_Centroids[3*index+2] < (xyzScaledDimension[2] - m_SubsurfaceDistance))
  {
	subsurfaceFlag = true;
  }

  if (subsurfaceFlag == true) { return true; }
  else { return false; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TiDwellFatigueCrystallographicAnalysis::determine_propagators(int index)
{
  float g[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};
  float w = 0.0f;
  const int propagatorPlane[12][4] =
  {{-1,0,1,7},
  {-1,1,0,-7},
  {0,-1,1,7},
  {1,0,-1,7},
  {1,-1,0,-7},
  {0,1,-1,7},
  {0,-1,1,-7},
  {0,1,-1,-7},
  {1,0,-1,-7},
  {-1,0,1,-7},
  {-1,1,0,7},
  {1,-1,0,7}};
  float propagatorPlaneNormal[12][3];
  for(size_t i = 0; i < 12; i++) { for(size_t j = 0; j < 3; j++) { propagatorPlaneNormal[i][j] = 0.0f; } }
  const float m_OneOverA = 1 / m_LatticeParameterA;
  const float m_OneOverAxSqrtThree = 1 / (m_LatticeParameterA * sqrtf(3.0f));
  const float m_OneOverC = 1 / m_LatticeParameterC;
  bool propagatorFlag = false;

  // convert Miller-Bravais to unit normal
  for (int j = 0; j < 12; ++j)
  {
	propagatorPlaneNormal[j][0] = propagatorPlane[j][0] * m_OneOverA;
	propagatorPlaneNormal[j][1] = (2.0f * propagatorPlane[j][1] + propagatorPlane[j][0]) * m_OneOverAxSqrtThree;
	propagatorPlaneNormal[j][2] = propagatorPlane[j][3] * m_OneOverC;
  }

  OrientationMath::EulertoMat(m_FeatureEulerAngles[3*index+0], m_FeatureEulerAngles[3*index+1], m_FeatureEulerAngles[3*index+2], g);
  if (m_FeaturePhases[index] == m_MTRPhase)
	{
	for (int j = 0; j < 12; ++j)
	{
	  w = find_angle(g, propagatorPlaneNormal[j][0], propagatorPlaneNormal[j][1], propagatorPlaneNormal[j][2]);
	  if (w >= m_PropagatorLowerThreshold && w <= m_PropagatorUpperThreshold)
	  {
		m_Propagators[index] = true;
		propagatorFlag = true;
		break;
      }
	}
  }
  if (propagatorFlag == true) { return true; }
  else { return false; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TiDwellFatigueCrystallographicAnalysis::determine_initiators(int index)
{
  const float caxis[3] = {0.0f,0.0f,1.0f};
  bool initiatorFlag = false;
  float w = 0.0f;
  float g[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};

  if (m_FeaturePhases[index] == m_AlphaGlobPhase)
  {
	OrientationMath::EulertoMat(m_FeatureEulerAngles[3*index+0], m_FeatureEulerAngles[3*index+1], m_FeatureEulerAngles[3*index+2], g);
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
  const float caxis[3] = {0.0f,0.0f,1.0f};
  float w = 0.0f;
  float g[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};
  bool softfeatureFlag = false;

  if (m_FeaturePhases[index] == m_MTRPhase)
  {
	OrientationMath::EulertoMat(m_FeatureEulerAngles[3*index+0], m_FeatureEulerAngles[3*index+1], m_FeatureEulerAngles[3*index+2], g);
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
void TiDwellFatigueCrystallographicAnalysis::assign_badactors(int index)
{
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *(m_NeighborList.lock());

  bool propagatorFlag = false;
  bool initiatorFlag = false;
  bool softfeatureFlag = false;
  int initiatorIndex = 0;
  int propagatorIndex = 0;
  int softfeatureIndex = 0;

  for (int j = 0; j < neighborlist[index].size(); ++j)
  {
	if (m_Initiators[index] == true && initiatorFlag == false)
	{
	  initiatorFlag = true; 
	  initiatorIndex = index;
	}
	if (m_Initiators[neighborlist[index][j]] == true && initiatorFlag == false) 
	{ 
	  initiatorFlag = true; 
	  initiatorIndex = j;
	}
	if (m_Propagators[index] == true && propagatorFlag == false)
	{
	  propagatorFlag = true; 
	  propagatorIndex = index;
	}
	if (m_Propagators[neighborlist[index][j]] == true && propagatorFlag == false) 
	{ 
	  propagatorFlag = true; 
	  propagatorIndex = j;
	}
	if (m_SoftFeatures[index] == true && softfeatureFlag == false)
	{
	  softfeatureFlag = true; 
	  softfeatureIndex = index;
	}
	if (m_SoftFeatures[neighborlist[index][j]] == true && softfeatureFlag == false)
	{ 
	  softfeatureFlag = true; 
	  softfeatureIndex = j;
	}
	// only flag as a bad acting pair if there's a neighboring group of initiator - propagator - soft feature and either the current index of the propagator or
	// soft feature have not already been flagged as a bad acting pair
	if ((m_DoNotAssumeInitiatorPresence == false || initiatorFlag == true) && propagatorFlag == true && softfeatureFlag == true && (m_BadActors[propagatorIndex] == false || m_BadActors[softfeatureIndex] == false))
	{
	  m_BadActors[propagatorIndex] = true;
	  m_BadActors[softfeatureIndex] = true;
	  break;
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
