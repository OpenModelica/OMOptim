TEMPLATE = app
TARGET = OMOptim

QT +=  core gui svg xml
greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= printsupport widgets webkitwidgets
}

CONFIG += warn_off

CONFIG(debug, debug|release){
    DEFINES+=DEBUG
    # ADD LINK TO OMOPTIM LIB
    LIBS += -L../bin -L../../OMOptimBasis/bin -lOMOptimd

    TARGET = $$join(TARGET,,,d)
}else{
    LIBS += -L../bin -L../../OMOptimBasis/bin -lOMOptim
}



win32 {
    include(OMOptim.windowsconfig.in)
}else {
    include(OMOptim.config)
}

CONFIG(debug, debug|release): PRE_TARGETDEPS += ../bin/libOMOptimd.a
else:CONFIG(release, debug|release): PRE_TARGETDEPS += ../bin/libOMOptim.a


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
              ../Core/Optim/EA/SPEA2Adaptative \
              ../Core/Optim/EA/SA1 \
              ../../OMOptimBasis/ \
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
              ../Core/Optim/EA/SPEA2Adaptative \
              ../Core/Optim/EA/SA1 \
              ../../OMOptimBasis/ \
                ../../OMOptimBasis/FileData \
                ../../OMOptimBasis/GUI \
                ../../OMOptimBasis/GUI/Tools \
                ../../OMOptimBasis/GUI/Dialogs \
                ../../OMOptimBasis/GUI/Widgets \
                ../../OMOptimBasis/Infos \
               ../../OMOptimBasis/Units \
                ../../OMOptimBasis/Tools \
               ./../OMOptimBasis/Problems

SOURCES += ../main.cpp


RESOURCES += \
    ../GUI/Resources/OMOptim.qrc

RC_FILE = ../GUI/Resources/rc_omoptim.rc

HEADERS +=
