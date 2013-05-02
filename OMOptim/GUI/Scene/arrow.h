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

     @file arrow.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version 
*/

 #ifndef ARROW_H
 #define ARROW_H

 #include <QGraphicsLineItem>

 #include "diagramitem.h"

 class QGraphicsPolygonItem;
 class QGraphicsLineItem;
 class QGraphicsScene;
 class QRectF;
 class QGraphicsSceneMouseEvent;
 class QPainterPath;

 class Arrow : public QObject,public QGraphicsLineItem
 {
     Q_OBJECT
 public:
     enum { Type = UserType + 4 };

     Arrow(DiagramItem *startItem, DiagramItem *endItem,
       QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

     int type() const
   { return Type; }
     QRectF boundingRect() const;
     QPainterPath shape() const;
     void setColor(const QColor &color)
   { myColor = color; }
     DiagramItem *startItem() const
   { return myStartItem; }
     DiagramItem *endItem() const
   { return myEndItem; }

 public slots:
     void updatePosition();

 protected:
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
          QWidget *widget = 0);

 private:
     DiagramItem *myStartItem;
     DiagramItem *myEndItem;
     QColor myColor;
     QPolygonF arrowHead;
 };

 #endif
