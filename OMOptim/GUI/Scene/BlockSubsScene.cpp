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

    @file BlockSubsScene.cpp
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version
*/

#include "BlockSubsScene.h"

BlockSubsScene::BlockSubsScene(BlockSubstitutions *_blockSubs,ModItemsTree* _modItemsTree, bool _isEditable)
{
    blockSubs = _blockSubs;
    modItemsTree = _modItemsTree;
    isEditable = _isEditable;

    // visual matters
    //setMode(DiagramScene::MoveItem);

    // connect update
    connect(blockSubs,SIGNAL(changed()),this,SLOT(refresh()));

    mainWidget = new QGraphicsWidget();

    refresh();

    deselectAll();
}

BlockSubsScene::~BlockSubsScene(void)
{
}

void BlockSubsScene::clearAll()
{
    for(int i = 0;i<orgBlocks.size();i++)
    {
        delete orgBlocks.at(i);
        for(int j=0;j<subBlocks.at(i).size();j++)
        {
            delete subBlocks.at(i).at(j);
            delete lines.at(i).at(j);
        }
    }

    orgBlocks.clear();
    subBlocks.clear();

    proxOrgBlocks.clear();
    proxSubBlocks.clear();

    lines.clear();
}


void BlockSubsScene::refresh()
{

    clearAll();

    int subMargin = 5;
    int orgMargin = 50;

    int startOrgY = 0;
    int endOrgY = 0;


    // add blocks
    QPoint _curOrgPos (0,0);
    QPoint _curSubPos (150,0);

    QGraphicsProxyWidget* _curOrgBlock;
    QGraphicsProxyWidget* _curSubBlock;

    int maxOrgBlockWidth=0;
    int maxSubBlockWidth=0;

    if(blockSubs!=NULL)
    {
        QStringList orgs = blockSubs->getReplacedComponentsNames();
        for(int i=0;i<orgs.size();i++)
        {
            ModItem* _orgEl = modItemsTree->findInDescendants(orgs.at(i));
            if(_orgEl)
            {

                startOrgY = endOrgY + orgMargin;
                //*************
                // adding org
                //*************
                _curOrgPos.setY(startOrgY);
                _curOrgBlock = addOrgBlock(_orgEl->name(), _curOrgPos);
                maxOrgBlockWidth = std::max<double>((double)maxOrgBlockWidth,_curOrgBlock->geometry().width());

                //*************
                // adding subs
                //*************
                QStringList subs = blockSubs->getReplacingComponentNames(orgs.at(i));
                int nbSubs = subs.size();

                _curSubPos.setY(startOrgY);

                for(int j=0;j<nbSubs;j++)
                {
                    ModItem* _subEl = modItemsTree->findInDescendants(subs.at(j));
                    if(_subEl)
                    {
                        _curSubBlock = addSubBlock(i,_subEl->name(), _curSubPos);
                        maxSubBlockWidth = std::max<double>((double)maxSubBlockWidth,_curSubBlock->geometry().width());
                        _curSubPos.setY(_curSubPos.y() + _curSubBlock->geometry().height() + subMargin);
                    }
                }

                endOrgY = _curSubPos.y() - subMargin;


                //***********************
                // set pos of OrgBlock
                //***********************
                _curOrgPos.setY((startOrgY+endOrgY)/2 - _curOrgBlock->geometry().height()/2);
                setBlockPos(_curOrgBlock,_curOrgPos);
                endOrgY = std::max<double>(_curOrgPos.y()+_curOrgBlock->geometry().height(),(double)_curSubPos.y());
            }
        }
    }

    //set width and position
    for(int iOrg = 0;iOrg<proxOrgBlocks.size();iOrg++)
    {
        QRectF newGeometry = proxOrgBlocks.at(iOrg)->geometry();
        newGeometry.setWidth(maxOrgBlockWidth);
        proxOrgBlocks.at(iOrg)->setGeometry(newGeometry);

        for(int iSub = 0;iSub<proxSubBlocks.at(iOrg).size();iSub++)
        {
            QRectF newGeometry = proxSubBlocks.at(iOrg).at(iSub)->geometry();
            newGeometry.setWidth(maxSubBlockWidth);
            newGeometry.moveLeft(maxOrgBlockWidth+50);
            proxSubBlocks.at(iOrg).at(iSub)->setGeometry(newGeometry);


            double x1 = proxOrgBlocks.at(iOrg)->boundingRect().right();
            double y1 = proxOrgBlocks.at(iOrg)->pos().y()+proxOrgBlocks.at(iOrg)->boundingRect().height()/2;
            double x2 = proxSubBlocks.at(iOrg).at(iSub)->pos().x();
            double y2 = proxSubBlocks.at(iOrg).at(iSub)->pos().y()+proxSubBlocks.at(iOrg).at(iSub)->boundingRect().height()/2;
            lines.at(iOrg).at(iSub)->setLine(x1,y1,x2,y2);
        }
    }

    deselectAll();

    // zoom
    zoomFit();
}


