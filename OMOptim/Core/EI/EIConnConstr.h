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

  @file EIConnConstr.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#ifndef EICONNCONSTR_H
#define EICONNCONSTR_H

#include "MOItem.h"
#include "EIItem.h"
#include "EIReader.h"
#include "EITree.h"

/*! \class EIConnConstr
   * \brief Each instance of this class corresponds to a connexion constraint
   * in energy integration.
   */
namespace EI
{
class EIConnConstr : public MOItem
{
public:
    enum Field
    {
        //Modelica fields
        ITEMA,
        ITEMB,
        TYPE
    };

    enum Type
    {
        FORBIDDEN
    };
    static const int nbFields = 3;


    EIConnConstr(EITree*);
    ~EIConnConstr(void);
    EIConnConstr(QDomElement &, EITree*);


    static QString strType(Type);

    // MOItem overwrited
    virtual unsigned getNbFields(){return nbFields;};
    virtual QString getClassName(){return "EIConnConstr";};


    // Specific functions
    void setItems(EIItem* a,EIItem* b);
    void setType(Type _type);
    EIConnConstr* clone() const;
    bool isValid() const;
    QMultiMap<QString,QString> getForbiddenMatchs(MOOptVector *variables);

    // access and edit functions
    QVariant getFieldValue(int iField, int role = Qt::UserRole) const;
    static QString sFieldName(int field, int role);
    QString getFieldName(int i, int role = Qt::DisplayRole){return EIConnConstr::sFieldName(i,role);};
    bool setFieldValue(int field,QVariant value);

protected:
    EIItem* a;
    EIItem* b;
    Type type;
    EITree* eiTree;
};
}

#endif
