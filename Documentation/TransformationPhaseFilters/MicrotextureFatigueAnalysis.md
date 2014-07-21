Microtexture Fatigue Analysis (#microtexturefatigueanalysis)
======
	
## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
Determines initiators, propagators and bad actors in microtextured voxelized structures.  Initiators are defined as {10-17} <= 30 degrees from a user defined stress axis.  Propagators are defined as [0001] between 70-90 degrees from the stress axis.  The propagators calculation is the same as the basal loading factor calculation.  BadActors are Initiator and Propagator pairs.  Note the stress axis does not have not be a unit vector.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Stress Axis X | Float | X component of the stress axis in the sample reference frame |
| Stress Axis Y | Float | Y component of the stress axis in the sample reference frame |
| Stress Axis Z | Float | Z component of the stress axis in the sample reference frame |

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each Feature in Bunge convention (Z-X-Z) |  | Find Average Orientations (Statistics), Match Crystallography (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | NeighborLists | List of the contiguous neighboring Features for a given Feature | | Find Feature Neighbors (Statistics), Find Feature Neighborhoods (Statistics) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | Initiators | Boolean value specifying if the **Feature** meets the Initiators criteria |  |
| Feature | Propagators | Boolean value specifying if the **Feature** meets the Propagators criteria |  |
| Feature | BadActors | Boolean value specifying if the **Feature** meets the BadActors criteria |  |

## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

