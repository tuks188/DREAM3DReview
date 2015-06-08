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

#include "FindCSLBoundaries.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"



#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "TransformationPhase/TransformationPhaseConstants.h"

#include "Plugins/Statistics/StatisticsConstants.h"

class CalculateCSLBoundaryImpl
{
    int m_CSLIndex;
    float m_AxisTol;
    float m_AngTol;
    int32_t* m_Labels;
    double* m_Normals;
    int32_t* m_Phases;
    float* m_Quats;
    bool* m_CSLBoundary;
    float* m_CSLBoundaryIncoherence;
    unsigned int* m_CrystalStructures;
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;

  public:
    CalculateCSLBoundaryImpl(int cslindex, float angtol, float axistol, int32_t* Labels, double* Normals, float* Quats, int32_t* Phases, unsigned int* CrystalStructures, bool* CSLBoundary, float* CSLBoundaryIncoherence) :
      m_CSLIndex(cslindex),
      m_AxisTol(axistol),
      m_AngTol(angtol),
      m_Labels(Labels),
      m_Normals(Normals),
      m_Phases(Phases),
      m_Quats(Quats),
      m_CSLBoundary(CSLBoundary),
      m_CSLBoundaryIncoherence(CSLBoundaryIncoherence),
      m_CrystalStructures(CrystalStructures)
    {
      m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
    }

    virtual ~CalculateCSLBoundaryImpl() {}