//
//void BlockSubsScene::refresh()
//{
//
//    clearAll();
//
//    int rectW = 200; // with margin
//    int rectH = 110; // with margin
//
//
//    int _rowOffset=0;
//    int _curRow;
//    double maxOrgWidth;
//    double maxSubWidth;
//
//    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
//    
//
//    if(blockSubs!=NULL)
//    {
//        QStringList orgs = blockSubs->getReplacedComponentsNames();
//        for(int i=0;i<orgs.size();i++)
//        {
//            ModItem* _orgEl = modelRoot->findComporModelInDescendants(orgs.at(i));
//            if(_orgEl)
//            {
//                QStringList subs = blockSubs->getReplacingComponentNames(orgs.at(i));
//                int nbSubs = subs.size();
//
//                if(nbSubs == 0)
//                {
//                    _curRow = _rowOffset;
//                    _rowOffset = _curRow+2;
//                }
//                else
//                    _curRow = _rowOffset + nbSubs -1;
//
//                // Org block
//                BlockDrawItem* _curOrgBlock = addOrgBlock(_orgEl, QPoint());
//                layout->addItem(_curOrgBlock,_curRow,0,2,1);
//                maxOrgWidth = max(_curOrgBlock->boundingRect().width(),maxOrgWidth);
//                
//                // Subs block
//                for(int j=0;j<nbSubs;j++)
//                {
//                    int iLibrary = libraries->findItem(subs.at(j).section(".",0,0));
//                    if(iLibrary>-1)
//                    {
//                        ModItem* _subEl = libraries->at(iLibrary)->findComporModelInDescendants(subs.at(j));
//                        if(_subEl)
//                        {
//                            BlockDrawItem* _curSubBlock = addSubBlock(_curOrgBlock,_subEl, QPoint());
//                            _curRow = _rowOffset+j*2;
//                            layout->addItem(_curSubBlock,_curRow,1,2,1);
//                            layout->setRowMinimumHeight(_curRow,_curSubBlock->boundingRect().height()/2);
//                            layout->setRowMinimumHeight(_curRow+1,_curSubBlock->boundingRect().height()/2);
//                            maxSubWidth = max(_curOrgBlock->boundingRect().width(),maxSubWidth);
//                        }
//                    }
//                }
//                _rowOffset = _rowOffset + nbSubs * 2;
//            }
//        }
//
//        layout->setColumnMinimumWidth(0,maxOrgWidth);
//        layout->setColumnMinimumWidth(1,maxSubWidth);
//
//        delete mainWidget;
//        mainWidget = new QGraphicsWidget();
//        mainWidget->setLayout(layout);
//        addItem(mainWidget);
//
//        QRectF _rect = layout->geometry();
//        QRectF _rect1 = layout->contentsRect();
//        
//        // zoom
//        zoomFit();
//    }
//}



