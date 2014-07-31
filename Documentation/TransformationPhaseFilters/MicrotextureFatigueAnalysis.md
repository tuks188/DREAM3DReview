Microtexture Fatigue Analysis {#microtexturefatigueanalysis}
======
	
## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
Determines **Initiators**, **Propagators** and **Bad Actors** in microtextured voxelized structures.  **Initiators** are defined as the user defined angle range between the {10-17} plane normal and the user defined stress axis.  **Propagators** are defined as the user defined angle range between the c-axis ([0001]) and the user defined stress axis.  Note that the **Propagators** calculation is the same as the **Find Basal Loading Factor** filter calculation.  **Bad Actors** are **Initiators** - **Propagators** pairs.  Note the **Stress Axis** does not have not be a unit vector.  Note that **Lattice Parameter A** and **Lattice Parameter C** is defaulted for alpha Ti.  The **Subsurface Feature Distance To Consider** defines a subvolume in which only **Features** whose centroid lies within the subvolume are considered for **Initiators**, **Propagators** and **Bad Actors** criteria.  All other **Features** are ignored.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Lattice Parameter A | Float | The hexagonal basal lattice parameter. |
| Lattice Parameter C | Float | The hexagonal height lattice parameter. |
| Subsurface Feature Distance To Consider | Int | The distance (microns) from the surface defining a subvolume. |
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
|------|--------------|-------------|---------|-----|
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each **Feature** in Bunge convention (Z-X-Z). |  | Find Average Orientations (Statistics), Match Crystallography (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**. | | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | NeighborLists | List of the contiguous neighboring **Features** for a given Feature. | | Find Feature Neighbors (Statistics), Find Feature Neighborhoods (Statistics) |
| Feature | Centroids | X, Y, Z coordinates (floats) of the **Feature** center of mass. | This filter will calculate **Feature** centroids if not previously calculated. | Find Feature Centroids (Generic) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2). | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | Initiators | Boolean value specifying if the **Feature** meets the **Initiators** criteria. |
| Feature | Propagators | Boolean value specifying if the **Feature** meets the **Propagators** criteria. |
| Feature | Bad Actors | Boolean value specifying if the **Feature** meets the **Bad Actors** criteria. |

## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.1.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