    void generate(size_t start, size_t end) const
    {
      int feature1, feature2;
      float normal[3];
      float g1[3][3];
      float w;
      unsigned int phase1, phase2;
      QuatF q1;
      QuatF q2;
      float axisdiffCSL, angdiffCSL;
      float n[3];
      float incoherence;
      float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;

      QuatF misq;
      QuatF sym_q;
      QuatF s1_misq;
      QuatF s2_misq;
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      float xstl_norm[3], s_xstl_norm[3], cslAxisNorm[3];
      float cslAxisNormDenom = 0.0f;
      cslAxisNormDenom = sqrtf(TransformationPhaseConstants::CSLAxisAngle[m_CSLIndex][2] + TransformationPhaseConstants::CSLAxisAngle[m_CSLIndex][3] + TransformationPhaseConstants::CSLAxisAngle[m_CSLIndex][4]);
      for (int i = 0; i < 3; ++i) { cslAxisNorm[i] = TransformationPhaseConstants::CSLAxisAngle[m_CSLIndex][i+2] / cslAxisNormDenom; };
      for (size_t i = start; i < end; i++)
      {
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        normal[0] = m_Normals[3 * i];
        normal[1] = m_Normals[3 * i + 1];
        normal[2] = m_Normals[3 * i + 2];
    // different than Find Twin Boundaries here because will only compare if
    // the features are different phases
        if(feature1 > 0 && feature2 > 0)// && m_Phases[feature1] != m_Phases[feature2])
        {
          w = 10000.0;

          QuaternionMathF::Copy(quats[feature1], q1);
          QuaternionMathF::Copy(quats[feature2], q2);

          phase1 = m_CrystalStructures[m_Phases[feature1]];
          phase2 = m_CrystalStructures[m_Phases[feature2]];
          if (phase1 == phase2)
          {
            int nsym = m_OrientationOps[phase1]->getNumSymOps();
            QuaternionMathF::Conjugate(q2);
            QuaternionMathF::Multiply(q2, q1, misq);
            FOrientArrayType om(9);
            FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
            om.toGMatrix(g1);
            MatrixMath::Multiply3x3with3x1(g1, normal, xstl_norm);
            for (int j = 0; j < nsym; j++)
            {
              m_OrientationOps[phase1]->getQuatSymOp(j, sym_q);
              //calculate crystal direction parallel to normal
              QuaternionMathF::Multiply(sym_q, misq, s1_misq);
              OrientationMath::MultiplyQuaternionVector(sym_q, xstl_norm, s_xstl_norm);
              for (int k = 0; k < nsym; k++)
              {
                //calculate the symmetric misorienation
                m_OrientationOps[phase1]->getQuatSymOp(k, sym_q);
                QuaternionMathF::Conjugate(sym_q);
                QuaternionMathF::Multiply(s1_misq, sym_q, s2_misq);

                FOrientArrayType ax(4);
                FOrientTransformsType::qu2ax(FOrientArrayType(s2_misq), ax);
                ax.toAxisAngle(n1, n2, n3, w);
                w = w * 180.0 / DREAM3D::Constants::k_Pi;
                axisdiffCSL = acosf(fabs(n1) * cslAxisNorm[0] + fabs(n2) * cslAxisNorm[1] + fabs(n3) * cslAxisNorm[2]);
                angdiffCSL = fabs(w - TransformationPhaseConstants::CSLAxisAngle[m_CSLIndex][1]);
                if (axisdiffCSL < m_AxisTol && angdiffCSL < m_AngTol)
                {
                  n[0] = n1;
                  n[1] = n2;
                  n[2] = n3;
                  m_CSLBoundary[i] = true;
                  incoherence = 180.0 * acos(GeometryMath::CosThetaBetweenVectors(n, s_xstl_norm)) / DREAM3D::Constants::k_Pi;
                  if(incoherence > 90.0) { incoherence = 180.0 - incoherence; }
                  if(incoherence < m_CSLBoundaryIncoherence[i]) { m_CSLBoundaryIncoherence[i] = incoherence; }
                }
              }
            }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCSLBoundaries::FindCSLBoundaries()  :
  AbstractFilter(),
  m_CSL(3.0f),
  m_AxisTolerance(0.0f),
  m_AngleTolerance(0.0f),
  m_AvgQuatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_FeaturePhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshCSLBoundaryArrayName(TransformationPhaseConstants::SurfaceMeshCSLBoundary),
  m_SurfaceMeshCSLBoundaryIncoherenceArrayName(TransformationPhaseConstants::SurfaceMeshCSLBoundaryIncoherence),
  m_AvgQuatsArrayName(DREAM3D::FeatureData::AvgQuats),
  m_AvgQuats(NULL),
  m_FeaturePhasesArrayName(DREAM3D::FeatureData::Phases),
  m_FeaturePhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshCSLBoundary(NULL),
  m_SurfaceMeshCSLBoundaryIncoherence(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindCSLBoundaries::~FindCSLBoundaries()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCSLBoundaries::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("CSL", "CSL", FilterParameterWidgetType::DoubleWidget , getCSL(), false, "Sigma"));
  parameters.push_back(FilterParameter::New("Axis Tolerance", "AxisTolerance", FilterParameterWidgetType::DoubleWidget, getAxisTolerance(), FilterParameter::Uncategorized, "Degrees"));
  parameters.push_back(FilterParameter::New("Angle Tolerance", "AngleTolerance", FilterParameterWidgetType::DoubleWidget, getAngleTolerance(), FilterParameter::Uncategorized, "Degrees"));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("AvgQuats", "AvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getAvgQuatsArrayPath(), FilterParameter::Uncategorized, ""));
  parameters.push_back(FilterParameter::New("FeaturePhases", "FeaturePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getFeaturePhasesArrayPath(), FilterParameter::Uncategorized, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), FilterParameter::Uncategorized, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::Uncategorized, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshFaceNormals", "SurfaceMeshFaceNormalsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSurfaceMeshFaceNormalsArrayPath(), FilterParameter::Uncategorized, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("SurfaceMeshCSLBoundary", "SurfaceMeshCSLBoundaryArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshCSLBoundaryArrayName(), FilterParameter::Uncategorized, ""));
  parameters.push_back(FilterParameter::New("SurfaceMeshCSLBoundaryIncoherence", "SurfaceMeshCSLBoundaryIncoherenceArrayName", FilterParameterWidgetType::StringWidget, getSurfaceMeshCSLBoundaryIncoherenceArrayName(), FilterParameter::Uncategorized, ""));
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void FindCSLBoundaries::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshCSLBoundaryIncoherenceArrayName(reader->readString("SurfaceMeshCSLBoundaryIncoherenceArrayName", getSurfaceMeshCSLBoundaryIncoherenceArrayName() ) );
  setSurfaceMeshCSLBoundaryArrayName(reader->readString("SurfaceMeshCSLBoundaryArrayName", getSurfaceMeshCSLBoundaryArrayName() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setCSL( reader->readValue("CSL", getCSL() ) );
  setAxisTolerance( reader->readValue("AxisTolerance", getAxisTolerance() ) );
  setAngleTolerance( reader->readValue("AngleTolerance", getAngleTolerance()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindCSLBoundaries::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshCSLBoundaryIncoherenceArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshCSLBoundaryArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CSL)
  DREAM3D_FILTER_WRITE_PARAMETER(AxisTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(AngleTolerance)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCSLBoundaries::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<size_t> dims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCSLBoundaries::dataCheckSurfaceMesh()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshCSLBoundaryArrayName() );
  m_SurfaceMeshCSLBoundaryPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshCSLBoundaryPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshCSLBoundary = m_SurfaceMeshCSLBoundaryPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshCSLBoundaryIncoherenceArrayName() );
  m_SurfaceMeshCSLBoundaryIncoherencePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 180.0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshCSLBoundaryIncoherencePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshCSLBoundaryIncoherence = m_SurfaceMeshCSLBoundaryIncoherencePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCSLBoundaries::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheckVoxel();
  dataCheckSurfaceMesh();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindCSLBoundaries::execute()
{
  setErrorCondition(0);
  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  int64_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

  int cslindex = 0;
  for (int i = 0; i < 21; ++i)
  {
  if (static_cast<int>( TransformationPhaseConstants::CSLAxisAngle[i][0] ) == static_cast<int>( m_CSL ))
  {
    cslindex = i;
    break;
  }
  }
  float angtol = m_AngleTolerance;
  float axistol = static_cast<float>( m_AxisTolerance * M_PI / 180.0f );

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateCSLBoundaryImpl(cslindex, angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshCSLBoundary, m_SurfaceMeshCSLBoundaryIncoherence), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateCSLBoundaryImpl serial(cslindex, angtol, axistol, m_SurfaceMeshFaceLabels, m_SurfaceMeshFaceNormals, m_AvgQuats, m_FeaturePhases, m_CrystalStructures, m_SurfaceMeshCSLBoundary, m_SurfaceMeshCSLBoundaryIncoherence);
    serial.generate(0, numTriangles);
  }

  notifyStatusMessage(getHumanLabel(), "FindCSLBoundaries Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindCSLBoundaries::newFilterInstance(bool copyFilterParameters)
{
  FindCSLBoundaries::Pointer filter = FindCSLBoundaries::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCSLBoundaries::getCompiledLibraryName()
{
  return TransformationPhaseConstants::TransformationPhaseBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCSLBoundaries::getGroupName()
{
  return TransformationPhaseConstants::TransformationPhaseFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCSLBoundaries::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::CrystallographicFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindCSLBoundaries::getHumanLabel()
{
  return "Find CSL Boundaries";
}

