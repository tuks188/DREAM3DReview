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
#include "MicrotextureFatigueAnalysis.h"

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
MicrotextureFatigueAnalysis::MicrotextureFatigueAnalysis() :
  AbstractFilter(),
  m_InitiatorsArrayName(TransformationPhase::Initiators),
  m_Initiators(NULL),
  m_PropagatorsArrayName(TransformationPhase::Propagators),
  m_Propagators(NULL),
  m_BadActorsArrayName(TransformationPhase::BadActors),
  m_BadActors(NULL),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_EulerAnglesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_EulerAngles(NULL),
  m_PhasesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_Phases(NULL),
  m_NeighborListArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::NeighborList),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
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
MicrotextureFatigueAnalysis::~MicrotextureFatigueAnalysis()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrotextureFatigueAnalysis::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Stress Axis", "StressAxis", FilterParameterWidgetType::FloatVec3Widget, getStressAxis(), false));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Euler Angles", "EulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Phases", "PhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("NeighborList Array", "NeighborListArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getNeighborListArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));

  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Initiators Array Name", "InitiatorsArrayName", FilterParameterWidgetType::StringWidget, getInitiatorsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Propagators Array Name", "PropagatorsArrayName", FilterParameterWidgetType::StringWidget, getPropagatorsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Bad Actors Array Name", "BadActorsArrayName", FilterParameterWidgetType::StringWidget, getBadActorsArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrotextureFatigueAnalysis::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setStressAxis(reader->readFloatVec3("Stress Axis", getStressAxis() ) );
  setInitiatorsArrayName(reader->readString("InitiatorsArrayName", getInitiatorsArrayName() ) );
  setPropagatorsArrayName(reader->readString("PropagatorsArrayName", getPropagatorsArrayName() ) );
  setBadActorsArrayName(reader->readString("BadActorsArrayName", getBadActorsArrayName() ) );
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setEulerAnglesArrayPath(reader->readDataArrayPath("EulerAnglesArrayPath", getEulerAnglesArrayPath() ) );
  setPhasesArrayPath(reader->readDataArrayPath("PhasesArrayPath", getPhasesArrayPath() ) );
  setNeighborListArrayPath( reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MicrotextureFatigueAnalysis::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(StressAxis)
  DREAM3D_FILTER_WRITE_PARAMETER(InitiatorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(PropagatorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(BadActorsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(EulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(PhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NeighborListArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrotextureFatigueAnalysis::dataCheck()
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

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getBadActorsArrayName() );
  m_BadActorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_BadActorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_BadActors = m_BadActorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_EulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EulerAngles = m_EulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_PhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Phases = m_PhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNeighborListArrayPath(), dims);

  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrotextureFatigueAnalysis::preflight()
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
void MicrotextureFatigueAnalysis::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }
  
  size_t totalFeatures = m_PhasesPtr.lock()->getNumberOfTuples();

  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *(m_NeighborList.lock());

  // Normalize input stress axis
  MatrixMath::Normalize3x1(m_StressAxis.x, m_StressAxis.y, m_StressAxis.z);
  float sampleLoading[3] = {m_StressAxis.x, m_StressAxis.y, m_StressAxis.z};
  float g[3][3] = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
  float gt[3][3] = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};
  float caxis[3] = {0.0f,0.0f,1.0f};
  float c[3] = {0.0f,0.0f,1.0f};
  float v[3] = {0.0f,0.0f,0.0f};
  float w = 0.0f;
  float initiatorPlane[3] = {1,0,7}; 
  /*  
  float initiatorPlane[24][3] = 
  {{1,0,7},
  {0,1,7},
  {1,7,0},
  {0,7,1},
  {7,1,0},
  {7,0,1},
  {-1,0,7},
  {0,-1,7},
  {-1,7,0},
  {0,7,-1},
  {7,-1,0},
  {7,0,-1},
  {1,0,-7},
  {0,1,-7},
  {1,-7,0},
  {0,-7,1},
  {-7,1,0},
  {-7,0,1},
  {-1,0,-7},
  {0,-1,-7},
  {-1,-7,0},
  {0,-7,-1},
  {-7,-1,0},
  {-7,0,-1}};
  */

  for (int i = 0; i < totalFeatures; ++i)
  {
	OrientationMath::EulertoMat(m_EulerAngles[3*i+0], m_EulerAngles[3*i+1], m_EulerAngles[3*i+2], g);
//	for (int j = 0; j < 24; ++j)
//	{
	  // Determine if it's an initiator
	  MatrixMath::Transpose3x3(g, gt);
//	  MatrixMath::Multiply3x3with3x1(gt, initiatorPlane[j], v);
	  MatrixMath::Multiply3x3with3x1(gt, initiatorPlane, v);
	  // Normalize so that the magnitude is 1
	  MatrixMath::Normalize3x1(v);
	  if(v[2] < 0) { MatrixMath::Multiply3x1withConstant(v, -1); }
	  w = GeometryMath::CosThetaBetweenVectors(v, sampleLoading);
	  w = acos(w);
	  // Convert from radian to degrees
	  w *= DREAM3D::Constants::k_180OverPi;
	  if (w <= 30) { m_Initiators[i] = true; }

      // Determine if it's a propagator
	  MatrixMath::Multiply3x3with3x1(gt, caxis, c);
	  // Normalize so that the magnitude is 1
	  MatrixMath::Normalize3x1(c);
	  if(c[2] < 0) { MatrixMath::Multiply3x1withConstant(c, -1); }
	  w = GeometryMath::CosThetaBetweenVectors(c, sampleLoading);
	  w = acos(w);
	  // Convert from radian to degrees
	  w *= DREAM3D::Constants::k_180OverPi;
	  if (w >= 70 && w <= 90) { m_Propagators[i] = true; }
//	}
  }

  // Determine bad actors
  for (int i = 0; i < totalFeatures; ++i)
  {
	if (m_Initiators[i] == true)
	{
	  // if the current grain is an iniator then cycle through its neighbors to 
	  // see if any are also propagators then flag those pairs as bad actors
  	  for (int j = 0; j < neighborlist[i].size(); ++j)
	  {
		if (m_Propagators[j] == true) { m_BadActors[i] = true; m_BadActors[j] = true; }
	  }
	}
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MicrotextureFatigueAnalysis::newFilterInstance(bool copyFilterParameters)
{
  MicrotextureFatigueAnalysis::Pointer filter = MicrotextureFatigueAnalysis::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}