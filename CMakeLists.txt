# CMake build for OMOptim (legacy optimization GUI).
#
# OMOptim is gated behind the OM_OMOPTIM_ENABLE option (OFF by default), set in
# the top-level OpenModelica CMakeLists.txt. It talks to OMC in-process through
# libOpenModelicaCompiler (the same way OMEdit does) instead of the old CORBA
# interface, see issue #14506.

cmake_minimum_required(VERSION 3.14)

project(OMOptim)

# Anything installed from here belongs to the 'omoptim' install component.
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME omoptim)

find_package(Qt${OM_QT_MAJOR_VERSION} COMPONENTS Core Gui Widgets Xml Svg PrintSupport REQUIRED)
if(OM_QT_MAJOR_VERSION VERSION_GREATER_EQUAL 6)
  find_package(Qt${OM_QT_MAJOR_VERSION} COMPONENTS Core5Compat REQUIRED)
endif()

#####################################################################
# ParadisEO static libraries (eo, eoutils, moeo)
#
# Built here directly instead of using ParadisEO's own CMakeLists.txt, which
# reassigns CMAKE_SOURCE_DIR and is therefore unsafe to add_subdirectory() into
# a larger project. The source lists mirror ParadisEO's own CMakeLists.
#####################################################################
set(PARADISEO ${CMAKE_CURRENT_SOURCE_DIR}/ParadisEO-2.0.1)

add_library(omoptim_eo STATIC
  ${PARADISEO}/eo/src/eoFunctorStore.cpp
  ${PARADISEO}/eo/src/eoPersistent.cpp
  ${PARADISEO}/eo/src/eoPrintable.cpp
  ${PARADISEO}/eo/src/eoCtrlCContinue.cpp
  ${PARADISEO}/eo/src/eoScalarFitnessAssembled.cpp
  ${PARADISEO}/eo/src/eoSIGContinue.cpp
)

add_library(omoptim_eoutils STATIC
  ${PARADISEO}/eo/src/utils/eoData.cpp
  ${PARADISEO}/eo/src/utils/eoFileMonitor.cpp
  ${PARADISEO}/eo/src/utils/eoGnuplot.cpp
  ${PARADISEO}/eo/src/utils/eoGnuplot1DMonitor.cpp
  ${PARADISEO}/eo/src/utils/eoGnuplot1DSnapshot.cpp
  ${PARADISEO}/eo/src/utils/eoIntBounds.cpp
  ${PARADISEO}/eo/src/utils/eoParser.cpp
  ${PARADISEO}/eo/src/utils/eoRealBounds.cpp
  ${PARADISEO}/eo/src/utils/eoRNG.cpp
  ${PARADISEO}/eo/src/utils/eoState.cpp
  ${PARADISEO}/eo/src/utils/eoOStreamMonitor.cpp
  ${PARADISEO}/eo/src/utils/eoUpdater.cpp
  ${PARADISEO}/eo/src/utils/make_help.cpp
  ${PARADISEO}/eo/src/utils/pipecom.cpp
  ${PARADISEO}/eo/src/utils/eoLogger.cpp
  ${PARADISEO}/eo/src/utils/eoParallel.cpp
  ${PARADISEO}/eo/src/utils/eoSignal.cpp
)

add_library(omoptim_moeo STATIC
  ${PARADISEO}/moeo/src/core/moeoObjectiveVectorTraits.cpp
)

set(PARADISEO_INCLUDE_DIRS
  ${PARADISEO}/eo/src
  ${PARADISEO}/eo/src/utils
  ${PARADISEO}/mo/src
  ${PARADISEO}/moeo/src
)

foreach(_pelib omoptim_eo omoptim_eoutils omoptim_moeo)
  target_include_directories(${_pelib} PUBLIC ${PARADISEO_INCLUDE_DIRS})
  # ParadisEO is old C++ code; keep its noise out of the build log.
  if(NOT MSVC)
    target_compile_options(${_pelib} PRIVATE -w)
  endif()
endforeach()
target_link_libraries(omoptim_eoutils PUBLIC omoptim_eo)
target_link_libraries(omoptim_moeo PUBLIC omoptim_eo)

#####################################################################
# OMOptim libraries / executable
#####################################################################
omc_add_subdirectory(OMOptimBasis)
omc_add_subdirectory(OMOptim)
