# @author Tsuguo Aramaki
# @date 2015 March 23

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16)
project(PJ)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Find ROOT, used for the .root output (/OutputFormat 1)
#
find_package(ROOT CONFIG COMPONENTS RIO Tree)
if(NOT ROOT_FOUND)
  message(FATAL_ERROR "ROOT not found. Run 'source <your ROOT install>/bin/thisroot.sh' "
                      "and then run cmake again in a clean build directory.")
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${PROJECT_SOURCE_DIR}/shared/include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc
                  ${PROJECT_SOURCE_DIR}/shared/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh
                  ${PROJECT_SOURCE_DIR}/shared/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 and ROOT libraries
#
add_executable(GRAMS GRAMS.cc ${sources} ${headers})
target_link_libraries(GRAMS ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build PJ. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(GRAMS_SCRIPTS
  Toy_model_info.txt
#  GRAMS.in
#  GRAMS.out
#  init_vis.mac
#  run1.mac
#  run2.mac
#  vis.mac
  )

set(GRAMS_FOLDERS
  macro
  )

foreach(_script ${GRAMS_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

foreach(_folder ${GRAMS_FOLDERS})
  file(COPY
    ${PROJECT_SOURCE_DIR}/${_folder}
    DESTINATION ${PROJECT_BINARY_DIR}
    PATTERN "*"
  )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(PJ DEPENDS GRAMS)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS GRAMS DESTINATION bin)


