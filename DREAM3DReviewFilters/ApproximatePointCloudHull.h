/* ============================================================================
* Software developed by US federal government employees (including military personnel) 
* as part of their official duties is not subject to copyright protection and is 
* considered “public domain” (see 17 USC Section 105). Public domain software can be used 
* by anyone for any purpose, and cannot be released under a copyright license 
* (including typical open source software licenses).
* 
* This source code file was originally written by United States DoD employees. The
* original source code files are released into the Public Domain.
* 
* Subsequent changes to the codes by others may elect to add a copyright and license
* for those changes.
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _approximatepointcloudhull_h_
#define _approximatepointcloudhull_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The ApproximatePointCloudHull class. See [Filter documentation](@ref approximatepointcloudhull) for details.
 */
class ApproximatePointCloudHull : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ApproximatePointCloudHull)
  SIMPL_FILTER_NEW_MACRO(ApproximatePointCloudHull)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ApproximatePointCloudHull, AbstractFilter)

  ~ApproximatePointCloudHull() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, VertexDataContainerName)
  Q_PROPERTY(DataArrayPath VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, HullDataContainerName)
  Q_PROPERTY(QString HullDataContainerName READ getHullDataContainerName WRITE setHullDataContainerName)

  SIMPL_FILTER_PARAMETER(FloatVec3Type, GridResolution)
  Q_PROPERTY(FloatVec3Type GridResolution READ getGridResolution WRITE setGridResolution)

  SIMPL_FILTER_PARAMETER(int, NumberOfEmptyNeighbors)
  Q_PROPERTY(int NumberOfEmptyNeighbors READ getNumberOfEmptyNeighbors WRITE setNumberOfEmptyNeighbors)

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
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

  /**
  * @brief getUuid Return the unique identifier for this filter.
  * @return A QUuid object.
  */
  const QUuid getUuid() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  ApproximatePointCloudHull();

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck();

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

private:
  ImageGeom::Pointer m_SamplingGrid;
  std::vector<float> m_MeshMinExtents;
  std::vector<float> m_MeshMaxExtents;

  ApproximatePointCloudHull(const ApproximatePointCloudHull&) = delete; // Copy Constructor Not Implemented
  ApproximatePointCloudHull(ApproximatePointCloudHull&&) = delete;      // Move Constructor Not Implemented
  void operator=(const ApproximatePointCloudHull&) = delete;            // Operator '=' Not Implemented
};

#endif /* _ApproximatePointCloudHull_H_ */
