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
#ifndef _TiDwellFatigueCrystallographicAnalysis_H_
#define _TiDwellFatigueCrystallographicAnalysis_H_

#include <QtCore/QString>

#include <boost/shared_array.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "Plugins/Statistics/StatisticsConstants.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"
#include "DREAM3DLib/FilterParameters/FloatVec3FilterParameter.h"

/**
 * @class TiDwellFatigueCrystallographicAnalysis TiDwellFatigueCrystallographicAnalysis.h /TransformationPhase/TiDwellFatigueCrystallographicAnalysis.h
 * @brief
 * @author Joseph C. Tucker
 * @date Jan 23, 2015
 * @version 5.1
 */
class TiDwellFatigueCrystallographicAnalysis : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(TiDwellFatigueCrystallographicAnalysis)
    DREAM3D_STATIC_NEW_MACRO(TiDwellFatigueCrystallographicAnalysis)
    DREAM3D_TYPE_MACRO_SUPER(TiDwellFatigueCrystallographicAnalysis, AbstractFilter)

    virtual ~TiDwellFatigueCrystallographicAnalysis();
    DREAM3D_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    DREAM3D_FILTER_PARAMETER(bool, AlphaGlobPhasePresent)
    Q_PROPERTY(bool AlphaGlobPhasePresent READ getAlphaGlobPhasePresent WRITE setAlphaGlobPhasePresent)
    DREAM3D_FILTER_PARAMETER(int, AlphaGlobPhase)
    Q_PROPERTY(int AlphaGlobPhase READ getAlphaGlobPhase WRITE setAlphaGlobPhase)
    DREAM3D_FILTER_PARAMETER(int, MTRPhase)
    Q_PROPERTY(int MTRPhase READ getMTRPhase WRITE setMTRPhase)
    DREAM3D_FILTER_PARAMETER(float, LatticeParameterA)
    Q_PROPERTY(float LatticeParameterA READ getLatticeParameterA WRITE setLatticeParameterA)
    DREAM3D_FILTER_PARAMETER(float, LatticeParameterC)
    Q_PROPERTY(float LatticeParameterC READ getLatticeParameterC WRITE setLatticeParameterC)
    DREAM3D_FILTER_PARAMETER(FloatVec3_t, StressAxis)
    Q_PROPERTY(FloatVec3_t StressAxis READ getStressAxis WRITE setStressAxis)
    DREAM3D_FILTER_PARAMETER(int, SubsurfaceDistance)
    Q_PROPERTY(int SubsurfaceDistance READ getSubsurfaceDistance WRITE setSubsurfaceDistance)
    Q_PROPERTY(float ConsiderationFraction READ getConsiderationFraction WRITE setConsiderationFraction)
    DREAM3D_FILTER_PARAMETER(float, ConsiderationFraction)

    DREAM3D_FILTER_PARAMETER(bool, DoNotAssumeInitiatorPresence)
    Q_PROPERTY(bool DoNotAssumeInitiatorPresence READ getDoNotAssumeInitiatorPresence WRITE setDoNotAssumeInitiatorPresence)
    DREAM3D_FILTER_PARAMETER(float, InitiatorLowerThreshold)
    Q_PROPERTY(float InitiatorLowerThreshold READ getInitiatorLowerThreshold WRITE setInitiatorLowerThreshold)
    DREAM3D_FILTER_PARAMETER(float, InitiatorUpperThreshold)
    Q_PROPERTY(float InitiatorUpperThreshold READ getInitiatorUpperThreshold WRITE setInitiatorUpperThreshold)
    DREAM3D_FILTER_PARAMETER(float, HardFeatureLowerThreshold)
    Q_PROPERTY(float HardFeatureLowerThreshold READ getHardFeatureLowerThreshold WRITE setHardFeatureLowerThreshold)
    DREAM3D_FILTER_PARAMETER(float, HardFeatureUpperThreshold)
    Q_PROPERTY(float HardFeatureUpperThreshold READ getHardFeatureUpperThreshold WRITE setHardFeatureUpperThreshold)
    DREAM3D_FILTER_PARAMETER(float, SoftFeatureLowerThreshold)
    Q_PROPERTY(float SoftFeatureLowerThreshold READ getSoftFeatureLowerThreshold WRITE setSoftFeatureLowerThreshold)
    DREAM3D_FILTER_PARAMETER(float, SoftFeatureUpperThreshold)
    Q_PROPERTY(float SoftFeatureUpperThreshold READ getSoftFeatureUpperThreshold WRITE setSoftFeatureUpperThreshold)


    DREAM3D_FILTER_PARAMETER(QString,NewCellFeatureAttributeMatrixName)
    Q_PROPERTY(QString NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(QString, SelectedFeaturesArrayName)
    Q_PROPERTY(QString SelectedFeaturesArrayName READ getSelectedFeaturesArrayName WRITE setSelectedFeaturesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, InitiatorsArrayName)
    Q_PROPERTY(QString InitiatorsArrayName READ getInitiatorsArrayName WRITE setInitiatorsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, HardFeaturesArrayName)
    Q_PROPERTY(QString HardFeaturesArrayName READ getHardFeaturesArrayName WRITE setHardFeaturesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, SoftFeaturesArrayName)
    Q_PROPERTY(QString SoftFeaturesArrayName READ getSoftFeaturesArrayName WRITE setSoftFeaturesArrayName)

    DREAM3D_FILTER_PARAMETER(QString, HardSoftGroupsArrayName)
    Q_PROPERTY(QString HardSoftGroupsArrayName READ getHardSoftGroupsArrayName WRITE setHardSoftGroupsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
    Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
	
	DREAM3D_FILTER_PARAMETER(QString, CellParentIdsArrayName)
    Q_PROPERTY(QString CellParentIdsArrayName READ getCellParentIdsArrayName WRITE setCellParentIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, FeatureParentIdsArrayName)
    Q_PROPERTY(QString FeatureParentIdsArrayName READ getFeatureParentIdsArrayName WRITE setFeatureParentIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, ActiveArrayName)
    Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
    Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
    Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    /**
    * @brief This returns the group that the filter belongs to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel();
    
    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    TiDwellFatigueCrystallographicAnalysis();

    bool determine_subsurfacefeatures(int index);
    bool determine_hardfeatures(int index);
    void determine_initiators(int index);
	void determine_softfeatures(int index);
	void group_flaggedfeatures(int index);
    void assign_HardSoftGroups(int index);
    float find_angle(float g[3][3], float planeNormalU, float planeNormalV, float planeNormalW);

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();
    void updateFeatureInstancePointers();

  private:
    DEFINE_DATAARRAY_VARIABLE(bool, SelectedFeatures)
    DEFINE_DATAARRAY_VARIABLE(bool, Initiators)
    DEFINE_DATAARRAY_VARIABLE(bool, HardFeatures)
    DEFINE_DATAARRAY_VARIABLE(bool, SoftFeatures)
    DEFINE_DATAARRAY_VARIABLE(bool, HardSoftGroups)

    // Feature Data - make sure these are all initialized to NULL in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
	DEFINE_DATAARRAY_VARIABLE(int32_t, CellParentIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureParentIds)
    DEFINE_DATAARRAY_VARIABLE(bool, Active)
	DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    NeighborList<int>::WeakPointer m_NeighborList;
    NeighborList<int>::WeakPointer m_ParentNeighborList;
    DEFINE_DATAARRAY_VARIABLE(float, Centroids)

    // Ensemble Data - make sure these are all initialized to NULL in the constructor
    DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)

    TiDwellFatigueCrystallographicAnalysis(const TiDwellFatigueCrystallographicAnalysis&); // Copy Constructor Not Implemented
    void operator=(const TiDwellFatigueCrystallographicAnalysis&); // Operator '=' Not Implemented
};

#endif /* _TiDwellFatigueCrystallographicAnalysis_H_ */


