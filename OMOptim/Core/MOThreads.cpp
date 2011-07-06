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

 	@file MOThreads.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "MOThreads.h"

namespace MOThreads
{

	GetCompAndConn::GetCompAndConn(ModModel* _modModel, ModModelPlus* _modModelPlus,Project* _project)
	{
		//	senderIsDatabase = false;
		//	models = new MOVector<Model>;
		//	models->addItem(_model);

		//	project = _project;

		/*connect(this,SIGNAL(componentsUpdated()),model,SIGNAL(componentsUpdated()));
		connect(this,SIGNAL(modifiersUpdated()),model,SIGNAL(modifiersUpdated()));
		connect(this,SIGNAL(connectionsUpdated()),model,SIGNAL(connectionsUpdated()));*/
	}

	void GetCompAndConn::run(void)
	{


		//Model* curModel;
		//bool readModelOk = false;

		//for(int i=0;i<models->items.size();i++)
		//{
		//	curModel = models->items.at(i);
		//	curModel->moomc->addUsingThread(this,"reading components and connections in modelica");
		//	if (project->componentMutex.tryLock())
		//	{
		//		readModelOk = curModel->readElements(project->libraries);
		//		project->componentMutex.unlock();
		//		//emit componentsUpdated();
		//		//emit modifiersUpdated();
		//	}

		//	if(readModelOk)
		//	{
		//		curModel->readTableEIStreams();

		//		if (project->connectionMutex.tryLock())
		//		{
		//			curModel->readConnections();
		//			project->connectionMutex.unlock();
		//			//emit connectionsUpdated();
		//		}

		//		curModel->moomc->removeUsingThread(this);
		//	}
		//}
	}

//	StartOms::StartOms(MOomc* _oms)
//	{
//		moomc = _oms;
//	}

//	void StartOms::run()
//	{
//		moomc->startServer();
//	}


    LaunchProblem::LaunchProblem(Problem* problem,ProblemConfig config)
	{
        _problem = problem;
        _config = config;
        _result = NULL;
	}

	void LaunchProblem::run()
	{
        QString msg = "Launching problem : name = "+_problem->name()+" ; type = "+_problem->getClassName();
        infoSender.send(Info(msg));
        _result = _problem->launch(_config);
        emit finished(_result);
	}

    Result* LaunchProblem::result()
    {
        return _result;
    }

	void LaunchProblem::publicExec()
	{
		exec();
	}

}
