### This .pro file also asks to build OMOptimBasis
TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS +=  OMOptimBasis \
            OMOptimLib \
            OMOptim


OMOptimBasis.file = ../OMOptimBasis/build/OMOptimBasis.pro

OMOptimLib.file = build/OMOptimLib.pro
OMOptimLib.depends = OMOptimBasis

OMOptim.file = build/OMOptim.pro
OMOptim.depends = OMOptimLib


## copy dependent files after compiling
win32 {
        release_libs.files = $${QMAKE_LIBDIR_QT}/QtCore4.dll \
                              $${QMAKE_LIBDIR_QT}/QtGui4.dll \
                               $${QMAKE_LIBDIR_QT}/QtXml4.dll \
                               $${QMAKE_LIBDIR_QT}/../bin/mingwm10.dll \
                                $${QMAKE_LIBDIR_QT}/../bin/libgcc_s_dw2-1.dll \
                                $$(OMDEV)/lib/qwt-6.1.0-mingw/lib/qwt.dll \
                                $$(OMDEV)/lib/omniORB-4.1.6-mingw/bin/x86_win32/omniORB416_rt.dll \
                                $$(OMDEV)/lib/omniORB-4.1.6-mingw/bin/x86_win32/omnithread34_rt.dll \
                                $$(OMDEV)/lib/omniORB-4.1.6-mingw/bin/x86_win32/omniDynamic416_rt.dll
        release_libs.path = ./bin

        debug_libs.files = $${QMAKE_LIBDIR_QT}/QtCored4.dll \
                              $${QMAKE_LIBDIR_QT}/QtGuid4.dll \
                               $${QMAKE_LIBDIR_QT}/QtXmld4.dll \
                               $${QMAKE_LIBDIR_QT}/../bin/mingwm10.dll \
                                $${QMAKE_LIBDIR_QT}/../bin/libgcc_s_dw2-1.dll \
                                $$(OMDEV)/lib/qwt-6.1.0-mingw/lib/qwtd.dll \
                                $$(OMDEV)/lib/omniORB-4.1.6-mingw/bin/x86_win32/omniORB416_rtd.dll \
                                $$(OMDEV)/lib/omniORB-4.1.6-mingw/bin/x86_win32/omnithread34_rtd.dll \
                                $$(OMDEV)/lib/omniORB-4.1.6-mingw/bin/x86_win32/omniDynamic416_rtd.dll
        debug_libs.path = ./bin


CONFIG(debug, debug|release){
    INSTALLS += debug_libs
}else{
    message("Release libs")
    INSTALLS += release_libs
}

}

unix {
    ## For unix builds
    # MY_LIB_FILES += $$QMAKE_LIBDIR_QT/...xxxxxx....
}
