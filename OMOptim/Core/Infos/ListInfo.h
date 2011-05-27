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

 	@file ListInfo.h
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#if !defined(_LISTINFO_H)
#define _LISTINFO_H

#define NBINFOS 53


namespace ListInfo{

	
	enum InfoType{
		NORMAL2,
		WARNING2,
		ERROR2,
		OMCNORMAL2,
		OMCWARNING2,
		OMCERROR2,
		OPTIMNORMAL,
		OPTIMERROR,
		OPTIMWARNING,
		DEBUG
	};

	enum InfoNum{
            //Files
		FILENOTEXISTS,
		FILEUNREADABLE,
		FILECORRUPTED,
                REMOVEDFOLDERSUCESS,
                REMOVEDFOLDERFAILED,
		LOADINGPROJECT,
		PROJECTLOADSUCCESSFULL,
		PROJECTLOADFAILED,
		TEMPFOLDERCREATED,
		COPYINGFILETOTEMP,
		ADDEDPROBLEM,
		ONESIMULATIONSUCCESS,
		ONESIMULATIONFAILED,
		PROJECTFILECORRUPTED,
		PROJECTFILENOTEXISTS,
		MODELFILENOTEXISTS,
		TEMPFOLDERUNREMOVABLE,
		FOLDERUNREMOVABLE,
		LOADINGMODEL,
		RESULTFILENOTEXISTS,
		RESULTFILECORRUPTED,
		PROBLEMFILENOTEXISTS,
		PROBLEMFILECORRUPTED,
		PROBLEMMODELNOTFOUND,
		//MODELS
		MODELCOMPILATIONSUCCESS,
		MODELCOMPILATIONFAIL,
		MODELLOADSUCCESS,
		MODELLOADFAIL,

		//Problem::OPTIMIZATION
		OPTNOOBJECTIVE,
		OPTNOVARIABLE,
		OPTIMIZATIONFAILED,
		OPTIMIZATIONSUCCESS,
		//Problem::OPTIMIZATION SUBMODELS
		SUBMODELNOTADDED,
		SUBMODELADDED,

		//Problem::EIPROBLEM,
		PROBLEMEIFAILED,
		PROBLEMEISUCCESS,

		//RECOMPUTING
		RECOMPUTINGPOINTS,
		RECOMPUTINGPOINTFAILED,
		RECOMPUTINGPOINTSUCCESS,
		//VARIABLES
		BEGINREADINGVARIABLES,
		READVARIABLESSUCCESS,
		//COMPONENTS
		BEGINREADINGCOMPONENTS,
		READCOMPONENTSSUCCESS,
		//CONNECTIONS
		BEGINREADINGCONNECTIONS,
		READCONNECTIONSSUCCESS,
		//MOomc
		OMSOBTMODIFIERSFAILED,
		OMSGETCOMPERROR,
		//MOITEM
		PROBLEMREADINGLINE,
		MODMODELFILECORRUPTED,
		MODELNOTEXISTSINMOFILE,
		//MILP
		MILPERRORMODEL,
		MILPERRORDATA,
		//LIBRARIES
		LIBRARYFILENOTEXISTS
	};




	typedef struct strInfo{
		InfoNum num;
		QString msg;
		InfoType type;
	} strInfo;

	




