# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

TEMPLATE = app
TARGET = OMOptim

QT += core gui svg xml printsupport widgets
equals(QT_MAJOR_VERSION, 6) {
  QT += core5compat
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

# OMOptim drives the OpenModelica compiler in-process and uses the
# MMC_TRY_TOP/threadData macros, which reference threadData->parentMutex.
# That member only exists when OM_HAVE_PTHREADS is defined (as OMEdit does),
# so define it here for all platforms.
DEFINES += OM_HAVE_PTHREADS

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
