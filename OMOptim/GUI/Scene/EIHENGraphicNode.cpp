
/****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/


#include "EIHENGraphicNode.h"
#include "EIHENGraphicEdge.cpp"

EIHENGraphicNode::EIHENGraphicNode(EIHEN_Node* node,double TXFactor)
{
    _node = node;
    _TXfactor = TXFactor;

    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);

    setToolTip(nodeToolTip());

}

QString EIHENGraphicNode::nodeToolTip()
{
    QString msg;

    EIHEN_HENode* curHENode;

    if(_node)
    {
        msg += "Stream : " + _node->_stream + "\n";

        switch(_node->nodeType())
        {
        case NodeSplitter :
        case NodeMixer :
        case NodeGeneric :
        case NodeEnd :
        case NodeRoot :
            msg += "Temperature : " + _node->_inletT.toString()+ "\n";
            break;
        case NodeHE :
            curHENode = dynamic_cast<EIHEN_HENode*>(_node);
            if(curHENode)
            {
                msg += "Inlet Temperature : " + curHENode->_inletT.toString()+ "\n";
                msg += "Outlet Temperature : " + curHENode->_outletT.toString()+ "\n";
                msg += "Heat flow : " + curHENode->_qFlow.toString()+ "\n";
            }
            break;
        }

        msg += "Mass flow : " + _node->massFlow().toString();
    }
    return msg;
}

void EIHENGraphicNode::addEdge(EIHENGraphicEdge *edge)
{
    _edges << edge;
    edge->adjust();
}


QList<EIHENGraphicEdge *> EIHENGraphicNode::edges() const
{
    return _edges;
}


QRectF EIHENGraphicNode::boundingRect() const
{

    switch(_node->nodeType())
    {


    case NodeGeneric :
        return QRectF(-5,-15,10,30);
    case NodeSplitter :
    case NodeMixer :
        return QRectF();
    case NodeEnd :
        return classicNodeRect(_node,false);
    case NodeRoot :
        return classicNodeRect(_node,true,Qt::AnchorTop);
    case NodeHE :
        return HERect(dynamic_cast<EIHEN_HENode*>(_node));
    }

    return QRectF();
}

QPainterPath EIHENGraphicNode::shape() const
{
    double factor = 0.2;
    QPainterPath path;
    switch(_node->nodeType())
    {

    case NodeGeneric :
        path.addRect(-5,-15,10,30);
        break;
    case NodeSplitter :
    case NodeMixer :
        break;
    case NodeEnd :
        path.addRect(classicNodeRect(_node,false));
    case NodeRoot :
        path.addRect(classicNodeRect(_node,true,Qt::AnchorTop));
        break;
    case NodeHE :
        path.addRect(HERect(dynamic_cast<EIHEN_HENode*>(_node)));
        break;
    }

    return path;
}

void EIHENGraphicNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    double factor = 0.2;
    painter->setPen(QPen(Qt::black, 0.5));
    painter->setBrush(Qt::darkGray);

    qreal textWidth = 160;
    qreal textHeight = 15;


    EIHEN_HENode* curNode;
    QRectF rect;

    switch(_node->nodeType())
    {

    case NodeGeneric :
        painter->drawRect(-5,-15,10,30);
        break;
    case NodeSplitter :
    case NodeMixer :
        break;
    case NodeEnd :
        painter->drawRect(classicNodeRect(_node,false));
        break;
    case NodeRoot :
        rect = classicNodeRect(_node,false);
        painter->drawRect(rect);
        painter->drawText(-textWidth/2,-textHeight+rect.top(),textWidth,textHeight,Qt::AlignHCenter |Qt::AlignVCenter,_node->_stream);
        break;
    case NodeHE :
        curNode = dynamic_cast<EIHEN_HENode*>(_node);
        if(curNode)
        {
            qreal textWidth = 160;
            qreal textHeight=15;
            //qreal width = std::abs(curNode->_outletT.value(METemperature::K)-curNode->_inletT.value(METemperature::K))*_TXfactor;
            qreal width = 30;
            qreal height = 30;

            painter->drawEllipse(-width/2,-height/2,width,height);
            painter->drawText(-textWidth/2,-height/2-textHeight,textWidth,textHeight,Qt::AlignCenter |Qt::AlignTop,curNode->_qFlow.toString());
        }
        break;
    }


}

QVariant EIHENGraphicNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (EIHENGraphicEdge *edge, _edges)
            edge->adjust();
        //graph->itemMoved();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void EIHENGraphicNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void EIHENGraphicNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

QRectF EIHENGraphicNode::HERect(EIHEN_HENode* node)  const
{
    if(!node)
        return QRectF();

    qreal textWidth = 160;
    qreal textHeight=15;

    //DX = DT * TXfactor
    //qreal width = std::abs(node->_outletT.value(METemperature::K)-node->_inletT.value(METemperature::K))*_TXfactor;
    //width=std::max(minTextWidth,width);

    qreal width = 30;
    qreal height = 30;


    return QRectF(-textWidth/2,-(height/2 + textHeight),textWidth,height+textHeight);
}

QPainterPath EIHENGraphicNode::HEPainter(EIHEN_HENode* node)  const
{
    QPainterPath path;
    if(!node)
        return path;

    qreal textWidth = 160;
    qreal textHeight=15;

    qreal width = 30;
    qreal height = 30;


    path.addEllipse(-width/2,-height/2,width,height);
    path.addRect(-textWidth/2,-height/2-textHeight,textWidth,textHeight);

    return path;
}


QRectF EIHENGraphicNode::classicNodeRect(EIHEN_Node* node,bool withText,Qt::AnchorPoint textPos) const
{
    if(!node)
        return QRectF();

    //DX = DT * TXfactor
    qreal width = 12;
    qreal height = 12;
    qreal top=-6;
    qreal left =-6;

    if(withText)
    {
        qreal textWidth = 160;
        qreal textHeight = 15;
        switch(textPos)
        {
            case Qt::AnchorLeft :
                top = -std::max(std::abs(top),textHeight/2);
                left = left - textWidth;
                height = std::max(height,textHeight);
                width = width + textWidth;
                break;
            case Qt::AnchorTop :
                top = top - textHeight;
                left = -std::max(std::abs(left),textWidth/2);
                height = height + textHeight;
                width = std::max(width,textWidth);
                break;
        case Qt::AnchorRight :
            top = -std::max(std::abs(top),textHeight/2);
            left = left;
            height = std::max(height,textHeight);
            width = width + textWidth;
            break;
        case Qt::AnchorBottom :
            top = top;
            left = -std::max(std::abs(left),textWidth/2);
            height = height + textHeight;
            width = std::max(width,textWidth);
            break;
        }
    }

    return QRectF(left,top,width,height);
}

QPainterPath EIHENGraphicNode::classicNodePainter(EIHEN_Node* node,bool withText,Qt::AnchorPoint textPos)  const
{
    QPainterPath path;
    if(!node)
        return path;


    path.addRect(classicNodeRect(node,withText,textPos));


    return path;

}

