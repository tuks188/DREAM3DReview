Find CSL Boundaries {#findcslboundaries}
==========

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter identifies all **Faces** between neighboring **Features** that have a coincident site lattice (CSL) relationship.  The filter uses the average orientation of the **Features** on either side of the **Face** to determine the *misorientation* between the **Features**.  If the *axis-angle* that describes the *misorientation* is within a both the axis and angle user-defined tolerance, then the **Face** is flagged as being a twin.  After the **Face** is flagged as a CSL boundary, the crystal direction parallel to the **Face** normal is determined and compared with the *misorientation axis*.  The misalignment of these two crystal directions is stored as the incoherence value for the **Face** (the value is in degrees).  Note this filter will only extract CSL boundaries if the CSL **Feature** is a different phase than the parent **Feature** -- this differs from Find Twin Boundaries where the phases have to be the same. 

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| CSL | Double | Coincident Site Lattice (CSL) Boundary.  DREAM.3D is implemented up to CSL 29b.  If a "b" CSL is desired, the proper entry is e.g. 11.5 of CSL 11b. |
| Axis Tolerance | Double | The axis tolerance, in degrees, for the misorientation habit plane comparison. |
| Angle Tolerance | Double | The angle tolerance, in degrees, for the misorientation angle comparision. |

## Required DataContainers ##
+ Volume
+ SurfaceMesh


## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|------------------------------|
| Face | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Face | SurfaceMeshFaceNormals | N x 3 Col of floats |  | Generate Triangle Normals Filter (SurfaceMeshing) |
| Face | SurfaceMeshTwinBoundary | boolean value equal to 1 for twin and 0 for non-twin |  | Find Twin Boundaries (Statistics) |
| Feature (Volume) | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Feature (Volume) | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble (Volume) | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Face | SurfaceMeshTwinBoundary | boolean value equal to 1 for twin and 0 for non-twin |  |
| Face | SurfaceMeshTwinBoundaryIncoherence | Angle in degrees between crystal direction parallel to **Face** normal and misorientation axis |  |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


