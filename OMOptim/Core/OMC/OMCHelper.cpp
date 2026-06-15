/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * Main Authors 2010: Syed Adeel Asghar, Sonia Tariq
 */


#include "OMCHelper.h"

QString OMCHelper::applicationName = "OMOptim";
QString OMCHelper::applicationVersion = "Version: 0.9";
QString OMCHelper::applicationIntroText = "OpenModelica Connection Editor";
// these two variables are set once we are connected to OMC......in OMCProxy::startServer().
QString OMCHelper::OpenModelicaHome = QString();
QString OMCHelper::OpenModelicaLibrary = QString();
QString OMCHelper::omcServerName = "OMOptim";
QString OMCHelper::omFileTypes = "*.mo";
QString OMCHelper::omFileOpenText = "Modelica Files (*.mo)";
QString OMCHelper::settingsFileName = QString("OMOptim-Settings.xml");
QString OMCHelper::documentationBaseUrl = QString("/Modelica/Images/");
QString OMCHelper::readOnly = QString("Read-Only");
QString OMCHelper::writeAble = QString("Writeable");
QString OMCHelper::iconView = QString("Icon View");
QString OMCHelper::diagramView = QString("Diagram View");
QString OMCHelper::modelicaTextView = QString("Modelica Text View");
int OMCHelper::viewWidth = 2000;
int OMCHelper::viewHeight = 2000;
qreal OMCHelper::globalIconXScale = 0.15;
qreal OMCHelper::globalIconYScale = 0.15;
qreal OMCHelper::globalDiagramXScale = 1.0;
qreal OMCHelper::globalDiagramYScale = 1.0;
int OMCHelper::treeIndentation = 13;
QSize OMCHelper::iconSize = QSize(20, 20);
QSize OMCHelper::buttonIconSize = QSize(20, 20);
int OMCHelper::headingFontSize = 18;
int OMCHelper::tabWidth = 20;
qreal OMCHelper::shapesStrokeWidth = 5.0;

QString OMCHelper::ModelicaSimulationMethods = "DASSL,DASSL2,Euler,Runge-Kutta";
QString OMCHelper::ModelicaSimulationOutputFormats = "mat,csv,plt,empty";
