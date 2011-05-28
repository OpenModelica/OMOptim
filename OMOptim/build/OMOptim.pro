TEMPLATE = app
TARGET = OMOptim

QT +=  core gui svg xml
VERSION = 0.9
CONFIG += qt warn_off
# if wants energy integration (pinch analysis)
# add CONFIG useei in command line

QMAKE_CXXFLAGS+=-fpermissive

# WINDOWS
win32 {
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

    DEFINES+= USEBLOCKSUB


contains(CONFIG,useei){
	
	message("useei enabled : will integrate pinch analysis functions")

	# if wants energy integration (pinch analysis)
	# add CONFIG+=useei in command line

	
	DEFINES+= USEEI
    LIBS +=	-L$$(GLPKLIB) \
                -lglpk_4_44
    INCLUDEPATH += $$(GLPKINCLUDE)
    HEADERS +=  ../Core/Optim/MILP/GLPKTools.h \
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
                ../Core/Problems/EIMER.h\
                ../Core/EI/EIMERResult.h \
                ../Core/EI/TableEIItems.h \
                ../Core/EI/EITree.h \
                ../Core/Tools/CCTools.h \
                ../GUI/Plots/MOCCCurve.h \
                ../GUI/Plots/MOCCPlot.h \
                ../GUI/Tabs/TabEITarget.h \
                ../GUI/Widgets/WidgetEIGroup.h \
                ../GUI/Widgets/WidgetEIInputVars.h \
                ../GUI/Tabs/tabEITargetResult.h \
                ../GUI/Widgets/WidgetTableEIGroups.h \
                ../GUI/Widgets/WidgetCCPlot.h \
				../GUI/Widgets/WidgetTreeStreams.h \
				../GUI/Widgets/WidgetTableConnConstr.h \
                ../GUI/Widgets/WidgetTableStreams.h \
                ../Core/EI/EIValueFiller.h \
                ../GUI/Widgets/WidgetLaunchTarget.h \
                ../Core/Problems/EIHEN1.h \
                ../Core/EI/EIHEN1Functions.h \
                ../Core/EI/EIModelContainer.h \
                ../Core/EI/EIConns.h \
                ../GUI/Widgets/WidgetEIConns.h \
                ../GUI/Widgets/WidgetEITargetResult.h \
				    ../Core/EI/EIHEN1Result.h

    SOURCES +=  ../Core/EI/EITargetResult.cpp \
                ../GUI/Tabs/TabEITarget.cpp \
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
                ../GUI/Tabs/tabEITargetResult.cpp \
                ../Core/Tools/CCTools.cpp \
                ../Core/Problems/EIMER.cpp\
				../GUI/Widgets/WidgetEIGroup.cpp \
				../GUI/Widgets/WidgetEIInputVars.cpp \
				../GUI/Widgets/WidgetTableConnConstr.cpp \
				../GUI/Widgets/WidgetTableEIGroups.cpp \
				../GUI/Widgets/WidgetTableStreams.cpp \
		        ../GUI/Widgets/WidgetTreeStreams.cpp \
                ../GUI/Widgets/WidgetCCPlot.cpp \
                ../GUI/Widgets/WidgetLaunchTarget.cpp \
                ../GUI/Widgets/WidgetEITargetResult.cpp \
                ../Core/Problems/EIHEN1.cpp \
                ../Core/EI/EIHEN1Functions.cpp \
                ../Core/EI/EIModelContainer.cpp \
                ../Core/EI/EIConns.cpp \
                ../GUI/Widgets/WidgetEIConns.cpp \
				../Core/EI/EIHEN1Result.cpp


    FORMS +=    ../GUI/Widgets/WidgetTableEIGroups.ui \
                ../GUI/Widgets/WidgetTableConnConstr.ui \
                ../GUI/Widgets/WidgetEIInputVars.ui \
                ../GUI/Widgets/WidgetTableStreams.ui  \
                ../GUI/Widgets/WidgetCCPlot.ui \
                ../GUI/Widgets/WidgetTreeStreams.ui \
                ../GUI/Widgets/WidgetLaunchTarget.ui \
                ../GUI/Widgets/WidgetEIConns.ui \
                ../GUI/Widgets/WidgetEITargetResult.ui
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
           ../Core/Modelica/ModModelParameter.h \
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
           ../Core/Problems/Problems.h \
           ../Core/Problems/Result.h \
           ../Core/Problems/Results.h \
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
           ../GUI/Dialogs/EAConfigDialog.h \
           ../GUI/Dialogs/HelpDlg.h \
           ../GUI/Dialogs/ModModelParametersDlg.h \
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
           ../GUI/Tabs/tabModelComponents.h \
           ../GUI/Tabs/tabModelVariables.h \
           ../GUI/Tabs/tabOMC.h \
           ../GUI/Tabs/tabOneSim.h \
           ../GUI/Tabs/tabOptimization.h \
           ../GUI/Tabs/tabProject.h \
           ../GUI/Tabs/tabResOneSim.h \
           ../GUI/Tabs/tabResOptimization.h \
           ../GUI/Tabs/tabResOptimization_Config.h \
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
           ../Core/Optim/EA/EAConfig.h \
           ../Core/Optim/EA/MyAlgorithm.h \
           ../Core/Optim/EA/MyAlgoUtils.h \
           ../Core/Optim/EA/MyEAEvalContinue.h \
           ../Core/Optim/EA/MyEAEvalFuncCounter.h \
           ../Core/Optim/EA/MyEAProgress.h \
           ../Core/Optim/EA/MyListAlgos.h \
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
    ../GUI/Widgets/WidgetFilesList.h 

   # ../Core/Problems/OMProblem.h





FORMS += ../GUI/MainWindow.ui \
         ../GUI/Dialogs/AboutOMOptim.ui \
         ../GUI/Dialogs/BlockSubstituteConnDlg.ui \
         ../GUI/Dialogs/HelpDlg.ui \
         ../GUI/Dialogs/newprojectform.ui \
         ../GUI/Dialogs/OtherDlg.ui \
         ../GUI/Tabs/tabGeneric.ui \
         ../GUI/Tabs/tabModelComponents.ui \
         ../GUI/Tabs/tabModelVariables.ui \
         ../GUI/Tabs/tabOMC.ui \
         ../GUI/Tabs/tabOptimization.ui \
         ../GUI/Tabs/tabProject.ui \
         ../GUI/Tabs/tabResOptimization.ui \
         ../GUI/Tabs/tabResOptimization_Config.ui \
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
    ../GUI/Widgets/WidgetFilesList.ui
 

 

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
           ../Core/Modelica/ModModelParameter.cpp \
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
           ../Core/Problems/Problems.cpp \
           ../Core/Problems/Result.cpp \
           ../Core/Problems/Results.cpp \
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
           ../GUI/Dialogs/EAConfigDialog.cpp \
           ../GUI/Dialogs/HelpDlg.cpp \
           ../GUI/Dialogs/ModModelParametersDlg.cpp \
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
           ../GUI/Tabs/tabModelComponents.cpp \
           ../GUI/Tabs/tabModelVariables.cpp \
           ../GUI/Tabs/tabOMC.cpp \
           ../GUI/Tabs/tabOneSim.cpp \
           ../GUI/Tabs/tabOptimization.cpp \
           ../GUI/Tabs/tabProject.cpp \
           ../GUI/Tabs/tabResOneSim.cpp \
           ../GUI/Tabs/tabResOptimization.cpp \
           ../GUI/Tabs/tabresoptimization_config.cpp \
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
           ../Core/Optim/EA/MyAlgorithm.cpp \
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
    ../GUI/Widgets/WidgetFilesList.cpp






  #  ../Core/Problems/OMProblem.cpp

RESOURCES += ../GUI/Resources/mineit.qrc

RC_FILE = ../GUI/Resources/rc_omoptim.rc
