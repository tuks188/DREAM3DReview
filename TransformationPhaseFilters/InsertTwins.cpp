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

#include "InsertTwins.h"
#include "DREAM3DLib/GenericFilters/FindGrainCentroids.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighbors.h"
#include "DREAM3DLib/GenericFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/ReconstructionFilters/ScalarSegmentGrains.h"
#include "DREAM3DLib/GenericFilters/RenameCellArray.h"
#include "DREAM3DLib/GenericFilters/RenameFieldArray.h"
#include "DREAM3DLib/GenericFilters/LinkFieldMapToCellArray.h"
#include "DREAM3DLib/GenericFilters/RemoveArrays.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
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
InsertTwins::InsertTwins() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
  m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_FieldParentIdsArrayName(DREAM3D::FieldData::ParentIds),
  m_NumGrainsPerParentArrayName(DREAM3D::FieldData::NumGrainsPerParent),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_PhaseTypesArrayName(DREAM3D::EnsembleData::PhaseTypes),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_NumFieldsArrayName(DREAM3D::EnsembleData::NumFields),
  m_TwinThickness(0.5f),
  m_NumTwinsPerGrain(2),
  m_VariantNum(1),
  m_CoherentFrac(1.0f),
  m_PeninsulaFrac(0.0f),
  m_GrainIds(NULL),
  m_CellEulerAngles(NULL),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
  m_FieldEulerAngles(NULL),
  m_FieldPhases(NULL),
  m_FieldParentIds(NULL),
  m_NumGrainsPerParent(NULL),
  m_CrystalStructures(NULL),
  m_PhaseTypes(NULL),
  m_ShapeTypes(NULL),
  m_NumFields(NULL)
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InsertTwins::~InsertTwins()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Twin Thickness");
    option->setPropertyName("TwinThickness");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("Equivalent Diameters");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Average Number Of Twins Per Grain");
    option->setPropertyName("NumTwinsPerGrain");
	option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Coherent Fraction");
    option->setPropertyName("CoherentFrac");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("\"Peninsula\" Twin Fraction");
    option->setPropertyName("PeninsulaFrac");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Variant Number");
    option->setPropertyName("VariantNum");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("int");
    option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setTwinThickness( reader->readValue("TwinThickness", getTwinThickness()) );
  setNumTwinsPerGrain( reader->readValue("NumTwinsPerGrain", getNumTwinsPerGrain()) );
  setCoherentFrac( reader->readValue("CoherentFrac", getCoherentFrac()) );
  setPeninsulaFrac( reader->readValue("PeninsulaFrac", getPeninsulaFrac()) );
  setVariantNum( reader->readValue("VariantNum", getVariantNum()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InsertTwins::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("TwinThickness", getTwinThickness() );
  writer->writeValue("NumTwinsPerGrain", getNumTwinsPerGrain() );
  writer->writeValue("CoherentFrac", getCoherentFrac() );
  writer->writeValue("PeninsulaFrac", getPeninsulaFrac() );
  writer->writeValue("VariantNum", getVariantNum() );
    writer->closeFilterGroup();
    return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  // Cell Data
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -301, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -310, float, FloatArrayType, voxels, 3)

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -302, float, FloatArrayType, fields, 4)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -304, float, FloatArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, -305, float, FloatArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -306, int32_t, Int32ArrayType, fields, 1)
 
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldParentIds, ss, int32_t, Int32ArrayType, 0, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, NumGrainsPerParent, ss, int32_t, Int32ArrayType, 0, fields, 1)

  // Ensemble Data  
  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -307, unsigned int, XTalStructArrayType, ensembles, 1)
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, PhaseTypes, ss, -308, unsigned int, PhaseTypeArrayType, ensembles, 1)
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, ShapeTypes, ss, -309, unsigned int, ShapeTypeArrayType, ensembles, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, NumFields, ss, int32_t, Int32ArrayType, 0, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  setErrorCondition(0);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  // setting ensemble level data
  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PTypeArrayType;
  typedef DataArray<unsigned int> STypeArrayType;
  XTalStructArrayType::Pointer m_XTalStructData = XTalStructArrayType::CreateArray(m->getNumEnsembleTuples()+1, DREAM3D::EnsembleData::CrystalStructures);
  PTypeArrayType::Pointer m_PhaseTypeData = PTypeArrayType::CreateArray(m->getNumEnsembleTuples()+1, DREAM3D::EnsembleData::PhaseTypes);
  STypeArrayType::Pointer m_ShapeTypeData = STypeArrayType::CreateArray(m->getNumEnsembleTuples()+1, DREAM3D::EnsembleData::ShapeTypes);
  //Initialize the arrays with the "Unknown" value
  m_XTalStructData->initializeWithValues(999);
  m_PhaseTypeData->initializeWithValues(999);
  m_ShapeTypeData->initializeWithValues(999);

  for (size_t i = 0; i < m->getNumEnsembleTuples(); ++i)
  {
	m_XTalStructData->SetValue(i, m_CrystalStructures[i]);
	m_PhaseTypeData->SetValue(i, m_PhaseTypes[i]);  
	m_ShapeTypeData->SetValue(i, m_ShapeTypes[i]);  
  }

  m_XTalStructData->SetValue(m->getNumEnsembleTuples(), Ebsd::CrystalStructure::Cubic_High);
  m_PhaseTypeData->SetValue(m->getNumEnsembleTuples(), DREAM3D::PhaseType::TransformationPhase);  
  m_ShapeTypeData->SetValue(m->getNumEnsembleTuples(), DREAM3D::ShapeType::EllipsoidShape);  

  m->addEnsembleData(DREAM3D::EnsembleData::CrystalStructures, m_XTalStructData);
  m->addEnsembleData(DREAM3D::EnsembleData::PhaseTypes, m_PhaseTypeData);
  m->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, m_ShapeTypeData);

  // start insert twins routine
  insert_twins();

  notifyStatusMessage("Placement Complete");

  // finding ensemble level number of fields per phase
  for(size_t i = 1; i < m->getNumEnsembleTuples(); i++)
  {
    m_NumFields[i] = 0;
  }
  for(size_t i = 1; i < m->getNumFieldTuples(); i++)
  {
    m_NumFields[m_FieldPhases[i]]++;
  }

  filter_calls();

  notifyStatusMessage("Execute Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::insert_twins()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  DREAM3D_RANDOMNG_NEW()

  size_t totalFields = m->getNumFieldTuples();
  int64_t totalPoints = m->getTotalPoints();

  // find the minimum resolution
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  float minRes = xRes;
  if (minRes > yRes) minRes = yRes;
  if (minRes > zRes) minRes = zRes;

  float sampleHabitPlane[3] = {0.0f,0.0f,0.0f}, crystalHabitPlane[3] = {0.0f,0.0f,0.0f};
  QuatF q1, q2;
  float g[3][3], gT[3][3], rotMat[3][3], newMat[3][3];
  float plateThickness = 0.0f;
  size_t numGrains = totalFields;
  float d, random, random2;
  float sig3 = 60.0f * (m_pi/180.0f);
  float e[3];
  std::vector<float> shifts;
  int numTwins;
  bool createdTwin = false;

  float voxelDiagonal = sqrtf(xRes*xRes + yRes*yRes + zRes*zRes);

  for (size_t curGrain = 1; curGrain < numGrains; ++curGrain)
  {
    QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

	// set the grain Id to the parent Id for if/when the fields get uniquely renumbered 
	m_FieldParentIds[curGrain] = curGrain;

	// pick a habit plane
	random = static_cast<float>(rg.genrand_res53());
	for (int i = 0; i < 3; ++i)
	{
	  crystalHabitPlane[i] = 1.0f;
	  // decide whether to make it coherent or incoherent

	  // NOTE: incoherent habit planes e[-20,20] & are ints
	  if (random > m_CoherentFrac) crystalHabitPlane[i] = int(rg.genrand_res53() * 20.0f);
	  random2 = static_cast<float>(rg.genrand_res53());
	  if (random2 < 0.5f) crystalHabitPlane[i] *= -1.0f;
	}

	// find where the habit plane points
	QuaternionMathF::Copy(avgQuats[curGrain], q1);
    OrientationMath::QuattoMat(q1, g);
	MatrixMath::Transpose3x3(g, gT);
    MatrixMath::Multiply3x3with3x1(g, crystalHabitPlane, sampleHabitPlane);

    rotMat[0][0]=0.5774;
    rotMat[0][1]=0.5774;
    rotMat[0][2]=0.5774;
    rotMat[1][0]=-0.4082;
    rotMat[1][1]=-0.4082;
    rotMat[1][2]=0.8165;
    rotMat[2][0]=0.7071;
    rotMat[2][1]=-0.7071;
    rotMat[2][2]=0;


	// generate twin orientation with a 60 degree rotation about the habit plane
    //OrientationMath::AxisAngletoMat(sig3, crystalHabitPlane[0], crystalHabitPlane[1], crystalHabitPlane[2], rotMat);
	MatrixMath::Multiply3x3with3x3(g, rotMat, newMat);
	OrientationMath::MatToEuler(newMat, e[0], e[1], e[2]);
	OrientationMath::EulertoQuat(q2, e[0], e[1], e[2]);

	// define plate = user input fraction of eq dia centered at centroid
	// NOTE: we multiply by 0.5 because the twin thickness will be established by
	// a search from both sides
	plateThickness = m_EquivalentDiameters[curGrain] * m_TwinThickness * 0.5f;
	// if the plate thickness is less than the minimum dimension resolution, 
	// then don't try to insert a twin because the grain is too small and
	// the twin thickness will be too small
	if (plateThickness > minRes)
	{
	  // select number of twins to insert per grain from 2x user input to zero
	  numTwins = int(rg.genrand_res53() * double(2*m_NumTwinsPerGrain + 1));
	  shifts.resize(numTwins);
	  int attempt = 0;

	  // each loop represents an attempt to insert a single twin (all in the same grain)
	  for (int i = 0; i < numTwins; ++i)
	  {
		// define the shift placement from centroid
		random = static_cast<float>(rg.genrand_res53());
		// shift defined as random (float) e[0,1] x grain's eq dia x 0.5
		// NOTE: we multiply by 0.5 because it starts at the center and we don't
		// want to run off the end of the grain
		shifts[i] = random * m_EquivalentDiameters[curGrain] * 0.5f;
		random = static_cast<float>(rg.genrand_res53());
		if (random < 0.5f) shifts[i] = -shifts[i];

		// check if new twin will overlap an old twin
		int ii = i;
		for (int j = 0; j <= ii; ++j)
		{
		  // if adding more than one twin in a grain, check the current shift from
		  // center against the previous ones to make sure the new twin does not
		  // overlap  
		  // NOTE: that the twins can touch since we're using equiv dia,
		  // they can be high aspect ratio grains inserted in the low aspect direction
		  // which skews the calculation (FIX)
		  // tolerance is set to 3x the plate thickness + 2x the voxel diagonal -- this empirically
		  // keeps the twins separated enough
		  if (fabs(shifts[i] - shifts[j]) <= (plateThickness*3.0f + voxelDiagonal*2.0f) && ii != j) 
		  {
			// if the attempted placement of a second, third, fourth, ... twin is too close to a
			// previously inserted twin in the current grain, then increment the attempts counter
			// and try a different shift
			++attempt;
			--i;
			break;
		  }
		  else 
		  {
			// set the origin of the plane
			d = -sampleHabitPlane[0] * (m_Centroids[3*curGrain+0] + shifts[i]) 
			  - sampleHabitPlane[1] * (m_Centroids[3*curGrain+1] + shifts[i]) 
			  - sampleHabitPlane[2] * (m_Centroids[3*curGrain+2] + shifts[i]);

			createdTwin = place_twin(curGrain, sampleHabitPlane, totalFields, plateThickness, d, numGrains);

			// change an isthmus twin to a peninsula twin
			random = static_cast<float>(rg.genrand_res53());
			if (createdTwin == true) 
			{
			  if (random < m_PeninsulaFrac) peninsula_twin(curGrain, totalFields);
		  
			  // filling in twin stats that already exist for parents
			  totalFields = transfer_attributes(totalFields, totalPoints, q2, e, curGrain);
			  ++m_NumGrainsPerParent[curGrain];
			}
		  }
		}
		// try 10 times to insert 2+ twins, if it can't, the grain is probably too small
		if (attempt == 10) break;
	  }
	}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool InsertTwins::place_twin(size_t curGrain, float sampleHabitPlane[3], size_t totalFields, float plateThickness, float d, size_t numGrains)
{
  VoxelDataContainer* m = getVoxelDataContainer();
  DREAM3D_RANDOMNG_NEW()

  int64_t totalPoints = m->getTotalPoints();
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
		int gnum = m_GrainIds[zStride+yStride+k];
		x = float(k) * xRes;
		y = float(j) * yRes;
		z = float(i) * zRes;

		// if the grain IDs match...
		if (gnum == curGrain)
		{
		  // calculate the distance between the cell and the plane
		  float denom = 1 / sqrtf(sampleHabitPlane[0] * sampleHabitPlane[0] 
		  + sampleHabitPlane[1] * sampleHabitPlane[1] 
		  + sampleHabitPlane[2] * sampleHabitPlane[2]);

		  D = sampleHabitPlane[0] * x * denom 
			+ sampleHabitPlane[1] * y * denom 
			+ sampleHabitPlane[2] * z * denom 
			+ d * denom;  

		  // if the cell-plane distance is less than the plate thickness then place a twin voxel
		  // and that a neighboring cell is not a twin cell
		  if (fabs(D) < plateThickness)
////			&& (k == 0 || m_GrainIds[zStride+yStride+k-1] < numGrains || m_GrainIds[zStride+yStride+k-1] == totalFields) 
////			&& (k == (m->getXPoints() - 1) || m_GrainIds[zStride+yStride+k+1] < numGrains || m_GrainIds[zStride+yStride+k+1] == totalFields)
////			&& (j == 0 || m_GrainIds[zStride+yStride+k-xPoints] < numGrains || m_GrainIds[zStride+yStride+k-xPoints] == totalFields)
////			&& (j == (m->getYPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints] < numGrains || m_GrainIds[zStride+yStride+k+xPoints] == totalFields)
////			&& (i == 0 || m_GrainIds[zStride+yStride+k-xPoints*yPoints] < numGrains || m_GrainIds[zStride+yStride+k-xPoints*yPoints] == totalFields)
////			&& (i == (m->getZPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints*yPoints] < numGrains || m_GrainIds[zStride+yStride+k+xPoints*yPoints] == totalFields))
//			&& (k == 0 || j == 0 || m_GrainIds[zStride+yStride+k-xPoints-1] < numGrains || m_GrainIds[zStride+yStride+k-xPoints-1] == totalFields)
//			&& (k == (m->getXPoints() - 1) || j == 0 || m_GrainIds[zStride+yStride+k-xPoints+1] < numGrains || m_GrainIds[zStride+yStride+k-xPoints+1] == totalFields)
//			&& (k == 0 || j == (m->getYPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints-1] < numGrains || m_GrainIds[zStride+yStride+k+xPoints-1] == totalFields)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints+1] < numGrains || m_GrainIds[zStride+yStride+k+xPoints+1] == totalFields)
//			&& (k == 0 || j == 0 || i == (m->getZPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints*yPoints-xPoints-1] < numGrains || m_GrainIds[zStride+yStride+k+xPoints*yPoints-xPoints-1] == totalFields)
//			&& (k == (m->getXPoints() - 1) || j == 0 || i == (m->getZPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints*yPoints-xPoints+1] < numGrains || m_GrainIds[zStride+yStride+k+xPoints*yPoints-xPoints+1] == totalFields)
//			&& (k == 0 || j == (m->getYPoints() - 1) || i == (m->getZPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints*yPoints+xPoints-1] < numGrains || m_GrainIds[zStride+yStride+k+xPoints*yPoints+xPoints-1] == totalFields)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || i == (m->getZPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints*yPoints+xPoints+1] < numGrains || m_GrainIds[zStride+yStride+k+xPoints*yPoints+xPoints+1] == totalFields)
//			&& (k == 0 || j == 0 || i == 0 || m_GrainIds[zStride+yStride+k-xPoints*yPoints-xPoints-1] < numGrains || m_GrainIds[zStride+yStride+k-xPoints*yPoints-xPoints-1] == totalFields)
//			&& (k == (m->getXPoints() - 1) || j == 0 || i == 0 || m_GrainIds[zStride+yStride+k-xPoints*yPoints-xPoints+1] < numGrains || m_GrainIds[zStride+yStride+k-xPoints*yPoints-xPoints+1] == totalFields)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || i == 0 || m_GrainIds[zStride+yStride+k-xPoints*yPoints+xPoints-1] < numGrains || m_GrainIds[zStride+yStride+k-xPoints*yPoints+xPoints-1] == totalFields)
//			&& (k == (m->getXPoints() - 1) || j == (m->getYPoints() - 1) || i == 0 || m_GrainIds[zStride+yStride+k-xPoints*yPoints+xPoints+1] < numGrains || m_GrainIds[zStride+yStride+k-xPoints*yPoints+xPoints+1] == totalFields))
		  {	
			// check if an "island" twin voxel will be placed (excluding the first voxel placement)
			if ((k == 0 || m_GrainIds[zStride+yStride+k-1] != totalFields)
				&& (k == (m->getXPoints() - 1) || m_GrainIds[zStride+yStride+k+1] != totalFields)
				&& (j == 0 || m_GrainIds[zStride+yStride+k-xPoints] != totalFields)
				&& (j == (m->getYPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints] != totalFields)
				&& (i == 0 || m_GrainIds[zStride+yStride+k-xPoints*yPoints] != totalFields)
				&& (i == (m->getZPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints*yPoints] != totalFields)
				&& firstVoxel == false)
			{
			  // if an "island" twin voxel is inserted flip all the twin voxels back to the grain ID
			  // and this will go in the books as a failed twin insertion attempt
			  for (int64_t m = 0; m < totalPoints; ++m) 
			  {
			    if (m == totalFields) m = curGrain;
			  }
			    flag = false;
			}
			else
			{
			  m_GrainIds[zStride+yStride+k] = totalFields;
			  flag = true;
              firstVoxel = false;
			}
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
void InsertTwins::peninsula_twin(size_t curGrain, size_t totalFields)
{
  VoxelDataContainer* m = getVoxelDataContainer();
  DREAM3D_RANDOMNG_NEW()
  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());

  int x1 = -1, x2 = -1, y1 = -1, y2 = -1, z1 = -1, z2 = -1;
  float twinLength = 0.0f, random = 0.0f, fractionKept = 0.0f, currentDistance = 0.0f;
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
		int gnum = m_GrainIds[zStride+yStride+k];
		// if the grain IDs match...
		if (gnum == totalFields)
		{
		  if (x1 == -1)
		  {
			// establish one extremum of the twin
			x1 = i;
			y1 = j;
			z1 = k;
		  }
		  // establish the other extremum of the twin
		  x2 = i;
		  y2 = j;
		  z2 = k;
		}
	  }
	}
  }

  // calculate the distance between the extrema
  twinLength = sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
  // if the twin is 1 voxel, set the length to 2 so it makes it through the below loop
  // but won't change anything
  if (twinLength == 0.0f) twinLength = 2.0f;

  // choose which extremum to start from
  random = static_cast<float>(rg.genrand_res53());
  if (random < 0.5f)
  {
	x2 = x1;
	y2 = y1;
	z2 = z1;
  }

  // choose how much of the twin to keep (has to be at least one voxel)
  while (int(fractionKept * twinLength) < 1) fractionKept = static_cast<float>(rg.genrand_res53());

  // loop through again to decide which twin Ids get flipped back to grain Ids
  for(int i = 0; i < zPoints; i++)
  {
	zStride = i * xPoints * yPoints;
	for (int j = 0 ; j < yPoints; j++)
	{
	  yStride = j * xPoints;
	  for(int k = 0; k < xPoints; k++)
	  {
		int gnum = m_GrainIds[zStride+yStride+k];
		// if the grain IDs match...
		if (gnum == totalFields)
		{
			currentDistance = sqrtf((i-x1)*(i-x1) + (j-y1)*(j-y1) + (k-z1)*(k-z1));			
			// if the distance is longer than the twin length, flip back to the parent ID 
			if (currentDistance > twinLength * fractionKept) m_GrainIds[zStride+yStride+k] = curGrain;
		}
	  }
	}
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t InsertTwins::transfer_attributes(size_t totalFields, size_t totalPoints, QuatF q, float e[], size_t curGrain)
{
  VoxelDataContainer* m = getVoxelDataContainer();
  m->resizeFieldDataArrays(totalFields + 1);
  dataCheck(false, totalPoints, totalFields + 1, m->getNumEnsembleTuples());
  m_AvgQuats[4*totalFields+0] = q.x;
  m_AvgQuats[4*totalFields+1] = q.y;
  m_AvgQuats[4*totalFields+2] = q.z;
  m_AvgQuats[4*totalFields+3] = q.w;
  m_Active[totalFields] = true;
  m_FieldEulerAngles[3*totalFields+0] = e[0];
  m_FieldEulerAngles[3*totalFields+1] = e[1];
  m_FieldEulerAngles[3*totalFields+2] = e[2];
  m_FieldPhases[totalFields] = m->getNumEnsembleTuples()-1;
  m_FieldParentIds[totalFields] = curGrain;
  m_NumGrainsPerParent[totalFields] = 0;
  return ++totalFields;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::filter_calls()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  // mapping euler angles from field to cell
  for (int64_t i = 0; i < m->getNumCellTuples(); ++i)
  {
	m_CellEulerAngles[3*i+0] = m_FieldEulerAngles[3*m_GrainIds[i]+0];
	m_CellEulerAngles[3*i+1] = m_FieldEulerAngles[3*m_GrainIds[i]+1];
	m_CellEulerAngles[3*i+2] = m_FieldEulerAngles[3*m_GrainIds[i]+2];
  }

  FindGrainCentroids::Pointer find_grain_centroids = FindGrainCentroids::New();
  find_grain_centroids->setObservers(this->getObservers());
  find_grain_centroids->setVoxelDataContainer(m);
  find_grain_centroids->setMessagePrefix(getMessagePrefix());
  find_grain_centroids->execute();
  int err1 = find_grain_centroids->getErrorCondition();
  if (err1 < 0)
  {
    setErrorCondition(find_grain_centroids->getErrorCondition());
    addErrorMessages(find_grain_centroids->getPipelineMessages());
    return;
  }

  FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
  find_neighbors->setObservers(this->getObservers());
  find_neighbors->setVoxelDataContainer(m);
  find_neighbors->setMessagePrefix(getMessagePrefix());
  find_neighbors->execute();
  int err2 = find_neighbors->getErrorCondition();
  if (err2 < 0)
  {
    setErrorCondition(find_neighbors->getErrorCondition());
    addErrorMessages(find_neighbors->getPipelineMessages());
    return;
  }

  FindSurfaceGrains::Pointer find_surface_grains = FindSurfaceGrains::New();
  find_surface_grains->setObservers(this->getObservers());
  find_surface_grains->setVoxelDataContainer(m);
  find_surface_grains->setMessagePrefix(getMessagePrefix());
  find_surface_grains->execute();
  int err3 = find_surface_grains->getErrorCondition();
  if (err3 < 0)
  {
    setErrorCondition(find_surface_grains->getErrorCondition());
    addErrorMessages(find_surface_grains->getPipelineMessages());
    return;
  }

  FindSizes::Pointer find_sizes = FindSizes::New();
  find_sizes->setObservers(this->getObservers());
  find_sizes->setVoxelDataContainer(m);
  find_sizes->setMessagePrefix(getMessagePrefix());
  find_sizes->execute();
  int err4 = find_sizes->getErrorCondition();
  if (err4 < 0)
  {
    setErrorCondition(find_sizes->getErrorCondition());
    addErrorMessages(find_sizes->getPipelineMessages());
    return;
  }
}
