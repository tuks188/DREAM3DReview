Point Sample Triangle Geometry {#pointsampletrianglegeometry}
=============

## Group (Subgroup) ##
DREAM3D Review (Geometry)

## Description ##
This **Filter** randomly samples point locations on **Triangles** in a **Triangle Geometry**.  The sampled point locations are then used to construct a **Vertex Geometry**.  The number of point samples may either be specified manually or by inferring from another **Geometry**:

| Geometry | Source for Number of Samples |
|----------|-----------|
| Image | Number of cells |
| Rectilinear Grid | Number of cells |
| Vertex | Number of vertices |
| Edge | Number of vertices |
| Triangle | Number of vertices |
| Quadrilateral | Number of vertices |
| Tetrahedral | Number of vertices |

In order to ensure an even sampling of the total surface are of the **Triangle Geometry**, the average number of points sampled per triangle is made proportional to the area of the triangle.  Within a given **Triangle**, a point is chosen using the following approach:

\f[
p_{i} = \left(1 - \sqrt{r_{1}}\right) a_{i} +
\left(\sqrt{r_{1}}\left(1 - r_{2}\right)\right) b_{i} +
\left(\sqrt{r_{1}} r_{2}\right) c_{i}  
\f]

where \f$ p_{i} \f$ are the coordinates of the sampled point; \f$ a_{i} \f$, \f$ b_{i} \f$, and \f$ c_{i} \f$ are the coordinates of the vertices beloning to the **Triangle**; and \f$ r_{1} \f$ and \f$ r_{2} \f$ are random real numbers on the interval \f$ [0, 1) \f$.  This approach has the benefit of uniform sampling within the **Triangle** area, and functions correctly regardless of the dimensionality of the space embedding (i.e., whether the **Triangle** is in the plane or embedded in 3D).

The user may opt to use a mask to prevent certain **Triangles** from being sampled; where the mask is _false_, the **Triangle** will not be sampled.  Additionally, the user may choose any number of **Face Attribute Arrays** to transfer to the created **Vertex Geometry**. The vertices in the new **Vertex Geometry** will gain the values of the **Faces** from which they were sampled.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Source for Number of Samples | Enumeration | Whether to input the number of samples manually or use another **Geometry** to determine the number of samples |
| Number of Sample Points | int32_t | Number of sample points to use, if _Manual_ is selected for _Source for Number of Samples_ |
| Use Mask | bool | Whether to use a boolean mask array to ignore certain **Trianlges** flagged as _false_ from the sampling algorithm |

## Required Geometry ###

Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | TriangleDataContainer | N/A | N/A | **Data Container** holding the **Triangle Geometry** to sample |
| **Data Container** | None | N/A | N/A | **Data Container** holding the **Geometry** used to determine the number of samples, if _Other Geometry_ is selected for _Source for Number of Samples_ |
| **Face Attribute Array**  | FaceAreas | double | (1) | Specifies the area of each **Face** |
| **Face Attribute Array** | Mask | bool | (1) | Specifies if the **Face** can be sampled, if _Use Mask_ is checked |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | VertexDataContainer | N/A | N/A | **Data Container** holding the **Vertex Geometry** that represents the sampling points |
| **Vertex Attribute Matrix** | VertexData | Vertex | N/A | **Vertex Attribute Matrix** for the created **Vertex Data Container** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users