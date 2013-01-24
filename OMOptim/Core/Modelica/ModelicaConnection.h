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

     @file ModelicaConnection.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 

  */
#if !defined(_MODELICACONNECTION_H)
#define _MODELICACONNECTION_H


#include <QtCore/QObject>
#include "MOItem.h"


class ModelicaConnection : public MOItem
{
public:

    enum Field
    {
        NAME,
        A,
        B
    };

    
    ModelicaConnection(void);
    ModelicaConnection(QDomElement &);
    ModelicaConnection(QString a,QString b);



    virtual QString getClassName() const {return "ModelicaConnection";};
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole){return ModelicaConnection::sFieldName(i,role);};
    virtual QVariant getFieldValue(int, int role = Qt::UserRole) const;
    virtual bool setFieldValue(int ifield, QVariant value_);



    static const int nbFields = 3;
    virtual unsigned getNbFields(){return nbFields;};



    ~ModelicaConnection(void);

    QString getA();
    QString getB();


    void setA(QString a);
    void setB(QString b);

private:
    QString _a;
    QString _b;

};


#endif
