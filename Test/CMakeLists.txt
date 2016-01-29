#--////////////////////////////////////////////////////////////////////////////
#--
#--  PLEASE PROVIDE COPYRIGHT INFORMATION HERE.
#--
#--////////////////////////////////////////////////////////////////////////////

# project(HEDMAnalysisTest)
set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)

#------------------------------------------------------------------------------
# Include this file from the SIMPL Project
include(${SIMPLProj_SOURCE_DIR}/Test/SIMPLTestFunctions.cmake)


include_directories(${${PLUGIN_NAME}_SOURCE_DIR})
include_directories(${${PLUGIN_NAME}_BINARY_DIR})
include_directories(${SIMPLProj_SOURCE_DIR}/Source)
include_directories(${SIMPLProj_BINARY_DIR})

set(TEST_TEMP_DIR ${${PLUGIN_NAME}Test_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestFileLocations.h.in
               ${${PLUGIN_NAME}_BINARY_DIR}/Test/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)


set(${PLUGIN_NAME}_Link_Libs Qt5::Core H5Support DREAM3DLib)



# AddSIMPLUnitTest(TESTNAME HedmReaderTest 
#                     SOURCES ${${PLUGIN_NAME}_SOURCE_DIR}/Test/HedmReaderTest.cpp
#                     FOLDER "${PLUGIN_NAME}Plugin/Test" 
#                     LINK_LIBRARIES Qt5::Core H5Support EbsdLib DREAM3DLib)
