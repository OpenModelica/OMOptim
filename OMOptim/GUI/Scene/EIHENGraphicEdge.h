#ifndef EIHENGRAP_H
#define EIHENGRAP_H

#include <QGraphicsItem>

class EIHENGraphicNode;

class EIHENGraphicEdge : public QGraphicsWidget
{
public:
     EIHENGraphicEdge(EIHENGraphicNode *sourceNode, EIHENGraphicNode *destNode);

     EIHENGraphicNode *sourceNode() const;
     EIHENGraphicNode *destNode() const;

     void adjust();

 protected:
     QRectF boundingRect() const;
     virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


     EIHENGraphicNode *_source, *_dest;

     QPointF _sourcePoint;
     QPointF _middlePoint1;
     QPointF _middlePoint2;
     QPointF _destPoint;
     qreal _arrowSize;

};


class EIHENGraphicQEdge : public EIHENGraphicEdge
{
    public :

     EIHENGraphicQEdge(EIHENGraphicNode *a, EIHENGraphicNode *b);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // EIHENGRAP_H
