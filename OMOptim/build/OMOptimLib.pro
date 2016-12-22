TEMPLATE = lib
TARGET = OMOptim
CONFIG += staticlib warn_off

QT +=  core gui svg xml
greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= printsupport widgets webkitwidgets
}

CONFIG(debug, debug|release){
    DEFINES+=DEBUG
    TARGET = $$join(TARGET,,,d)
    message("OMOptimLibDebug")
}

win32 {
    include(OMOptim.windowsconfig.in)
}else {
    include(OMOptim.config)
}

DESTDIR = ../bin
DEPENDPATH += ../bin

INCLUDEPATH += . \
              .. \
              ../../ \
              ../Core \
              ../GUI \
              ../Vld \
              ../Core/Dymola \
              ../Core/FileData \
              ../Core/Infos \
              ../Core/Modelica \
              ../Core/OMC \
              ../Core/OpenModelica \
              ../Core/Problems \
              ../Core/Tools \
              ../Core/Util \
              ../Core/Units \
              ../GUI/Dialogs \
              ../GUI/Plots \
              ../GUI/Resources \
              ../GUI/Scene \
              ../GUI/Tabs \
              ../GUI/Tools \
              ../GUI/Views \
              ../GUI/Widgets \
              ../Core/Optim \
              ../Core/Optim/EA \
              ../Core/Optim/MILP \
         #     ../Core/Problems/BlockSubs \
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
              ../Core/Optim/EA/SPEA2Adaptative \
              ../Core/Optim/EA/SA1 \
              ../Core/Optim/EA/PSO \
               ../../OMOptimBasis \
                ../../OMOptimBasis/FileData \
                ../../OMOptimBasis/GUI \
                ../../OMOptimBasis/GUI/Tools \
                ../../OMOptimBasis/GUI/Dialogs \
                ../../OMOptimBasis/GUI/Widgets \
                ../../OMOptimBasis/Infos \
               ../../OMOptimBasis/Units \
                ../../OMOptimBasis/Tools \
                ../../OMOptimBasis/Problems

DEPENDPATH += . \
              .. \
              ../../ \
              ../Core \
              ../GUI \
              ../Vld \
              ../Core/Dymola \
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
              ../GUI/Views \
              ../GUI/Widgets \
              ../Core/Optim \
              ../Core/Optim/EA \
              ../Core/Optim/MILP \
      #        ../Core/Problems/BlockSubs \
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
              ../Core/Optim/EA/SPEA2Adaptative \
              ../Core/Optim/EA/SA1 \
              ../Core/Optim/EA/PSO \
               ../../OMOptimBasis \
                ../../OMOptimBasis/FileData \
                ../../OMOptimBasis/GUI \
                ../../OMOptimBasis/GUI/Tools \
                ../../OMOptimBasis/GUI/Dialogs \
                ../../OMOptimBasis/GUI/Widgets \
                ../../OMOptimBasis/Infos \
               ../../OMOptimBasis/Units \
                ../../OMOptimBasis/Tools \
                ../../OMOptimBasis/Problems

HEADERS += ../Core/OptObjective.h \
           ../Core/ParetoDominance.h \
           ../Core/Project.h \
           ../Core/VariablesManip.h \
           ../GUI/MainWindow.h \
           ../Core/Dymola/Dymola.h \
           ../Core/Dymola/ModPlusDymolaCtrl.h \
           ../Core/FileData/CSV.h \
           ../Core/FileData/LoadOMOptim.h \
           ../Core/FileData/SaveOMOptim.h \
           ../Core/Modelica/ModItem.h \
           ../Core/Modelica/ModItemsTree.h \
           ../Core/Modelica/ModComponent.h \
           ../Core/Modelica/OMOptimModelica.h \
           ../Core/Modelica/ModelicaConnection.h \
        #   ../Core/Modelica/ModelicaConnections.h \
           ../Core/Modelica/ModelicaModifier.h \
           ../Core/Modelica/ModExecution.h \
           ../Core/Modelica/ModModel.h \
           ../Core/Modelica/ModModelPlus.h \
           ../Core/Modelica/ModPackage.h \
           ../Core/Modelica/ModPlusCtrl.h \
           ../Core/Modelica/ModLoader.h \
           ../Core/Modelica/ModRecord.h \
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
           ../Core/Tools/reportingHook.h \
           ../Core/Tools/vqtconvert.h \
           ../GUI/Dialogs/AboutOMOptim.h \
        #   ../GUI/Dialogs/BlockSubstituteConnDlg.h \
           ../GUI/Dialogs/HelpDlg.h \
           ../GUI/Dialogs/newprojectform.h \
           ../GUI/Plots/MOOptPlot.h \
           ../GUI/Scene/arrow.h \
        #   ../GUI/Scene/BlockDrawItem.h \
        #   ../GUI/Scene/BlockSubsScene.h \
           ../GUI/Scene/diagramitem.h \
           ../GUI/Scene/diagramscene.h \
           ../GUI/Scene/diagramtextitem.h \
          # ../GUI/Widgets/WidgetBlocks.h \
           ../GUI/Widgets/WidgetCalculateMooPoints.h \
           ../GUI/Widgets/WidgetMooPlot.h \
           ../GUI/Widgets/WidgetOneSimVars.h \
           ../GUI/Widgets/WidgetOptParameters.h \
           ../GUI/Widgets/WidgetOptTable.h \
           ../GUI/Widgets/WidgetProgress.h \
