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
 	@version 0.9 
*/

#include "MOMainTab.h"
#include "MOGuiTools.h"

MOMainTab::MOMainTab(QWidget* _mainWindow,Project* _project):QTabWidget(_mainWindow)
{
	project = _project;
	mainWindow = _mainWindow;
	setMovable(true);

	setContentsMargins(2,0,2,0);
        setTabsClosable(false);

        connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabCloseRequested(int)));
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
	  iProblem = project->problems()->items.indexOf(selProblem);
	  if(iProblem>-1)
	  {
		  QMenu* problemMenu = GuiTools::createProblemPopupMenu(project,mainWindow, mapToGlobal(pEvent->pos()),selProblem,iProblem); 
		  problemMenu->exec(this->mapToGlobal(pEvent->pos()));
	  }
  }

  // Try Result
  Result* selResult = dynamic_cast<Result*>(_item);
  if(selResult)
	  {
      iResult = project->results()->items.indexOf(selResult);
      if(iResult>-1)
		  {
          QMenu* resultMenu = GuiTools::createResultPopupMenu(project,mainWindow, mapToGlobal(pEvent->pos()),selResult,iResult);
          resultMenu->exec(this->mapToGlobal(pEvent->pos()));
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
		((QTabWidget*)this)->removeTab(iTab);
}


void MOMainTab::removeTab(int index)
{
    if(index < this->count())
    {
        QWidget* tabWidget =  this->widget(index);

        // remove tab
	((QTabWidget*)this)->removeTab(index);

        // delete widget
        tabWidget->deleteLater();
}
}

void MOMainTab::addProblemTab(Project *project, Problem * problem)
{
	connect(problem,SIGNAL(renamed(QString)),this,SLOT(onProblemRenamed(QString)));
	switch(problem->type())
	{
	case Problem::ONESIMULATION:
		{
			//adding tab
                        TabOneSim* newTab = new TabOneSim(project,(OneSimulation*)problem,this);
			newTab->setBackgroundRole(QPalette::Window);
			newTab->setAutoFillBackground(true);
			newTab->setWindowTitle(problem->name());

			//Adding tab
			addTab(newTab,problem->name());
			setCurrentWidget(newTab);
			break;
		}
	case Problem::OPTIMIZATION:
		{
			//adding tab
                        TabOptimization* newTab = new TabOptimization(project,(Optimization*)problem,this);
			newTab->setBackgroundRole(QPalette::Window);
			newTab->setAutoFillBackground(true);
			newTab->setWindowTitle(problem->name());

			//Adding tab
			addTab(newTab,problem->name());
			setCurrentWidget(newTab);
			break;
		}
#ifdef USEEI
	case Problem::EIPROBLEM:
        case Problem::EITARGET:
        case Problem::EIHEN1 :
        case Problem::EIMER :
		{
			//adding tab
                        TabEIProblem* newTab = new TabEIProblem(project,(EITarget*)problem,this);
			newTab->setBackgroundRole(QPalette::Window);
			newTab->setAutoFillBackground(true);
			newTab->setWindowTitle(problem->name());


			//Adding tab
			addTab(newTab,problem->name());
			setCurrentWidget(newTab);
			break;
		}
#endif
	}

}

void MOMainTab::addResultTab(Project *project, Result * result)
{

        connect(result,SIGNAL(renamed(QString)),this,SLOT(onProblemRenamed(QString)));
switch(result->problemType())
	{
	case Problem::ONESIMULATION:
		{
			//adding table
                        TabResOneSim* newTab = new TabResOneSim(project,(OneSimResult*)result,this);
			newTab->setBackgroundRole(QPalette::Window);
			newTab->setAutoFillBackground(true);
                        newTab->setWindowTitle(result->name());

			//Adding tab
                        addTab(newTab,result->name());
			setCurrentWidget(newTab);
			break;
		}
	case Problem::OPTIMIZATION:
		{
			//adding table
                        TabResOptimization* newTab = new TabResOptimization(project,(OptimResult*)result,this);
			newTab->setBackgroundRole(QPalette::Window);
			newTab->setAutoFillBackground(true);
                        newTab->setWindowTitle(result->name());

			//Adding tab
                        addTab(newTab,result->name());
			setCurrentWidget(newTab);
			break;
		}

#ifdef USEEI
        case Problem::EITARGET:
		{
			//adding tab
                        TabEITargetResult* newTab = new TabEITargetResult(project,(EITargetResult*)result,this);
			newTab->setBackgroundRole(QPalette::Window);
			newTab->setAutoFillBackground(true);
                        newTab->setWindowTitle(result->name());

			//Adding tab
                        addTab(newTab,result->name());
			setCurrentWidget(newTab);
			break;
		}

        case Problem::EIHEN1:
                {
                        //adding tab
                        TabEIHEN1Result* newTab = new TabEIHEN1Result(project,(EIHEN1Result*)result,this);
                        newTab->setBackgroundRole(QPalette::Window);
                        newTab->setAutoFillBackground(true);
                        newTab->setWindowTitle(result->name());

                        //Adding tab
                        addTab(newTab,result->name());
                        setCurrentWidget(newTab);
                        break;
                }
#endif
	}
}

void MOMainTab::onProblemRenamed(QString newName)
{

	void* _sender = qobject_cast<Problem *>(sender());


	// find tab corresponding
	int iTab=0;
	bool found=false;
	MOTabBase* curTab;
	while((iTab<count())&&!found)
	{
		curTab = dynamic_cast<MOTabBase*>(widget(iTab));
		if(curTab)
		{
			if(curTab->getItem()==_sender)
				found = true;
			else
				iTab++;
		}
	}

	if(found)
		setTabText(iTab,newName);
}


void MOMainTab::onTabCloseRequested(int index)
{
    this->removeTab(index);
}
