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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

  @file EIConn.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#ifndef EIConn_H
#define EIConn_H

#include "MOItem.h"
#include "EIItem.h"
#include "EIReader.h"
#include "EITree.h"

namespace EI
{
/*! \class EIConn
   * \brief Each instance of this class corresponds to a heat connexion between two streams. It can concerns only fractions of these streams
   * 
   */
class EIConn : public MOItem
{
public:
    enum Field
    {
        NAME,
        STREAMA,
         FLOWA_V,
        FLOWA_U,
        TINA_V,
        TINA_U,
        TOUTA_V,
        TOUTA_U,
        STREAMB,
        FLOWB_V,
       FLOWB_U,
        TINB_V,
        TINB_U,
        TOUTB_V,
        TOUTB_U,
        QFLOW_V,
        QFLOW_U,
    };


    static const int nbFields = 17;


    EIConn();
    ~EIConn(void);
    EIConn(const EIConn &);
    EIConn(QDomElement &, const EITree &);
    EIConn& operator=(const EIConn&);


    // MOItem overwrited
    virtual unsigned getNbFields(){return nbFields;}
    virtual QString getClassName(){return "EIConn";}
    bool isValid() const;

    // Specific functions
    void setA(const QString &a,const METemperature &Tina,const METemperature &Touta,MEMassFlow flowA);
    void setB(const QString &b,const METemperature &Tinb,const METemperature &Toutb,MEMassFlow flowB);
    void setQFlow(MEQflow);

    QString aName()const{return _streamA;}
    QString bName()const{return _streamB;}

    METemperature TinA()const{return _TinA;}
    METemperature TinB()const{return _TinB;}
    METemperature ToutA()const{return _ToutA;}
    METemperature ToutB()const{return _ToutB;}
    MEQflow qFlow()const{return _qFlow;}

    METemperature Tin(QString stream)const;
    METemperature Tout(QString stream)const;
    MEMassFlow massFlow(QString stream)const;

    // access and edit functions
    QVariant getFieldValue(int iField, int role = Qt::UserRole) const;
    static QString sFieldName(int field, int role);
    QString getFieldName(int i, int role = Qt::DisplayRole){return EIConn::sFieldName(i,role);}
    bool setFieldValue(int field,QVariant value);

protected:
    QString _streamA;
    QString _streamB;
    METemperature _TinA;
    METemperature _ToutA;
    METemperature _TinB;
    METemperature _ToutB;
    MEQflow _qFlow;
    MEMassFlow _flowA;
    MEMassFlow _flowB;

};
}

#endif
