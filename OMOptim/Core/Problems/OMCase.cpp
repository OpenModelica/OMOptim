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

  @file OMCase.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
        @version

  */
#include "OMCase.h"
#include "LowTools.h"
#include "EABase.h"
#include "Project.h"

OMCase::OMCase(void)
{
    QString msg = "new OMCase"+name();
    qDebug(msg.toLatin1().data());
}


OMCase::OMCase(Project* project)
{
    _project = project;
    _modItemsTree = _project->modItemsTree();
}

OMCase::OMCase(const OMCase &omCase)
{


    _name = omCase._name;
    _project = omCase._project;
    _modItemsTree = omCase._modItemsTree;


    _filesToCopy = omCase._filesToCopy;
    _saveFolder = omCase._saveFolder;
    _saveFileName = omCase._saveFileName;

    QString msg = "new OMCase"+name();
    qDebug(msg.toLatin1().data());
}

OMCase::~OMCase(void)
{  
    QString msg = "deleted OMCase"+name();
    qDebug(msg.toLatin1().data());
}


void OMCase::setName(QString name)
{
    _name=name;
    emit renamed(_name);

    if(_saveFileName.isEmpty())
        setDefaultSaveFileName();
}


void OMCase::setProject(Project* project)
{
    _project=project;
}

void OMCase::setSaveFolder(QString saveFolder)
{
    _saveFolder=saveFolder;
}

void OMCase::setEntireSavePath(QString savePath)
{
    QFileInfo fInfo(savePath);
    setSaveFolder(fInfo.canonicalPath());
    _saveFileName = fInfo.fileName();
}

QString OMCase::saveFolder()
{
    return _saveFolder;
}

QString OMCase::saveFileName()
{
    return _saveFileName;
}

QString OMCase::entireSavePath()
{
    return _saveFolder + QDir::separator() + _saveFileName;
}



void OMCase::openFolder()
{
    LowTools::openFolder(_saveFolder);
}


/**
* Stores problem files in destFolder. Is called when a problem resolution is finished.
* @param destFolder destination folder path.
* @param tempDir dir from where problem files are copied
*/
void OMCase::store(QString destFolder, QString tempDir)
{

    // update save paths
    setSaveFolder(destFolder);
    setDefaultSaveFileName();

    QString savePath = _saveFolder + QDir::separator() + _saveFileName;


    QDir dir = QDir(_saveFolder);

    if (!dir.exists())
    {
        dir.mkpath(_saveFolder);
    }
    else
    {
        LowTools::removeDir(_saveFolder);
        dir.mkdir(_saveFolder);
    }

    //copy needed path from old place to new one
    if(tempDir != "")
    {
        QDir tmpDir(tempDir);
        QDir newDir(_saveFolder);

        // copy problem files and folders
        QStringList fileNames = tmpDir.entryList();
        for(int i=0;i<fileNames.size();i++)
        {
            QFile::copy(tempDir + QDir::separator() + fileNames.at(i),_saveFolder + QDir::separator() + fileNames.at(i));
        }

        for(int i=0;i<_filesToCopy.size();i++)
        {
            QFile::copy(tempDir + QDir::separator() + _filesToCopy.at(i),_saveFolder + QDir::separator() + _filesToCopy.at(i));
        }
    }

    setSaveFolder(destFolder);
}


/**
* Renames omcase
* @param newName new omcase name
* @param changeFolder if yes, rename folder also
*/
void OMCase::rename(QString newName, bool changeFolder)
{
    QString oldName = _name;
    setName(newName);

    if(changeFolder)
    {
        QString oldSaveFolder = saveFolder();
        QString newSaveFolder = oldSaveFolder;
        newSaveFolder.replace(oldName,newName);

        QString oldSaveFileName = saveFileName();
        QString newSaveFileName = oldSaveFileName;
        newSaveFileName.replace(oldName,newName);

        QDir newDir(newSaveFolder);
        if(newDir.exists())
        {
            newDir.cd("..");
            newDir.rmdir(newSaveFolder);
            newDir.setCurrent(newSaveFolder);
        }

        LowTools::copyDir(oldSaveFolder,newSaveFolder);
        LowTools::removeDir(oldSaveFolder);

        newDir.rename(oldSaveFileName,newSaveFileName);

        setSaveFolder(newSaveFolder);
        _saveFileName = newSaveFileName;
    }
}

