TEMPLATE = lib
TARGET = OMOptimBasis
CONFIG += staticlib warn_off

include(../vars.pri)

QT +=  core gui xml
greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= printsupport widgets webkitwidgets
}

CONFIG(debug, debug|release){
    LIBS += -L$$(OMBUILDDIR)/lib/omc -lomqwtd
    DESTDIR = ../bin
    UI_DIR = debug/generatedfiles/ui
    OBJECTS_DIR = debug/generatedfiles
    MOC_DIR = debug/generatedfiles/moc
    RCC_DIR = debug/generatedfiles/rcc
    INCLUDEPATH  += debug/generatedfiles/ui

}else {
    LIBS += -L$$(OMBUILDDIR)/lib/omc -lomqwt
    DESTDIR = ../bin
    UI_DIR = release/generatedfiles/ui
    OBJECTS_DIR = release/generatedfiles
    MOC_DIR = release/generatedfiles/moc
    RCC_DIR = release/generatedfiles/rcc
    INCLUDEPATH  += release/generatedfiles/ui
}


win32 {
    include(OMOptimBasis.windowsconfig.in)
}else {
    include(OMOptimBasis.config)
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
    ../GUI/MOTableView.h \
    ../MOParameter.h \
    ../MOOptVector.h \
    ../MOItem.h \
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
    ../GUI/Dialogs/NewProjectBaseForm.h \
    ../Problems/ProblemInterfaces.h \
    ../MOVectors.h \
    ../registermetatypes.h \
    ../Units/METime.h \
    ../GUI/Dialogs/DlgSelect.h \
    ../GUI/Widgets/WidgetMOItemsList.h \
    ../MOItemModel.h \
    ../GUI/MOItemViews.h \
    ../Units/MEGeneric.h \
    ../scriptparser.h \
    ../GUI/Dialogs/ScriptTextDlg.h \
    ../GUI/Dialogs/HelpDlg.h \
    ../GUI/ConsoleWindow.h

SOURCES += \
    ../Variables.cpp \
    ../Variable.cpp \
    ../MOThreads.cpp \
    ../GUI/MOTableView.cpp \
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
    ../GUI/Dialogs/NewProjectBaseForm.cpp \
    ../Problems/ProblemInterfaces.cpp \
    ../registermetatypes.cpp \
    ../Units/METime.cpp \
    ../GUI/Dialogs/DlgSelect.cpp \
    ../GUI/Widgets/WidgetMOItemsList.cpp \
    ../MOItemModel.cpp \
    ../GUI/MOItemViews.cpp \
    ../Units/MEGeneric.cpp \
    ../scriptparser.cpp \
    ../GUI/Dialogs/ScriptTextDlg.cpp \
    ../GUI/Dialogs/HelpDlg.cpp \
    ../GUI/ConsoleWindow.cpp

FORMS += \
    ../GUI/Widgets/WidgetFilesList.ui \
    ../GUI/Widgets/WidgetMOItem.ui \
    ../GUI/Tabs/TabGeneric.ui \
    ../GUI/Tabs/TabProjectBase.ui \
    ../GUI/Widgets/WidgetProjectBaseInfos.ui \
    ../GUI/Widgets/WidgetSelectVars.ui \
    ../GUI/Dialogs/NewProjectBaseForm.ui \
    ../GUI/Widgets/WidgetMOItemsList.ui \
    ../GUI/Dialogs/ScriptTextDlg.ui \
    ../GUI/Dialogs/HelpDlg.ui \
    ../GUI/ConsoleWindow.ui

RESOURCES += \
    ../GUI/Resources/OMOptimBasis.qrc
