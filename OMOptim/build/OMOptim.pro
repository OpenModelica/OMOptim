TEMPLATE = app
TARGET = OMOptim

QT +=  core gui svg xml

# Define the preprocessor macro to get the application version in our application.
#DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += qt warn_off
# if wants energy integration (pinch analysis)
# add CONFIG useei in command line

# WINDOWS
win32 {
QMAKE_LFLAGS += -enable-auto-import

DEFINES += __x86__ \
    __NT__ \
    __OSVERSION__=4 \
    __WIN32__ \
    WIN32 \
    _WINDOWS \
    _USRDLL \
    _CRT_SECURE_NO_DEPRECATE \
    _WIN32_WINNT=0x0400 \
    _MBCS \
    QWT_DLL

#QMAKE_CXXFLAGS+=-fpermissive

# Version numbering (independent from OpenModelica)
VERSION_HEADER = ../version.h
versiontarget.target = $$VERSION_HEADER
versiontarget.commands = UpdateRevision.bat
versiontarget.depends += FORCE
PRE_TARGETDEPS += $$VERSION_HEADER
QMAKE_EXTRA_TARGETS += versiontarget

CONFIG(debug, debug|release){
    LIBS += -L$$(OMDEV)/lib/omniORB-4.1.4-mingw/lib/x86_win32 \
    -lomniORB414_rtd \
    -lomnithread34_rtd \
    -lomniDynamic414_rtd \
    -L$$(OMDEV)/lib/paradiseo-mingw/paradiseo-eo/lib \
    -leo \
    -leoutils \
    -L$$(OMDEV)/lib/paradiseo-mingw/paradiseo-moeo/lib \
    -lmoeo \
    -L$$(OMDEV)/lib/qwt-5.2.1-mingw/lib \
    -lqwtd5

DESTDIR = ../debug/
UI_DIR = ../debug/generatedfiles/ui
MOC_DIR = ../debug/generatedfiles/moc
RCC_DIR = ../debug/generatedfiles/rcc



}else {
    LIBS += -L$$(OMDEV)/lib/omniORB-4.1.4-mingw/lib/x86_win32 \
    -lomniORB414_rt \
    -lomnithread34_rt \
    -lomniDynamic414_rt \
    -L$$(OMDEV)/lib/paradiseo-mingw/paradiseo-eo/lib \
    -leo \
    -leoutils \
    -L$$(OMDEV)/lib/paradiseo-mingw/paradiseo-moeo/lib \
    -lmoeo \
    -L$$(OMDEV)/lib/qwt-5.2.1-mingw/lib \
    -lqwt5

        DESTDIR = ../bin
        UI_DIR = ../generatedfiles/ui
        MOC_DIR = ../generatedfiles/moc
        RCC_DIR = ../generatedfiles/rcc
}




INCLUDEPATH += $$(OMDEV)/lib/omniORB-4.1.4-mingw/include \
               $$(OMDEV)/lib/qwt-5.2.1-mingw/include \
               $$(OMDEV)/lib/paradiseo-mingw/paradiseo-eo/src \
               $$(OMDEV)/lib/paradiseo-mingw/paradiseo-eo/src/utils \
               $$(OMDEV)/lib/paradiseo-mingw/paradiseo-moeo/src
}else {
    include(OMOptim.config)
}
#---------End WINDOWS


CONFIG(debug, debug|release){
    DEFINES+=DEBUG
}



    DEFINES+= USEBLOCKSUB


