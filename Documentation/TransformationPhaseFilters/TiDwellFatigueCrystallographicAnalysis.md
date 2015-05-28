Ti Dwell Fatigue Crystallographic Analysis {#tidwellfatiguecrystallographicanalysis}
======
	
## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
Determines **Initiators**, **Hard Features**, **Soft Features** and **Hard-Soft Pairs** in microtextured voxelized structures.  **Initiators** are alpha globs defined as the user defined angle range between the c-axis ([0001]) and the user defined stress axis.  **Hard Features** are microtextured regions (MTRs) defined as the user defined angle range between {10-17} plane normal and the user defined stress axis.  **Soft Features** are MTRs defined as the user defined angle range between the c-axis ([0001]) and the user defined stress axis.  Note that the **Initiators** and **Soft Features** calculations is the same as the **Find Basal Loading Factor** filter calculation.  **Hard-Soft Pairs** are **HardFeatures** - **Soft Features** pairs with a neighboring **Initiator** that is not flagged here.  Note the **Stress Axis** does not have not be a unit vector.  Note that **Lattice Parameter A** and **Lattice Parameter C** is defaulted for alpha Ti.  The **Subsurface Feature Distance To Consider** defines a subvolume in which only **Features** whose centroid lies within the subvolume are considered for **Initiators**, **Hard Features**, **Soft Features** and **Hard-Soft Pairs** criteria.  All other **Features** are ignored.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Lattice Parameter A | Float | The hexagonal basal lattice parameter. |
| Lattice Parameter C | Float | The hexagonal height lattice parameter. |
| Subsurface Feature Distance To Consider | Int | The distance (microns) from the surface defining a subvolume. |
| Fraction Of Features To Consider | Float | The fraction of **Features** to randomly consider from the input volume element. |
| Stress Axis X | Float | The X component of the stress axis in the sample reference frame. |
| Stress Axis Y | Float | The Y component of the stress axis in the sample reference frame. |
| Stress Axis Z | Float | The Z component of the stress axis in the sample reference frame. |
| Initiator Lower Threshold | Float | The lower threshold (degrees) between the **Feature** {10-17} plane normal and the **Stress Axis**. | 
| Initiator Upper Threshold | Float | The upper threshold (degrees) between the **Feature** {10-17} plane normal and the **Stress Axis**. | 
| Propagator Lower Threshold | Float | The lower threshold (degrees) between the **Feature** c-axis ([0001]) and the **Stress Axis**. | 
| Propagator Upper Threshold | Float | The upper threshold (degrees) between the **Feature** c-axis ([0001]) and the **Stress Axis**. | 

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|------------------------------|
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each **Feature** in Bunge convention (Z-X-Z). |  | Find Average Orientations (Statistics), Match Crystallography (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**. | | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | NeighborLists | List of the contiguous neighboring **Features** for a given Feature. | | Find Feature Neighbors (Statistics), Find Feature Neighborhoods (Statistics) |
| Feature | Centroids | X, Y, Z coordinates (floats) of the **Feature** center of mass. | This filter will calculate **Feature** centroids if not previously calculated. | Find Feature Centroids (Generic) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2). | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | SelectedFeatures | Boolean value specifying if the **Feature** is randomly selected based on the **Fraction Of Features To Consider** criterion. |
| Feature | Initiators | Boolean value specifying if the **Feature** meets the **Initiators** criteria. |
| Feature | HardFeatures | Boolean value specifying if the **Feature** meets the **Hard Features** criteria. |
| Feature | SoftFeatures | Boolean value specifying if the **Feature** meets the **Soft Features** criteria. |
| Feature | HardSoftPairs | Boolean value specifying if the **Feature** meets the **Hard-Soft Pairs** criteria. |
| Feature | ParentIds | List of grouped hard or soft **Features**. |
| Feature | ParentNumNeighbors | Value (int) equal to the number of contiguous neighboring **ParentIds** for given **ParentIds**. |  |
| Feature | ParentNeighborLists | List of the contiguous neighboring **ParentIds** for given **ParentIds**. |  |
| Feature | ParentSharedSurfaceAreaLists | List of the area shared between contiguous neighboring **ParentIds** for given **ParentIds**. |  |
| Cell | ParentIds | List of grouped hard or soft **Cells**. |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


