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

        @file MOSettings.cpp
        @brief Comments for file documentation.
        @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
        Company : CEP - ARMINES (France)
        http://www-cep.ensmp.fr/english/
        @version

  */
#include "MOSettings.h"
#include <QApplication>

MOSettings* MOSettings::_instance = NULL;


MOSettings::MOSettings(void)
    :MOParameters()
{
}

MOSettings::~MOSettings(void)
{
}

MOSettings* MOSettings::instance()
{
    // instance pointer is stored in qapp properties : allows to share with plugins
    // Otherwise, plugins create a new instance
    // A correct way would be to build an OMOptim shared lib, that exe and plugin would share
    if(!_instance)
    {
        if(qApp->property("MOSettings").isValid())
        {
            _instance = dynamic_cast<MOSettings*>(qApp->property("MOSettings").value<QObject*>());
        }
        else
        {
            _instance = new MOSettings();
            qApp->setProperty("MOSettings", QVariant::fromValue(qobject_cast<QObject*>(_instance)));
        }
    }
    return _instance;
}


void MOSettings::initialize(bool preferDefault)
{
    setFromDefaultValues();
    if(!preferDefault)
        updateFromSavedValues();
    save();
}

void MOSettings::updateFromSavedValues()
{
    QSettings globalSettings("MO", "Settings");
    QString settingName;
    QString group;
    QVariant value;
    for(int i=0;i<instance()->size();i++)
    {
        group = instance()->at(i)->getFieldValue(MOParameter::GROUP).toString();

        settingName = instance()->at(i)->name();
        if(!group.isEmpty())
            settingName = group+"/"+settingName;

        value = globalSettings.value(settingName,QVariant());
        if(!value.isNull())
            instance()->at(i)->setFieldValue(MOParameter::VALUE,value);
    }
}

void MOSettings::save()
{
    QSettings globalSettings("MO", "Settings");
    QString settingName;
    QString group;
    QVariant value;
    for(int i=0;i<instance()->size();i++)
    {
        group = instance()->at(i)->getFieldValue(MOParameter::GROUP).toString();

        settingName = instance()->at(i)->name();
        if(!group.isEmpty())
            settingName = group+"/"+settingName;

        value = instance()->at(i)->value();
        globalSettings.setValue(settingName,value);
    }
}

void MOSettings::setFromDefaultValues()
{
    QStringList names;
    QStringList descs;
    QStringList groups;
    QVariantList defaultValues;
    QVector<MOParameter::Type> types;




    // processing
    MOParameter *param;

    for(int i=0; i<names.size();i++)
    {
            // update
            param = new MOParameter(names.at(i),descs.at(i),defaultValues.at(i),types.at(i));
            param->setFieldValue(MOParameter::GROUP,groups.at(i));
            instance()->addItem(param);
    }
}

void MOSettings::addParameters(MOParameters* addedParams, QString tabName)
{
    for(int i=0;i<addedParams->size();i++)
        instance()->addItem(addedParams->at(i));

    updateFromSavedValues();
}


QVariant MOSettings::value(QString name,QVariant defaultValue)
{
    return ((MOParameters*)(instance()))->value(name,defaultValue);
}


