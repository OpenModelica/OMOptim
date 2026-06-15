# OMOptimBasis : base library shared by OMOptim (GUI widgets, problems, infos, ...)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE OMOPTIMBASIS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE OMOPTIMBASIS_UIS     ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)
file(GLOB         OMOPTIMBASIS_QRCS    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Resources/*.qrc)

# MOVector.cpp and MERefValue.cpp are template implementations #included by their
# headers; they must not be compiled as standalone translation units.
list(FILTER OMOPTIMBASIS_SOURCES EXCLUDE REGEX ".*/(MOVector|MERefValue)\\.cpp$")

add_library(OMOptimBasis STATIC
  ${OMOPTIMBASIS_SOURCES}
  ${OMOPTIMBASIS_UIS}
  ${OMOPTIMBASIS_QRCS}
)

# All OMOptimBasis subdirectories are include roots because the code includes
# headers by base name (e.g. #include "Info.h").
target_include_directories(OMOptimBasis PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/FileData
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Dialogs
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Plots
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Tabs
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Tools
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/Infos
  ${CMAKE_CURRENT_SOURCE_DIR}/Problems
  ${CMAKE_CURRENT_SOURCE_DIR}/Tools
  ${CMAKE_CURRENT_SOURCE_DIR}/Units
)
# AUTOUIC generates ui_*.h into <target>_autogen/include. Export it PUBLICly so that
# OMOptim (which includes OMOptimBasis dialog/widget headers, e.g. HelpDlg.h) can find
# the generated ui_*.h, mirroring the qmake build's shared generatedfiles/ui include.
target_include_directories(OMOptimBasis PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/OMOptimBasis_autogen/include)

target_link_libraries(OMOptimBasis PUBLIC
  Qt${OM_QT_MAJOR_VERSION}::Core
  Qt${OM_QT_MAJOR_VERSION}::Gui
  Qt${OM_QT_MAJOR_VERSION}::Widgets
  Qt${OM_QT_MAJOR_VERSION}::Xml
  Qt${OM_QT_MAJOR_VERSION}::PrintSupport
  omqwt
)
if(OM_QT_MAJOR_VERSION VERSION_GREATER_EQUAL 6)
  target_link_libraries(OMOptimBasis PUBLIC Qt${OM_QT_MAJOR_VERSION}::Core5Compat)
endif()

# qwt headers (OMPlot/qwt) are needed by the plotting widgets.
target_include_directories(OMOptimBasis PUBLIC ${OMPlot_SOURCE_DIR}/qwt/src)

if(NOT MSVC)
  target_compile_options(OMOptimBasis PRIVATE -w)
endif()