contains(CONFIG,useei){
	
	message("useei enabled : will integrate pinch analysis functions")

	# if wants energy integration (pinch analysis)
	# add CONFIG+=useei in command line

	
    DEFINES+= USEEI

win32 {
    LIBS +=	-L$$(OMDEV)/lib/glpk/lib \
                -lglpk.dll
}else{
    LIBS += -lglpk
}

RESOURCES += \
    ../Core/Optim/MILP/OMOptimEI.qrc

    INCLUDEPATH += $$(OMDEV)/lib/glpk/include
    HEADERS +=  ../Core/Optim/MILP/GLPKTools.h \
                ../Core/Optim/MILP/GlpkCtrl.h \
                ../Core/Optim/MILP/CbcCtrl.h \
                ../Core/Optim/MILP/CBCTools.h \
                ../Core/Optim/MILP/MilpTarget.h \
                ../Core/Optim/MILP/SimpleMilpTarget.h \
                ../Core/Optim/MILP/MilpHEN1.h \
                ../Core/Problems/EITarget.h \
                ../Core/Problems/EIProblem.h \
                ../Core/EI/EIConn.h \
                ../Core/EI/EIConnConstr.h \
                ../Core/EI/EIConnConstrs.h \
                ../Core/EI/EIGroup.h \
                ../Core/EI/EIGroupFact.h \
                ../Core/EI/EIItem.h \
                ../Core/EI/EIReader.h \
                ../Core/EI/EIControler.h \
                ../Core/EI/EIStream.h \
                ../Core/EI/EITargetResult.h \
                ../Core/EI/EITools.h \
                ../Core/EI/EIModelExtractor.h \
                ../Core/EI/EILinguist.h \
                ../Core/EI/ModEIConverter.h \
                ../Core/Problems/EIMERProblem.h\
                ../Core/EI/EIMERResult.h \
                ../Core/EI/TableEIItems.h \
                ../Core/EI/EITree.h \
                ../Core/Tools/CCTools.h \
                ../GUI/Plots/MOCCCurve.h \
                ../GUI/Plots/MOCCPlot.h \
                ../GUI/Widgets/WidgetEIGroup.h \
                ../GUI/Widgets/WidgetEIInputVars.h \
                ../GUI/Widgets/WidgetTableEIGroups.h \
                ../GUI/Widgets/WidgetCCPlot.h \
                ../GUI/Widgets/WidgetTreeStreams.h \
                ../GUI/Widgets/WidgetTableConnConstr.h \
                ../GUI/Widgets/WidgetTableStreams.h \
                ../Core/EI/EIValueFiller.h \
                ../GUI/Widgets/WidgetLaunchEI.h \
                ../Core/Problems/EIHEN1Problem.h \
                ../Core/EI/EIHEN1Parameters.h \
                ../Core/EI/EIMERParameters.h \
                ../Core/EI/EITargetParameters.h \
                ../Core/EI/EIHEN1Functions.h \
                ../Core/EI/EIModelContainer.h \
                ../Core/EI/EIConns.h \
                ../GUI/Widgets/WidgetEIConns.h \
                ../GUI/Widgets/WidgetEITargetResult.h \
                ../GUI/Widgets/WidgetEIHEN1Result.h \
    ../GUI/Tabs/TabEIHEN1Result.h \
    ../GUI/Tabs/TabEIProblem.h \
    ../GUI/Tabs/TabEITargetResult.h \
                ../Core/EI/EIHEN1Result.h \
    ../Core/Optim/MILP/MilpSet.h \
    ../Core/Optim/MILP/MilpTools.h \
    ../Core/Optim/MILP/MilpParam.h \
    ../Core/Optim/MILP/MilpVariableResult.h \
    ../Core/EI/EIHEN.h\
../Core/EI/EIHENNodes.h\
    ../GUI/Scene/EIHENScene.h \
    ../GUI/Scene/EIHENGraphicNode.h  \
    ../GUI/Scene/EIHENGraphicEdge.h \
../GUI/Scene/EIHENView.h \
../GUI/Widgets/WidgetEIHENView.h \
    ../Core/Modelica/ModelicaHENImplementer.h

    SOURCES +=  ../Core/Optim/MILP/GLPKTools.cpp \
                ../Core/Optim/MILP/GlpkCtrl.cpp \
                ../Core/Optim/MILP/CbcCtrl.cpp \
                ../Core/Optim/MILP/CBCTools.cpp \
                ../Core/EI/EITargetResult.cpp \
                ../GUI/Tabs/TabEIProblem.cpp \
                ../Core/Problems/EIProblem.cpp \
                ../Core/Problems/EITarget.cpp \
                ../Core/Optim/MILP/MilpTarget.cpp \
                ../Core/Optim/MILP/SimpleMilpTarget.cpp \
                ../Core/Optim/MILP/MilpHEN1.cpp \
                ../Core/EI/EIConn.cpp \
                ../Core/EI/EIConnConstr.cpp \
                ../Core/EI/EIConnConstrs.cpp \
                ../Core/EI/EIGroup.cpp \
                ../Core/EI/EIGroupFact.cpp \
                ../Core/EI/EIItem.cpp \
                ../Core/EI/EIReader.cpp \
                ../Core/EI/EIControler.cpp \
                ../Core/EI/EIStream.cpp \
                ../Core/EI/EITools.cpp \
                ../Core/EI/EIModelExtractor.cpp \
                ../Core/EI/ModEIConverter.cpp \
                ../Core/EI/EIValueFiller.cpp \
                ../Core/EI/EIMERResult.cpp \
                ../Core/EI/TableEIItems.cpp \
                ../Core/EI/EITree.cpp \
                ../GUI/Plots/MOCCCurve.cpp \
                ../GUI/Plots/MOCCPlot.cpp \
                ../GUI/Tabs/TabEITargetResult.cpp \
                ../Core/Tools/CCTools.cpp \
                ../Core/Problems/EIMERProblem.cpp\
                ../GUI/Widgets/WidgetEIGroup.cpp \
                ../GUI/Widgets/WidgetEIInputVars.cpp \
                ../GUI/Widgets/WidgetTableConnConstr.cpp \
                ../GUI/Widgets/WidgetTableEIGroups.cpp \
                ../GUI/Widgets/WidgetTableStreams.cpp \
                ../GUI/Widgets/WidgetTreeStreams.cpp \
                ../GUI/Widgets/WidgetCCPlot.cpp \
                ../GUI/Widgets/WidgetLaunchEI.cpp \
                ../GUI/Widgets/WidgetEITargetResult.cpp \
                                ../GUI/Widgets/WidgetEIHEN1Result.cpp \
                ../Core/Problems/EIHEN1Problem.cpp \
                ../Core/EI/EIHEN1Functions.cpp \
                ../Core/EI/EIModelContainer.cpp \
                ../Core/EI/EIConns.cpp \
                ../GUI/Widgets/WidgetEIConns.cpp \
                                ../Core/EI/EIHEN1Result.cpp \
    ../GUI/Tabs/TabEIHEN1Result.cpp \
    ../Core/Optim/MILP/MilpSet.cpp \
    ../Core/Optim/MILP/MilpTools.cpp \
    ../Core/Optim/MILP/MilpParam.cpp \
    ../Core/Optim/MILP/MilpVariableResult.cpp \
    ../Core/EI/EIHEN.cpp\
./Core/EI/EIHENNodes.cpp\
    ../GUI/Scene/EIHENScene.cpp\
    ../GUI/Scene/EIHENGraphicNode.cpp  \
    ../GUI/Scene/EIHENGraphicEdge.cpp \
../GUI/Scene/EIHENView.cpp \
../GUI/Widgets/WidgetEIHENView.cpp \
    ../Core/Modelica/ModelicaHENImplementer.cpp




    FORMS +=    ../GUI/Widgets/WidgetTableEIGroups.ui \
                ../GUI/Widgets/WidgetTableConnConstr.ui \
                ../GUI/Widgets/WidgetEIInputVars.ui \
                ../GUI/Widgets/WidgetTableStreams.ui  \
                ../GUI/Widgets/WidgetCCPlot.ui \
                ../GUI/Widgets/WidgetTreeStreams.ui \
                ../GUI/Widgets/WidgetLaunchEI.ui \
                ../GUI/Widgets/WidgetEIConns.ui \
                ../GUI/Widgets/WidgetEITargetResult.ui \
                ../GUI/Widgets/WidgetEIHEN1Result.ui \
                  ../GUI/Widgets/WidgetEIHENView.ui
}

