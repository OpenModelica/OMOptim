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

     @file diagramscene.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/
/****************************************************************************
 **
 ** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #ifndef DIAGRAMSCENE_H
 #define DIAGRAMSCENE_H

 #include <QGraphicsScene>
 #include "diagramitem.h"
 #include "diagramtextitem.h"

 class QGraphicsSceneMouseEvent;
 class QMenu;
 class QPointF;
 class QGraphicsLineItem;
 class QFont;
 class QGraphicsTextItem;
 class QColor;

 class DiagramScene : public QGraphicsScene
 {
     Q_OBJECT

 public:
     enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

     DiagramScene(QMenu *itemMenu=0, QObject *parent = 0);

     QFont font() const
         { return myFont; }
     QColor textColor() const
         { return myTextColor; }
     QColor itemColor() const
         { return myItemColor; }
     QColor lineColor() const
         { return myLineColor; }
     void setLineColor(const QColor &color);
     void setTextColor(const QColor &color);
     void setItemColor(const QColor &color);
     void setFont(const QFont &font);

 public slots:
     void setMode(Mode mode);
     void setItemType(DiagramItem::DiagramType type);
     void editorLostFocus(DiagramTextItem *item);

 signals:
     void itemInserted(DiagramItem *item);
     void textInserted(QGraphicsTextItem *item);
     void itemSelected(QGraphicsItem *item);

 protected:
     virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
     void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

 private:
     bool isItemChange(int type);

     DiagramItem::DiagramType myItemType;
     QMenu *myItemMenu;
     Mode myMode;
     bool leftButtonDown;
     QPointF startPoint;
     QGraphicsLineItem *line;
     QFont myFont;
     DiagramTextItem *textItem;
     QColor myTextColor;
     QColor myItemColor;
     QColor myLineColor;
 };

 #endif
