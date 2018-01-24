/*
 * Your License or Copyright can go here
 */

#ifndef _importmassifdata_h_
#define _importmassifdata_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The ImportMASSIFData class. See [Filter documentation](@ref importmassifdata) for details.
 */
class ImportMASSIFData : public AbstractFilter
{
  Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(ImportMASSIFData)
    SIMPL_STATIC_NEW_MACRO(ImportMASSIFData)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ImportMASSIFData, AbstractFilter)

    virtual ~ImportMASSIFData();

    SIMPL_FILTER_PARAMETER(QString, MassifInputFilePath)
    Q_PROPERTY(QString MassifInputFilePath READ getMassifInputFilePath WRITE setMassifInputFilePath)

    SIMPL_FILTER_PARAMETER(QString, FilePrefix)
    Q_PROPERTY(QString FilePrefix READ getFilePrefix WRITE setFilePrefix)

    SIMPL_FILTER_PARAMETER(int, StepNumber)
    Q_PROPERTY(int StepNumber READ getStepNumber WRITE setStepNumber)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName() const override;

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString() const override;

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion() const override;

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName() const override;

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName() const override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    virtual const QUuid getUuid() override;

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel() const override;

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters() override;

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute() override;

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight() override;

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
    ImportMASSIFData();

    /**
    * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
    */
    void dataCheck();

    /**
    * @brief Initializes all the private instance variables.
    */
    void initialize();

  private:
    QString           m_PaddedStep = "";

    DEFINE_DATAARRAY_WEAKPTR(float, DField)
    DEFINE_DATAARRAY_WEAKPTR(float, EField)
    DEFINE_DATAARRAY_WEAKPTR(float, SField)

    /**
     * @brief createDataArrayPaths
     * @return
     */
    QVector<DataArrayPath> createDataArrayPaths();

    /**
     * @brief createHDF5DatasetPaths
     * @return
     */
    QVector<QString> createHDF5DatasetPaths();

    /**
     * @brief getDataContainerGeometry
     * @param tDims
     * @param origin
     * @param res
     */
    void getDataContainerGeometry(QVector<size_t> &tDims, QVector<float> &origin, QVector<float> &res);

    /**
     * @brief readIDataArray
     * @param gid
     * @param name
     * @param metaDataOnly
     * @return
     */
    IDataArray::Pointer readIDataArray(hid_t gid, const QString& name, QVector<size_t> geoDims, bool metaDataOnly);

    ImportMASSIFData(const ImportMASSIFData&) = delete; // Copy Constructor Not Implemented
    void operator=(const ImportMASSIFData&) = delete;   // Operator '=' Not Implemented
};

#endif /* _ImportMASSIFData_H_ */
