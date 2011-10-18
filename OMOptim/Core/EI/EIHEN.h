#ifndef EIHEN_H
#define EIHEN_H


#include "EITree.h"
#include "EIConns.h"
#include "EIHENNodes.h"

#include <QtCore/QList>

namespace EI
{


class EIHEN
{

public :
    EIHEN();
    virtual ~EIHEN();
    //EIHEN(const EIHEN&);
    EIHEN& operator=(const EIHEN&);


    bool setData(EITree & , const EIConns &);
    bool isValid(QString &msg) const;

    QList<EIHEN_Node*> allNodes()const {return _allNodes;}
    QList<EIHEN_HE*> hes()const {return _hes;}
    QMap<QString,EIHEN_RootNode*> mapRoots()const {return _mapRoots;}

    METemperature Tmin();
    METemperature Tmax();

private :

    void disconnect(EIHEN_Node* from,EIHEN_Node *to);
    void connect(EIHEN_Node* from,EIHEN_Node *to);
    void setHE(const EIConns &); // from eiconns, create all heat exchangers
    EIHEN_HENode* findHENode(const QString &stream,const METemperature &Tin, const METemperature &Tout, const MEQflow &qflow);

    QList<EIHEN_Node*> _allNodes;
    QList<EIHEN_HE*> _hes; // heat exchangers


    QMap<QString,EIHEN_RootNode*> _mapRoots;

    double _relMassFlowError; /** relative massflow error accepted in junctions (since MILP results have a limited precision)*/
    double _relQFlowError; /** relative qflow error accepted in junctions (since MILP results have a limited precision)*/
    METemperature _absTempError; /** absolute temperature error accepted in junctions*/


};

}

#endif // EIHEN_H