DEPENDPATH += . \
              .. \
              ../Core \
              ../GUI \
              ../Vld \
              ../Core/Dymola \
              ../Core/EI \
              ../Core/FileData \
              ../Core/Infos \
              ../Core/Modelica \
              ../Core/OMC \
              ../Core/OpenModelica \
              ../Core/Problems \
              ../Core/Tools \
              ../Core/Units \
              ../GUI/Dialogs \
              ../GUI/Plots \
              ../GUI/Resources \
              ../GUI/Scene \
              ../GUI/Tabs \
              ../GUI/Tools \
              ../GUI/Widgets \
              ../Core/Optim/EA \
              ../Core/Optim/MILP \
              ../Core/Problems/BlockSubs \
              ../Core/Optim/EA/Checkpoints \
              ../Core/Optim/EA/Chromosome \
              ../Core/Optim/EA/Crossover \
              ../Core/Optim/EA/Evaluations \
              ../Core/Optim/EA/Init \
              ../Core/Optim/EA/Monitor \
              ../Core/Optim/EA/Mutations \
              ../Core/Optim/EA/NSGA2 \
              ../Core/Optim/EA/Results \
              ../Core/Optim/EA/SPEA2 \
              ../Core/Optim/EA/SPEA2Adaptative

INCLUDEPATH += . \
              .. \
              ../Core \
              ../GUI \
              ../Vld \
              ../Core/Dymola \
              ../Core/EI \
              ../Core/FileData \
              ../Core/Infos \
              ../Core/Modelica \
              ../Core/OMC \
              ../Core/OpenModelica \
              ../Core/Problems \
              ../Core/Tools \
              ../Core/Units \
              ../GUI/Dialogs \
              ../GUI/Plots \
              ../GUI/Resources \
              ../GUI/Scene \
              ../GUI/Tabs \
              ../GUI/Tools \
              ../GUI/Widgets \
              ../Core/Optim/EA \
              ../Core/Optim/MILP \
              ../Core/Problems/BlockSubs \
              ../Core/Optim/EA/Checkpoints \
              ../Core/Optim/EA/Chromosome \
              ../Core/Optim/EA/Crossover \
              ../Core/Optim/EA/Evaluations \
              ../Core/Optim/EA/Init \
              ../Core/Optim/EA/Monitor \
              ../Core/Optim/EA/Mutations \
              ../Core/Optim/EA/NSGA2 \
              ../Core/Optim/EA/Results \
              ../Core/Optim/EA/SPEA2 \
              ../Core/Optim/EA/SPEA2Adaptative

