
# DREAM.3D Review #

This plugin is a staging ground for filters that might eventually make their way into either the SIMPL or DREAM3D repositories.

The DDDAnalysis plugin implements utilities for to handle dislocation dynamics files.

The MASSIFUtilities plugin implements utilities for handling inputs and outputs for the MASSIF code ("the FFT code").

External DREAM.3D plugin that contains tools to analyze HEDM data.



## Anisotropy ##
Anisotropy is a DREAM.3D plugin which contains filters that help to analyse an anisotropy in the morphology of grains and correctly align 2D slices which build up a 3D data set of anisotropic material. 

Anisotropy is analysed via Steiner compact which is estimated by counting the intersections of grain boundaries with random lines in systematically selected directions. As a result, a polygon (in 2D) or polyhedra (in 3D) is obtained which approximates the average grain shape.

For correct alignment, a complementary information is utilized, coming from either lower-resolution SEM images (provided by EDAX microscope) where the mapped area is marked by a green rectangle, or user-defined shifts in X- and Y- direction between the first and last slice of the stack. If the user-defined shifts are set to zero, no systematic trend in the shifts is expected and the slices are aligned accordingly.
