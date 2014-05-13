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
#include "DREAM3DLib\GenericFilters\FindGrainCentroids.h"
#include "DREAM3DLib\StatisticsFilters\FindNeighbors.h"
#include "DREAM3DLib\GenericFilters\FindSurfaceGrains.h"
#include "DREAM3DLib\StatisticsFilters\FindSizes.h"
#include "DREAM3DLib\ReconstructionFilters\ScalarSegmentGrains.h"
#include "DREAM3DLib\GenericFilters\RenameCellArray.h"
#include "DREAM3DLib\GenericFilters\RenameFieldArray.h"

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
  m_AvgQuatsArrayName(DREAM3D::FieldData::AvgQuats),
  m_ActiveArrayName(DREAM3D::FieldData::Active),
  m_CentroidsArrayName(DREAM3D::FieldData::Centroids),
  m_EquivalentDiametersArrayName(DREAM3D::FieldData::EquivalentDiameters),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
  m_FieldPhasesArrayName(DREAM3D::FieldData::Phases),
  m_FieldParentIdsArrayName(DREAM3D::FieldData::ParentIds),
  //m_NumParentsPer
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_TwinThickness(0.5f),
  m_NumTwinsPerGrain(2),
  m_CoherentFrac(1.0f),
  m_PeninsulaFrac(0.0f),
  m_UniqueRenum(false),
  m_GrainIds(NULL),
  m_AvgQuats(NULL),
  m_Active(NULL),
  m_Centroids(NULL),
  m_EquivalentDiameters(NULL),
  m_FieldEulerAngles(NULL),
  m_FieldPhases(NULL),
  m_CrystalStructures(NULL)
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
    option->setHumanLabel("Uniquely Renumber Contiguous Regions");
    option->setPropertyName("UniqueRenum");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
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
  setUniqueRenum( reader->readValue("UniqueRenum", getUniqueRenum()) );
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
  writer->writeValue("UniqueRenum", getUniqueRenum() );
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
  GET_PREREQ_DATA(m, DREAM3D, FieldData, AvgQuats, ss, -302, float, FloatArrayType, fields, 4)

  // Field Data
  GET_PREREQ_DATA(m, DREAM3D, FieldData, EquivalentDiameters, ss, -302, float, FloatArrayType, fields, 1)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, Centroids, ss, -304, float, FloatArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, -305, float, FloatArrayType, fields, 3)
  GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -306, int32_t, Int32ArrayType, fields, 1)
 
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, Active, ss, bool, BoolArrayType, true, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, FieldParentIds, ss, int32_t, Int32ArrayType, 0, fields, 1)

  // Ensemble Data  
  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -307, unsigned int, XTalStructArrayType, ensembles, 1)
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

  // start insert twins routine
  insert_twins();

  // if true, uniquely renumber so that every contiguous region receives a grain ID
  if (m_UniqueRenum == true) unique_renumber();

  // recalculating the stats that existed before this filter was run
  filter_calls();

  notifyStatusMessage("Completed");
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

  float sampleHabitPlane[3] = {0.0f,0.0f,0.0f};
  float crystalHabitPlane[3] = {0.0f,0.0f,0.0f};
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

  float voxelDiagonal = sqrtf(m->getXRes()*m->getXRes() + m->getYRes()*m->getYRes() + m->getZRes()*m->getZRes());

  for (size_t curGrain = 1; curGrain < numGrains; ++curGrain)
  {
    QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

	// pick a habit plane
	random2 = static_cast<float>(rg.genrand_res53());
	for (int i = 0; i < 3; ++i)
	{
	  crystalHabitPlane[i] = 1.0f;
	  // decide whether to make it coherent or incoherent
	  if (random2 > m_CoherentFrac) crystalHabitPlane[i] = int(rg.genrand_res53() * 20.0f);
	  random = static_cast<float>(rg.genrand_res53());
	  if (random < 0.5f) crystalHabitPlane[i] *= -1.0f;
	}

	// find where the habit plane points
	QuaternionMathF::Copy(avgQuats[curGrain], q1);
    OrientationMath::QuattoMat(q1, g);
	MatrixMath::Transpose3x3(g, gT);
    MatrixMath::Multiply3x3with3x1(g, crystalHabitPlane, sampleHabitPlane);

	// generate twin orientation with a 60 degree rotation about the habit plane
	OrientationMath::AxisAngletoMat(sig3, crystalHabitPlane[0], crystalHabitPlane[1], crystalHabitPlane[2], rotMat);
	MatrixMath::Multiply3x3with3x3(g, rotMat, newMat);
	OrientationMath::MatToEuler(newMat, e[0], e[1], e[2]);
	OrientationMath::EulertoQuat(q2, e[0], e[1], e[2]);

	// define plate = user input fraction of eq dia centered at centroid
	plateThickness = m_EquivalentDiameters[curGrain] * m_TwinThickness * 0.5f;

	// select number of twins to insert per grain from 2x user input to zero
	numTwins = int(rg.genrand_res53() * double(2*m_NumTwinsPerGrain + 1));
	shifts.resize(numTwins);
	int attempt = 0;
	for (int i = 0; i < numTwins; ++i)
	{
	  // define the shift placement from center
	  random = static_cast<float>(rg.genrand_res53());
	  shifts[i] = random * m_EquivalentDiameters[curGrain] * 0.5f;
	  random = static_cast<float>(rg.genrand_res53());
	  if (random < 0.5f) shifts[i] = -shifts[i];

	  // check if new twin will overlap an old twin
	  int ii = i;
	  for (int j = 0; j <= ii; ++j)
	  {
		// if adding more than one twin in a grain, check the current shift from
		// center against the previous ones to make sure the new twin does not
		// overlap.  NOTE that the twins can touch since we're using equiv dia,
		// they can be high aspect ratio grains inserted in the low aspect direction
		// which skews the calculation
		if (fabs(shifts[i] - shifts[j]) <= (plateThickness*2.0f + voxelDiagonal*2.0f) && ii != j) 
		{
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

		  random = static_cast<float>(rg.genrand_res53());
		  // change an isthmus twin to a peninsula twin
		  if (createdTwin == true) 
		  {
			if (random < m_PeninsulaFrac) peninsula_twin(curGrain, totalFields);
		  
			// filling in twin stats that already exist for parents
			totalFields = transfer_attributes(totalFields, totalPoints, q2, e, curGrain);
		  }
		}
	  }
	  // try 10 times to insert 2+ twins, if it can't, the grain is probably too small
	  if (attempt == 10) break;
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

  int xPoints = static_cast<int>(m->getXPoints());
  int yPoints = static_cast<int>(m->getYPoints());
  int zPoints = static_cast<int>(m->getZPoints());
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();
  bool flag = false;
  float x, y, z, D;

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
		  if (fabs(D) < plateThickness 
			&& (k == 0 || m_GrainIds[zStride+yStride+k-1] < numGrains || m_GrainIds[zStride+yStride+k-1] == totalFields) 
			&& (k == (m->getXPoints() - 1) || m_GrainIds[zStride+yStride+k+1] < numGrains || m_GrainIds[zStride+yStride+k+1] == totalFields)
			&& (j == 0 || m_GrainIds[zStride+yStride+k-xPoints] < numGrains || m_GrainIds[zStride+yStride+k-xPoints] == totalFields)
			&& (j == (m->getYPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints] < numGrains || m_GrainIds[zStride+yStride+k+xPoints] == totalFields)
			&& (i == 0 || m_GrainIds[zStride+yStride+k-xPoints*yPoints] < numGrains || m_GrainIds[zStride+yStride+k-xPoints*yPoints] == totalFields)
			&& (i == (m->getZPoints() - 1) || m_GrainIds[zStride+yStride+k+xPoints*yPoints] < numGrains || m_GrainIds[zStride+yStride+k+xPoints*yPoints] == totalFields))
		  {
			m_GrainIds[zStride+yStride+k] = totalFields;
			flag = true;
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
			x1 = i;
			y1 = j;
			z1 = k;
		  }
		  x2 = i;
		  y2 = j;
		  z2 = k;
		}
	  }
	}
  }

  // calculate the distance between cells
  twinLength = sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));

  // if the twin is 1 voxel, set the length to 2 so it makes it through the below loop
  if (twinLength == 0.0f) twinLength = 2.0f;

  // choose which end to start from
  random = static_cast<float>(rg.genrand_res53());
  if (random < 0.5f)
  {
	x2 = x1;
	y2 = y1;
	z2 = z1;
  }

  // choose how much of the twin to keep (has to be at least one voxel)
  while (int(fractionKept * twinLength) < 1) fractionKept = static_cast<float>(rg.genrand_res53());

  // loop through again to decide which twin Ids get flipped back
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
  m_Active[totalFields] = 1;
  m_FieldEulerAngles[3*totalFields+0] = e[0];
  m_FieldEulerAngles[3*totalFields+1] = e[1];
  m_FieldEulerAngles[3*totalFields+2] = e[2];
  m_FieldPhases[totalFields] = 2;
  m_FieldParentIds[totalFields] = curGrain;

  return ++totalFields;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::unique_renumber()
{
  VoxelDataContainer* m = getVoxelDataContainer();

  int32_t* oldGrainIds = reinterpret_cast<int32_t*>(m_GrainIds);

  RenameCellArray::Pointer rename_cell_array = RenameCellArray::New();
  rename_cell_array->setObservers(this->getObservers());
  rename_cell_array->setVoxelDataContainer(m);
  rename_cell_array->setMessagePrefix(getMessagePrefix());
  rename_cell_array->setSelectedCellArrayName(m_GrainIdsArrayName);
  rename_cell_array->setNewCellArrayName("oldGrainIds");
  rename_cell_array->preflight();
  int err1 = rename_cell_array->getErrorCondition();
  if (err1 < 0)
  {
    setErrorCondition(rename_cell_array->getErrorCondition());
    addErrorMessages(rename_cell_array->getPipelineMessages());
    return;
  }
  
  ScalarSegmentGrains::Pointer scalar_segment_grains = ScalarSegmentGrains::New();
  scalar_segment_grains->setObservers(this->getObservers());
  scalar_segment_grains->setVoxelDataContainer(m);
  scalar_segment_grains->setMessagePrefix(getMessagePrefix());
  scalar_segment_grains->setScalarArrayName("oldGrainIds");
  scalar_segment_grains->setScalarTolerance(0);
  scalar_segment_grains->execute();
  int err2 = scalar_segment_grains->getErrorCondition();
  if (err2 < 0)
  {
    setErrorCondition(scalar_segment_grains->getErrorCondition());
    addErrorMessages(scalar_segment_grains->getPipelineMessages());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InsertTwins::filter_calls()
{
  VoxelDataContainer* m = getVoxelDataContainer();

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