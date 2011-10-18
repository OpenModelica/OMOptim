#include "GlpkCtrl.h"


GlpkCtrl::GlpkCtrl(glp_prob* glpProblem,glp_tran* glpTran)
{
    _glpProblem = glpProblem;
    _glpTran = glpTran;
    GlpkCallBack::setStop(false);
}

void GlpkCallBack::foo_bar(glp_tree *tree, void *info)
{
    if(stop())
    {
        glp_ios_terminate(tree);
}
}
/**
     * This hook function will be called if an error occurs when
     * calling the GLPK library.
     */
void GlpkCtrl::errorHook(void *in)
{
    qDebug(QString("error hook").toLatin1().data());
}

void GlpkCtrl::stop()
{
     GlpkCallBack::setStop(true);
    glp_error_(NULL,NULL);
}


bool GlpkCtrl::run(QString &msg)
{
    GlpkCallBack::setStop(false);

    // parameters
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.cb_func = GlpkCallBack::foo_bar;

    glp_error_hook(errorHook, NULL);

    bool ok;
    int ret = glp_simplex(_glpProblem, NULL);

    if(ret==0)
    {
        ret = glp_intopt(_glpProblem,  &parm);
        ret = glp_mip_status(_glpProblem);

        return (ret == GLP_OPT);
    }
    else
        return false;

}
