#pragma once

#include <QtCore/QString>

#define EXECUTE_FUNCTION_TEMPLATE_NO_BOOL(observableObj, templateName, inputData, ...)                                                                                                                 \
  if(TemplateHelpers::CanDynamicCast<FloatArrayType>()(inputData))                                                                                                                                     \
  {                                                                                                                                                                                                    \
    templateName<float>(__VA_ARGS__);                                                                                                                                                                  \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<DoubleArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<double>(__VA_ARGS__);                                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int8ArrayType>()(inputData))                                                                                                                                 \
  {                                                                                                                                                                                                    \
    templateName<int8_t>(__VA_ARGS__);                                                                                                                                                                 \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt8ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<uint8_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int16ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<int16_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt16ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<uint16_t>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int32ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<int32_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt32ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<uint32_t>(__VA_ARGS__);                                                                                                                                                               \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<Int64ArrayType>()(inputData))                                                                                                                                \
  {                                                                                                                                                                                                    \
    templateName<int64_t>(__VA_ARGS__);                                                                                                                                                                \
  }                                                                                                                                                                                                    \
  else if(TemplateHelpers::CanDynamicCast<UInt64ArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<uint64_t>(__VA_ARGS__);                                                                                                                                                               \
  } \
  else if(TemplateHelpers::CanDynamicCast<SizeTArrayType>()(inputData))                                                                                                                               \
  {                                                                                                                                                                                                    \
    templateName<size_t>(__VA_ARGS__);                                                                                                                                                               \
  }  \
  else                                                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    observableObj->setErrorCondition(TemplateHelpers::Errors::UnsupportedDataType, "The input array was of unsupported type");                                                             \
  }

/**
 * @brief This namespace is used to define some Constants for the plugin itself.
 */
namespace DREAM3DReviewConstants
{
const QString DREAM3DReviewPluginFile("DREAM3DReviewPlugin");
const QString DREAM3DReviewPluginDisplayName("DREAM3D Review Plugin");
const QString DREAM3DReviewBaseName("DREAM3DReview");

namespace FilterGroups
{
const QString DREAM3DReviewFilters("DREAM3D Review");
}

namespace FilterSubGroups
{
const QString RotationTransformationFilters("Rotation/Transforming");
const QString StatisticsFilters("Statistics");
const QString RegistrationFilters("Registration");
const QString MemoryManagementFilters("Memory/Management");
const QString GenerationFilters("Generation");
const QString CropCutFilters("Cropping/Cutting");
const QString ClusteringFilters("Clustering");
const QString GeometryFilters("Geometry");
const QString DimensionalityReductionFilters("Dimensionality Reduction");
const QString ThresholdFilters("Threshold");
const QString Coarsening("Coarsening");
const QString InterpolationFilters("InterpolationFilters");
const QString PointCloudFilters("PointCloudFilters");

} // namespace FilterSubGroups
} // namespace DREAM3DReviewConstants
/**
 * @brief This namespace is used to define some Constants for the plugin itself.
 */
namespace MASSIFUtilitiesConstants
{
const QString MASSIFUtilitiesPluginFile("MASSIFUtilitiesPlugin");
const QString MASSIFUtilitiesPluginDisplayName("MASSIF Utilities Plugin");
const QString MASSIFUtilitiesBaseName("MASSIFUtilities");

namespace FilterGroups
{
const QString MASSIFUtilitiesFilters("MASSIFUtilities");
}

namespace ImportMassifData
{
const QString MassifDC = "MassifDataContainer";
const QString MassifAM = "MassifAttributeMatrix";
const QString DField = "D11";
const QString EField = "El11";
const QString SField = "S11";
const QString DimGrpName = "Dimension";
const QString OriginGrpName = "Origin";
const QString SpacingGrpName = "Spacing";
const QString DCGrpName = "3Ddatacontainer";
const QString GeometryGrpName = "Geometry";
const QString EVM = "EVM";
const QString SVM = "SVM";
const QString GrainID = "Grainid";
const QString Phase = "Phase";
const QString Datapoint = "Datapoint";
const QString DFieldsGrpName = "Dfields";
const QString EFieldsGrpName = "Elastic strain";
const QString SFieldsGrpName = "Sfields";
const QString EulerAngleGrpName = "Eulerangle";
const QString Phi1 = "Phi1";
const QString Phi = "Phi";
const QString Phi2 = "phi2";

const int MaxStepNumber = 999999;
} // namespace ImportMassifData
} // namespace MASSIFUtilitiesConstants

#include "itkImage.h"
#include "itkRGBPixel.h"

#include "SIMPLib/DataArrays/DataArray.hpp"

#include "DREAM3DReview/DREAM3DReviewConfig.h"