#           ../GUI/Widgets/WidgetSelectComponents.h \
           ../GUI/Widgets/WidgetSelectModModel.h \
           ../GUI/Widgets/WidgetSelectOptVars.h \
           ../GUI/Widgets/WidgetSelPointScan.h \
           ../GUI/Widgets/WidgetTableRecVar.h \
           ../GUI/Widgets/WidgetToolBar.h \
           ../GUI/Widgets/widgetloadexemodel.h \
           ../Core/Optim/EA/AlgoParameter.h \
           ../Core/Optim/EA/EABase.h \
           ../Core/Optim/EA/OptimAlgo.h \
           ../Core/Optim/EA/OMEAEvalContinue.h \
           ../Core/Optim/EA/OMEAEvalFuncCounter.h \
           ../Core/Optim/EA/OMEAProgress.h \
           ../Core/Optim/EA/OptimAlgosList.h \
#           ../Core/Problems/BlockSubs/BlockSubstitution.h \
#           ../Core/Problems/BlockSubs/BlockSubstitutions.h \
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
           #../Core/Optim/EA/PSO/PSO.h \
           #../Core/Optim/EA/PSO/PSOAlgo.h \
           #../Core/Optim/EA/PSO/PSOBreed.h \
           ../Core/Optim/EA/EA \
           ../Core/OMC/OMCHelper.h \
           ../Core/OMC/StringHandler.h \
           #../Core/Optim/EA/PSO/updateGbest.h \
           #../Core/Optim/EA/PSO/updatePbest.h \
           #../Core/Optim/EA/PSO/updateVelocity.h \
        ../GUI/Widgets/WidgetMooPointsList.h \
        ../GUI/Widgets/WidgetProjectInfos.h \
    ../GUI/Widgets/WidgetProblemActions.h \
    ../version.h \
    ../Core/Optim/EA/NSGA2/NSGA2Parameters.h \
    ../Core/Optim/EA/SPEA2/SPEA2Parameters.h \
    ../Core/Optim/EA/SPEA2Adaptative/SPEA2AdaptParameters.h \
    #../Core/Optim/EA/PSO/PSOParameters.h \
    ../GUI/Tabs/TabOMC.h \
    ../GUI/Tabs/TabOneSim.h \
    ../GUI/Tabs/TabOptimization.h \
    ../GUI/Tabs/TabProject.h \
    ../GUI/Tabs/TabResOneSim.h \
    ../GUI/Tabs/TabResOptimization.h \
    ../Core/Optim/EA/SA1/SA1Parameters.h \
    ../Core/Optim/EA/SA1/SA1Algo.h \
    ../Core/Optim/EA/SA1/SA1.h \
    ../Core/Optim/EA/SA1/SA1moShiftNeighbor.h \
    ../Core/Optim/EA/SA1/SA1moRndWithReplNeighborhood.h \
    ../Core/Optim/EA/SA1/SA1explorer.h \
    ../Core/Optim/EA/SA1/SA1mo.h \
    ../Core/Optim/EA/SA1/SA1moFullEvalByCopy.h \
    ../Core/Optim/EA/SA1/SA1moSolNeighborComparator.h \
    ../Core/Optim/EA/SA1/SA1FitnessAssignement.h \
    ../Core/Optim/EA/SA1/myEOArchiveUpdater.h \
    ../Core/Optim/EA/SA1/SA1moLocalSearch.h \
    ../Core/Optim/EA/SA1/SA1moNeighborhoodExplorer.h \
    ../Core/Optim/EA/SA1/SA1Archive.h \
    ../Core/OptObjectives.h \
    ../Core/Modelica/ModPlusCtrls.h \
    ../GUI/Widgets/WidgetCtrlParameters.h \
    ../Core/Problems/optimizationinterface.h \
    ../Core/Problems/onesimulationinterface.h \
    ../GUI/Widgets/WidgetResultInfos.h \
    $${OPENMODELICAHOME}/include/omc/c/util/read_matlab4.h \
    $${OPENMODELICAHOME}/include/omc/c/util/read_matlab4.c \
    ../Core/OMC/OMCThreads.h \
    ../GUI/Tools/OMOptimGuiTools.h \
    ../GUI/Widgets/WidgetModelsList.h \
    ../Core/OMOptimSettings.h \
    ../Core/Dymola/DymolaParameters.h \
    ../Core/OpenModelica/OpenModelicaParameters.h \
    ../Core/ModExePlus.h \
    ../Core/ModelPlus.h \
    ../Core/ExeModel.h \
    ../Core/ModPlusExeCtrl.h \
    ../Core/Tools/ModPlusOMExeCtrl.h \
    ../Core/ModPlusDymolaExeCtrl.h \
    ../Core/PlugInterface.h \
    ../Core/ModPlusBlackBoxExeCtrl.h \
    ../scriptparseromoptim.h \
    ../GUI/Widgets/WidgetSelectEditVars.h \
    ../Core/Tools/StringHandler.h \
    ../Core/Tools/Helper.h \
    ../Core/OMC/ComponentInfo.h \
    ../Core/Util/Helper.h \
    ../Core/Util/StringHandler.h \
    ../Core/Util/Utilities.h
    #../Core/ModPlusTherExeCtrl.h

