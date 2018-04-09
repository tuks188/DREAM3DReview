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

#ifndef _findcslboundaries_h_
#define _findcslboundaries_h_

#include <vector>
#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/LaueOps/LaueOps.h"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"

/**
 * @class FindCSLBoundaries FindCSLBoundaries.h /TransformationPhase/FindCSLBoundaries.h
 * @brief
 * @author Joseph C. Tucker
 * @date July 28, 2014
 * @version 5.1
 */
class FindCSLBoundaries : public AbstractFilter
{
    Q_OBJECT
    PYB11_CREATE_BINDINGS(FindCSLBoundaries SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(float CSL READ getCSL WRITE setCSL)
    PYB11_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)
    PYB11_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)
    PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
    PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
    PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
    PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)
    PYB11_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)
    PYB11_PROPERTY(QString SurfaceMeshCSLBoundaryArrayName READ getSurfaceMeshCSLBoundaryArrayName WRITE setSurfaceMeshCSLBoundaryArrayName)
    PYB11_PROPERTY(QString SurfaceMeshCSLBoundaryIncoherenceArrayName READ getSurfaceMeshCSLBoundaryIncoherenceArrayName WRITE setSurfaceMeshCSLBoundaryIncoherenceArrayName)
  public:
    SIMPL_SHARED_POINTERS(FindCSLBoundaries)
    SIMPL_FILTER_NEW_MACRO(FindCSLBoundaries)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindCSLBoundaries, AbstractFilter)

    ~FindCSLBoundaries() override;

    SIMPL_FILTER_PARAMETER(float, CSL)
    Q_PROPERTY(float CSL READ getCSL WRITE setCSL)
    SIMPL_FILTER_PARAMETER(float, AxisTolerance)
    Q_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)
    SIMPL_FILTER_PARAMETER(float, AngleTolerance)
    Q_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)

    SIMPL_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
    Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceNormalsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceNormalsArrayPath READ getSurfaceMeshFaceNormalsArrayPath WRITE setSurfaceMeshFaceNormalsArrayPath)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshCSLBoundaryArrayName)
    Q_PROPERTY(QString SurfaceMeshCSLBoundaryArrayName READ getSurfaceMeshCSLBoundaryArrayName WRITE setSurfaceMeshCSLBoundaryArrayName)

    SIMPL_FILTER_PARAMETER(QString, SurfaceMeshCSLBoundaryIncoherenceArrayName)
    Q_PROPERTY(QString SurfaceMeshCSLBoundaryIncoherenceArrayName READ getSurfaceMeshCSLBoundaryIncoherenceArrayName WRITE setSurfaceMeshCSLBoundaryIncoherenceArrayName)

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
    FindCSLBoundaries();

  private:
    QVector<LaueOps::Pointer> m_OrientationOps;
    CubicOps::Pointer m_CubicOps;
    HexagonalOps::Pointer m_HexOps;
    OrthoRhombicOps::Pointer m_OrthoOps;

    DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_DATAARRAY_VARIABLE(double, SurfaceMeshFaceNormals)
    DEFINE_DATAARRAY_VARIABLE(bool, SurfaceMeshCSLBoundary)
    DEFINE_DATAARRAY_VARIABLE(float, SurfaceMeshCSLBoundaryIncoherence)

    void dataCheckVoxel();
    void dataCheckSurfaceMesh();

  public:
    FindCSLBoundaries(const FindCSLBoundaries&) = delete; // Copy Constructor Not Implemented
    FindCSLBoundaries(FindCSLBoundaries&&) = delete;      // Move Constructor
    FindCSLBoundaries& operator=(const FindCSLBoundaries&) = delete; // Copy Assignment Not Implemented
    FindCSLBoundaries& operator=(FindCSLBoundaries&&) = delete;      // Move Assignment
};

#endif /* FindCSLBoundaries_H_ */