namespace AnisotropyConstants
{
const QString AnisotropyPluginFile("AnisotropyPlugin");
const QString AnisotropyPluginDisplayName("Anisotropy");
const QString AnisotropyBaseName("Anisotropy");

const QString Version("1.0");
const QString CompatibilityVersion("1.0");

const QString VendorName("Czech Academy of Sciences, Institute of Physics, Group of Bulk Nanomaterials and Interfaces");
const QString URL("http://ams.fzu.cz");
const QString Copyright("(C) 2016 Czech Academy of Sciences, v.v.i.");

namespace FilterSubGroups
{
const QString AnisotropicAlignment("Anisotropic Alignment");
}

// define pixels for dream3d variable types
typedef int8_t Int8PixelType;
typedef uint8_t UInt8PixelType;
typedef int16_t Int16PixelType;
typedef uint16_t UInt16PixelType;
typedef int32_t Int32PixelType;
typedef uint32_t UInt32PixelType;
typedef int64_t Int64PixelType;
typedef uint64_t UInt64PixelType;
typedef float FloatPixelType;
typedef double DoublePixelType;

// define default pixel type
#if Anisotropy_BitDepth == 8
typedef UInt8PixelType DefaultPixelType;
typedef DataArray<DefaultPixelType> DefaultArrayType;
#elif Anisotropy_BitDepth == 16
typedef UInt16PixelType DefaultPixelType;
typedef UInt16ArrayType DefaultArrayType;
#elif Anisotropy_BitDepth == 32
typedef FloatPixelType DefaultPixelType;
typedef FloatArrayType DefaultArrayType;
#else
typedef UInt8PixelType DefaultPixelType;
typedef UInt8ArrayType DefaultArrayType;
#endif

// multicomponent pixels
typedef itk::RGBPixel<uint8_t> RGBUInt8PixelType; // ipf color etc
// typedef itk::RGBAPixel <float> RGBAFloatPixelType; //may be able to handle quats with this?

// define dimensionality
const unsigned int SliceDimension = 2;
const unsigned int ImageDimension = 3;

// slice directions
const unsigned int XSlice = 0;
const unsigned int YSlice = 1;
const unsigned int ZSlice = 2;

// define image types
typedef itk::Image<DefaultPixelType, ImageDimension> DefaultImageType;
typedef itk::Image<Int8PixelType, ImageDimension> Int8ImageType;
typedef itk::Image<UInt8PixelType, ImageDimension> UInt8ImageType;
typedef itk::Image<Int16PixelType, ImageDimension> Int16ImageType;
typedef itk::Image<UInt16PixelType, ImageDimension> UInt16ImageType;
typedef itk::Image<Int32PixelType, ImageDimension> Int32ImageType;
typedef itk::Image<UInt32PixelType, ImageDimension> UInt32ImageType;
typedef itk::Image<Int64PixelType, ImageDimension> Int64ImageType;
typedef itk::Image<UInt64PixelType, ImageDimension> UInt64ImageType;
typedef itk::Image<FloatPixelType, ImageDimension> FloatImageType;
typedef itk::Image<DoublePixelType, ImageDimension> DoubleImageType;

typedef itk::Image<DefaultPixelType, SliceDimension> DefaultSliceType;
typedef itk::Image<Int8PixelType, SliceDimension> Int8SliceType;
typedef itk::Image<UInt8PixelType, SliceDimension> UInt8SliceType;
typedef itk::Image<Int16PixelType, SliceDimension> Int16SliceType;
typedef itk::Image<UInt16PixelType, SliceDimension> UInt16SliceType;
typedef itk::Image<Int32PixelType, SliceDimension> Int32SliceType;
typedef itk::Image<UInt32PixelType, SliceDimension> UInt32SliceType;
typedef itk::Image<Int64PixelType, SliceDimension> Int64SliceType;
typedef itk::Image<UInt64PixelType, SliceDimension> UInt64SliceType;
typedef itk::Image<FloatPixelType, SliceDimension> FloatSliceType;
typedef itk::Image<DoublePixelType, SliceDimension> DoubleSliceType;
}

/**
 * @brief This namespace is used to define some Constants for the plugin itself.
 */
namespace HEDMAnalysisConstants
{
const QString HEDMAnalysisPluginFile("HEDMAnalysisPlugin");
const QString HEDMAnalysisPluginDisplayName("HEDMAnalysis");
const QString HEDMAnalysisBaseName("HEDMAnalysis");

namespace FilterGroups
{
const QString HEDMAnalysisFilters("HEDM Analysis");
}
}

/**
 * @brief This namespace is used to define some Constants for the plugin itself.
 */
namespace DDDAnalysisToolboxConstants
{
const QString DDDAnalysisToolboxPluginFile("DDDAnalysisToolboxPlugin");
const QString DDDAnalysisToolboxPluginDisplayName("DDDAnalysisToolbox");
const QString DDDAnalysisToolboxBaseName("DDDAnalysisToolbox");

namespace FilterGroups
{
const QString DDDAnalyticsToolboxFilters("DDD Analytics");
}
} // namespace DDDAnalysisToolboxConstants

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}
