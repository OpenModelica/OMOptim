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

  @file BlockSubsScene.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version
*/

#ifndef BLOCKSUBSSCENE_H
#define BLOCKSUBSSCENE_H


#include "diagramscene.h"
#include "BlockSubstitutions.h"
#include "BlockDrawItem.h"
#include "ModItem.h"
#include "ModModel.h"
#include "ModLoader.h"
#include "BlockSubstituteConnDlg.h"

#include <QtGui/QGraphicsSceneEvent>
#include <QtGui/QGraphicsGridLayout>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QMenu>
#include <QtGui/QGraphicsLineItem>

class BlockSubsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    BlockSubsScene(BlockSubstitutions *, ModModel* ,ModItemsTree*, bool _isEditable);
    ~BlockSubsScene(void);


    void zoomFit();
public slots:

    void refresh();
    void clearAll();
    bool openConnections();
    void removeBlock();

signals:

    void zoomRect(QRectF);

public :


    void selectOrg(QString, bool deselectAll);
    void selectOrgs(QStringList, bool deselectAll);
    void selectOrg(int ,bool deselectAll);

    void selectSub(QString _org,QString _sub, bool doDeselectAll);
    void selectSubs(QStringList _orgs,QStringList _subs,bool doDeselectAll);
    void selectSub(int iOrg,int iSub, bool doDeselectAll);
    void deselectAll();

    QStringList getSelectedOrg();
    QStringList getSelectedSub();

private:

    ModModel* model;
    ModItemsTree* modClassTree;


    bool isEditable;


    BlockSubstitutions *blockSubs;


    QGraphicsProxyWidget* addOrgBlock(ModItem* _orgComponent,QPoint _pos);
    QGraphicsProxyWidget* addSubBlock(int iOrg, ModItem* _subComponent,QPoint _pos);



    // main widget
    QGraphicsWidget *mainWidget;

    // Block draw items
    QList<BlockDrawItem* > orgBlocks;
    QList<QList< BlockDrawItem* > > subBlocks;

    QList<QGraphicsProxyWidget* > proxOrgBlocks;
    QList<QList< QGraphicsProxyWidget* > > proxSubBlocks;

    QList<QList<QGraphicsLineItem*> > lines;

    QList<QGraphicsProxyWidget*> selectedOrgProx;
    QStringList selectedOrg;

    QList<QGraphicsProxyWidget*> selectedSubProx;
    QStringList selectedSub;





protected :
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * contextMenuEvent );
    virtual void  keyReleaseEvent ( QKeyEvent * keyEvent );
    void setBlockPos(QGraphicsProxyWidget*, QPointF);

    bool openConnections(BlockSubstitution* _blockSub);

    bool findBlockDrawItem(QGraphicsItem* _item ,int & iOrg,int & iSub, bool & isOrg);
    int findOrgBlock(QString _org);
    bool findSubBlock(QString _org,QString _sub,int & iOrg, int & iSub);


};


#endif
