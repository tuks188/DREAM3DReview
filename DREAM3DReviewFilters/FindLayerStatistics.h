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
#ifndef _findlayerstatistics_h_
#define _findlayerstatistics_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The FindLayerStatistics class. See [Filter documentation](@ref findlayerstatistics) for details.
 */
class FindLayerStatistics : public AbstractFilter
{
  Q_OBJECT
public:
  SIMPL_SHARED_POINTERS(FindLayerStatistics)
  SIMPL_FILTER_NEW_MACRO(FindLayerStatistics)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindLayerStatistics, AbstractFilter)

  ~FindLayerStatistics() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, SelectedArrayPath)
  Q_PROPERTY(DataArrayPath SelectedArrayPath READ getSelectedArrayPath WRITE setSelectedArrayPath)

  SIMPL_FILTER_PARAMETER(unsigned int, Plane)
  Q_PROPERTY(unsigned int Plane READ getPlane WRITE setPlane)

  SIMPL_FILTER_PARAMETER(QString, LayerIDsArrayName)
  Q_PROPERTY(QString LayerIDsArrayName READ getLayerIDsArrayName WRITE setLayerIDsArrayName)

  SIMPL_FILTER_PARAMETER(QString, LayerAttributeMatrixName)
  Q_PROPERTY(QString LayerAttributeMatrixName READ getLayerAttributeMatrixName WRITE setLayerAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, LayerMinArrayName)
  Q_PROPERTY(QString LayerMinArrayName READ getLayerMinArrayName WRITE setLayerMinArrayName)

  SIMPL_FILTER_PARAMETER(QString, LayerMaxArrayName)
  Q_PROPERTY(QString LayerMaxArrayName READ getLayerMaxArrayName WRITE setLayerMaxArrayName)

  SIMPL_FILTER_PARAMETER(QString, LayerAvgArrayName)
  Q_PROPERTY(QString LayerAvgArrayName READ getLayerAvgArrayName WRITE setLayerAvgArrayName)

  SIMPL_FILTER_PARAMETER(QString, LayerStdArrayName)
  Q_PROPERTY(QString LayerStdArrayName READ getLayerStdArrayName WRITE setLayerStdArrayName)

  SIMPL_FILTER_PARAMETER(QString, LayerVarArrayName)
  Q_PROPERTY(QString LayerVarArrayName READ getLayerVarArrayName WRITE setLayerVarArrayName)

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
  FindLayerStatistics();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_IDATAARRAY_VARIABLE(InData)

  DEFINE_DATAARRAY_VARIABLE(int32_t, LayerIDs)
  DEFINE_DATAARRAY_VARIABLE(float, LayerMin)
  DEFINE_DATAARRAY_VARIABLE(float, LayerMax)
  DEFINE_DATAARRAY_VARIABLE(float, LayerAvg)
  DEFINE_DATAARRAY_VARIABLE(float, LayerStd)
  DEFINE_DATAARRAY_VARIABLE(float, LayerVar)

  FindLayerStatistics(const FindLayerStatistics&) = delete; // Copy Constructor Not Implemented
  FindLayerStatistics(FindLayerStatistics&&) = delete;      // Move Constructor Not Implemented
  void operator=(const FindLayerStatistics&) = delete;      // Operator '=' Not Implemented
};

#endif /* _findlayerstatistics_h_ */
