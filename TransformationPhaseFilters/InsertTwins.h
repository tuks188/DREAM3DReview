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

#ifndef INSERTTWINS_H_
#define INSERTTWINS_H_

#include <vector>
#include <string>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/DataArrays/NeighborList.hpp"


/**
 * @class InsertTwins InsertTwins.h DREAM3DLib/SyntheticBuildingFilters/InsertTwins.h
 * @brief
 * @author
 * @date May 21, 2014
 * @version 1.0
 */
class DREAM3DLib_EXPORT InsertTwins : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(InsertTwins)
    DREAM3D_STATIC_NEW_MACRO(InsertTwins)
    DREAM3D_TYPE_MACRO_SUPER(InsertTwins, AbstractFilter)

    virtual ~InsertTwins();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
    //------ Created Cell Data
    //------ Required Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(AvgQuatsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CentroidsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EquivalentDiametersArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldPhasesArrayName)
    //------ Created Field Data
    DREAM3D_INSTANCE_STRING_PROPERTY(ActiveArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldParentIdsArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(NumGrainsPerParentArrayName)
    //------ Required Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseTypesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ShapeTypesArrayName)
    //------ Created Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(NumFieldsArrayName)

    DREAM3D_INSTANCE_PROPERTY(float, TwinThickness)
    DREAM3D_INSTANCE_PROPERTY(int, NumTwinsPerGrain)
    DREAM3D_INSTANCE_PROPERTY(float, CoherentFrac)
    DREAM3D_INSTANCE_PROPERTY(float, PeninsulaFrac)
    DREAM3D_INSTANCE_PROPERTY(int, VariantNum)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SyntheticBuildingFilters; }
	virtual const std::string getSubGroupName() {return DREAM3D::FilterSubGroups::PackingFilters;}
    virtual const std::string getHumanLabel() { return "Insert Twins"; }

    virtual void setupFilterParameters();
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
    virtual void preflight();

	void insert_twins();
	bool place_twin(size_t curGrain, float sample111[], size_t totalFields, float plateThickness, float d, size_t numGrains);
    void peninsula_twin(size_t curGrain, size_t totalFields);
	size_t transfer_attributes(size_t totalFields, size_t totalPoints, QuatF q, float e[], size_t curGrain);
	void filter_calls();

  protected:
    InsertTwins();

  private:
    int32_t* m_GrainIds;
    float* m_CellEulerAngles;
    float* m_AvgQuats;
    bool* m_Active;
    float* m_Centroids;
    float* m_EquivalentDiameters;
	float* m_FieldEulerAngles;
	int32_t* m_FieldPhases;
	int32_t* m_FieldParentIds;
	int32_t* m_NumGrainsPerParent;

    unsigned int* m_CrystalStructures;
    unsigned int* m_PhaseTypes;
    unsigned int* m_ShapeTypes;
    int32_t* m_NumFields;

    std::vector<OrientationOps::Pointer> m_OrientationOps;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


    InsertTwins(const InsertTwins&); // Copy Constructor Not Implemented
    void operator=(const InsertTwins&); // Operator '=' Not Implemented
};

#endif /* INSERTTWINS_H_ */
