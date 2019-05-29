# DREAM.3D Review #

This plugin is a staging ground for filters that might eventually make their way into either the SIMPL or DREAM3D repositories.

The DDDAnalysis plugin implements utilities for to handle dislocation dynamics files.

The MASSIFUtilities plugin implements utilities for handling inputs and outputs for the MASSIF code ("the FFT code").

External DREAM.3D plugin that contains tools to analyze HEDM data.

## Anisotropy ##

Anisotropy is a DREAM.3D plugin which contains filters that help to analyse an anisotropy in the morphology of grains and correctly align 2D slices which build up a 3D data set of anisotropic material. 

Anisotropy is analysed via Steiner compact which is estimated by counting the intersections of grain boundaries with random lines in systematically selected directions. As a result, a polygon (in 2D) or polyhedra (in 3D) is obtained which approximates the average grain shape.

For correct alignment, a complementary information is utilized, coming from either lower-resolution SEM images (provided by EDAX microscope) where the mapped area is marked by a green rectangle, or user-defined shifts in X- and Y- direction between the first and last slice of the stack. If the user-defined shifts are set to zero, no systematic trend in the shifts is expected and the slices are aligned accordingly.

## TransformationPhase ##

### Intro ###

This repository holds filters for DREAM3D (Version 6.x currently) that act on _Transformation_ phases. The filters in this plugin are **HIGHLY** experimental and can give erroneous results. The plugin is not under any active development as of Q1 2018 due to lack of funding.

If you need more help with a filter, please consider asking your question on the [DREAM3D Users mailing list](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

### Transformation Phase Plans ###

Updated: 7/28/14

+ StatsGenerator Tabs
+ Size
+ Omega-3
+ Shape
+ Variant
+ Axis-ODF
+ Burgers Relationship (need bulk load feature)
    + 1 or 2 parallel sets of planes
    + 1 or 2 parallel sets of directions

+ Transformation Phase Properties
+ Parent Phase
+ Fraction of Cross Secting Transformation

+ Need an orientation relationship and morphological relationship – need to add these in (stats generator?)
+ Have to run match crystallography before this filter on the primary phase
+ Transformation phase needs to know its parent (different than precipitate phase)
+ Make an enumeration of known orientation relationships
    + Alpha Laths
    + Pearlite colony size
    + Parallel plates
    + Grow off GBs
    + Beta - HCP
    + Colony size
    + Variant GBs

### Twin Insertion To-Do ###

+ Vary twin thickness
+ Take out parent id

## License ##

See the license file for copying and distribution rights

## Contact ##
