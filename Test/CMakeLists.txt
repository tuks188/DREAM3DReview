#--////////////////////////////////////////////////////////////////////////////
#--
#--  PLEASE PROVIDE COPYRIGHT INFORMATION HERE.
#--
#--////////////////////////////////////////////////////////////////////////////

# project(HEDMAnalysisTest)
set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)
include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PLUGIN_NAME}Test_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Test/TestFileLocations.h.in
               ${${PLUGIN_NAME}_BINARY_DIR}/Test/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)


set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support DREAM3DLib)



# AddDREAM3DUnitTest(TESTNAME HedmReaderTest 
#                     SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/Test/HedmReaderTest.cpp
#                     FOLDER "${PLUGIN_NAME}Plugin/Test" 
#                     LINK_LIBRARIES Qt5::Core H5Support EbsdLib DREAM3DLib)