FORMS += ../GUI/MainWindow.ui \
         ../GUI/Dialogs/AboutOMOptim.ui \
        # ../GUI/Dialogs/BlockSubstituteConnDlg.ui \
         ../GUI/Dialogs/newprojectform.ui \
         ../GUI/Dialogs/OtherDlg.ui \
      #   ../GUI/Widgets/WidgetBlocks.ui \
         ../GUI/Widgets/WidgetCalculateMooPoints.ui \
         ../GUI/Widgets/WidgetMooPlot.ui \
         ../GUI/Widgets/WidgetOptParameters.ui \
         ../GUI/Widgets/WidgetOptTable.ui \
         ../GUI/Widgets/WidgetProgress.ui \
#         ../GUI/Widgets/WidgetSelectComponents.ui \
         ../GUI/Widgets/WidgetSelectModModel.ui \
         ../GUI/Widgets/WidgetSelPointScan.ui \
         ../GUI/Widgets/WidgetTableRecVar.ui \
         ../GUI/Widgets/WidgetToolBar.ui \
         ../GUI/Widgets/WidgetMooPointsList.ui \
         ../GUI/Widgets/WidgetProjectInfos.ui \
         ../GUI/Widgets/WidgetProblemActions.ui \
         ../GUI/Tabs/TabResOptimization.ui \
         ../GUI/Tabs/TabProject.ui \
         ../GUI/Tabs/TabOptimization.ui \
    ../GUI/Tabs/TabOMC.ui \
    ../GUI/Widgets/WidgetResultInfos.ui \
    ../GUI/Widgets/WidgetModelsList.ui \
    ../GUI/Widgets/widgetloadexemodel.ui \
    ../GUI/Widgets/WidgetSelectEditVars.ui

