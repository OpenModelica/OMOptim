#include "EIHENNodes.h"

using namespace EI;



EIHEN_Node* EIHEN_RootNode::clone() const
{
    EIHEN_RootNode* newNode = new EIHEN_RootNode(_stream);
    newNode->_inlets = this->_inlets;
    newNode->_outlets = this->_outlets;
    newNode->_inletT = this->_inletT;
    newNode->_outletT = this->_outletT;
    newNode->_massFlow = this->_massFlow;
    newNode->_stream = this->_stream;

    return newNode;
}

MEMassFlow EIHEN_RootNode::massFlow() const
{
    return _massFlow;
}

void EIHEN_RootNode::setMassFlow(const MEMassFlow &massFlow)
{
    _massFlow = massFlow;
}


bool EIHEN_RootNode::isValid(QString &msg) const
{
    if(!_inlets.isEmpty())
    {
        msg += _stream + " root has inlets but should not have.";
        msg+="\n";
        return false;
    }
    else
        return true;
}


EIHEN_Node* EIHEN_EndNode::clone() const
{
    EIHEN_EndNode* newNode = new EIHEN_EndNode(_stream);
    newNode->_inlets = this->_inlets;
    newNode->_outlets = this->_outlets;
    newNode->_inletT = this->_inletT;
    newNode->_outletT = this->_outletT;
    newNode->_stream = this->_stream;

    return newNode;
}

bool EIHEN_EndNode::isValid(QString &msg) const
{
    if(!_outlets.isEmpty())
    {
        msg += _stream + " end has outlets but should not have.";
        msg+="\n";
        return false;
    }
    else
        return true;
}

MEMassFlow EIHEN_EndNode::massFlow() const
{
    MEMassFlow totalFlow;
    for(int i=0;i<_inlets.size();i++)
        totalFlow+=_inlets.at(i)->massFlow();

    return totalFlow;
}


EIHEN_Node* EIHEN_HENode::clone() const
{
    EIHEN_HENode* newNode = new EIHEN_HENode(_stream);
    newNode->_inlets = this->_inlets;
    newNode->_outlets = this->_outlets;
    newNode->_inletT = this->_inletT;
    newNode->_outletT = this->_outletT;
    newNode->_massFlow = this->_massFlow;
    newNode->_stream = this->_stream;
    newNode->_qFlow = this->_qFlow;

    return newNode;
}

bool EIHEN_HENode::isValid(QString & msg) const
{
    return true;
}

MEMassFlow EIHEN_HENode::massFlow() const
{
    return _massFlow;
}

void EIHEN_HENode::setMassFlow(const MEMassFlow &massFlow)
{
    _massFlow = massFlow;
}

EIHEN_Node* EIHEN_Splitter::clone() const
{
    EIHEN_Splitter* newNode = new EIHEN_Splitter(_stream);
    newNode->_inlets = this->_inlets;
    newNode->_outlets = this->_outlets;
    newNode->_inletT = this->_inletT;
    newNode->_outletT = this->_outletT;
    newNode->_stream = this->_stream;

    return newNode;
}

bool EIHEN_Splitter::isValid(QString &msg) const
{
    bool ok = true;

    double massTolerance = 0.01; // tolerance 1%
    double tempTolerance = 0.01; // tolerance 1%


    if(_inlets.size()!=1)
    {
        msg += _stream + " splitter has no or several inlets.";
        msg+="\n";
        ok = false;
    }

    for(int i=0;i<_outlets.size();i++)
    {
        if(!_inletT.equals(_outlets.at(i)->_inletT,tempTolerance))
        {
            msg += _stream + " splitter has different bound temperatures (";
            msg += _inletT.toString() + "!= " + _outlets.at(i)->_inletT.toString()+")";
            msg+="\n";
            ok = false;
        }
    }

    //check massFlow (with tolerance)

    MEMassFlow outFlow;
    for(int i=0;i<_outlets.size();i++)
        outFlow+=_outlets.at(i)->massFlow();

    if((_inlets.size()==0)||!_inlets.at(0)->massFlow().equals(outFlow,massTolerance))
    {
        msg+= _stream + "mass incoherency in splitter : ";
        msg+= _inlets.at(0)->massFlow().toString() +"!="+ outFlow.toString();
        msg+="\n";
        ok = false;
    }

    return ok;
}

MEMassFlow EIHEN_Splitter::massFlow() const
{
    MEMassFlow totalFlow;
    if(_inlets.size()==1)
        totalFlow = _inlets.at(0)->massFlow();

    return totalFlow;
}

EIHEN_Node* EIHEN_Mixer::clone() const
{
    EIHEN_Mixer* newNode = new EIHEN_Mixer(_stream);
    newNode->_inlets = this->_inlets;
    newNode->_outlets = this->_outlets;
    newNode->_inletT = this->_inletT;
    newNode->_outletT = this->_outletT;
    newNode->_stream = this->_stream;

    return newNode;
}

bool EIHEN_Mixer::isValid(QString & msg) const
{
    bool ok = true;

    double massTolerance = 0.01; // tolerance 1%
    double tempTolerance = 0.01; // tolerance 1%

    if(_outlets.size()!=1)
    {
        msg += _stream + " mixer has no or several outlets.";
        msg+="\n";
        ok = false;
    }

    for(int i=0;i<_inlets.size();i++)
    {
        if(!_outletT.equals(_inlets.at(i)->_outletT,tempTolerance))
        {
            msg += _stream + " mixer has different bound temperatures (";
            msg += _outletT.toString() + "!= " + _inlets.at(i)->_outletT.toString()+")";
            msg+="\n";
            ok = false;
        }
    }

    //check massFlow
    MEMassFlow inFlow;
    for(int i=0;i<_inlets.size();i++)
        inFlow+=_inlets.at(i)->massFlow();

    if((_outlets.size()==0)||!_outlets.at(0)->massFlow().equals(inFlow,massTolerance))
    {
        msg+= _stream + "mass incoherency in mixer : ";
        msg+= _outlets.at(0)->massFlow().toString() +"!="+ inFlow.toString();
        msg+="\n";
        ok = false;
    }

    return ok;

}

MEMassFlow EIHEN_Mixer::massFlow() const
{
    MEMassFlow totalFlow;
    for(int i=0;i<_inlets.size();i++)
        totalFlow+=_inlets.at(i)->massFlow();

    return totalFlow;
}

EIHEN_HE::EIHEN_HE(EIHEN_Node* nodeA, EIHEN_Node* nodeB)
{
    _nodeA = nodeA;
    _nodeB = nodeB;
    _qflow = 0;
    _A = 0;
}

EIHEN_HE::EIHEN_HE(const EIHEN_HE &b)
{
    _nodeA = b._nodeA;
    _nodeB = b._nodeB;
    _qflow = b._qflow;
    _A = b._A;
}

void EIHEN_HE::setQFlow(const MEQflow & qflow)
{
    _qflow = qflow;
}

void EIHEN_HE::setA(const MESurface & A)
{
    _A = A;
}
