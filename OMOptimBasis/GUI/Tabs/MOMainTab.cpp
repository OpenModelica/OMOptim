// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

  @file MOMainTab.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#include "MOMainTab.h"
#include "GuiTools.h"
#include "Problems.h"
#include "Results.h"

MOMainTab::MOMainTab(QWidget* mainWindow,ProjectBase* project):QTabWidget(mainWindow)
{
    _project = project;
    _mainWindow = mainWindow;
    setMovable(true);

    setContentsMargins(0,0,0,0);
    setTabsClosable(false);
}

MOMainTab::~MOMainTab(void)
{
}


void MOMainTab::contextMenuEvent(QContextMenuEvent* pEvent)
{
    QTabBar* bar = tabBar();

    // Looking for tab clicked
    bool found = false;
    int iTab=0;
    while((iTab<bar->count())&&!found)
    {
  if (bar->tabRect(iTab).contains(pEvent->pos()))
      found = true;
  else
      iTab++;
    }

    // if no bar was hit, return
    if (!found)
  return;

    // ... otherwise  bring tab to front
    setCurrentIndex(iTab);

    // Create popup menu
    MOTabBase* curTab = dynamic_cast<MOTabBase*>(widget(iTab));
    if(!curTab)
  return;

    MOItem* _item = curTab->getItem();

    int iProblem,iResult;

    // Try Problem
    Problem* selProblem = dynamic_cast<Problem*>(_item);
    if(selProblem)
    {
  iProblem = _project->problems()->items.indexOf(selProblem);
  if(iProblem>-1)
  {
      QMenu* problemMenu = GuiTools::createProblemPopupMenu(_project,_mainWindow, mapToGlobal(pEvent->pos()),selProblem,iProblem);
      problemMenu->exec(this->mapToGlobal(pEvent->pos()));
  }
    }
    else
    {

  // Try Result
  Result* selResult = dynamic_cast<Result*>(_item);
  if(selResult)
  {
      iResult = _project->results()->items.indexOf(selResult);
      if(iResult>-1)
      {
          QMenu* resultMenu = GuiTools::createResultPopupMenu(_project,_mainWindow, mapToGlobal(pEvent->pos()),selResult,iResult);
          resultMenu->exec(this->mapToGlobal(pEvent->pos()));
      }
  }
    }
}

void MOMainTab::removeTab(MOTabBase::TabType type,QString name)
{
    int iTab=0;
    bool found=false;
    MOTabBase* curTab;
    while((iTab<count())&&!found)
    {
  curTab = dynamic_cast<MOTabBase*>(widget(iTab));

  if((curTab->tabType()==type)&&(tabText(iTab)==name))
      found = true;
  else
      iTab++;
    }

    if(found)
  removeTab(iTab);
}


void MOMainTab::removeTab(int index)
{
    if(index < this->count())
    {
  QWidget* tabWidget =  this->widget(index);

  // remove tab
  ((QTabWidget*)this)->removeTab(index);

  // delete widget
//  tabWidget->dumpObjectInfo();
//  tabWidget->dumpObjectTree();
  delete tabWidget; // if deleteLater, should keep OMCase alive !!
    }
}

void MOMainTab::enableCaseTab(OMCase* omCase)
{
    int i=0;
    MOTabBase* tab;

    bool found = false;
    while(i<count() && !found)
    {
  tab = dynamic_cast<MOTabBase*>(widget(i));
  if(tab && (tab->getItem()==omCase))
      found =true;
  else
      i++;
    }
    if (found)
  setCurrentIndex(i);
}

void MOMainTab::addProblemTab(Problem* problem,QWidget* widget)
{
    connect(problem,SIGNAL(renamed(QString)),this,SLOT(onOMCaseRenamed(QString)));

    _problemTabs.insert(problem,widget);
    this->addTab(widget,problem->name());
}

void MOMainTab::addResultTab(Result* result,QWidget* widget)
{
    connect(result,SIGNAL(renamed(QString)),this,SLOT(onOMCaseRenamed(QString)));

    _resultTabs.insert(result,widget);
    this->addTab(widget,result->name());
}

void MOMainTab::removeTab(Problem* problem)
{
    QWidget* widget = _problemTabs.value(problem,NULL);

    if(widget)
    {
  int index = this->indexOf(widget);
  if(index>-1)
  {
      this->removeTab(index);
      _problemTabs.remove(problem);
  }
    }
}

void MOMainTab::removeTab(Result* result)
{

    QWidget* widget = _resultTabs.value(result,NULL);

    if(widget)
    {
  int index = this->indexOf(widget);
  if(index>-1)
  {
      this->removeTab(index);
      _resultTabs.remove(result);
  }
    }
}

void MOMainTab::onOMCaseRenamed(QString newName)
{


    // find tab corresponding
    Problem* problem = qobject_cast<Problem*>(sender());
    if(problem)
    {
  QWidget* widget = _problemTabs.value(problem,NULL);
  if(widget)
  {
      int index = this->indexOf(widget);
      if(index>-1)
      {
          this-> setTabText(index,newName);
      }
  }
    }
    else
    {
  Result* result = qobject_cast<Result*>(sender());
  if(result)
  {
      QWidget* widget = _resultTabs.value(result,NULL);
      if(widget)
      {
          int index = this->indexOf(widget);
          if(index>-1)
          {
              this-> setTabText(index,newName);
          }
      }
  }
    }
}




