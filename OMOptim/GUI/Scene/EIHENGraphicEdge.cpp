#include "EIHENGraphicEdge.h"

#include <QPainter>

#include "EIHENGraphicNode.h"

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

EIHENGraphicEdge::EIHENGraphicEdge(EIHENGraphicNode *sourceNode, EIHENGraphicNode *destNode)
    : _arrowSize(0)
{
    setAcceptedMouseButtons(0);
    _source = sourceNode;
    _dest = destNode;
    _source->addEdge(this);
    _dest->addEdge(this);
    adjust();
}

EIHENGraphicNode *EIHENGraphicEdge::sourceNode() const
{
    return _source;
}

EIHENGraphicNode *EIHENGraphicEdge::destNode() const
{
    return _dest;
}

void EIHENGraphicEdge::adjust()
{
    if (!_source || !_dest)
        return;

    QLineF line(mapFromItem(_source, 0, 0), mapFromItem(_dest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    //    if (length > qreal(20.)) {
    //        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
    //        _sourcePoint = line.p1() + edgeOffset;
    //        _destPoint = line.p2() - edgeOffset;
    //    } else {
    //        _sourcePoint = _destPoint = line.p1();
    //    }
    _sourcePoint = line.p1();
    _destPoint = line.p2();

    if(std::isnan(_sourcePoint.x()))
        _sourcePoint.setX(0);
    if(std::isnan(_destPoint.x()))
        _destPoint.setX(0);


    switch(_source->node()->nodeType())
    {
    case NodeSplitter :
    case NodeHE :
    case NodeRoot :
    case NodeEnd :
    case NodeMixer :
    case NodeGeneric :
    default :
        _middlePoint1.setX((_sourcePoint.x()+_destPoint.x())/2);
        _middlePoint1.setY(_sourcePoint.y());
        _middlePoint2.setX((_sourcePoint.x()+_destPoint.x())/2);
        _middlePoint2.setY(_destPoint.y());
        break;
    }

}

QRectF EIHENGraphicEdge::boundingRect() const
{
    if (!_source || !_dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + _arrowSize) / 2.0;

    return QRectF(_sourcePoint, QSizeF(_destPoint.x() - _sourcePoint.x(),
                                       _destPoint.y() - _sourcePoint.y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}

void EIHENGraphicEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!_source || !_dest)
        return;

    QLineF line1(_sourcePoint, _middlePoint1);
    QLineF line2(_middlePoint1, _middlePoint2);
    QLineF line3(_middlePoint2, _destPoint);

    if (qFuzzyCompare(line1.length()+line2.length()+line3.length(), qreal(0.)))
        return;

    // Draw the line itself
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    painter->drawLine(line1);
    painter->drawLine(line2);
    painter->drawLine(line3);


}

 EIHENGraphicQEdge::EIHENGraphicQEdge(EIHENGraphicNode *a, EIHENGraphicNode *b)
     :EIHENGraphicEdge(a,b)
 {
 }


void EIHENGraphicQEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!_source || !_dest)
        return;

    QLineF line1(_sourcePoint, _destPoint);


    if (qFuzzyCompare(line1.length(), qreal(0.)))
        return;

    // Draw the line itself
    painter->setPen(QPen(Qt::darkRed, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line1);

//    // Draw the arrows
//    if(_arrowSize>0)
//    {
//        double angle = ::acos(line.dx() / line.length());
//        if (line.dy() >= 0)
//            angle = TwoPi - angle;

//        QPointF sourceArrowP1 = _sourcePoint + QPointF(sin(angle + Pi / 3) * _arrowSize,
//                                                       cos(angle + Pi / 3) * _arrowSize);
//        QPointF sourceArrowP2 = _sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * _arrowSize,
//                                                       cos(angle + Pi - Pi / 3) * _arrowSize);
//        QPointF destArrowP1 = _destPoint + QPointF(sin(angle - Pi / 3) * _arrowSize,
//                                                   cos(angle - Pi / 3) * _arrowSize);
//        QPointF destArrowP2 = _destPoint + QPointF(sin(angle - Pi + Pi / 3) * _arrowSize,
//                                                   cos(angle - Pi + Pi / 3) * _arrowSize);

//        painter->setBrush(Qt::black);
//        painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
//        painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
//    }

}
