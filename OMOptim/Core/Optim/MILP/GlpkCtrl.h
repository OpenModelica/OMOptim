#ifndef GLPKCTRL_H
#define GLPKCTRL_H


#include <glpk.h>
#include "GLPKTools.h"
#include "EIHEN1Parameters.h"

class GlpkCallBack
{
public :
    static void foo_bar(glp_tree *tree, void *info);
    static void setStop(bool stop){_stop = stop;}
    static bool stop(){return _stop;}
private :
    static bool _stop;
};



class GlpkCtrl
{
public:
    GlpkCtrl(glp_prob*,glp_tran*);

    void stop();
    static void errorHook(void *in);
    bool run(QString &msg);
private :
    glp_prob* _glpProblem;
    glp_tran* _glpTran;
};



#endif // GLPKCTRL_H