SOURCES += ../Core/OptObjective.cpp \
           ../Core/ParetoDominance.cpp \
           ../Core/Project.cpp \
           ../GUI/MainWindow.cpp \
           ../Core/Dymola/Dymola.cpp \
           ../Core/Dymola/ModPlusDymolaCtrl.cpp \
           ../Core/FileData/CSV.cpp \
           ../Core/FileData/LoadOMOptim.cpp \
           ../Core/FileData/SaveOMOptim.cpp \
           ../Core/Modelica/ModItem.cpp \
           ../Core/Modelica/ModItemsTree.cpp \
           ../Core/Modelica/ModComponent.cpp \
           ../Core/Modelica/ModelicaConnection.cpp \
    #       ../Core/Modelica/ModelicaConnections.cpp \
           ../Core/Modelica/ModelicaModifier.cpp \
           ../Core/Modelica/ModExecution.cpp \
           ../Core/Modelica/ModModel.cpp \
           ../Core/Modelica/ModModelPlus.cpp \
           ../Core/Modelica/ModPackage.cpp \
           ../Core/Modelica/ModPlusCtrl.cpp \
           ../Core/Modelica/ModLoader.cpp \
            ../Core/Modelica/ModRecord.cpp \
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
           ../Core/Tools/reportingHook.cpp \
           ../Core/Tools/vqtconvert.cpp \
           ../GUI/Dialogs/AboutOMOptim.cpp \
          # ../GUI/Dialogs/BlockSubstituteConnDlg.cpp \
           ../GUI/Dialogs/newprojectform.cpp \
           #../GUI/Plots/MOPlot.cpp \
           ../GUI/Scene/arrow.cpp \
         #  ../GUI/Scene/BlockDrawItem.cpp \
          # ../GUI/Scene/BlockSubsScene.cpp \
           ../GUI/Scene/diagramitem.cpp \
           ../GUI/Scene/diagramscene.cpp \
           ../GUI/Scene/diagramtextitem.cpp \
           ../GUI/Tabs/TabOMC.cpp \
           ../GUI/Tabs/TabOneSim.cpp \
           ../GUI/Tabs/TabOptimization.cpp \
           ../GUI/Tabs/TabProject.cpp \
           ../GUI/Tabs/TabResOneSim.cpp \
           ../GUI/Tabs/TabResOptimization.cpp \
       #    ../GUI/Widgets/WidgetBlocks.cpp \
           ../GUI/Widgets/WidgetCalculateMooPoints.cpp \
           ../GUI/Widgets/WidgetMooPlot.cpp \
           ../GUI/Widgets/WidgetOneSimVars.cpp \
           ../GUI/Widgets/WidgetOptParameters.cpp \
           ../GUI/Widgets/WidgetOptTable.cpp \
           ../GUI/Widgets/WidgetProgress.cpp \
#           ../GUI/Widgets/WidgetSelectComponents.cpp \
           ../GUI/Widgets/WidgetSelectModModel.cpp \
           ../GUI/Widgets/WidgetSelectOptVars.cpp \
           ../GUI/Widgets/WidgetSelPointScan.cpp \
           ../GUI/Widgets/WidgetTableRecVar.cpp \
           ../GUI/Widgets/WidgetToolBar.cpp \
           ../GUI/Widgets/widgetloadexemodel.cpp\
           ../Core/Optim/EA/AlgoParameter.cpp \
           ../Core/Optim/EA/EABase.cpp \
           ../Core/Optim/EA/OptimAlgo.cpp \
#           ../Core/Problems/BlockSubs/BlockSubstitution.cpp \
#           ../Core/Problems/BlockSubs/BlockSubstitutions.cpp \
           ../Core/Optim/EA/Monitor/MyEoGnuplot.cpp \
           ../Core/Optim/EA/Monitor/MyEoGnuplot1DMonitor.cpp \
           ../Core/Optim/EA/NSGA2/NSGA2.cpp \
           ../Core/Optim/EA/SPEA2/SPEA2.cpp \
           ../Core/Optim/EA/SPEA2Adaptative/SPEA2Adapt.cpp \
            ../Core/OMC/OMCHelper.cpp \
            ../GUI/Widgets/WidgetMooPointsList.cpp \
            ../GUI/Widgets/WidgetProjectInfos.cpp \
            ../GUI/Widgets/WidgetProblemActions.cpp \
            ../Core/Optim/EA/SA1/SA1.cpp \
             ../Core/Optim/EA/PSO/PSO.cpp \
        ../Core/OptObjectives.cpp \
    ../Core/Modelica/ModPlusCtrls.cpp \
    ../GUI/Widgets/WidgetCtrlParameters.cpp \
    ../Core/Problems/optimizationinterface.cpp \
    ../Core/Problems/onesimulationinterface.cpp \
    ../GUI/Widgets/WidgetResultInfos.cpp \
    ../Core/OMC/OMCThreads.cpp \
    ../Core/VariablesManip.cpp \
    ../GUI/Tools/OMOptimGuiTools.cpp \
    ../Core/OMOptimSettings.cpp \
    ../GUI/Widgets/WidgetModelsList.cpp \
    ../Core/Optim/EA/OptimAlgosList.cpp \
    ../Core/ModPlusExeCtrl.cpp \
    ../Core/ModExePlus.cpp \
    ../Core/ModelPlus.cpp \
    ../Core/ExeModel.cpp \
    ../Core/Tools/ModPlusOMExeCtrl.cpp \
    ../Core/ModPlusDymolaExeCtrl.cpp \
    ../Core/ModPlusBlackBoxExeCtrl.cpp \
    ../scriptparseromoptim.cpp \
    ../GUI/Widgets/WidgetSelectEditVars.cpp \
    ../Core/OMC/ComponentInfo.cpp \
    ../Core/Util/Helper.cpp \
    ../Core/Util/StringHandler.cpp \
    ../Core/Util/Utilities.cpp
    #../Core/ModPlusTherExeCtrl.cpp

