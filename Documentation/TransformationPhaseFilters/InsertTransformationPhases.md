Insert Transformation Phases {#inserttransformationphases}
======
	
## Group (Subgroup) ##
Synthetic Building Filters (Packing)

## Description ##
Takes an already formed voxelized structure and inserts transformation phase morphologies with the correct orientation relationship (and habit plane if coherent).

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Parent Phase | Int | The phase which the transformation phase is inserted. |
| Transformation Phase Thickness | Float (Fraction between zero and 1) | The fraction of the current grain's sphere equivalent diameter. |
| Average Number Of Transformation Phases Per Feature | Integer | This will vary between zero and 2 times the input value |
| Coherent Fraction | Float (Fraction between zero and 1) | THe coherent vs. incoherent transformation phase fraction. |
| "Peninsula" Transformation Phase Fraction | Float (Fraction between zero and 1) | Specific transformation phases that do not fully section the feature.  Variable peninsula lengths will result. |
| Variant Number | Integer (Between 1 and the maximum number of variants) | |
| Transformation Phase Crystal Structure | Default numeric values set in EBSDLib | Hexagonal = 0, Cubic = 1 |

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|------------------------------|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of each Cell in Bunge convention (Z-X-Z) | | Import Orientation File(s) to H5Ebsd (IO) |
| Feature | AvgQuats | Five (4) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Feature | Centroids | X, Y, Z coordinates (floats) of **Feature** center of mass | Filter will calculate **Feature** centroids if not previously calculated | Find Feature Centroids (Generic) |
| Feature | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Feature**. |  | Find Sizes (Statistics) |
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each Feature in Bunge convention (Z-X-Z) |  | Find Average Orientations (Statistics), Match Crystallography (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  | Intialize Synthetic Volume (SyntheticBuilding), Generate Ensemble Statistics (Statistics) |
| Ensemble | ShapeTypes |  |  | Establish Shape Types (SyntheticBuilding) |
| Ensemble | Statistics |  |  | Generate Ensemble Statistics (Statistics), StatsGenerator Application |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | Active | Boolean value specifying if the **Feature** is still in the sample (1 if the **Feature** is in the sample and 0 if it is not). At the end of the filter, all **Features** will be "Active" as the "Inactive" **Features** will have been removed. |
| Feature | FeatureParentIds | List of grouped microtexture region **Features**. |
| Feature | NumFeaturesPerParent | Int of the number of **Features** contained in each **Feature**. |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


