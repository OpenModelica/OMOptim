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

  @file OMCase.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#if !defined(_OMCASE_H)
#define _OMCASE_H

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "InfoSender.h"
#include "MOItem.h"

class ProjectBase;



/**
* \brief OMCase is an abstract class inherited by Problem or Result.
    It provides basic functions like storage or displaying.
*/
class OMCase: public MOItem
{
    Q_OBJECT


protected :

    // General information
    ProjectBase* _project;

    // Files informations
    QString _saveFolder;
    QString _saveFileName;
    QStringList _neededFiles; /** Needed files for simulation */
    bool _isSaved; /// defines whether project has been modified since last save



public:
    QStringList _filesToCopy; /** Files to copy in temp directory of in save directory */
    QStringList _foldersToCopy; /** Files to copy in temp directory of in save directory */


    // CTOR
    OMCase();
    OMCase(ProjectBase*);
    OMCase(const OMCase & s);
    virtual ~OMCase(void);

    // Managment functions
    /**
 * Description Save problem information in XML form.
 */
    virtual QDomElement toXmlData(QDomDocument & doc) = 0;
    virtual void store(QString destFolder, QString tempDir);
    virtual void rename(QString name,bool changeFolder);

    // Get functions
    QString saveFolder();
    QString saveFileName();
    QString entireSavePath();
    QStringList filesToCopy(){return _filesToCopy;};
    ProjectBase* project(){return _project;};


    // Set functions
    void setName(QString);
    void setSaveFolder(QString);
    virtual void setDefaultSaveFileName() = 0;
    void setEntireSavePath(QString);
    void setProject(ProjectBase*);
    void setIsSaved(bool);
    bool isSaved();

public slots:
    /**
    * Open in an explorer the folder where OMCase is stored
    */
    void openFolder();

    void modified(); /// set _isSaved to false;


signals:
    void renamed(QString);
};





#endif
