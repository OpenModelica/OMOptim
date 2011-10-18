#include "EIHEN.h"

namespace EI
{
EIHEN::EIHEN()
{
    // relative tolerance
    _relMassFlowError=0.0001;
    _relQFlowError=0.001;
    _absTempError.setValue(0.001,METemperature::K);

}

EIHEN::~EIHEN()
{
    for(int i=0;i<_allNodes.size();i++)
        delete _allNodes.at(i);

}


EIHEN & EIHEN::operator=(const EIHEN& b)
{


    // copying nodes and map
    QMap<EIHEN_Node*,EIHEN_Node*> mapCorres; //<bNodePtr,thisNodePtr>
    EIHEN_Node* curNewNode;
    EIHEN_RootNode* curRootNode;
    for(int i=0;i<b._allNodes.size();i++)
    {
        curNewNode = b._allNodes.at(i)->clone();

        mapCorres.insert(b._allNodes.at(i),curNewNode);

        _allNodes.push_back(curNewNode);

        curRootNode = dynamic_cast<EIHEN_RootNode*>(b._allNodes.at(i));

        if(curRootNode && b._mapRoots.values().contains(curRootNode))
        {
            _mapRoots.insert(b._mapRoots.key(curRootNode),dynamic_cast<EIHEN_RootNode*>(curNewNode));
        }
    }

    // update _inlets and outlets
    for(int i=0;i<_allNodes.size();i++)
    {
        curNewNode = _allNodes.at(i);

        for(int j=0;j<curNewNode->_inlets.size();j++)
            curNewNode->_inlets.replace(j,mapCorres.value(curNewNode->_inlets.at(j)));

        for(int j=0;j<curNewNode->_outlets.size();j++)
            curNewNode->_outlets.replace(j,mapCorres.value(curNewNode->_outlets.at(j)));
    }

    //copying hes
    EIHEN_HE* newHE;
    for(int i=0;i<b._hes.size();i++)
    {
        newHE = new EIHEN_HE(*b._hes.at(i));
        //update nodes
        newHE->setNodeA(mapCorres.value(newHE->nodeA()));
        newHE->setNodeB(mapCorres.value(newHE->nodeB()));
        _hes.push_back(newHE);
    }
}


/**
  * Tries to create a HEN from a set of EIConnection.
  * While EIConnections only focus on heat transfer, HEN also consider masstransfers.
  * That's why all EIConns can not lead to a coherent HEN.
  * Returns false if HEN is not coherent, true otherwise.
  */


bool EIHEN::setData( EITree & eiTree, const EIConns & eiConns)
{
    bool ok;
    QString shortStreamName;

    EIStream* curStream;
    bool isHot;
    EIHEN_HENode* henode;
    MEMassFlow curStreamMFlow;
    EIGroup* curGroup;
    EIGroupFact* curGroupFact;
    QList<EIHEN_Node*> pendingNodes;

    bool alreadyDone;
    //copying eiTree


    QStringList allStreams = eiConns.allStreams();


    // creating nodes for each stream
    for(int i=0;i<allStreams.size();i++)
    {
        curStream = dynamic_cast<EIStream*>(eiTree.findItem(allStreams.at(i)));
        pendingNodes.clear();

        if(curStream)
        {
            isHot = curStream->isHot();

            // get temperatures concerning this stream and sort
            QList<METemperature> allT = eiConns.TList(curStream->name());
            Q_ASSERT(allT.size()>0);
            if(allT.size()==0)
                return false;

            if(isHot)
                qSort(allT.begin(),allT.end(),METemperature::ThoterThan); //T(0) > T(1) > T(2)...
            else
                qSort(allT.begin(),allT.end(),METemperature::TcolderThan); //T(0) < T(1) < T(2)...

            //creating root node
            EIHEN_RootNode* rootNode = new EIHEN_RootNode(curStream->name());
            _allNodes.push_back(rootNode);
            _mapRoots.insert(curStream->name(),rootNode);
            rootNode->_outletT = allT.at(0);
            rootNode->_inletT = allT.at(0);

            // get and ste mass flow
            if(EIReader::getFirstParentGroupFact(curStream,curGroupFact,curGroup))
                curStreamMFlow = curStream->massFlowNum()*curGroupFact->value;
            else
                curStreamMFlow = curStream->massFlowNum();

            rootNode->setMassFlow(curStreamMFlow);

            pendingNodes.push_back(rootNode);

            for(int iT=0;iT<allT.size();iT++)
            {
                QList<EIConn> comingConns = eiConns.filterTin(curStream->name(),allT.at(iT));

                //if next temperatures are really near current one (i.e. difference under _tempError)
                //consider next temperature as equivalent as this one (append corresponding eiconns)

                while((iT+1<allT.size())&&(allT.at(iT+1).equalsAbs(allT.at(iT),_absTempError)))
                {
                    //append eiconns
                    comingConns.append(eiConns.filterTin(curStream->name(),allT.at(iT+1)));

                    //jump current interval
                    iT++;
                }

                // a boolean : do one case only (avoid many "else")
                alreadyDone = false;

                // one pending-one coming
                if(!alreadyDone && (pendingNodes.size()==1)&&(comingConns.size()==1))
                {


                    alreadyDone = true;

                    // create new node
                    henode = new EIHEN_HENode(curStream->name());
                    _allNodes.push_back(henode);

                    //set information
                    henode->setMassFlow(comingConns.at(0).massFlow(curStream->name()));
                    henode->_inletT = comingConns.at(0).Tin(curStream->name());
                    henode->_outletT = comingConns.at(0).Tout(curStream->name());
                    henode->_qFlow = comingConns.at(0).qFlow();

                    Q_ASSERT( henode->_inletT.value(METemperature::K)>0);
                    Q_ASSERT( henode->_outletT.value(METemperature::K)>0);

                    // connect it
                    connect(pendingNodes.at(0),henode);

                    // update pendingList
                    pendingNodes.clear();
                    pendingNodes.push_back(henode);
                }

                //if one pending-several coming
                if(!alreadyDone && (pendingNodes.size()==1)&&(comingConns.size()>1))
                {
                    alreadyDone = true;

                    // create a splitter
                    EIHEN_Splitter* hesplitter = new EIHEN_Splitter(curStream->name());
                    _allNodes.push_back(hesplitter);

                    //set information
                    hesplitter->_inletT = comingConns.at(0).Tin(curStream->name());
                    hesplitter->_outletT = hesplitter->_inletT;

                    Q_ASSERT( hesplitter->_inletT.value(METemperature::K)>0);
                    Q_ASSERT( hesplitter->_outletT.value(METemperature::K)>0);

                    // connect it
                    connect(pendingNodes.at(0),hesplitter);

                    // clear pendingNodes
                    pendingNodes.clear();

                    // create henodes
                    for(int iHe=0;iHe<comingConns.size();iHe++)
                    {
                        henode = new EIHEN_HENode(curStream->name());
                        _allNodes.push_back(henode);

                        //set information
                        henode->setMassFlow(comingConns.at(iHe).massFlow(curStream->name()));
                        henode->_inletT = comingConns.at(iHe).Tin(curStream->name());
                        henode->_outletT = comingConns.at(iHe).Tout(curStream->name());
                        henode->_qFlow = comingConns.at(iHe).qFlow();


                        Q_ASSERT( henode->_inletT.value(METemperature::K)>0);
                        Q_ASSERT( henode->_outletT.value(METemperature::K)>0);

                        //connect them
                        connect(hesplitter,henode);

                        // update pendingList
                        pendingNodes.push_back(henode);
                    }
                }

                //if several pending - one coming
                if(!alreadyDone && (pendingNodes.size()>1)&&(comingConns.size()==1))
                {
                    alreadyDone = true;

                    // create a mixer (will be deleted if not needed or added otherwise)
                    EIHEN_Mixer* hemixer = new EIHEN_Mixer(curStream->name());
                    hemixer->_inletT = comingConns.at(0).Tin(curStream->name());
                    hemixer->_outletT = hemixer->_inletT;

                    Q_ASSERT( hemixer->_inletT.value(METemperature::K)>0);
                    Q_ASSERT( hemixer->_outletT.value(METemperature::K)>0);

                    // create henode
                    henode = new EIHEN_HENode(curStream->name());
                    _allNodes.push_back(henode);

                    //set information
                    henode->setMassFlow(comingConns.at(0).massFlow(curStream->name()));
                    henode->_inletT = comingConns.at(0).Tin(curStream->name());
                    henode->_outletT = comingConns.at(0).Tout(curStream->name());
                    henode->_qFlow = comingConns.at(0).qFlow();


                    Q_ASSERT( henode->_inletT.value(METemperature::K)>0);
                    Q_ASSERT( henode->_outletT.value(METemperature::K)>0);


                    // filter pendingNodes concerned (Tout(pendingNode)==Tin(comingNode))
                    QList<EIHEN_Node*> subPendingNodes;

                    for(int iPn=0;iPn<pendingNodes.size();iPn++)
                    {
                        if(pendingNodes.at(iPn)->_outletT.equalsAbs(comingConns.at(0).Tin(curStream->name()),_absTempError))
                        {
                            //connect concerned
                            connect(pendingNodes.at(iPn),hemixer);
                        }
                    }

                    //if no one connected, ERROR
                    if(hemixer->_inlets.size()==0)
                    {
                        //ERROR
                        infoSender.send(Info("HEN Inconsistency",ListInfo::ERROR2));
                        return false;
                    }
                    // if only one connected, remove mixer and connect it directly
                    if(hemixer->_inlets.size()==1)
                    {
                        EIHEN_Node* from = hemixer->_inlets.at(0);
                        disconnect(from,hemixer);
                        connect(from,henode);
                        pendingNodes.removeAll(from);
                        pendingNodes.push_back(henode);
                        delete hemixer;
                    }
                    else
                    {
                        _allNodes.push_back(hemixer);

                        // if several connected, connect mixer to coming node
                        connect(hemixer,henode);

                        //update pending nodes
                        for(int iInlet=0;iInlet<hemixer->_inlets.size();iInlet++)
                            pendingNodes.removeAll(hemixer->_inlets.at(iInlet));
                        pendingNodes.push_back(henode);
                    }
                }

                //if several pending - several coming
                //most complicated, hold on !!
                if(!alreadyDone && (pendingNodes.size()>1)&&(comingConns.size()>1))
                {
                    alreadyDone = true;

                    // for the moment, we don't manage multi-stage spliting (spliting of a splited branch)
                    if(pendingNodes.size()!=comingConns.size())
                    {
                        infoSender.send(Info("HEN Inconsistency",ListInfo::ERROR2));
                        return false;
                    }

                    bool correspOk=true;
                    bool found;

                    int iC;
                    MEMassFlow curPMassFlow;
                    while(correspOk && pendingNodes.size())
                    {
                        curPMassFlow = pendingNodes.at(0)->massFlow();
                        found = false;
                        iC=0;
                        while(!found && (iC<comingConns.size()))
                        {
                            //if corresponds
                            if(curPMassFlow.equalsRel(comingConns.at(iC).massFlow(curStream->name()),_relMassFlowError))
                            {
                                found = true;
                                //create henode
                                henode = new EIHEN_HENode(curStream->name());
                                henode->setMassFlow(comingConns.at(iC).massFlow(curStream->name()));
                                henode->_inletT = comingConns.at(iC).Tin(curStream->name());
                                henode->_outletT = comingConns.at(iC).Tout(curStream->name());
                                henode->_qFlow = comingConns.at(iC).qFlow();

                                _allNodes.push_back(henode);


                                //connect
                                connect(pendingNodes.at(0),henode);

                                //update
                                pendingNodes.removeAt(0);
                                comingConns.removeAt(iC);
                                pendingNodes.push_back(henode);
                            }
                            else
                                iC++;
                        }

                        //if not found, there should be a splitter between pending and comming
                        if(!found)
                        {
                            // create a mixer
                            EIHEN_Mixer* hemixer = new EIHEN_Mixer(curStream->name());
                            hemixer->_inletT = comingConns.at(0).Tin(curStream->name());
                            hemixer->_outletT = hemixer->_inletT;
                            _allNodes.push_back(hemixer);

                            // connect inlets
                            while(pendingNodes.size()>0)
                            {
                                connect(pendingNodes.at(0),hemixer);

                                //update
                                pendingNodes.removeAt(0);
                            }

                            // create a splitter
                            EIHEN_Splitter* hesplitter = new EIHEN_Splitter(curStream->name());
                            hesplitter->_inletT = hemixer->_inletT;
                            hesplitter->_outletT = hemixer->_inletT;
                            connect(hemixer,hesplitter);
                            _allNodes.push_back(hesplitter);


                            // connect outlets
                            while(comingConns.size()>0)
                            {
                                // create henode
                                henode = new EIHEN_HENode(curStream->name());
                                _allNodes.push_back(henode);

                                //set information
                                henode->setMassFlow(comingConns.at(0).massFlow(curStream->name()));
                                henode->_inletT = comingConns.at(0).Tin(curStream->name());
                                henode->_outletT = comingConns.at(0).Tout(curStream->name());
                                henode->_qFlow = comingConns.at(0).qFlow();

                                connect(hesplitter,henode);

                                comingConns.removeAt(0);

                                //update
                                pendingNodes.push_back(henode);
                            }
                        }
                        correspOk = found;
                    }
                }

                //if several or one pending - no coming : that is the end !!
                if(!alreadyDone && (pendingNodes.size()>0)&&(comingConns.size()==0))
                {
                    alreadyDone = true;
                    //create a end node
                    EIHEN_EndNode* endnode = new EIHEN_EndNode(curStream->name());
                    endnode->_inletT=pendingNodes.at(0)->_outletT;
                    endnode->_outletT=endnode->_inletT;
                    _allNodes.push_back(endnode);

                    if(pendingNodes.size()>1)
                    {
                        // create mixer
                        // create a mixer (will be deleted if not needed or added otherwise)
                        EIHEN_Mixer* hemixer = new EIHEN_Mixer(curStream->name());
                        hemixer->_inletT = pendingNodes.at(0)->_outletT;
                        hemixer->_outletT = hemixer->_inletT;
                        _allNodes.push_back(hemixer);

                    for(int iP=0;iP<pendingNodes.size();iP++)
                    {
                            connect(pendingNodes.at(iP),hemixer);
                    }
                        connect(hemixer,endnode);
                    }
                    else
                    {
                        connect(pendingNodes.at(0),endnode);
                    }

                    //update pending nodes
                    pendingNodes.clear();
                }
            }
        }
    }

    // adding HE
    setHE(eiConns);
}

void EIHEN::disconnect(EIHEN_Node* from,EIHEN_Node *to)
{
    from->_outlets.removeAll(to);
    to->_inlets.removeAll(from);
}

void EIHEN::connect(EIHEN_Node* from,EIHEN_Node *to)
{
    from->addOutlet(to);
    to->addInlet(from);
}

bool EIHEN::isValid(QString & msg) const
{
    bool result = true;
    QString subMsg;

    EIHEN_HENode* curHENode;
    EIHEN_HENode* curHENode2;

    for(int i=0;i<_allNodes.size();i++)
    {
        subMsg.clear();
        result = result && _allNodes.at(i)->isValid(subMsg);
        msg += subMsg;

        curHENode = dynamic_cast<EIHEN_HENode*>(_allNodes.at(i));

        if(curHENode)
        {
            // check if heat exchanger exists
            int iHE=0;
            bool found=false;
            while(!found && (iHE<hes().size()))
            {
                if(_hes.at(iHE)->nodeA()==curHENode)
                {
                    found = true;
                    curHENode2 = dynamic_cast<EIHEN_HENode*>(_hes.at(iHE)->nodeB());
    }
                else if(_hes.at(iHE)->nodeB()==curHENode)
                {
                    found = true;
                    curHENode2 =dynamic_cast<EIHEN_HENode*>( _hes.at(iHE)->nodeA());
                }
                if(!found)
                    iHE++;
            }
            if(!found)
            {
                result = false;
                msg += "Inconsistent : heat exchanger is absent (stream :"+curHENode->stream()+", qflow: "+curHENode->_qFlow.toString()+")\n";
            }
            else
            {
                //check qflows correspond
                if(!curHENode2->_qFlow.equalsRel(curHENode->_qFlow,_relMassFlowError))
                {
                    result = false;
                    msg += "Inconsistent : heat exchanger is incoherent (streams :"+curHENode->stream()+","+curHENode2->stream();
                    msg+=" ; qflows : "+curHENode->_qFlow.toString()+","+curHENode2->_qFlow.toString()+")\n";
                }
            }
        }
    }
    return result;
}

/** From EIConns, create all heat exchangers. Need to be called after all nodes have been created
   *
   */

void EIHEN::setHE(const EIConns & eiConns)
{

    EIConn* curConn;
    EIHEN_HENode* aNode;
    EIHEN_HENode* bNode;

    for(int iC=0;iC<eiConns.items.size();iC++)
    {
        curConn = eiConns.items.at(iC);

        // find A
        aNode = findHENode(curConn->aName(),curConn->TinA(),curConn->ToutA(),curConn->qFlow());

        // find B
        bNode = findHENode(curConn->bName(),curConn->TinB(),curConn->ToutB(),curConn->qFlow());

        if(aNode && bNode)
        {
            EIHEN_HE* newHE = new EIHEN_HE(aNode,bNode);
            newHE->setQFlow(curConn->qFlow());
            _hes.push_back(newHE);
        }
    }
}

EIHEN_HENode* EIHEN::findHENode(const QString &stream,const METemperature &Tin, const METemperature &Tout, const MEQflow &qflow)
{
    bool found=false;
    int i=0;
    EIHEN_HENode* curHeNode;

    while(!found && (i<_allNodes.size()))
    {
        curHeNode = dynamic_cast<EIHEN_HENode*>(_allNodes.at(i));
        if(curHeNode)
        {
            found = curHeNode->stream()==stream;
            found = found && curHeNode->_inletT.equalsAbs(Tin,_absTempError);
            found = found && curHeNode->_outletT.equalsAbs(Tout,_absTempError);
            found = found && curHeNode->_qFlow.equalsRel(qflow,_relQFlowError);
        }
        i++;
    }

    if(found)
        return curHeNode;
    else
        return NULL;
}

METemperature EIHEN::Tmin()
{
    METemperature tmin;
    for(int i=0;i<_allNodes.size();i++)
    {
        if(i==0)
            tmin = _allNodes.at(i)->_inletT;

        tmin = std::min(tmin, _allNodes.at(i)->_inletT);
        tmin = std::min(tmin, _allNodes.at(i)->_outletT);
    }
    return tmin;
}

METemperature EIHEN::Tmax()
{
    METemperature tmax;
    for(int i=0;i<_allNodes.size();i++)
    {
        if(i==0)
            tmax = _allNodes.at(i)->_inletT;

        tmax = std::max(tmax, _allNodes.at(i)->_inletT);
        tmax = std::max(tmax, _allNodes.at(i)->_outletT);
    }
    return tmax;
}
}