# Input
HEADERS += ../config.h \
           ../Core/MOClasses.h \
           ../Core/MOItem.h \
           ../Core/MOOptVector.h \
           ../Core/MOSettings.h \
           ../Core/MOTableView.h \
           ../Core/MOThreads.h \
           ../Core/MOVector.h \
           ../Core/OptObjective.h \
           ../Core/ParetoDominance.h \
           ../Core/Project.h \
           ../Core/Software.h \
           ../Core/Variable.h \
           ../Core/VariablesManip.h \
           ../GUI/MainWindow.h \
           ../Vld/vld.h \
           ../Vld/vldapi.h \
           ../Core/Dymola/Dymola.h \
           ../Core/Dymola/ModPlusDymolaCtrl.h \
           ../Core/FileData/CSV.h \
           ../Core/FileData/Load.h \
           ../Core/FileData/Save.h \
           ../Core/Infos/Info.h \
           ../Core/Infos/InfoSender.h \
           ../Core/Infos/ListInfo.h \
           ../Core/Modelica/ModClass.h \
           ../Core/Modelica/ModClassTree.h \
           ../Core/Modelica/ModComponent.h \
           ../Core/Modelica/Modelica.h \
           ../Core/Modelica/ModelicaConnection.h \
           ../Core/Modelica/ModelicaConnections.h \
           ../Core/Modelica/ModelicaModifier.h \
           ../Core/Modelica/ModExecution.h \
           ../Core/Modelica/ModModel.h \
           ../Core/Modelica/MOParameter.h \
           ../Core/Modelica/ModModelPlus.h \
           ../Core/Modelica/ModPackage.h \
           ../Core/Modelica/ModPlusCtrl.h \
           ../Core/Modelica/ModReader.h \
           ../Core/OMC/commandunit.h \
           ../Core/OMC/inputcelldelegate.h \
           ../Core/OMC/MOomc.h \
           ../Core/OMC/omc_communication.h \
           ../Core/OMC/omc_communicator.h \
           ../Core/OMC/omcinteractiveenvironment.h \
           ../Core/OpenModelica/ModPlusOMCtrl.h \
           ../Core/OpenModelica/OpenModelica.h \
           ../Core/Problems/OneSimResult.h \
           ../Core/Problems/OneSimulation.h \
           ../Core/Problems/Optimization.h \
           ../Core/Problems/OptimResult.h \
           ../Core/Problems/Problem.h \
           ../Core/Problems/ProblemConfig.h \
           ../Core/Problems/OMCases.h \
           ../Core/Problems/Result.h \
           ../Core/Tools/HighTools.h \
           ../Core/Tools/LowTools.h \
           ../Core/Tools/reportingHook.h \
           ../Core/Tools/SleeperThread.h \
           ../Core/Tools/vqtconvert.h \
           ../Core/Tools/XMLTools.h \
           ../Core/Units/MEDimValue.h \
           ../Core/Units/MEQflow.h \
           ../Core/Units/MERefValue.h \
           ../Core/Units/METemperature.h \
           ../GUI/Dialogs/AboutOMOptim.h \
           ../GUI/Dialogs/BlockSubstituteConnDlg.h \
           ../GUI/Dialogs/DlgSelectVars.h \
           ../GUI/Dialogs/DlgSettings.h \
           ../GUI/Dialogs/HelpDlg.h \
           ../GUI/Dialogs/MOParametersDlg.h \
           ../GUI/Dialogs/newprojectform.h \
           ../GUI/Plots/MOOptPlot.h \
           ../GUI/Plots/MOPlot.h \
           ../GUI/Plots/MyQwtPlotPicker.h \
           ../GUI/Scene/arrow.h \
           ../GUI/Scene/BlockDrawItem.h \
           ../GUI/Scene/BlockSubsScene.h \
           ../GUI/Scene/diagramitem.h \
           ../GUI/Scene/diagramscene.h \
           ../GUI/Scene/diagramtextitem.h \
           ../GUI/Tabs/MO2ColTab.h \
           ../GUI/Tabs/MOMainTab.h \
           ../GUI/Tabs/MOTab.h \
           ../GUI/Tabs/MOTabBase.h \
           ../GUI/Tabs/TabGeneric.h \
           ../GUI/Tools/GuiTools.h \
           ../GUI/Tools/MOGuiTools.h \
           ../GUI/Tools/MOSplitter.h \
           ../GUI/Tools/MOStyleSheet.h \
           ../GUI/Tools/MyDelegates.h \
           ../GUI/Tools/MyTextLog.h \
           ../GUI/Widgets/WidgetBlocks.h \
           ../GUI/Widgets/WidgetList.h \
           ../GUI/Widgets/WidgetCalculateMooPoints.h \
           ../GUI/Widgets/WidgetMOItem.h \
           ../GUI/Widgets/WidgetMooPlot.h \
           ../GUI/Widgets/WidgetOneSimVars.h \
           ../GUI/Widgets/WidgetOptParameters.h \
           ../GUI/Widgets/WidgetOptTable.h \
           ../GUI/Widgets/WidgetProgress.h \
           ../GUI/Widgets/WidgetSelectComponents.h \
           ../GUI/Widgets/WidgetSelectModModel.h \
           ../GUI/Widgets/WidgetSelectOptVars.h \
           ../GUI/Widgets/WidgetSelectVars.h \
           ../GUI/Widgets/WidgetSelPointScan.h \
           ../GUI/Widgets/WidgetTableRecVar.h \
           ../GUI/Widgets/WidgetTableVar.h \
           ../GUI/Widgets/WidgetToolBar.h \
           ../Core/Optim/EA/AlgoConfig.h \
           ../Core/Optim/EA/AlgoParameter.h \
           ../Core/Optim/EA/EABase.h \
           ../Core/Optim/EA/OptimAlgo.h \
           ../Core/Optim/EA/OptimAlgoUtils.h \
           ../Core/Optim/EA/OMEAEvalContinue.h \
           ../Core/Optim/EA/OMEAEvalFuncCounter.h \
           ../Core/Optim/EA/OMEAProgress.h \
           ../Core/Optim/EA/OptimAlgosList.h \
           ../Core/Problems/BlockSubs/BlockSubstitution.h \
           ../Core/Problems/BlockSubs/BlockSubstitutions.h \
           ../Core/Optim/EA/Checkpoints/EAStdCheckPoint.h \
           ../Core/Optim/EA/Checkpoints/EAUpdaterDispObjGUI.h \
           ../Core/Optim/EA/Chromosome/EOAdapt.h \
           ../Core/Optim/EA/Chromosome/EOStd.h \
           ../Core/Optim/EA/Crossover/SBCrossover.h \
           ../Core/Optim/EA/Evaluations/EAStdOptimizationEval.h \
           ../Core/Optim/EA/Init/EAAdaptReinitStdDev.h \
           ../Core/Optim/EA/Init/EAStdBounds.h \
           ../Core/Optim/EA/Init/EAStdInitBounded.h \
           ../Core/Optim/EA/Monitor/MyEoGnuplot.h \
           ../Core/Optim/EA/Monitor/MyEoGnuplot1DMonitor.h \
           ../Core/Optim/EA/Mutations/EAStdMutation.h \
           ../Core/Optim/EA/NSGA2/NSGA2.h \
           ../Core/Optim/EA/NSGA2/NSGA2Algo.h \
           ../Core/Optim/EA/Results/EAStdResult.h \
           ../Core/Optim/EA/SPEA2/SPEA2.h \
           ../Core/Optim/EA/SPEA2/SPEA2Algo.h \
           ../Core/Optim/EA/SPEA2/SPEA2Crossover.h \
           ../Core/Optim/EA/SPEA2Adaptative/SPEA2Adapt.h \
           ../Core/Optim/EA/SPEA2Adaptative/SPEA2AdaptInitBounded.h \
           ../Core/Optim/EA/SPEA2Adaptative/SPEA2AdaptMutation.h \
           ../Core/Units/MERefValue.cpp \
           ../Core/Optim/EA/EA \
           ../Core/OMC/OMCHelper.h \
           ../Core/OMC/StringHandler.h \
    ../Core/FileData/XML.h \
    ../Core/VariableType.h \
    ../GUI/Widgets/WidgetMooPointsList.h \
    ../Core/MOKeepVector.h \
    ../GUI/Tools/MyTreeView.h \
    ../GUI/Widgets/WidgetProjectInfos.h \
    ../GUI/Widgets/WidgetFilesList.h \
    ../GUI/Widgets/WidgetOptimActions.h \
    ../Core/Problems/OMCase.h \
    ../version.h \
    ../Core/Optim/EA/NSGA2/NSGA2Parameters.h \
    ../Core/Optim/EA/SPEA2/SPEA2Parameters.h \
    ../Core/Optim/EA/SPEA2Adaptative/SPEA2AdaptParameters.h \
    ../Core/Problems/Problems.h \
    ../Core/Problems/Results.h \
    ../Core/Tools/OMProcess.h \
    ../Core/Units/MEMassFlow.h \
    ../GUI/Tabs/TabModelComponents.h \
    ../GUI/Tabs/TabModelVariables.h \
    ../GUI/Tabs/TabOMC.h \
    ../GUI/Tabs/TabOneSim.h \
    ../GUI/Tabs/TabOptimization.h \
    ../GUI/Tabs/TabProject.h \
    ../GUI/Tabs/TabResOneSim.h \
    ../GUI/Tabs/TabResOptimization.h \
    ../GUI/Tabs/TabResOptimization_Config.h \
    ../Core/Units/MESurface.h \
    ../Core/Variables.h \
    ../Core/OptObjectives.h






