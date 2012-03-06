TEMPLATE = lib
TARGET = OMOptimBasis
CONFIG += staticlib
CONFIG += qt warn_off

QT +=  core gui xml

win32 {
    include(../build/OMOptimBasis.windowsconfig.in)
}else {
    include(../build/OMOptimBasis.config)
}

CONFIG(debug, debug|release){
    DEFINES+=DEBUG
    TARGET = $$join(TARGET,,,d)
}

INCLUDEPATH += . \
              .. \
              ../GUI \
              ../GUI/Widgets \
              ../GUI/Tools \
              ../GUI/Dialogs \
              ../Units \
              ../Tools \
              ../Infos \
              ../FileData \
              ../Problems

DEPENDPATH += . \
              .. \
              ../GUI \
              ../GUI/Widgets \
              ../GUI/Tools \
              ../GUI/Dialogs \
              ../Units \
              ../Tools \
              ../Infos \
              ../FileData \
              ../Problems

HEADERS += \
    ../Variable.h \
    ../MOVector.h \
    ../MOThreads.h \
    ../MOTableView.h \
    ../MOParameter.h \
    ../MOOptVector.h \
    ../MOKeepVector.h \
    ../MOItem.h \
    ../MOClasses.h \
    ../Variables.h \
    ../GUI/Widgets/WidgetMOItem.h \
    ../GUI/Widgets/WidgetFilesList.h \
    ../VariableType.h \
    ../Units/METemperature.h \
    ../Units/MESurface.h \
    ../Units/MESpecHeatCapacity.h \
    ../Units/MERefValue.h \
    ../Units/MEQflow.h \
    ../Units/MEMassFlow.h \
    ../Units/MEHTCoeff.h \
    ../Units/MEDimValue.h \
    ../Units/Units.h \
    ../Tools/SleeperThread.h \
    ../Tools/LowTools.h \
    ../Tools/XMLTools.h \
    ../Problems/Result.h \
    ../Problems/Problems.h \
    ../Problems/ProblemInterface.h \
    ../Problems/ProblemConfig.h \
    ../Problems/Problem.h \
    ../Problems/OMCases.h \
    ../Problems/OMCase.h \
    ../Problems/Results.h \
    ../Infos/InfoSender.h \
    ../Infos/Info.h \
    ../Infos/ListInfo.h \
    ../GUI/Tools/qsciencespinbox.h \
    ../FileData/Save.h \
    ../FileData/Load.h \
    ../FileData/CSVBase.h \
    ../FileData/XML.h \
    ../ProjectBase.h \
    ../MOSettings.h \
    ../Tools/HighTools.h \
    ../GUI/Dialogs/MOParametersDlg.h \
    ../Tools/OMProcess.h \
    ../GUI/Tools/MyTreeView.h \
    ../GUI/Tools/MyTextLog.h \
    ../GUI/Tools/MyDelegates.h \
    ../GUI/Tools/MOStyleSheet.h \
    ../GUI/Tools/MOSplitter.h \
    ../GUI/Tools/GuiTools.h \
    ../GUI/Tabs/TabGeneric.h \
    ../GUI/Tabs/MOTabBase.h \
    ../GUI/Tabs/MOTab.h \
    ../GUI/Tabs/MOMainTab.h \
    ../GUI/Tabs/MO2ColTab.h \
    ../GUI/Widgets/WidgetTableVar.h \
    ../GUI/Plots/MOPlot.h \
    ../GUI/Plots/MyQwtPlotPicker.h \
    ../GUI/Tabs/TabProjectBase.h \
    ../GUI/Widgets/WidgetProjectBaseInfos.h \
    ../GUI/Dialogs/DlgSettings.h \
    ../GUI/Dialogs/DlgSelectVars.h \
    ../GUI/Dialogs/DlgSelectFiles.h \
    ../GUI/Widgets/WidgetSelectVars.h \
    ../GUI/Widgets/WidgetList.h \
    ../GUI/Dialogs/NewProjectBaseForm.h

SOURCES += \
    ../Variables.cpp \
    ../Variable.cpp \
    ../MOThreads.cpp \
    ../MOTableView.cpp \
    ../MOParameter.cpp \
    ../MOOptVector.cpp \
    ../MOItem.cpp \
    ../GUI/Widgets/WidgetMOItem.cpp \
    ../GUI/Widgets/WidgetFilesList.cpp \
    ../Units/METemperature.cpp \
    ../Units/MESurface.cpp \
    ../Units/MESpecHeatCapacity.cpp \
    ../Units/MERefValue.cpp \
    ../Units/MEQflow.cpp \
    ../Units/MEMassFlow.cpp \
    ../Units/MEHTCoeff.cpp \
    ../Units/MEDimValue.cpp \
    ../Tools/XMLTools.cpp \
    ../Tools/LowTools.cpp \
    ../Problems/Results.cpp \
    ../Problems/Result.cpp \
    ../Problems/Problems.cpp \
    ../Problems/ProblemInterface.cpp \
    ../Problems/ProblemConfig.cpp \
    ../Problems/Problem.cpp \
    ../Problems/OMCases.cpp \
    ../Problems/OMCase.cpp \
    ../Infos/InfoSender.cpp \
    ../Infos/Info.cpp \
    ../GUI/Tools/qsciencespinbox.cpp \
    ../FileData/XML.cpp \
    ../FileData/Save.cpp \
    ../FileData/Load.cpp \
    ../FileData/CSVBase.cpp \
    ../ProjectBase.cpp \
    ../MOSettings.cpp \
    ../Tools/HighTools.cpp \
    ../GUI/Dialogs/MOParametersDlg.cpp \
    ../Tools/OMProcess.cpp \
    ../GUI/Tools/MyTreeView.cpp \
    ../GUI/Tools/MyTextLog.cpp \
    ../GUI/Tools/MyDelegates.cpp \
    ../GUI/Tools/MOStyleSheet.cpp \
    ../GUI/Tools/MOSplitter.cpp \
    ../GUI/Tools/GuiTools.cpp \
    ../GUI/Tabs/TabGeneric.cpp \
    ../GUI/Tabs/MOTabBase.cpp \
    ../GUI/Tabs/MOTab.cpp \
    ../GUI/Tabs/MOMainTab.cpp \
    ../GUI/Tabs/MO2ColTab.cpp \
    ../GUI/Widgets/WidgetTableVar.cpp \
    ../GUI/Plots/MyQwtPlotPicker.cpp \
    ../GUI/Plots/MOPlot.cpp \
    ../GUI/Tabs/TabProjectBase.cpp \
    ../GUI/Widgets/WidgetProjectBaseInfos.cpp \
    ../GUI/Dialogs/DlgSettings.cpp \
    ../GUI/Dialogs/DlgSelectVars.cpp \
    ../GUI/Dialogs/DlgSelectFiles.cpp \
    ../GUI/Widgets/WidgetSelectVars.cpp \
    ../GUI/Widgets/WidgetList.cpp \
    ../GUI/Dialogs/NewProjectBaseForm.cpp

FORMS += \
    ../GUI/Widgets/WidgetFilesList.ui \
    ../GUI/Widgets/WidgetMOItem.ui \
    ../GUI/Tabs/TabGeneric.ui \
    ../GUI/Tabs/TabProjectBase.ui \
    ../GUI/Widgets/WidgetProjectBaseInfos.ui \
    ../GUI/Widgets/WidgetSelectVars.ui \
    ../GUI/Dialogs/NewProjectBaseForm.ui
