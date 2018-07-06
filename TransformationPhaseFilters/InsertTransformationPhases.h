/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2015 Dr. Joseph C. Tucker (UES, Inc.)
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

#pragma once

#include <vector>

#include <QtCore/QString>

#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "TransformationPhase/TransformationPhaseDLLExport.h"

/**
 * @class InsertTransformationPhases InsertTransformationPhases.h DREAM3DLib/SyntheticBuildingFilters/InsertTransformationPhases.h
 * @brief
 * @author Joseph C. Tucker (UES, Inc.)
 * @author Sudipto Mandal (CMU)
 * @date Feb 6, 2015
 * @version 5.1
 */
class TransformationPhase_EXPORT InsertTransformationPhases : public AbstractFilter
{
    Q_OBJECT
    PYB11_CREATE_BINDINGS(InsertTransformationPhases SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(int ParentPhase READ getParentPhase WRITE setParentPhase)
    PYB11_PROPERTY(int TransCrystalStruct READ getTransCrystalStruct WRITE setTransCrystalStruct)
    PYB11_PROPERTY(float TransformationPhaseMisorientation READ getTransformationPhaseMisorientation WRITE setTransformationPhaseMisorientation)
    PYB11_PROPERTY(FloatVec3_t TransformationPhaseHabitPlane READ getTransformationPhaseHabitPlane WRITE setTransformationPhaseHabitPlane)
    PYB11_PROPERTY(bool DefineHabitPlane READ getDefineHabitPlane WRITE setDefineHabitPlane)
    PYB11_PROPERTY(bool UseAllVariants READ getUseAllVariants WRITE setUseAllVariants)
    PYB11_PROPERTY(float CoherentFrac READ getCoherentFrac WRITE setCoherentFrac)
    PYB11_PROPERTY(float TransformationPhaseThickness READ getTransformationPhaseThickness WRITE setTransformationPhaseThickness)
    PYB11_PROPERTY(int NumTransformationPhasesPerFeature READ getNumTransformationPhasesPerFeature WRITE setNumTransformationPhasesPerFeature)
    PYB11_PROPERTY(float PeninsulaFrac READ getPeninsulaFrac WRITE setPeninsulaFrac)
    PYB11_PROPERTY(DataArrayPath StatsGenCellEnsembleAttributeMatrixPath READ getStatsGenCellEnsembleAttributeMatrixPath WRITE setStatsGenCellEnsembleAttributeMatrixPath)
    PYB11_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
    PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
    PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
    PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
    PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
    PYB11_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)
    PYB11_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)
    PYB11_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)
    PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
    PYB11_PROPERTY(QString FeatureParentIdsArrayName READ getFeatureParentIdsArrayName WRITE setFeatureParentIdsArrayName)
    PYB11_PROPERTY(DataArrayPath NumFeaturesPerParentArrayPath READ getNumFeaturesPerParentArrayPath WRITE setNumFeaturesPerParentArrayPath)
    PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
    PYB11_PROPERTY(DataArrayPath PhaseTypesArrayPath READ getPhaseTypesArrayPath WRITE setPhaseTypesArrayPath)
    PYB11_PROPERTY(DataArrayPath ShapeTypesArrayPath READ getShapeTypesArrayPath WRITE setShapeTypesArrayPath)
    PYB11_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)
  public:
    SIMPL_SHARED_POINTERS(InsertTransformationPhases)
    SIMPL_FILTER_NEW_MACRO(InsertTransformationPhases)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(InsertTransformationPhases, AbstractFilter)

    ~InsertTransformationPhases() override;

    SIMPL_FILTER_PARAMETER(int, ParentPhase)
    Q_PROPERTY(int ParentPhase READ getParentPhase WRITE setParentPhase)

    SIMPL_FILTER_PARAMETER(int, TransCrystalStruct)
    Q_PROPERTY(int TransCrystalStruct READ getTransCrystalStruct WRITE setTransCrystalStruct)

    SIMPL_FILTER_PARAMETER(float, TransformationPhaseMisorientation)
    Q_PROPERTY(float TransformationPhaseMisorientation READ getTransformationPhaseMisorientation WRITE setTransformationPhaseMisorientation)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, TransformationPhaseHabitPlane)
    Q_PROPERTY(FloatVec3_t TransformationPhaseHabitPlane READ getTransformationPhaseHabitPlane WRITE setTransformationPhaseHabitPlane)

    SIMPL_FILTER_PARAMETER(bool, DefineHabitPlane)
    Q_PROPERTY(bool DefineHabitPlane READ getDefineHabitPlane WRITE setDefineHabitPlane)

    SIMPL_FILTER_PARAMETER(bool, UseAllVariants)
    Q_PROPERTY(bool UseAllVariants READ getUseAllVariants WRITE setUseAllVariants)

    SIMPL_FILTER_PARAMETER(float, CoherentFrac)
    Q_PROPERTY(float CoherentFrac READ getCoherentFrac WRITE setCoherentFrac)

    SIMPL_FILTER_PARAMETER(float, TransformationPhaseThickness)
    Q_PROPERTY(float TransformationPhaseThickness READ getTransformationPhaseThickness WRITE setTransformationPhaseThickness)

    SIMPL_FILTER_PARAMETER(int, NumTransformationPhasesPerFeature)
    Q_PROPERTY(int NumTransformationPhasesPerFeature READ getNumTransformationPhasesPerFeature WRITE setNumTransformationPhasesPerFeature)

    SIMPL_FILTER_PARAMETER(float, PeninsulaFrac)
    Q_PROPERTY(float PeninsulaFrac READ getPeninsulaFrac WRITE setPeninsulaFrac)

    SIMPL_FILTER_PARAMETER(DataArrayPath, StatsGenCellEnsembleAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath StatsGenCellEnsembleAttributeMatrixPath READ getStatsGenCellEnsembleAttributeMatrixPath WRITE setStatsGenCellEnsembleAttributeMatrixPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixName)
    Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
    
    SIMPL_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
    Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
    Q_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(QString, FeatureParentIdsArrayName)
    Q_PROPERTY(QString FeatureParentIdsArrayName READ getFeatureParentIdsArrayName WRITE setFeatureParentIdsArrayName)

    SIMPL_FILTER_PARAMETER(DataArrayPath, NumFeaturesPerParentArrayPath)
    Q_PROPERTY(DataArrayPath NumFeaturesPerParentArrayPath READ getNumFeaturesPerParentArrayPath WRITE setNumFeaturesPerParentArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, PhaseTypesArrayPath)
    Q_PROPERTY(DataArrayPath PhaseTypesArrayPath READ getPhaseTypesArrayPath WRITE setPhaseTypesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, ShapeTypesArrayPath)
    Q_PROPERTY(DataArrayPath ShapeTypesArrayPath READ getShapeTypesArrayPath WRITE setShapeTypesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
    Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    const QString getCompiledLibraryName() const override;

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    const QString getBrandingString() const override;

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    const QString getFilterVersion() const override;

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    const QString getGroupName() const override;

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    const QString getSubGroupName() const override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    const QUuid getUuid() override;

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    const QString getHumanLabel() const override;

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    void setupFilterParameters() override;

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    void preflight() override;

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    InsertTransformationPhases();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

    /**
     * @brief insertTransformationPhases
     */
    void insertTransformationPhases();

    /**
     * @brief placeTransformationPhase
     * @param curFeature
     * @param sample111
     * @param totalFeatures
     * @param plateThickness
     * @param d
     * @param numFeatures
     * @param euler
     * @return
     */
    bool placeTransformationPhase(int32_t curFeature, float sample111[], int32_t totalFeatures, float plateThickness, float d, float* euler);

    /**
     * @brief peninsulaTransformationPhase
     * @param curFeature
     * @param totalFeatures
     */
    void peninsulaTransformationPhase(int32_t curFeature, int32_t totalFeatures);

    /**
     * @brief transferAttributes
     * @param totalFeatures
     * @param q
     * @param e
     * @param curFeature
     * @return
     */
    size_t transferAttributes(size_t totalFeatures, QuatF q, float e[], int32_t curFeature);

  private:
    QVector<LaueOps::Pointer> m_OrientationOps;

    // Cell Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)

    // Feature Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_DATAARRAY_WEAKPTR(bool, Active)
    DEFINE_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_DATAARRAY_VARIABLE(float, EquivalentDiameters)
    DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureParentIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumFeaturesPerParent)

    // Ensemble Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(PhaseType::EnumType, PhaseTypes)
    DEFINE_DATAARRAY_VARIABLE(ShapeType::EnumType, ShapeTypes)
    DEFINE_DATAARRAY_VARIABLE(int32_t, NumFeatures)

    FloatVec3_t m_NormalizedTransformationPhaseHabitPlane;

    /**
     * @brief updateFeatureInstancePointers
     */
    void updateFeatureInstancePointers();

    /**
     * @brief updateStatsGenEnsembleInstancePointers
     */
    void updateStatsGenEnsembleInstancePointers();

    /**
     * @brief updateVolEnsembleInstancePointers
     */
    void updateVolEnsembleInstancePointers();

  public:
    InsertTransformationPhases(const InsertTransformationPhases&) = delete; // Copy Constructor Not Implemented
    InsertTransformationPhases(InsertTransformationPhases&&) = delete;      // Move Constructor
    InsertTransformationPhases& operator=(const InsertTransformationPhases&) = delete; // Copy Assignment Not Implemented
    InsertTransformationPhases& operator=(InsertTransformationPhases&&) = delete;      // Move Assignment
};