FORMS += ../GUI/MainWindow.ui \
         ../GUI/Dialogs/AboutOMOptim.ui \
         ../GUI/Dialogs/BlockSubstituteConnDlg.ui \
         ../GUI/Dialogs/HelpDlg.ui \
         ../GUI/Dialogs/newprojectform.ui \
         ../GUI/Dialogs/OtherDlg.ui \
         ../GUI/Widgets/WidgetBlocks.ui \
         ../GUI/Widgets/WidgetCalculateMooPoints.ui \
         ../GUI/Widgets/WidgetMOItem.ui \
         ../GUI/Widgets/WidgetMooPlot.ui \
         ../GUI/Widgets/WidgetOneSimVars.ui \
         ../GUI/Widgets/WidgetOptParameters.ui \
         ../GUI/Widgets/WidgetOptTable.ui \
         ../GUI/Widgets/WidgetProgress.ui \
         ../GUI/Widgets/WidgetSelectComponents.ui \
         ../GUI/Widgets/WidgetSelectModModel.ui \
         ../GUI/Widgets/WidgetSelectOptVars.ui \
         ../GUI/Widgets/WidgetSelectVars.ui \
         ../GUI/Widgets/WidgetSelPointScan.ui \
         ../GUI/Widgets/WidgetTableRecVar.ui \
         ../GUI/Widgets/WidgetToolBar.ui \
    ../GUI/Widgets/WidgetMooPointsList.ui \
    ../GUI/Widgets/WidgetProjectInfos.ui \
    ../GUI/Widgets/WidgetFilesList.ui \
    ../GUI/Widgets/WidgetOptimActions.ui \
    ../GUI/Tabs/TabResOptimization_Config.ui \
    ../GUI/Tabs/TabResOptimization.ui \
    ../GUI/Tabs/TabProject.ui \
    ../GUI/Tabs/TabOptimization.ui \
    ../GUI/Tabs/TabOMC.ui \
    ../GUI/Tabs/TabModelVariables.ui \
    ../GUI/Tabs/TabModelComponents.ui \
    ../GUI/Tabs/TabGeneric.ui
 



