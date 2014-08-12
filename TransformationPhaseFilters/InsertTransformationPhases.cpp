/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
 * Copyright (c) 2014 Sudipto Mandal & Prof. Anthony D. Rollett (Carnegie Mellon
 * University)
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
 * Sudipto Mandal, Anthony D. Rollett, UES, Inc., the US Air Force, BlueQuartz
 * Software, Carnegie Mellon University nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
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
 *     FA8650-07-D-5800 and FA8650-10-D-5226 and The Boeing Company
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "InsertTransformationPhases.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include "Plugins/Generic/GenericFilters/FindFeatureCentroids.h"
#include "Plugins/Statistics/StatisticsFilters/FindNeighbors.h"
#include "Plugins/Generic/GenericFilters/FindSurfaceFeatures.h"
#include "Plugins/Statistics/StatisticsFilters/FindSizes.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = static_cast<float>(M_PI);

#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertTransformationPhases::InsertTransformationPhases() :
  AbstractFilter(),
  m_TransCrystalStruct(Ebsd::CrystalStructure::UnknownCrystalStructure),
  m_TransformationPhaseMisorientation(60.0f),
  m_DefineHabitPlane(true),
  m_UseAllVariants(true),
  m_CoherentFrac(1.0f),
  m_TransformationPhaseThickness(0.2f),
  m_NumTransformationPhasesPerFeature(1),
  m_PeninsulaFrac(0.0f),
  m_StatsGenCellEnsembleAttributeMatrixPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_VolCellEnsembleAttributeMatrixPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_CentroidsArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_EquivalentDiametersArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EquivalentDiameters),
  m_FeatureEulerAnglesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::EulerAngles),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_FeatureParentIdsArrayName(DREAM3D::FeatureData::ParentIds),
  m_NumFeaturesPerParentArrayName(DREAM3D::FeatureData::NumFeaturesPerParent),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_PhaseTypesArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_ShapeTypesArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::ShapeTypes),
  m_NumFeaturesArrayPath(DREAM3D::Defaults::SyntheticVolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::NumFeatures),
  m_FeatureIds(NULL),
  m_CellEulerAngles(NULL),
  m_AvgQuats(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhases(NULL),
  m_FeatureParentIds(NULL),
  m_NumFeaturesPerParent(NULL),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL),
  m_ShapeTypes(NULL),
  m_NumFeaturesArrayName(DREAM3D::EnsembleData::NumFeatures),
  m_NumFeatures(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
  m_TransformationPhaseHabitPlane.x = 1.0f;
  m_TransformationPhaseHabitPlane.y = 1.0f;
  m_TransformationPhaseHabitPlane.z = 1.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertTransformationPhases::~InsertTransformationPhases()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Transformation Phase Crystal Structure");
    option->setPropertyName("TransCrystalStruct");
    option->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    //option->setValueType("unsigned int");
    QVector<QString> choices;
    // The choices here are IN ORDER of the enumerations from the EBSDLib. DO NOT CHANGE THE ORDER.
    choices.push_back("Hexagonal-High 6/mmm");
    choices.push_back("Cubic-High m-3m");
    choices.push_back("Hexagonal-Low 6/m");
    choices.push_back("Cubic-Low m-3 (Tetrahedral)");
    choices.push_back("TriClinic -1");
    choices.push_back("Monoclinic 2/m");
    choices.push_back("OrthoRhombic mmm");
    choices.push_back("Tetragonal-Low 4/m");
    choices.push_back("Tetragonal-High 4/mmm");
    choices.push_back("Trigonal-Low -3");
    choices.push_back("Trigonal-High -3m");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  parameters.push_back(FilterParameter::New("Transformation Phase Misorientation", "TransformationPhaseMisorientation", FilterParameterWidgetType::DoubleWidget, getTransformationPhaseMisorientation(), false));
  QStringList linkedProps1;
  linkedProps1 << "TransformationPhaseHabitPlane" << "UseAllVariants";
  parameters.push_back(FilterParameter::NewConditional("Define Habit Plane", "DefineHabitPlane", FilterParameterWidgetType::LinkedBooleanWidget, getDefineHabitPlane(), false, linkedProps1));
  parameters.push_back(FilterParameter::New("Transformation Phase Habit Plane", "TransformationPhaseHabitPlane", FilterParameterWidgetType::FloatVec3Widget, getTransformationPhaseHabitPlane(), false));
  QStringList linkedProps2("CoherentFrac");
  parameters.push_back(FilterParameter::NewConditional("Use All Variants", "UseAllVariants", FilterParameterWidgetType::LinkedBooleanWidget, getUseAllVariants(), false, linkedProps2));
  parameters.push_back(FilterParameter::New("Coherent Fraction", "CoherentFrac", FilterParameterWidgetType::DoubleWidget, getCoherentFrac(), false));
  parameters.push_back(FilterParameter::New("Transformation Phase Thickness", "TransformationPhaseThickness", FilterParameterWidgetType::DoubleWidget, getTransformationPhaseThickness(), false));
  parameters.push_back(FilterParameter::New("Average Number Of Transformation Phases Per Feature", "NumTransformationPhasesPerFeature", FilterParameterWidgetType::IntWidget, getNumTransformationPhasesPerFeature(), false));
  parameters.push_back(FilterParameter::New("\"Peninsula\" Transformation Phase Fraction", "PeninsulaFrac", FilterParameterWidgetType::DoubleWidget, getPeninsulaFrac(), false));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("StatsGenerator Cell Ensemble Attribute Matrix Name", "StatsGenCellEnsembleAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getStatsGenCellEnsembleAttributeMatrixPath(), true, ""));
  parameters.push_back(FilterParameter::New("Volume Cell Ensemble Attribute Matrix Name", "VolCellEnsembleAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getVolCellEnsembleAttributeMatrixPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Feature Attribute Matrix Name", "CellFeatureAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureIdsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Euler Angles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Average Quaterions", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Centroids", "CentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCentroidsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Equivalent Diameters", "EquivalentDiametersArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getEquivalentDiametersArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Euler Angles", "FeatureEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeatureEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Feature Phases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Phase Types Array", "PhaseTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getPhaseTypesArrayPath(), true));
  parameters.push_back(FilterParameter::New("Shape Types Array", "ShapeTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getShapeTypesArrayPath(), true));
  parameters.push_back(FilterParameter::New("Number of Features", "NumFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getNumFeaturesArrayPath(), true, ""));

  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Feature Parent Ids", "FeatureParentIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureParentIdsArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Number Of Features Per Parent", "NumFeaturesPerParentArrayName", FilterParameterWidgetType::StringWidget, getNumFeaturesPerParentArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setTransCrystalStruct(reader->readValue("TransCrystalStruct", getTransCrystalStruct()) );
  setTransformationPhaseMisorientation(reader->readValue("TransformationPhaseMisorientation", getTransformationPhaseMisorientation()) );
  setDefineHabitPlane(reader->readValue("DefineHabitPlane", getDefineHabitPlane()) );
  setTransformationPhaseHabitPlane(reader->readFloatVec3("TransformationPhaseHabitPlane", getTransformationPhaseHabitPlane()) );
  setUseAllVariants(reader->readValue("UseAllVariants", getUseAllVariants()) );
  setCoherentFrac(reader->readValue("CoherentFrac", getCoherentFrac()) );
  setTransformationPhaseThickness(reader->readValue("TransformationPhaseThickness", getTransformationPhaseThickness()) );
  setNumTransformationPhasesPerFeature(reader->readValue("NumTransformationPhasesPerFeature", getNumTransformationPhasesPerFeature()) );
  setPeninsulaFrac(reader->readValue("PeninsulaFrac", getPeninsulaFrac()) );

  setStatsGenCellEnsembleAttributeMatrixPath(reader->readDataArrayPath("StatsGenCellEnsembleAttributeMatrixPath", getStatsGenCellEnsembleAttributeMatrixPath()));
  setVolCellEnsembleAttributeMatrixPath(reader->readDataArrayPath("VolCellEnsembleAttributeMatrixPath", getVolCellEnsembleAttributeMatrixPath()));
  setCellFeatureAttributeMatrixName(reader->readDataArrayPath("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath() ) );
  setEquivalentDiametersArrayPath(reader->readDataArrayPath("EquivalentDiametersArrayPath", getEquivalentDiametersArrayPath() ) );
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName() ) );
  setNumFeaturesPerParentArrayName(reader->readString("NumFeaturesPerParentArrayName", getNumFeaturesPerParentArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setPhaseTypesArrayPath(reader->readDataArrayPath("PhaseTypesArrayPath", getPhaseTypesArrayPath() ) );
  setShapeTypesArrayPath(reader->readDataArrayPath("ShapeTypesArrayPath", getShapeTypesArrayPath() ) );
  setNumFeaturesArrayPath(reader->readDataArrayPath("NumFeaturesArrayPath", getNumFeaturesArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InsertTransformationPhases::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(TransCrystalStruct)
  DREAM3D_FILTER_WRITE_PARAMETER(TransformationPhaseMisorientation)
  DREAM3D_FILTER_WRITE_PARAMETER(DefineHabitPlane)
  DREAM3D_FILTER_WRITE_PARAMETER(TransformationPhaseHabitPlane)
  DREAM3D_FILTER_WRITE_PARAMETER(TransformationPhaseThickness)
  DREAM3D_FILTER_WRITE_PARAMETER(UseAllVariants)
  DREAM3D_FILTER_WRITE_PARAMETER(CoherentFrac)
  DREAM3D_FILTER_WRITE_PARAMETER(NumTransformationPhasesPerFeature)
  DREAM3D_FILTER_WRITE_PARAMETER(PeninsulaFrac)

  DREAM3D_FILTER_WRITE_PARAMETER(StatsGenCellEnsembleAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(VolCellEnsembleAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(EquivalentDiametersArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureParentIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(NumFeaturesPerParentArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(PhaseTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ShapeTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NumFeaturesArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_NumFeaturesPerParentPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeaturesPerParent = m_NumFeaturesPerParentPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::updateStatsGenEnsembleInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if( NULL != m_ShapeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::updateVolEnsembleInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || m == NULL) { return; }
  AttributeMatrix::Pointer statsGenAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getStatsGenCellEnsembleAttributeMatrixPath().getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || statsGenAttrMat == NULL) { return; }
  AttributeMatrix::Pointer volAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getVolCellEnsembleAttributeMatrixPath().getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || volAttrMat == NULL) { return; }

  QVector<size_t> dims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  dims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_EquivalentDiametersPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getEquivalentDiametersArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_EquivalentDiametersPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_EquivalentDiameters = m_EquivalentDiametersPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCentroidsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Centroids = m_CentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getFeatureParentIdsArrayName() );
  m_FeatureParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, -1, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureParentIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getCellFeatureAttributeMatrixName().getDataContainerName(), getCellFeatureAttributeMatrixName().getAttributeMatrixName(), getNumFeaturesPerParentArrayName() );
  m_NumFeaturesPerParentPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPerParentPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeaturesPerParent = m_NumFeaturesPerParentPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> PhaseTypeArrayType;
  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getPhaseTypesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> ShapeTypeArrayType;
  m_ShapeTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getShapeTypesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ShapeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getVolCellEnsembleAttributeMatrixPath().getDataContainerName(), getVolCellEnsembleAttributeMatrixPath().getAttributeMatrixName(), getNumFeaturesPerParentArrayName() );
  m_NumFeaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  getNumFeaturesArrayPath(), 0, dims, getNumFeaturesArrayName()); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NumFeaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();

  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getAttributeMatrix(getFeaturePhasesArrayPath());
  if(attrMat == NULL) { setInPreflight(false); return; }

  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getFeatureIdsArrayPath().getDataContainerName());
  DataContainerArray::Pointer dca = getDataContainerArray();

  // defining separate ensemble attribute matrix for statsgen & vol
  AttributeMatrix::Pointer statsGenAttrMat = dca->getAttributeMatrix(getStatsGenCellEnsembleAttributeMatrixPath());
  AttributeMatrix::Pointer volAttrMat = dca->getAttributeMatrix(getVolCellEnsembleAttributeMatrixPath());

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  // resizing statsgen attribute matrix to add new phase info
  QVector<size_t> tDims(1, numensembles + 1);
  statsGenAttrMat->resizeAttributeArrays(tDims);
  updateStatsGenEnsembleInstancePointers();

  // hard-coded to the below stats for now
  m_CrystalStructures[numensembles] = m_TransCrystalStruct;
  m_PhaseTypes[numensembles] = DREAM3D::PhaseType::TransformationPhase;
  m_ShapeTypes[numensembles] = DREAM3D::ShapeType::EllipsoidShape;

  // start insert transformation phases routine
  insert_transformationphases();

  notifyStatusMessage(getHumanLabel(), "Placement Complete");

  // updating number of ensembles and features
  totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  numensembles = m_PhaseTypesPtr.lock()->getNumberOfTuples();

  // resizing vol attribute matrix
  volAttrMat->resizeAttributeArrays(tDims);
  updateVolEnsembleInstancePointers();

  // finding ensemble level number of features per phase
  for(size_t i = 1; i < numensembles; ++i)
  {
    m_NumFeatures[i] = 0;
  }
  for(size_t i = 1; i < totalFeatures; ++i)
  {
    ++m_NumFeatures[m_FeaturePhases[i]];
  }

  //filter_calls();

  notifyStatusMessage(getHumanLabel(), "Execute Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::insert_transformationphases()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getFeatureIdsArrayPath().getDataContainerName());
  DREAM3D_RANDOMNG_NEW()

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  int64_t totalPoints = static_cast<size_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());
  QVector<size_t> tDims(1, 1);

  // find the minimum resolution
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  float minRes = xRes;
  if (minRes > yRes) minRes = yRes;
  if (minRes > zRes) minRes = zRes;

  float sampleHabitPlane[3] = {0.0f};
  // NOTE: incoherent habit planes e[-20,20] & are ints
  float crystalHabitPlane[3] = {int(rg.genrand_res53() * 20.0f),int(rg.genrand_res53() * 20.0f),int(rg.genrand_res53() * 20.0f)};
  QuatF q1, q2;
  float g[3][3], gT[3][3], rotMat[3][3], newMat[3][3], newMatCopy[3][3], symMat[3][3];
  float plateThickness = 0.0f;
  size_t numFeatures = totalFeatures;
  float d, random=0.0f, random2=0.0f;
  float sig3 = m_TransformationPhaseMisorientation * (m_pi/180.0f);
  float e[3];
  float traceMin = -1.0f;
  float trace = 0.0f;
  int minPos = 0;
  std::vector<float> shifts;
  int numTransformationPhases = 0;
  bool createdTransformationPhase = false;

  float voxelDiagonal = sqrtf(xRes*xRes + yRes*yRes + zRes*zRes);

  for (size_t curFeature = 1; curFeature < numFeatures; ++curFeature)
  {
    QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  // set the grain Id to the parent Id for if/when the features get uniquely renumbered
  m_FeatureParentIds[curFeature] = curFeature;

  if (m_DefineHabitPlane == true)
  {
    crystalHabitPlane[0] = m_TransformationPhaseHabitPlane.x;
    crystalHabitPlane[1] = m_TransformationPhaseHabitPlane.y;
    crystalHabitPlane[2] = m_TransformationPhaseHabitPlane.z;
  }
  // pick a habit plane variant if user desires
  if (m_UseAllVariants == true)
  {
    random = static_cast<float>(rg.genrand_res53());
    for (int i = 0; i < 3; ++i)
    {
    // decide whether to make it coherent or incoherent &
    // decide whether its negative or positive
    if (random < m_CoherentFrac && random2 < 0.5f) crystalHabitPlane[i] *= -1.0f;
    }
    // jumble the i,j,k order
    float tempCrystalHabitPlane[3] = {crystalHabitPlane[0],crystalHabitPlane[1],crystalHabitPlane[2]};
    random = static_cast<float>(rg.genrand_res53());
    if (random < 0.16667f)
    {
    crystalHabitPlane[0] = tempCrystalHabitPlane[0];
    crystalHabitPlane[1] = tempCrystalHabitPlane[1];
    crystalHabitPlane[2] = tempCrystalHabitPlane[2];
    }
    else if (random >= 0.16667f && random < 0.33333f)
    {
    crystalHabitPlane[0] = tempCrystalHabitPlane[0];
    crystalHabitPlane[1] = tempCrystalHabitPlane[2];
    crystalHabitPlane[2] = tempCrystalHabitPlane[1];
    }
    else if (random >= 0.33333f && random < 0.5f)
    {
    crystalHabitPlane[0] = tempCrystalHabitPlane[1];
    crystalHabitPlane[1] = tempCrystalHabitPlane[0];
    crystalHabitPlane[2] = tempCrystalHabitPlane[2];
    }
    else if (random >= 0.5f && random < 0.66667f)
    {
    crystalHabitPlane[0] = tempCrystalHabitPlane[1];
    crystalHabitPlane[1] = tempCrystalHabitPlane[2];
    crystalHabitPlane[2] = tempCrystalHabitPlane[0];
    }
    else if (random >= 0.66667f && random < 0.83333f)
    {
    crystalHabitPlane[0] = tempCrystalHabitPlane[2];
    crystalHabitPlane[1] = tempCrystalHabitPlane[0];
    crystalHabitPlane[2] = tempCrystalHabitPlane[1];
    }
    else
    {
    crystalHabitPlane[0] = tempCrystalHabitPlane[2];
    crystalHabitPlane[1] = tempCrystalHabitPlane[1];
    crystalHabitPlane[2] = tempCrystalHabitPlane[0];
    }
  }
  // find where the habit plane points
  QuaternionMathF::Copy(avgQuats[curFeature], q1);
    OrientationMath::QuattoMat(q1, g);
  MatrixMath::Transpose3x3(g, gT);
    MatrixMath::Multiply3x3with3x1(gT, crystalHabitPlane, sampleHabitPlane);

//    rotMat[0][0]=0.5774;
//    rotMat[0][1]=0.5774;
//    rotMat[0][2]=0.5774;
//    rotMat[1][0]=-0.4082;
//    rotMat[1][1]=-0.4082;
//    rotMat[1][2]=0.8165;
//    rotMat[2][0]=0.7071;
//    rotMat[2][1]=-0.7071;
//    rotMat[2][2]=0;

  // generate transformation phase orientation with a user-defined rotation about the habit plane
  OrientationMath::AxisAngletoMat(sig3, crystalHabitPlane[0], crystalHabitPlane[1], crystalHabitPlane[2], rotMat);
  MatrixMath::Multiply3x3with3x3(rotMat, g, newMat);

  // find the minimum angle
  MatrixMath::Copy3x3(newMat, newMatCopy);
  // Get our OrientationOps pointer for the selected crystal structure
  OrientationOps::Pointer orientOps = m_OrientationOps[m_TransCrystalStruct];

  //get number of symmetry operators
  traceMin = -1.0f;
  minPos = 0;
  int n_sym = orientOps->getNumSymOps();
  for (int i = 0; i < n_sym; ++i)
  {
    orientOps->getMatSymOp(i, symMat);
    MatrixMath::Multiply3x3with3x3(symMat, newMatCopy, newMat);
    trace = newMat[0][0] + newMat[1][1] + newMat[2][2];
    if (trace > traceMin)
    {
    traceMin = trace;
    minPos = i;
    }
  }
  // assign our symmetry matrix to that which produced the minimum angle
  orientOps->getMatSymOp(minPos, symMat);
  MatrixMath::Multiply3x3with3x3(symMat, newMatCopy, newMat);
  OrientationMath::MattoEuler(newMat, e[0], e[1], e[2]);
  OrientationMath::EulertoQuat(e[0], e[1], e[2], q2);

  // define plate = user input fraction of eq dia centered at centroid
  // NOTE: we multiply by 0.5 because the transformation phase thickness will be established by
  // a search from both sides
  plateThickness = m_EquivalentDiameters[curFeature] * m_TransformationPhaseThickness * 0.5f;
  // if the plate thickness is less than the minimum dimension resolution,
  // then don't try to insert a transformation phase because the grain is too small and
  // the transformation phase thickness will be too small
  if (plateThickness > minRes)
  {
    // select number of transformation phases to insert per grain from 2x user input to zero
    numTransformationPhases = int(rg.genrand_res53() * double(2*m_NumTransformationPhasesPerFeature + 1));
    shifts.resize(numTransformationPhases);
    int attempt = 0;

    // each loop represents an attempt to insert a single transformation phase (all in the same grain)
    for (int i = 0; i < numTransformationPhases; ++i)
    {
    // define the shift placement from centroid
    random = static_cast<float>(rg.genrand_res53());
    // shift defined as random (float) e[0,1] x grain's eq dia x 0.5
    // NOTE: we multiply by 0.5 because it starts at the center and we don't
    // want to run off the end of the grain
    shifts[i] = random * m_EquivalentDiameters[curFeature] * 0.5f;
    random = static_cast<float>(rg.genrand_res53());
    if (random < 0.5f) shifts[i] = -shifts[i];

    // check if new transformation phase will overlap an old transformation phase
    int ii = i;
    for (int j = 0; j <= ii; ++j)
    {
      // if adding more than one transformation phase in a grain, check the current shift from
      // center against the previous ones to make sure the new transformation phase does not
      // overlap
      // NOTE: that the transformation phases can touch since we're using equiv dia,
      // they can be high aspect ratio grains inserted in the low aspect direction
      // which skews the calculation (FIX)
      // tolerance is set to 3x the plate thickness + 2x the voxel diagonal -- this empirically
      // keeps the transformation phases separated enough
      if (fabs(shifts[i] - shifts[j]) <= (plateThickness*3.0f + voxelDiagonal*2.0f) && ii != j)
      {
      // if the attempted placement of a second, third, fourth, ... transformation phase is too close to a
      // previously inserted transformation phase in the current grain, then increment the attempts counter
      // and try a different shift
      ++attempt;
      --i;
      break;
      }
      else
      {
      // set the origin of the plane
      d = -sampleHabitPlane[0] * (m_Centroids[3*curFeature+0] + shifts[i])
        - sampleHabitPlane[1] * (m_Centroids[3*curFeature+1] + shifts[i])
        - sampleHabitPlane[2] * (m_Centroids[3*curFeature+2] + shifts[i]);

      createdTransformationPhase = place_transformationphase(curFeature, sampleHabitPlane, totalFeatures, plateThickness, d, numFeatures);
      int stop = 0;
      // change an isthmus transformation phase to a peninsula transformation phase
      random = static_cast<float>(rg.genrand_res53());
      if (createdTransformationPhase == true)
      {
        if (random < m_PeninsulaFrac) peninsula_transformationphase(curFeature, totalFeatures);

        // filling in transformation phase stats that already exist for parents
        tDims[0] = totalFeatures + 1;
        m->getAttributeMatrix(getFeaturePhasesArrayPath().getAttributeMatrixName())->resizeAttributeArrays(tDims);
        updateFeatureInstancePointers();
        totalFeatures = transfer_attributes(totalFeatures, q2, e, curFeature);
        ++m_NumFeaturesPerParent[curFeature];
      }
      }
    }
    // try 10 times to insert 2+ transformation phases, if it can't, the grain is probably too small
    if (attempt == 10) break;
    }
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InsertTransformationPhases::place_transformationphase(size_t curFeature, float sampleHabitPlane[3], size_t totalFeatures, float plateThickness, float d, size_t numFeatures)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getFeatureIdsArrayPath().getDataContainerName());
  DREAM3D_RANDOMNG_NEW()

  size_t totalPoints = static_cast<size_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());
  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  bool flag = false;
  float x, y, z, D;
  bool firstVoxel = true;

  // loop through all cells to find matching grain IDs
  int zStride, yStride;
  for(int i = 0; i < zPoints; i++)
  {
  zStride = i * xPoints * yPoints;
  for (int j = 0 ; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for(int k = 0; k < xPoints; k++)
    {
    int gnum = m_FeatureIds[zStride+yStride+k];
    x = float(k) * xRes;
    y = float(j) * yRes;
    z = float(i) * zRes;

    // if the grain IDs match...
    if (gnum == curFeature)
    {
      // calculate the distance between the cell and the plane
      float denom = 1 / sqrtf(sampleHabitPlane[0] * sampleHabitPlane[0]
      + sampleHabitPlane[1] * sampleHabitPlane[1]
      + sampleHabitPlane[2] * sampleHabitPlane[2]);

      D = sampleHabitPlane[0] * x * denom
      + sampleHabitPlane[1] * y * denom
      + sampleHabitPlane[2] * z * denom
      + d * denom;

      // if the cell-plane distance is less than the plate thickness then place a transformation phase voxel
      // and that a neighboring cell is not a transformation phase cell
      if (fabs(D) < plateThickness)
////			&& (k == 0 || m_FeatureIds[zStride+yStride+k-1] < numFeatures || m_FeatureIds[zStride+yStride+k-1] == totalFeatures)
////			&& (k == (m->getXPoints() - 1) || m_FeatureIds[zStride+yStride+k+1] < numFeatures || m_FeatureIds[zStride+yStride+k+1] == totalFeatures)
////			&& (j == 0 || m_FeatureIds[zStride+yStride+k-xPoints] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints] == totalFeatures)
////			&& (j == (m->getYPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints] == totalFeatures)
////			&& (i == 0 || m_FeatureIds[zStride+yStride+k-xPoints*yPoints] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints*yPoints] == totalFeatures)
////			&& (i == (m->getZPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints*yPoints] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints*yPoints] == totalFeatures))
//			&& (k == 0 || j == 0 || m_FeatureIds[zStride+yStride+k-xPoints-1] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints-1] == totalFeatures)
//			&& (k == (m->getXPoints() - 1) || j == 0 || m_FeatureIds[zStride+yStride+k-xPoints+1] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints+1] == totalFeatures)
//			&& (k == 0 || j == (m->getYPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints-1] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints-1] == totalFeatures)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints+1] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints+1] == totalFeatures)
//			&& (k == 0 || j == 0 || i == (m->getZPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints*yPoints-xPoints-1] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints*yPoints-xPoints-1] == totalFeatures)
//			&& (k == (m->getXPoints() - 1) || j == 0 || i == (m->getZPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints*yPoints-xPoints+1] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints*yPoints-xPoints+1] == totalFeatures)
//			&& (k == 0 || j == (m->getYPoints() - 1) || i == (m->getZPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints*yPoints+xPoints-1] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints*yPoints+xPoints-1] == totalFeatures)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || i == (m->getZPoints() - 1) || m_FeatureIds[zStride+yStride+k+xPoints*yPoints+xPoints+1] < numFeatures || m_FeatureIds[zStride+yStride+k+xPoints*yPoints+xPoints+1] == totalFeatures)
//			&& (k == 0 || j == 0 || i == 0 || m_FeatureIds[zStride+yStride+k-xPoints*yPoints-xPoints-1] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints*yPoints-xPoints-1] == totalFeatures)
//			&& (k == (m->getXPoints() - 1) || j == 0 || i == 0 || m_FeatureIds[zStride+yStride+k-xPoints*yPoints-xPoints+1] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints*yPoints-xPoints+1] == totalFeatures)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || i == 0 || m_FeatureIds[zStride+yStride+k-xPoints*yPoints+xPoints-1] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints*yPoints+xPoints-1] == totalFeatures)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || i == 0 || m_FeatureIds[zStride+yStride+k-xPoints*yPoints+xPoints+1] < numFeatures || m_FeatureIds[zStride+yStride+k-xPoints*yPoints+xPoints+1] == totalFeatures))
      {
      // check if an "island" transformation phase voxel will be placed (excluding the first voxel placement)
      /*
      if ((k == 0 || m_FeatureIds[zStride+yStride+k-1] != totalFeatures)
        && (k == (xPoints - 1) || m_FeatureIds[zStride+yStride+k+1] != totalFeatures)
        && (j == 0 || m_FeatureIds[zStride+yStride+k-xPoints] != totalFeatures)
        && (j == (yPoints - 1) || m_FeatureIds[zStride+yStride+k+xPoints] != totalFeatures)
        && (i == 0 || m_FeatureIds[zStride+yStride+k-xPoints*yPoints] != totalFeatures)
        && (i == (zPoints - 1) || m_FeatureIds[zStride+yStride+k+xPoints*yPoints] != totalFeatures)
        && firstVoxel == false)
      {
        // if an "island" transformation phase voxel is inserted flip all the transformation phase voxels back to the grain ID
        // and this will go in the books as a failed transformation phase insertion attempt
        for (int64_t l = 0; l < totalPoints; ++l)
        {
        if (m_FeatureIds[l] == totalFeatures)
        {
  //			  int goal = m_FeatureIds[l];
          m_FeatureIds[l] = curFeature;
  //			  int one = m_FeatureIds[zStride+yStride+k+xPoints*yPoints];
  //			  int two =  m_FeatureIds[zStride+yStride+k-xPoints*yPoints];
  //			  int three = m_FeatureIds[zStride+yStride+k+xPoints];
  //			  int four = m_FeatureIds[zStride+yStride+k-xPoints];
  //			  int five = m_FeatureIds[zStride+yStride+k+1];
  //			  int six = m_FeatureIds[zStride+yStride+k-1];
         int stop = 0;
        }
        }
          flag = false;
      }
      else
      {
        */
        m_FeatureIds[zStride+yStride+k] = totalFeatures;
        flag = true;
        firstVoxel = false;
  //		}
      }
    }
    }
  }
  }
  if (flag == true) return true;
  else return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::peninsula_transformationphase(size_t curFeature, size_t totalFeatures)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getFeatureIdsArrayPath().getDataContainerName());
  DREAM3D_RANDOMNG_NEW()
  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  int x1 = -1, x2 = -1, y1 = -1, y2 = -1, z1 = -1, z2 = -1;
  float transformationPhaseLength = 0.0f, random = 0.0f, fractionKept = 0.0f, currentDistance = 0.0f;
  int zStride, yStride;

  // loop through all cells to find matching grain IDs
  for(int i = 0; i < zPoints; i++)
  {
  zStride = i * xPoints * yPoints;
  for (int j = 0 ; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for(int k = 0; k < xPoints; k++)
    {
    int gnum = m_FeatureIds[zStride+yStride+k];
    // if the grain IDs match...
    if (gnum == totalFeatures)
    {
      if (x1 == -1)
      {
      // establish one extremum of the transformation phase
      x1 = i;
      y1 = j;
      z1 = k;
      }
      // establish the other extremum of the transformation phase
      x2 = i;
      y2 = j;
      z2 = k;
    }
    }
  }
  }

  // calculate the distance between the extrema
  transformationPhaseLength = sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
  // if the transformation phase is 1 voxel, set the length to 2 so it makes it through the below loop
  // but won't change anything
  if (transformationPhaseLength == 0.0f) transformationPhaseLength = 2.0f;

  // choose which extremum to start from
  random = static_cast<float>(rg.genrand_res53());
  if (random < 0.5f)
  {
  x2 = x1;
  y2 = y1;
  z2 = z1;
  }

  // choose how much of the transformation phase to keep (has to be at least one voxel)
  while (int(fractionKept * transformationPhaseLength) < 1) fractionKept = static_cast<float>(rg.genrand_res53());

  // loop through again to decide which transformation phase Ids get flipped back to grain Ids
  for(int i = 0; i < zPoints; i++)
  {
  zStride = i * xPoints * yPoints;
  for (int j = 0; j < yPoints; j++)
  {
    yStride = j * xPoints;
    for(int k = 0; k < xPoints; k++)
    {
    int gnum = m_FeatureIds[zStride+yStride+k];
    // if the grain IDs match...
    if (gnum == totalFeatures)
    {
      currentDistance = sqrtf((i-x1)*(i-x1) + (j-y1)*(j-y1) + (k-z1)*(k-z1));
      // if the distance is longer than the transformation phase length, flip back to the parent ID
      if (currentDistance > transformationPhaseLength * fractionKept) m_FeatureIds[zStride+yStride+k] = curFeature;
    }
    }
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t InsertTransformationPhases::transfer_attributes(size_t totalFeatures, QuatF q, float e[], size_t curFeature)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getFeatureIdsArrayPath().getDataContainerName());

  m_AvgQuats[4*totalFeatures+0] = q.x;
  m_AvgQuats[4*totalFeatures+1] = q.y;
  m_AvgQuats[4*totalFeatures+2] = q.z;
  m_AvgQuats[4*totalFeatures+3] = q.w;
  m_Centroids[3*totalFeatures+0] = 1.0f;
  m_Centroids[3*totalFeatures+1] = 2.0f;
  m_Centroids[3*totalFeatures+2] = 3.0f;
  m_FeatureEulerAngles[3*totalFeatures+0] = e[0];
  m_FeatureEulerAngles[3*totalFeatures+1] = e[1];
  m_FeatureEulerAngles[3*totalFeatures+2] = e[2];
  m_EquivalentDiameters[totalFeatures] = 4.0f;
  m_FeaturePhases[totalFeatures] = m_PhaseTypesPtr.lock()->getNumberOfTuples() - 1;;
  m_FeatureParentIds[totalFeatures] = curFeature;
  m_NumFeaturesPerParent[totalFeatures] = 0;
  return ++totalFeatures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTransformationPhases::filter_calls()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getFeatureIdsArrayPath().getDataContainerName());

 // // mapping euler angles from feature to cell
 // for (int64_t i = 0; i < m->getNumCellTuples(); ++i)
 // {
  //m_CellEulerAngles[3*i+0] = m_FeatureEulerAngles[3*m_FeatureIds[i]+0];
  //m_CellEulerAngles[3*i+1] = m_FeatureEulerAngles[3*m_FeatureIds[i]+1];
  //m_CellEulerAngles[3*i+2] = m_FeatureEulerAngles[3*m_FeatureIds[i]+2];
 // }

 // FindFeatureCentroids::Pointer find_grain_centroids = FindFeatureCentroids::New();
 // find_grain_centroids->setObservers(this->getObservers());
 // find_grain_centroids->setVoxelDataContainer(m);
 // find_grain_centroids->setMessagePrefix(getMessagePrefix());
 // find_grain_centroids->execute();
 // int err1 = find_grain_centroids->getErrorCondition();
 // if (err1 < 0)
 // {
 //   setErrorCondition(find_grain_centroids->getErrorCondition());
 //   addErrorMessages(find_grain_centroids->getPipelineMessages());
 //   return;
 // }

 // FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
 // find_neighbors->setObservers(this->getObservers());
 // find_neighbors->setVoxelDataContainer(m);
 // find_neighbors->setMessagePrefix(getMessagePrefix());
 // find_neighbors->execute();
 // int err2 = find_neighbors->getErrorCondition();
 // if (err2 < 0)
 // {
 //   setErrorCondition(find_neighbors->getErrorCondition());
 //   addErrorMessages(find_neighbors->getPipelineMessages());
 //   return;
 // }

 // FindSurfaceFeatures::Pointer find_surface_grains = FindSurfaceFeatures::New();
 // find_surface_grains->setObservers(this->getObservers());
 // find_surface_grains->setVoxelDataContainer(m);
 // find_surface_grains->setMessagePrefix(getMessagePrefix());
 // find_surface_grains->execute();
 // int err3 = find_surface_grains->getErrorCondition();
 // if (err3 < 0)
 // {
 //   setErrorCondition(find_surface_grains->getErrorCondition());
 //   addErrorMessages(find_surface_grains->getPipelineMessages());
 //   return;
 // }

 // FindSizes::Pointer find_sizes = FindSizes::New();
 // find_sizes->setObservers(this->getObservers());
 // find_sizes->setVoxelDataContainer(m);
 // find_sizes->setMessagePrefix(getMessagePrefix());
 // find_sizes->execute();
 // int err4 = find_sizes->getErrorCondition();
 // if (err4 < 0)
 // {
 //   setErrorCondition(find_sizes->getErrorCondition());
 //   addErrorMessages(find_sizes->getPipelineMessages());
 //   return;
 // }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InsertTransformationPhases::newFilterInstance(bool copyFilterParameters)
{
  /*
  * ApplyToVoxelVolume
  * ApplyToSurfaceMesh
  * Origin
  */
  InsertTransformationPhases::Pointer filter = InsertTransformationPhases::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