void BlockSubsScene::zoomFit()
{

    /*
    if(proxOrgBlocks.size()>0)
    {
    double xMin = proxOrgBlocks.at(0)->pos().x();
    double xMax = xMin + proxOrgBlocks.at(0)->boundingRect().width();
    double yMin = proxOrgBlocks.at(0)->pos().y();
    double yMax = yMin + proxOrgBlocks.at(0)->boundingRect().height();

    for(int i=0;i<proxOrgBlocks.size();i++)
    {
    xMin = min(xMin,proxOrgBlocks.at(i)->pos().x();
    xMax = max(xMax,proxOrgBlocks.at(i)->pos().x() + proxOrgBlocks.at(i)->boundingRect().width());
    yMin = min(yMin,proxOrgBlocks.at(i)->pos().y();
    yMax = max(yMax,proxOrgBlocks.at(i)->pos().y() + proxOrgBlocks.at(i)->boundingRect().height());

    for(int j=0; j<proxOrgBlocks.at(i).size();j++)
    {
    xMin = min(xMin,proxOrgBlocks.at(i)->pos().x();
    xMax = max(xMax,proxOrgBlocks.at(i)->pos().x() + proxOrgBlocks.at(i)->boundingRect().width());
    yMin = min(yMin,proxOrgBlocks.at(i)->pos().y();
    yMax = max(yMax,proxOrgBlocks.at(i)->pos().y() + proxOrgBlocks.at(i)->boundingRect().height());
    }
    }

    QGraphicsProxyWidget* _firstBlock;
    QGraphicsProxyWidget* _lastBlock;

    if(proxSubBlocks.at(0).size()>0)
    _firstBlock = proxSubBlocks.at(0).at(0);
    else
    _firstBlock = proxOrgBlocks.at(0);


    QPointF _firstPos(proxOrgBlocks.at(0)->pos().x()-10,_firstBlock->pos().y()-10);


    if(subBlocks.at(proxOrgBlocks.size()-1).size()>0)
    _lastBlock = proxSubBlocks.at(proxOrgBlocks.size()-1).at(proxSubBlocks.at(proxOrgBlocks.size()-1).size()-1);
    else
    _lastBlock = proxOrgBlocks.at(proxOrgBlocks.size()-1);

    QPointF _lastPos = _lastBlock->boundingRect().bottomRight() + _lastBlock->pos() + QPointF(10,10);

    emit zoomRect(_firstPos,_lastPos);
    }*/

    //setSceneRect(QRectF());

    QRectF _rect = itemsBoundingRect();
    double minHeight = 50;
    if(_rect.height()< minHeight)
    {
        _rect.setWidth(_rect.width()*minHeight/_rect.height());
        _rect.setHeight(minHeight);
    }


    emit zoomRect(_rect);
}

QGraphicsProxyWidget* BlockSubsScene::addOrgBlock(QString orgName,QPoint _pos)
{
    BlockDrawItem* _newBlock = new BlockDrawItem(orgName,BlockDrawItem::REPLACED);


    // add rectangle
    //_newBlock->setPos(_pos);

    // draw it
    QGraphicsProxyWidget* _newProxBlock = addWidget(_newBlock);
    _newProxBlock->setPos(_pos);
    //addItem(_newBlock->text);

    // store it
    orgBlocks.push_back(_newBlock);
    subBlocks.push_back(QList<BlockDrawItem*>());

    proxOrgBlocks.push_back(_newProxBlock);
    proxSubBlocks.push_back(QList<QGraphicsProxyWidget*>());

    lines.push_back(QList<QGraphicsLineItem*>());

    return _newProxBlock;
}


