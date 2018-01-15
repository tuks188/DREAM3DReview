/*
 * Your License or Copyright can go here
 */

#ifndef _ffthdfwriterfilter_H_
#define _ffthdfwriterfilter_H_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/FileWriter.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The FFTHDFWriterFilter class. See [Filter documentation](@ref ffthdfwriterfilter) for details.
 */
class FFTHDFWriterFilter : public AbstractFilter
{
  Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */

  public:
    SIMPL_SHARED_POINTERS(FFTHDFWriterFilter)
    SIMPL_STATIC_NEW_MACRO(FFTHDFWriterFilter)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FFTHDFWriterFilter, AbstractFilter)

    virtual ~FFTHDFWriterFilter();

    SIMPL_FILTER_PARAMETER(QString, OutputFile)
    Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

    SIMPL_INSTANCE_PROPERTY(bool, WritePipeline)

      //    SIMPL_FILTER_PARAMETER(bool, WriteXdmfFile)
      //    Q_PROPERTY(bool WriteXdmfFile READ getWriteXdmfFile WRITE setWriteXdmfFile)

    SIMPL_INSTANCE_PROPERTY(bool, AppendToExisting)
    //-------------------------------------------------------------------

    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    SIMPL_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
    Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)



    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName() override;

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString() override;

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion() override;

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) override;

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName() override;

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName() override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    virtual const QUuid getUuid() override;

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel() override;

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters() override;

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

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
    FFTHDFWriterFilter();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

     /**
     * @brief openFile Opens or creates an HDF5 file to write data into
     * @param append Should a new file be created or append data to a currently existing file
     * @return
     */
    hid_t openFile(bool append = false);

    /**
     * @brief closeFile Closes the currently open file
     * @return Integer error value
     */
   herr_t closeFile();

   /**
     * @brief writePipeline Writes the existing pipeline to the HDF5 file
     * @return
     */
    int writePipeline();


    /**
     * @brief writeXdmfHeader Writes the Xdmf header
     * @param out QTextStream for output
     */
    void writeXdmfHeader(QTextStream& out);

    /**
     * @brief writeXdmfFooter Writes the Xdmf footer
     * @param out QTextStream for output
     */
    void writeXdmfFooter(QTextStream& out);




  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)

    hid_t m_FileId;

    FFTHDFWriterFilter(const FFTHDFWriterFilter&) = delete; // Copy Constructor Not Implemented
    void operator=(const FFTHDFWriterFilter&) = delete;     // Operator '=' Not Implemented
};

#endif /* _ffthdfwriterfilter_H_ */
