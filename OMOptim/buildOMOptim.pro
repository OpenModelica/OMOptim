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
        release_libs.files =
        release_libs.path = ./bin

        debug_libs.files =
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