QGraphicsProxyWidget* BlockSubsScene::addSubBlock(int iOrg, QString subName, QPoint _pos)
{

    // draw it
    BlockDrawItem* _newBlock = new BlockDrawItem(subName,BlockDrawItem::REPLACING);
    QGraphicsProxyWidget* _newProxBlock = addWidget(_newBlock);
    _newProxBlock->setPos(_pos);


    // store it
    QList<BlockDrawItem*> _subBlocks = subBlocks.at(iOrg);
    _subBlocks.push_back(_newBlock);
    subBlocks.replace(iOrg,_subBlocks);

    QList<QGraphicsProxyWidget*> _proxSubBlocks = proxSubBlocks.at(iOrg);
    _proxSubBlocks.push_back(_newProxBlock);
    proxSubBlocks.replace(iOrg,_proxSubBlocks);

    // add line
    QList<QGraphicsLineItem*> _orgLines = lines.at(iOrg);
    double x1 = proxOrgBlocks.at(iOrg)->boundingRect().right();
    double y1 = (proxOrgBlocks.at(iOrg)->boundingRect().top()+proxOrgBlocks.at(iOrg)->boundingRect().bottom())/2;
    double x2 = _pos.x();
    double y2 = _pos.y()+_newProxBlock->boundingRect().height()/2;
    QGraphicsLineItem* _newLine = new QGraphicsLineItem(x1,y1,x2,y2,0,this);
    _newLine->setPen(QPen(QBrush(QColor(Qt::darkGray)),2,Qt::SolidLine,Qt::SquareCap));
    _orgLines.push_back(_newLine);
    lines.replace(iOrg,_orgLines);

    return _newProxBlock;
}

void BlockSubsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        QPointF _clkPos = mouseEvent->scenePos();
        QGraphicsItem* _item = itemAt(_clkPos);

        if(mouseEvent->modifiers()!=Qt::ControlModifier)
            deselectAll();

        if(_item)
        {
            int _iOrg,_iSub;
            bool _isOrg;

            if(findBlockDrawItem(_item , _iOrg, _iSub, _isOrg))
            {
                if(_isOrg)
                    selectOrg(_iOrg,false);
                else
                    selectSub(_iOrg,_iSub,false);
            }
        }
    }


}

bool BlockSubsScene::findBlockDrawItem(QGraphicsItem*_item ,int & iOrg,int & iSub, bool & isOrg)
{
    bool found = false;
    int _iO = 0;
    int _iS;

    while(!found && (_iO < proxOrgBlocks.size()))
    {
        if((_item == proxOrgBlocks.at(_iO)))
        {
            found = true;
            isOrg = true;
            iOrg = _iO;
            return true;
        }
        else
        {
            _iS=0;
            while(!found && (_iS < proxSubBlocks.at(_iO).size()))
            {
                if((_item == proxSubBlocks.at(_iO).at(_iS)))
                {
                    found = true;
                    isOrg = false;
                    iOrg = _iO;
                    iSub = _iS;
                    return true;
                }
                else
                    _iS++;
            }
        }
        _iO++;
    }
    return false;
}

int BlockSubsScene::findOrgBlock(QString _org)
{
    int i=0;
    bool found = false;

    while((i<orgBlocks.size()) && !found)
    {
        if(orgBlocks.at(i)->componentName==_org)
            return i;
        else
            i++;
    }
    return -1;
}

bool BlockSubsScene::findSubBlock(QString _org,QString _sub,int & iOrg, int & iSub)
{
    iOrg = findOrgBlock(_org);

    if(iOrg==-1)
        return false;
    else
    {
        iSub = 0;
        bool found = false;
        while((iSub < subBlocks.at(iOrg).size()) && !found)
        {
            if(subBlocks.at(iOrg).at(iSub)->componentName==_sub)
                return true;
            else
                iSub++;
        }
    }
    return false;
}

void BlockSubsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent )
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        QPointF _clkPos = mouseEvent->scenePos();
        QGraphicsItem* _item = itemAt(_clkPos);

        int _iOrg;
        int _iSub;
        bool _isOrg;

        bool found = findBlockDrawItem(_item ,_iOrg, _iSub, _isOrg);

        if(found && !_isOrg)
        {

            QString replacedName = orgBlocks.at(_iOrg)->componentName;
            QString replacingName = subBlocks.at(_iOrg).at(_iSub)->componentName;

            BlockSubstitution* curBlockSub = blockSubs->find(replacedName,replacingName);

            if(curBlockSub!=NULL)
            {
                openConnections(curBlockSub);
            }

        }

        if(!found)
        {
            zoomFit();
        }
    }
}

