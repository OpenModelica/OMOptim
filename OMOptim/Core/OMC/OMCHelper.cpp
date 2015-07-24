/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Linkoping University,
 * Department of Computer and Information Science,
 * SE-58183 Linkoping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3
 * AND THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S
 * ACCEPTANCE OF THE OSMC PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linkoping University, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main Authors 2010: Syed Adeel Asghar, Sonia Tariq
 *
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

QString GUIMessages::getMessage(int type)
{
    switch (type)
    {
    case SAME_COMPONENT_NAME:
        return "A Component with the same name already exists. Please choose another Name.";
    case SAME_PORT_CONNECT:
        return "You can not connect a port to itself.";
    case NO_OPEN_MODEL:
        return "There is no open Model to simulate.";
    case NO_SIMULATION_STARTTIME:
        return "Simulation Start Time is not defined. Default value (0.0) will be used.";
    case NO_SIMULATION_STOPTIME:
        return "Simulation Stop Time is not defined.";
    case SIMULATION_STARTTIME_LESSTHAN_STOPTIME:
        return "Simulation Start Time should be less than Stop Time.";
    case ENTER_NAME:
        return "Please enter %1 Name.";
    case MODEL_ALREADY_EXISTS:
        return "%1 %2 already exits %3.";
    case ITEM_ALREADY_EXISTS:
        return "An item with the same name already exists. Please try some other name.";
    case OPEN_MODELICA_HOME_NOT_FOUND:
        return "Could not find environment variable OPENMODELICAHOME. Please make sure OpenModelica is installed properly.";
    case ERROR_OCCURRED:
        return "Following Error has occurred. \n\n%1";
    case ERROR_IN_MODELICA_TEXT:
        return "Following Errors are found in Modelica Text. \n\n%1";
    case UNDO_OR_FIX_ERRORS:
        return "\n\nFor normal users it is recommended to choose 'Undo changes'. You can also choose 'Let me fix errors' if you want to fix them by your own.";
    case NO_OPEN_MODELICA_KEYWORDS:
        return "Please make sure you are not using any OpenModelica Keywords like (model, package, record, class etc.)";
    case INCOMPATIBLE_CONNECTORS:
        return "Incompatible types for the connectors.";
    case SAVE_CHANGES:
        return "Do you want to save your changes before closing?";
    case DELETE_FAIL:
        return "Unable to delete. Server error has occurred while trying to delete.";
    case ONLY_MODEL_ALLOWED:
        return "This item is not a model.";
    case UNABLE_TO_LOAD_FILE:
        return "Error has occurred while loading the file. Unable to load the file.";
    case DELETE_AND_LOAD:
        return "Delete the existing models before loading the file.";
    case REDEFING_EXISTING_MODELS:
        return "Redefing models '%1' which already exists.";
    case INVALID_COMPONENT_ANNOTATIONS:
        return "The Annotations for the component %1 (%2) are not correct. Unable to add component.";
    case SAVED_MODEL:
        return "The %1 '%2' is not saved.";
    case COMMENT_SAVE_ERROR:
        return "Following Error has occurred while saving component comment. \n\n %1.";
    case ATTRIBUTES_SAVE_ERROR:
        return "Following Error has occurred while saving component attributes. \n\n %1.";
    case CHILD_MODEL_SAVE:
        return "The %1 '%2' is contained inside a package. It is automatically saved when you save the package.";
    case SEARCH_STRING_NOT_FOUND:
        return "The search string '%1' is not found.";
    default:
        return "";
    }
}
