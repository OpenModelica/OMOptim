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
#include "ProjectBase.h"

//OMCase::OMCase(void)
//{

//}


OMCase::OMCase(ProjectBase* project)
    :MOItem(project)
{
    _project = project;
}

OMCase::OMCase(const OMCase &omCase)
{
    _name = omCase._name;
    _project = omCase._project;
 //   this->setParent(_project);

    _filesToCopy = omCase._filesToCopy;
    _relSaveFolder = omCase._relSaveFolder;
    _saveFileName = omCase._saveFileName;
}

OMCase::~OMCase(void)
{  

}


void OMCase::setName(QString name)
{
    _name=name;
    emit renamed(_name);

    if(_saveFileName.isEmpty())
        setDefaultSaveFileName();
}

void OMCase::setIsSaved(bool isSaved)
{
    _isSaved = isSaved;
}

bool OMCase::isSaved()
{
    return _isSaved;
}

void OMCase::setProject(ProjectBase* project)
{
    _project=project;
}

void OMCase::setSaveFolder(QString saveFolder)
{
    QDir projectDir = _project->folder();
   _relSaveFolder = projectDir.relativeFilePath(saveFolder);
}

void OMCase::setEntireSavePath(QString savePath)
{
    QFileInfo fInfo(savePath);
    setSaveFolder(fInfo.canonicalPath());
    _saveFileName = fInfo.fileName();
}

QString OMCase::saveFolder()
{
    return _project->folder().absoluteFilePath(_relSaveFolder);
}

QString OMCase::saveFileName()
{
    return _saveFileName;
}

QString OMCase::entireSavePath()
{
    return _project->folder().absoluteFilePath(_relSaveFolder + QDir::separator() + _saveFileName);
}

void OMCase::openFolder()
{
    LowTools::openFolder(saveFolder());
}

void OMCase::modified()
{
    setIsSaved(false);
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

    QString savePath = _relSaveFolder + QDir::separator() + _saveFileName;


    QDir dir = QDir(_relSaveFolder);

    if (!dir.exists())
    {
        dir.mkpath(_relSaveFolder);
    }
    //    else
    //    {
    //        LowTools::removeDir(_relSaveFolder);
    //        dir.mkpath(_relSaveFolder);
    //    }

    //copy needed path from old place to new one
    if(tempDir != "")
    {
        QDir tmpDir(tempDir);
        QString fileToCopy;

        // copy problem files and folders
        QStringList fileNames = tmpDir.entryList();
        for(int i=0;i<fileNames.size();i++)
        {
            QFile::copy(tempDir + QDir::separator() + fileNames.at(i),_relSaveFolder + QDir::separator() + fileNames.at(i));
        }

        for(int i=0;i<_filesToCopy.size();i++)
        {
            if(_filesToCopy.at(i).isRelative())
                fileToCopy = tmpDir.absoluteFilePath(_filesToCopy.at(i).filePath());
            else
                fileToCopy= _filesToCopy.at(i).absoluteFilePath();

            QFile::copy(fileToCopy,_relSaveFolder + QDir::separator() + _filesToCopy.at(i).fileName());
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
    if(newName != _name)
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
            if(oldSaveFolder!=newSaveFolder)
                LowTools::removeDir(oldSaveFolder);

            newDir.rename(oldSaveFileName,newSaveFileName);

            setSaveFolder(newSaveFolder);
            _saveFileName = newSaveFileName;
        }
    }
}