SOURCES += ../main.cpp \
           ../Core/MOItem.cpp \
           ../Core/MOOptVector.cpp \
           ../Core/MOSettings.cpp \
           ../Core/MOTableView.cpp \
           ../Core/MOThreads.cpp \
           ../Core/OptObjective.cpp \
           ../Core/ParetoDominance.cpp \
           ../Core/Project.cpp \
           ../Core/Variable.cpp \
           ../Core/VariablesManip.cpp \
           ../GUI/MainWindow.cpp \
           ../Core/Dymola/Dymola.cpp \
           ../Core/Dymola/ModPlusDymolaCtrl.cpp \
           ../Core/FileData/CSV.cpp \
           ../Core/FileData/Load.cpp \
           ../Core/FileData/Save.cpp \
           ../Core/Infos/Info.cpp \
           ../Core/Infos/InfoSender.cpp \
           ../Core/Modelica/ModClass.cpp \
           ../Core/Modelica/ModClassTree.cpp \
           ../Core/Modelica/ModComponent.cpp \
           ../Core/Modelica/ModelicaConnection.cpp \
           ../Core/Modelica/ModelicaConnections.cpp \
           ../Core/Modelica/ModelicaModifier.cpp \
           ../Core/Modelica/ModExecution.cpp \
           ../Core/Modelica/ModModel.cpp \
           ../Core/Modelica/MOParameter.cpp \
           ../Core/Modelica/ModModelPlus.cpp \
           ../Core/Modelica/ModPackage.cpp \
           ../Core/Modelica/ModPlusCtrl.cpp \
           ../Core/Modelica/ModReader.cpp \
           ../Core/OMC/MOomc.cpp \
           ../Core/OMC/omc_communication.cpp \
           ../Core/OMC/omc_communicator.cpp \
           ../Core/OMC/omcinteractiveenvironment.cpp \
           ../Core/OpenModelica/ModPlusOMCtrl.cpp \
           ../Core/OpenModelica/OpenModelica.cpp \
           ../Core/Problems/OneSimResult.cpp \
           ../Core/Problems/OneSimulation.cpp \
           ../Core/Problems/Optimization.cpp \
           ../Core/Problems/OptimResult.cpp \
           ../Core/Problems/Problem.cpp \
           ../Core/Problems/ProblemConfig.cpp \
           ../Core/Problems/OMCases.cpp \
           ../Core/Problems/Result.cpp \
           ../Core/Tools/HighTools.cpp \
           ../Core/Tools/LowTools.cpp \
           ../Core/Tools/reportingHook.cpp \
           ../Core/Tools/vqtconvert.cpp \
           ../Core/Tools/XMLTools.cpp \
           ../Core/Units/MEDimValue.cpp \
           ../Core/Units/MEQflow.cpp \
           ../Core/Units/MERefValue.cpp \
           ../Core/Units/METemperature.cpp \
           ../GUI/Dialogs/AboutOMOptim.cpp \
           ../GUI/Dialogs/BlockSubstituteConnDlg.cpp \
           ../GUI/Dialogs/DlgSelectVars.cpp \
           ../GUI/Dialogs/DlgSettings.cpp \
           ../GUI/Dialogs/HelpDlg.cpp \
           ../GUI/Dialogs/MOParametersDlg.cpp \
           ../GUI/Dialogs/newprojectform.cpp \
           ../GUI/Plots/MOPlot.cpp \
           ../GUI/Plots/MyQwtPlotPicker.cpp \
           ../GUI/Scene/arrow.cpp \
           ../GUI/Scene/BlockDrawItem.cpp \
           ../GUI/Scene/BlockSubsScene.cpp \
           ../GUI/Scene/diagramitem.cpp \
           ../GUI/Scene/diagramscene.cpp \
           ../GUI/Scene/diagramtextitem.cpp \
           ../GUI/Tabs/MO2ColTab.cpp \
           ../GUI/Tabs/MOMainTab.cpp \
           ../GUI/Tabs/MOTab.cpp \
           ../GUI/Tabs/MOTabBase.cpp \
            ../GUI/Tabs/TabGeneric.cpp \
           ../GUI/Tabs/TabModelComponents.cpp \
           ../GUI/Tabs/TabModelVariables.cpp \
           ../GUI/Tabs/TabOMC.cpp \
           ../GUI/Tabs/TabOneSim.cpp \
           ../GUI/Tabs/TabOptimization.cpp \
           ../GUI/Tabs/TabProject.cpp \
           ../GUI/Tabs/TabResOneSim.cpp \
           ../GUI/Tabs/TabResOptimization.cpp \
           ../GUI/Tabs/Tabresoptimization_config.cpp \
           ../GUI/Tools/GuiTools.cpp \
           ../GUI/Tools/MOGuiTools.cpp \
           ../GUI/Tools/MOSplitter.cpp \
           ../GUI/Tools/MOStyleSheet.cpp \
           ../GUI/Tools/MyDelegates.cpp \
           ../GUI/Tools/MyTextLog.cpp \
           ../GUI/Widgets/WidgetBlocks.cpp \
           ../GUI/Widgets/WidgetCalculateMooPoints.cpp \
           ../GUI/Widgets/WidgetList.cpp \
           ../GUI/Widgets/WidgetMOItem.cpp \
           ../GUI/Widgets/WidgetMooPlot.cpp \
           ../GUI/Widgets/WidgetOneSimVars.cpp \
           ../GUI/Widgets/WidgetOptParameters.cpp \
           ../GUI/Widgets/WidgetOptTable.cpp \
           ../GUI/Widgets/WidgetProgress.cpp \
           ../GUI/Widgets/WidgetSelectComponents.cpp \
           ../GUI/Widgets/WidgetSelectModModel.cpp \
           ../GUI/Widgets/WidgetSelectOptVars.cpp \
           ../GUI/Widgets/WidgetSelectVars.cpp \
           ../GUI/Widgets/WidgetSelPointScan.cpp \
           ../GUI/Widgets/WidgetTableRecVar.cpp \
           ../GUI/Widgets/WidgetTableVar.cpp \
           ../GUI/Widgets/WidgetToolBar.cpp \
           ../Core/Optim/EA/AlgoParameter.cpp \
           ../Core/Optim/EA/EABase.cpp \
           ../Core/Optim/EA/OptimAlgo.cpp \
           ../Core/Problems/BlockSubs/BlockSubstitution.cpp \
           ../Core/Problems/BlockSubs/BlockSubstitutions.cpp \
           ../Core/Optim/EA/Monitor/MyEoGnuplot.cpp \
           ../Core/Optim/EA/Monitor/MyEoGnuplot1DMonitor.cpp \
           ../Core/Optim/EA/NSGA2/NSGA2.cpp \
           ../Core/Optim/EA/SPEA2/SPEA2.cpp \
           ../Core/Optim/EA/SPEA2Adaptative/SPEA2Adapt.cpp \
            ../Core/OMC/OMCHelper.cpp \
            ../Core/OMC/StringHandler.cpp \
            ../Core/FileData/XML.cpp \
            ../GUI/Widgets/WidgetMooPointsList.cpp \
            ../GUI/Widgets/WidgetProjectInfos.cpp \
            ../GUI/Widgets/WidgetFilesList.cpp \
            ../GUI/Widgets/WidgetOptimActions.cpp \
            ../Core/Problems/OMCase.cpp \
        ../Core/Problems/Problems.cpp \
        ../Core/Problems/Results.cpp \
    ../Core/Tools/OMProcess.cpp \
    ../Core/Units/MEMassFlow.cpp \
    ../Core/Units/MESurface.cpp \
    ../Core/Variables.cpp \
    ../Core/OptObjectives.cpp


RESOURCES += \
    ../GUI/Resources/OMOptim.qrc

RC_FILE = ../GUI/Resources/rc_omoptim.rc
