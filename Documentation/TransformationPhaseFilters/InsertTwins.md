Insert Twins {#inserttwins}
======
	
## Group (Subgroup) ##
Transformation Phase Filters (Packing)

## Description ##
Takes an already formed voxelized structure and inserts twin morphologies with the correct orientation relationship (and habit plane if coherent).

## Parameters ##

| Name | Type |
|------|------|
| Twin Thickness | Float (Fraction between zero and 1) |
| Average Number Of Twins Per Grain | Integer |
| Coherent Fraction | Float (Fraction between zero and 1) |
| "Peninsula" Twin Fraction | Float (Fraction between zero and 1) |
| Uniquely Renumber Contiguous Regions | Boolean (Yes or no) |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. | Find Field Average Orientations (Statistics) |
| Field | Centroids | X, Y, Z coordinates (floats) of **Field** center of mass | Filter will calculate **Field** centroids if not previously calculated | Find Field Centroids (Generic) |
| Field | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Field**. | Find Sizes (Statistics) |
| Field | FieldEulerAngles | Three (3) angles (floats) defining the orientation of each **Field** in Bunge convention (Z-X-Z) | Find Average Orientations (Statistics)
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| | Find Field Phases (Generic), Read Field Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  | Intialize Synthetic Volume (SyntheticBuilding), Generate Ensemble Statistics (Statistics) |
| Ensemble | ShapeTypes |  |  | Initialize Synthetic Volume (SyntheticBuilding) |
| Ensemble | Statistics |  |  | Generate Ensemble Statistics (Statistics), StatsGenerator Application |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |
| Field | ParentIds | List of grouped microtexture region **Fields**. |  |

## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 4.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

