#include "ModPlusCtrls.h"

#include "ModPlusOMCtrl.h"
#include "ModPlusDymolaCtrl.h"
#include "ModPlusOMExeCtrl.h"
#include "ModPlusDymolaExeCtrl.h"
#include "ModPlusBlackBoxExeCtrl.h"
#include "Project.h"

ModPlusCtrls:: ModPlusCtrls(Project* project,ModelPlus* modPlus,QObject* parent):
QObject(parent)
{
    _project = project;
    _modelPlus = modPlus;

    if(_modelPlus)
    {
        QList<ModPlusCtrl*> compatibleCtrls = getCompatibleCtrls(_project,_modelPlus);

        for(int i=0;i<compatibleCtrls.size();i++)
            this->insertCtrl(compatibleCtrls.at(i)->type(),compatibleCtrls.at(i));

        if(_modelPlus->modelType()==ModelPlus::MODELICA)
        {
#if DEFAULTSIMULATOR==0
            setCurrentCtrlType(ModPlusCtrl::OPENMODELICA);
#else
            setCurrentCtrlType(ModPlusCtrl::DYMOLA);
#endif
        }
        if(_modelPlus->modelType()==ModelPlus::EXECUTABLE)
        {
            setCurrentCtrlType(ModPlusCtrl::DYMOLAEXECUTABLE);
        }
    }
}

ModPlusCtrls::ModPlusCtrls(Project* project,ModelPlus* ModPlus,const QDomElement & cControlers,QObject* parent)
    :QObject(parent)
{
    _project = project;
    _modelPlus = ModPlus;


    if(_modelPlus)
    {
        QList<ModPlusCtrl*> compatibleCtrls = getCompatibleCtrls(_project,_modelPlus);

        for(int i=0;i<compatibleCtrls.size();i++)
            this->insertCtrl(compatibleCtrls.at(i)->type(),compatibleCtrls.at(i));

        if(_modelPlus->modelType()==ModelPlus::MODELICA)
        {
#if DEFAULTSIMULATOR==0
            setCurrentCtrlType(ModPlusCtrl::OPENMODELICA);
#else
            setCurrentCtrlType(ModPlusCtrl::DYMOLA);
#endif
        }
        if(_modelPlus->modelType()==ModelPlus::EXECUTABLE)
        {
            setCurrentCtrlType(ModPlusCtrl::DYMOLAEXECUTABLE);
        }
    }


    if(!cControlers.isNull() && cControlers.tagName()==className())
    {
        // Controler type
        this->setCurrentCtrlType((ModPlusCtrl::Type)cControlers.attribute("curType","").toInt());

        // Update controler parameters
        QDomNodeList domControlerList = cControlers.elementsByTagName("Controler");

        QDomElement cParams;
        QDomElement cControler;
        ModPlusCtrl* curCtrl;
        ModPlusCtrl::Type curCtrlType;
        for(int iC=0;iC<domControlerList.size();iC++)
        {
            cControler = domControlerList.at(iC).toElement();
            curCtrlType = (ModPlusCtrl::Type)cControler.attribute("type","-1").toInt();
            curCtrl = this->value(curCtrlType);

            if(curCtrl)
            {
                cParams = cControler.firstChildElement("parameters");
                if(!cParams.isNull())
                {
                    curCtrl->parameters()->update(cParams);
                }
            }
        }
    }
}


ModPlusCtrls::~ModPlusCtrls()
{
    for(int i=0;i<values().size();i++)
    {
        delete values().at(i);
    }
}

ModPlusCtrls* ModPlusCtrls::clone()
{
    ModPlusCtrls* cloned = new ModPlusCtrls(_project,_modelPlus,NULL);

    // first remove automatically created
    for(int i=0;i<cloned->values().size();i++)
    {
        delete cloned->values().at(i);
    }
    cloned->clear();

    // then add cloned
    for(int i=0;i<keys().size();i++)
    {
        cloned->insertCtrl(this->keys().at(i),this->value(keys().at(i))->clone());
    }

    cloned->_ctrlType = _ctrlType;

    return cloned;
}

