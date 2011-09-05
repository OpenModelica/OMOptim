#ifndef EIHENNODES_H
#define EIHENNODES_H

#include <QtCore/QList>
#include "METemperature.h"
#include "MEQflow.h"
#include "MEMassFlow.h"
#include "MEQflow.h"
#include "MESurface.h"


namespace EI
{


enum EIHEN_NodeType
{
    NodeHE,
    NodeRoot,
    NodeEnd,
    NodeSplitter,
    NodeMixer,
    NodeGeneric
};

class EIHEN_Node
{

public :
    EIHEN_Node(QString stream):_stream(stream){};
    virtual ~EIHEN_Node(){};

    virtual bool isValid(QString & msg)const=0;
    virtual EIHEN_NodeType nodeType(){return NodeGeneric;}
    virtual EIHEN_Node* clone() const =0;
    virtual MEMassFlow massFlow() const =0;

    void addInlet(EIHEN_Node* node){_inlets.push_back(node);}
    void addOutlet(EIHEN_Node* node){_outlets.push_back(node);}

    QString stream()const {return _stream;}


    QList<EIHEN_Node*> _inlets;
    QList<EIHEN_Node*> _outlets;

    METemperature _inletT;
    METemperature _outletT;
    QString _stream;
};


class EIHEN_RootNode : public EIHEN_Node
{
public :
    EIHEN_RootNode(QString stream):EIHEN_Node(stream){};
    ~ EIHEN_RootNode(){};

    virtual EIHEN_Node* clone() const;

    virtual EIHEN_NodeType nodeType(){return NodeRoot;}

    bool isValid(QString &msg) const;

    MEMassFlow massFlow() const;
    void setMassFlow(const MEMassFlow &);

protected :
    MEMassFlow _massFlow;
};

class EIHEN_EndNode : public EIHEN_Node
{
public :

    EIHEN_EndNode(QString stream):EIHEN_Node(stream){};
    ~ EIHEN_EndNode(){};

    EIHEN_Node* clone() const;

    EIHEN_NodeType nodeType(){return NodeEnd;}

    bool isValid(QString &msg)const;

    MEMassFlow massFlow() const;
};

class EIHEN_HENode: public EIHEN_Node
{
public :
    EIHEN_HENode(QString stream):EIHEN_Node(stream){};
    ~ EIHEN_HENode(){};

    EIHEN_Node* clone() const;
    EIHEN_NodeType nodeType(){return NodeHE;}

    bool isValid(QString &msg)const;
    MEMassFlow massFlow() const;
    void setMassFlow(const MEMassFlow &);

    MEQflow _qFlow;
protected:
    MEMassFlow _massFlow;
};


class EIHEN_Splitter : public EIHEN_Node
{
public :
    EIHEN_Splitter(QString stream):EIHEN_Node(stream){};
    ~ EIHEN_Splitter(){};

    EIHEN_Node* clone() const;
    EIHEN_NodeType nodeType(){return NodeSplitter;}

    bool isValid(QString &msg)const;
    MEMassFlow massFlow() const;

};

class EIHEN_Mixer : public EIHEN_Node
{
public:
    EIHEN_Mixer(QString stream):EIHEN_Node(stream){};
    ~ EIHEN_Mixer(){};

    EIHEN_NodeType nodeType(){return NodeMixer;}

    EIHEN_Node* clone() const;

    bool isValid(QString &msg)const;
    MEMassFlow massFlow() const;

};

class EIHEN_HE
{
public :
    EIHEN_HE(EIHEN_Node* nodeA, EIHEN_Node* nodeB);
    EIHEN_HE(const EIHEN_HE &);
    void setQFlow(const MEQflow &);
    void setA(const MESurface &);

    void setNodeA(EIHEN_Node* nodeA){_nodeA = nodeA;}
    void setNodeB(EIHEN_Node* nodeB){_nodeB = nodeB;}
    EIHEN_Node* nodeA()const{return _nodeA;}
    EIHEN_Node* nodeB()const{return _nodeB;}

private :

    EIHEN_Node* _nodeA;
    EIHEN_Node* _nodeB;
    MESurface _A;
    MEQflow _qflow;


};

}


#endif // EIHENNODES_H