void BlockSubsScene::contextMenuEvent( QGraphicsSceneContextMenuEvent * contextMenuEvent )
{
    QPointF _clkPos = contextMenuEvent->scenePos();
    QGraphicsItem* _item = itemAt(_clkPos);

    if(_item)
    {
        int _iOrg,_iSub;
        bool _isOrg;

        if(findBlockDrawItem(_item , _iOrg, _iSub, _isOrg))
        {

            // select item
            if(_isOrg)
                selectOrg(_iOrg,true);
            else
                selectSub(_iOrg,_iSub,true);


            QMenu *menu = new QMenu();

            QStringList data;
            data << orgBlocks.at(_iOrg)->componentName;
            if(!_isOrg)
                data << subBlocks.at(_iOrg).at(_iSub)->componentName;

            if(!_isOrg)
            {
                // edit connections
                QAction *editConn = new QAction("Edit connections...",menu);
                editConn->setData(data);
                connect(editConn,SIGNAL(triggered()),this,SLOT(openConnections()));
                menu->addAction(editConn);
            }


            if(isEditable)
            {
                // remove block
                QAction *remove = new QAction("Remove",menu);
                remove->setData(data);
                connect(remove,SIGNAL(triggered()),this,SLOT(removeBlock()));
                menu->addAction(remove);
            }
            if(menu->actions().size()>0)
                menu->exec(contextMenuEvent->screenPos());
        }
    }
}

void BlockSubsScene::removeBlock()
{
    // get BlockSubstitution corresponding
    QAction *action = qobject_cast<QAction *>(sender());
    QStringList data = action->data().toStringList();

    // if isOrg
    if(data.size()==1)
    {
        blockSubs->removeBlocks(data.at(0));
    }
    else
    {
        // it is sub
        blockSubs->removeBlock(data.at(0),data.at(1));
    }
}

void BlockSubsScene::deselectAll()
{
    for(int i=0; i <orgBlocks.size(); i++)
    {
        orgBlocks.at(i)->desactivate();
        for(int j=0;j<subBlocks.at(i).size();j++)
            subBlocks.at(i).at(j)->desactivate();
    }

    selectedOrg.clear();
    selectedOrgProx.clear();

    selectedSub.clear();
    selectedSubProx.clear();



}

bool BlockSubsScene::openConnections()
{
    // get BlockSubstitution corresponding
    QAction *action = qobject_cast<QAction *>(sender());
    QStringList data = action->data().toStringList();

    BlockSubstitution* _blockSub = blockSubs->find(data.at(0),data.at(1));
    return openConnections(_blockSub);
}


bool BlockSubsScene::openConnections(BlockSubstitution* blockSub)
{
    // creating form for specifying connections
    BlockSubstituteConnDlg *form = new BlockSubstituteConnDlg(blockSub,isEditable,0);
    return (form->exec() == QDialog::Accepted);
}

void BlockSubsScene::selectOrg(int i,bool doDeselectAll)
{
    if(doDeselectAll)
        deselectAll();

    if(i<orgBlocks.size())
    {
        orgBlocks.at(i)->activate();

        selectedOrg.push_back(orgBlocks.at(i)->componentName);
        selectedOrgProx.push_back(proxOrgBlocks.at(i));
    }
}


void BlockSubsScene::selectOrg(QString _org,bool doDeselectAll)
{
    // look for corresponding block
    int i= findOrgBlock(_org);
    if(i>-1)
    {
        selectOrg(i,doDeselectAll);
    }
}


void BlockSubsScene::selectOrgs(QStringList _orgs,bool doDeselectAll)
{
    if(doDeselectAll)
        deselectAll();

    for(int i=0; i<_orgs.size();i++)
        selectOrg(_orgs.at(i),false);
}




