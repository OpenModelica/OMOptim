# OMOptim : the optimization library (OMOptimLib) and the OMOptim executable.

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#####################################################################
# Generate omc_config.h (was produced by autoconf from omc_config.h.in)
#####################################################################
set(OPENMODELICAHOME "${CMAKE_INSTALL_PREFIX}")
set(SOURCE_REVISION "")
set(LSB_RELEASE "")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/omc_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/omc_config.h @ONLY)

#####################################################################
# OMOptimLib : all of Core + GUI
#####################################################################
file(GLOB_RECURSE OMOPTIMLIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/GUI/*.cpp)
file(GLOB_RECURSE OMOPTIMLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/GUI/*.h)
file(GLOB_RECURSE OMOPTIMLIB_UIS     ${CMAKE_CURRENT_SOURCE_DIR}/GUI/*.ui)
list(APPEND OMOPTIMLIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/scriptparseromoptim.cpp)
list(APPEND OMOPTIMLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/scriptparseromoptim.h)

# Compile exactly the same set of sources as the qmake build (OMOptimLib.pro).
# These files exist on disk but are NOT part of the qmake build: the obsolete CORBA
# layer (replaced by in-process OMC, #14506), the disabled "BlockSubs" feature
# (USEBLOCKSUB undefined), and a few stale sources that reference missing headers
# (e.g. Modelica.h) or use Qt4-only includes. The headers are listed as target
# sources as well so that AUTOMOC processes the header-only Q_OBJECT classes
# (e.g. OMEAProgress, MOOptPlot); the disabled headers are excluded for the same reason.
set(OMOPTIM_EXCLUDE_REGEX
    ".*/(omc_communication|omc_communicator|omcinteractiveenvironment|BlockDrawItem|BlockSubsScene|BlockSubstituteConnDlg|BlockSubstitution|BlockSubstitutions|ModelicaConnections|ModelicaHENImplementer|WidgetBlocks|WidgetSelectComponents|commandcompletion|sa1moshiftneighbor)\\.(cpp|h)$")
list(FILTER OMOPTIMLIB_SOURCES EXCLUDE REGEX ${OMOPTIM_EXCLUDE_REGEX})
list(FILTER OMOPTIMLIB_HEADERS EXCLUDE REGEX ${OMOPTIM_EXCLUDE_REGEX})
list(FILTER OMOPTIMLIB_UIS EXCLUDE REGEX
     ".*/(BlockSubstituteConnDlg|WidgetBlocks|WidgetSelectComponents)\\.ui$")

add_library(OMOptimLib STATIC
  ${OMOPTIMLIB_SOURCES}
  ${OMOPTIMLIB_HEADERS}
  ${OMOPTIMLIB_UIS}
)

set(OMOPTIM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Core
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Dymola
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/FileData
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Modelica
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/OMC
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/OpenModelica
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Problems
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Tools
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Util
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Checkpoints
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Chromosome
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Crossover
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Evaluations
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Init
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Monitor
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Mutations
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/NSGA2
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/PSO
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/Results
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/SPEA2
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/SPEA2Adaptative
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Optim/EA/SA1
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Problems/BlockSubs
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Dialogs
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Plots
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Scene
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Tabs
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Tools
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(OMOptimLib PUBLIC ${OMOPTIM_INCLUDE_DIRS})
# Export the AUTOUIC-generated ui_*.h directory PUBLICly so the OMOptim executable
# (main.cpp -> MainWindow.h -> ui_*.h) can find them.
target_include_directories(OMOptimLib PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/OMOptimLib_autogen/include)
# C runtime headers (meta/meta_modelica.h, util/omc_error.h) for the in-process OMC.
target_include_directories(OMOptimLib PUBLIC ${OMCompiler_SOURCE_DIR}/SimulationRuntime/c)

target_link_libraries(OMOptimLib PUBLIC
  OMOptimBasis
  omoptim_moeo
  omoptim_eoutils
  omoptim_eo
  omqwt
  OpenModelicaCompiler
  OpenModelicaRuntimeC
  Qt${OM_QT_MAJOR_VERSION}::Core
  Qt${OM_QT_MAJOR_VERSION}::Gui
  Qt${OM_QT_MAJOR_VERSION}::Widgets
  Qt${OM_QT_MAJOR_VERSION}::Xml
  Qt${OM_QT_MAJOR_VERSION}::Svg
  Qt${OM_QT_MAJOR_VERSION}::PrintSupport
)
if(OM_QT_MAJOR_VERSION VERSION_GREATER_EQUAL 6)
  target_link_libraries(OMOptimLib PUBLIC Qt${OM_QT_MAJOR_VERSION}::Core5Compat)
endif()

if(NOT MSVC)
  target_compile_options(OMOptimLib PRIVATE -w)
endif()

#####################################################################
# OMOptim executable
#####################################################################
file(GLOB OMOPTIM_QRCS ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Resources/*.qrc)

add_executable(OMOptim
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${OMOPTIM_QRCS}
)

target_link_libraries(OMOptim PUBLIC OMOptimLib)

if(NOT MSVC)
  target_compile_options(OMOptim PRIVATE -w)
endif()

omc_install_gui_client(OMOptim)