ModPlusCtrl* ModPlusCtrls::getNewCtrl(ModPlusCtrl::Type type,Project* project,ModelPlus* modPlus)
{
    switch(type)
    {
    case ModPlusCtrl::OPENMODELICA:
        return new ModPlusOMCtrl(project,(ModModelPlus*)modPlus,project->moomc());
    case ModPlusCtrl::DYMOLA:
        return new ModPlusDymolaCtrl(project,(ModModelPlus*)modPlus,project->moomc());
    case ModPlusCtrl::OMEXECUTABLE:
        return new ModPlusOMExeCtrl(project, modPlus);
    case ModPlusCtrl::DYMOLAEXECUTABLE:
        return new ModPlusDymolaExeCtrl(project, modPlus);
        //    case ModPlusCtrl::BLACKBOXEXECUTABLE:
        //        return new ModPlusBlackBoxExeCtrl(project, modPlus);
    default:
        return NULL;
    }
}

QDomElement ModPlusCtrls::toXmlData(QDomDocument & doc)
{
    QDomElement cControlers = doc.createElement(className());
    cControlers.setAttribute("curType",(int)currentCtrlType());

    QDomElement ccurCtrl;
    QDomElement ccurParams;
    ModPlusCtrl* curCtrl;

    for(int iCtrl=0;iCtrl<this->values().size();iCtrl++)
    {
        curCtrl = this->values().at(iCtrl);
        ccurCtrl = doc.createElement("Controler");
        ccurCtrl.setAttribute("type",(int)curCtrl->type());
        ccurParams = curCtrl->parameters()->toXmlData(doc,"parameters");

        ccurCtrl.appendChild(ccurParams);
        cControlers.appendChild(ccurCtrl);
    }

    return cControlers;
}

ModPlusCtrl* ModPlusCtrls::currentCtrl() const
{
    return this->value(_ctrlType);
}

ModPlusCtrl::Type ModPlusCtrls::currentCtrlType() const
{
    return _ctrlType;
}

void ModPlusCtrls::setCurrentCtrlType(ModPlusCtrl::Type type)
{
    if(type != _ctrlType)
    {
        _ctrlType = type;
        emit modified();
    }
}

void ModPlusCtrls::setFromOtherCtrls(const ModPlusCtrls &newCtrls)
{
    // clear content
    for(int i=0;i<values().size();i++)
    {
        delete values().at(i);
    }
    this->clear();

    // add clones
    ModPlusCtrl::Type curType;
    ModPlusCtrl* newCtrl;
    for(int i=0;i<newCtrls.keys().size();i++)
    {
        curType = newCtrls.keys().at(i);
        newCtrl = newCtrls.value(curType)->clone();
        this->insertCtrl(curType,newCtrl);
    }
    this->_ctrlType = newCtrls._ctrlType;
}

QList<ModPlusCtrl *> ModPlusCtrls::getCompatibleCtrls(Project* project, ModelPlus* modelPlus)
{
    QList<ModPlusCtrl *> result;
    for(int i=0;i<ModPlusCtrl::nbTypes;i++)
    {
        ModPlusCtrl* newCtrl;
        newCtrl = getNewCtrl((ModPlusCtrl::Type)i,project,modelPlus);
        if(newCtrl && newCtrl->compatibleModels().contains(modelPlus->modelType()))
            result.push_back(newCtrl);
        else
            delete newCtrl;
    }
    result.removeAll(NULL);
    return result;
}

void ModPlusCtrls::insertCtrl(ModPlusCtrl::Type i,ModPlusCtrl* ctrl)
{
    this->insert(i,ctrl);
    connect(ctrl,SIGNAL(modified()),this,SIGNAL(modified()));
}