void BlockSubsScene::selectSub(int iOrg,int iSub, bool doDeselectAll)
{
    if(doDeselectAll)
        deselectAll();

    if((iOrg<orgBlocks.size()) && (iSub<subBlocks.at(iOrg).size()))
    {
        selectOrg(iOrg,false);

        subBlocks.at(iOrg).at(iSub)->activate();

        selectedSub.push_back(subBlocks.at(iOrg).at(iSub)->componentName);
        selectedSubProx.push_back(proxSubBlocks.at(iOrg).at(iSub));
    }
}

void BlockSubsScene::selectSub(QString _org,QString _sub, bool doDeselectAll)
{
    int iOrg,iSub;

    if(doDeselectAll)
        deselectAll();

    if(findSubBlock(_org,_sub,iOrg,iSub))
    {
        selectSub(iOrg,iSub,false);
    }

}
void BlockSubsScene::selectSubs(QStringList _orgs,QStringList _subs,bool doDeselectAll)
{
    if(doDeselectAll)
        deselectAll();

    if(_orgs.size()==_subs.size())
    {
        for(int i=0;i<_orgs.size();i++)
        {
            selectSub(_orgs.at(i),_subs.at(i),false);
        }
    }
}


QStringList BlockSubsScene::getSelectedOrg()
{
    return selectedOrg;
}

QStringList BlockSubsScene::getSelectedSub()
{
    return selectedSub;
}

void BlockSubsScene::setBlockPos(QGraphicsProxyWidget* _prox, QPointF _newPos)
{
    int iOrg,iSub;
    bool isOrg;

    _prox->setPos(_newPos);

    if(findBlockDrawItem(_prox,iOrg,iSub,isOrg))
    {
        if(isOrg)
        {
            for(int iL=0;iL<lines.at(iOrg).size();iL++)
            {
                double x1 = _newPos.x() + _prox->boundingRect().width();
                double y1 = _newPos.y() +_prox->boundingRect().height()/2;
                double x2 = lines.at(iOrg).at(iL)->line().x2();
                double y2 = lines.at(iOrg).at(iL)->line().y2();
                lines.at(iOrg).at(iL)->setLine(x1,y1,x2,y2);
            }
        }
        else
        {
            double x2 = _prox->boundingRect().left();
            double y2 = (_prox->boundingRect().top()+_prox->boundingRect().bottom())/2;
            double x1 = lines.at(iOrg).at(iSub)->line().x1();
            double y1 = lines.at(iOrg).at(iSub)->line().y1();
            lines.at(iOrg).at(iSub)->setLine(x1,y1,x2,y2);
        }
    }
}


void BlockSubsScene::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    if(keyEvent->key()==Qt::Key_Delete)
    {
        for(int iS=0;iS<selectedSubProx.size();iS++)
        {
            QGraphicsItem* curItem = selectedSubProx.at(iS);
            int iOrg,iSub;
            bool isOrg;
            bool found = findBlockDrawItem(curItem,iOrg,iSub,isOrg);

            if(found)
            {
                QString orgName = orgBlocks.at(iOrg)->componentName;

                if(isOrg)
                    blockSubs->removeBlocks(orgName);
                else
                {
                    QString subName = subBlocks.at(iOrg).at(iSub)->componentName;
                    blockSubs->removeBlock(orgName,subName);
                }
            }
        }

        for(int iS=0;iS<selectedOrgProx.size();iS++)
        {
            QGraphicsItem* curItem = selectedOrgProx.at(iS);
            int iOrg,iSub;
            bool isOrg;
            bool found = findBlockDrawItem(curItem,iOrg,iSub,isOrg);

            if(found)
            {
                QString orgName = orgBlocks.at(iOrg)->componentName;

                if(isOrg)
                    blockSubs->removeBlocks(orgName);
                else
                {
                    QString subName = subBlocks.at(iOrg).at(iSub)->componentName;
                    blockSubs->removeBlock(orgName,subName);
                }
            }
        }
    }
}