	static strInfo strInfos[NBINFOS] = 
	{
		{FILENOTEXISTS, "File %s does not exist",WARNING2},
		{FILEUNREADABLE,"File %s is not readable",WARNING2},
		{FILECORRUPTED,"File %s is corrupted",WARNING2},
                {REMOVEDFOLDERSUCESS, "Removed folder %s", NORMAL2},
                {REMOVEDFOLDERFAILED, "Failed to remove folder %s", WARNING2},
                {LOADINGPROJECT, "Loading project (%s) ...",NORMAL2},
		{PROJECTLOADSUCCESSFULL, "Project loading successfull (%s) ",NORMAL2},
		{PROJECTLOADFAILED, "Project loading failed (%s) ",NORMAL2},
		{TEMPFOLDERCREATED, "Temporary folder created (%s) ",NORMAL2},
		{COPYINGFILETOTEMP, "Copying file(%s) to temporary folder",NORMAL2},
		{ADDEDPROBLEM, "Problem \"%s\" added to project",NORMAL2},
		{ONESIMULATIONSUCCESS, "Simulation has run successfully",NORMAL2},
		{ONESIMULATIONFAILED, "Simulation has failed. See <A href=\"file:///%s\">log file</A> for detail.",NORMAL2},
		{PROJECTFILECORRUPTED, "MO project file %s is corrupted.",ERROR2},
		{PROJECTFILENOTEXISTS, "MO project file %s does not exist",ERROR2},
		{MODELFILENOTEXISTS, "Model file \"%s\" does not exist",ERROR2},
		{TEMPFOLDERUNREMOVABLE, "Temp folder (%s) not removable. Stopping computation.",ERROR2},
		{FOLDERUNREMOVABLE, "Folder (%s) not removable. Stopping computation.",ERROR2},
		{LOADINGMODEL, "Loading model file (%s) ...",NORMAL2},
		{RESULTFILENOTEXISTS, "Result file (%s) does not exist. This result will not be imported.",ERROR2},
		{RESULTFILECORRUPTED, "MO result file %s is corrupted.",ERROR2},
		{PROBLEMFILENOTEXISTS, "Problem file (%s) does not exist. This result will not be imported.",ERROR2},
		{PROBLEMFILECORRUPTED, "Problem file is corrupted (error : %s, file : %s)",ERROR2},
		{PROBLEMMODELNOTFOUND, "Model \"%s\" of problem \"%s\" can't be found.",ERROR2},
		//MODEL
		{MODELCOMPILATIONSUCCESS, "Model \"%s\" compiled successfully",NORMAL2},
		{MODELCOMPILATIONFAIL, "Model \"%s\" failed to compile",ERROR2},
		{MODELLOADSUCCESS, "Model loaded successfully\"%s\" ",NORMAL2},
		{MODELLOADFAIL, "Model failed to be loaded \"%s\"  :\n %s",ERROR2},
		//Problem::OPTIMIZATION
		{OPTNOOBJECTIVE, "No objective in optimization problem",ERROR2},
		{OPTNOVARIABLE, "No variable in optimization problem",ERROR2},
		{OPTIMIZATIONFAILED, "Optimization has failed",ERROR2},
		{OPTIMIZATIONSUCCESS, "Optimization has run successfully",NORMAL2},

		//SUBMODEL
		{SUBMODELNOTADDED, "Impossible to add submodel %s in optimization",WARNING2},
		{SUBMODELNOTADDED, "Submodel %s added in optimization",NORMAL2},

		//Problem::EIPROBLEM
		{PROBLEMEIFAILED, "Problem EI has failed",ERROR2},
		{PROBLEMEISUCCESS, "Problem EI has run successfully",NORMAL2},

		//RECOMPUTING
		{RECOMPUTINGPOINTS, "Recomputing %s points.",NORMAL2},
		{RECOMPUTINGPOINTFAILED, "Recomputing point %s failed.",ERROR2},
		{RECOMPUTINGPOINTSUCCESS, "Recomputing point %s has run successfully.",NORMAL2},

		//VARIABLES
		{BEGINREADINGVARIABLES, "Reading model variables...",NORMAL2},
		{READVARIABLESSUCCESS, "Reading model variables has run successfully.",NORMAL2},

		//COMPONENTS
		{BEGINREADINGCOMPONENTS, "Reading model components...",NORMAL2},
		{READCOMPONENTSSUCCESS, "Reading model components has run successfully.",NORMAL2},

		//CONNECTIONS
		{BEGINREADINGCONNECTIONS, "Reading model connections...",NORMAL2},
		{READCONNECTIONSSUCCESS, "Reading model connections has run successfully.",NORMAL2},

		//MOomc
		{OMSOBTMODIFIERSFAILED, "Cannot obtain modifiers from component %s. ",WARNING2},
		{OMSGETCOMPERROR, "Modelica Error : impossible to get components of class %s.",ERROR2},

		//MOITEM
		{PROBLEMREADINGLINE, "Line is corrupted. Item won't be imported (%s)",WARNING2},

		{MODMODELFILECORRUPTED, "Model file is corrupted (error : %s, file : %s)",ERROR2},
		{MODELNOTEXISTSINMOFILE, "Model %s does not exist in file %s",ERROR2},
		
		//MILP
		{MILPERRORMODEL, "GLPK : Model file is corrupted ",ERROR2},
		{MILPERRORDATA, "GLPK : Data file is corrupted ",ERROR2},
		

		//LIBRARIES
		{LIBRARYFILENOTEXISTS, "Library file does not exist (%s)",ERROR2},

	};


}


#endif




