TEMPLATE = app
TARGET = arapFramework
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
DEPENDPATH += ./include
DISTFILES += *.png
QT *= xml \
    opengl
CONFIG -= debug \
    debug_and_release
CONFIG += release \
    qt \
    opengl \
    warn_on \
    thread \
    rtti \
    console \
    embed_manifest_exe
# to specify with your own configuration: locate libcholmod folder (likely in the folder /usr/include #
EXT_DIR = ../../extern

INCLUDEPATH = ./Manipulator \
    /usr/include/suitesparse
HEADERS += Window.h \
    ARAPViewer.h \
    Manipulator/PCATools.h \
    Manipulator/Manipulator.h \
    Vec3D.h \
    GLUtilityMethods.h \
    Manipulator/RectangleSelection.h \
    MeshManipInterface.h \
    AsRigidAsPossible.h \
    Triangle.h \
    Edge.h \
    Mesh.h \
    openglincludeQtComp.h
SOURCES += Window.cpp \
    ARAPViewer.cpp \
    Main.cpp \
    GLUtilityMethods.cpp \
    AsRigidAsPossible.cpp \
    Mesh.cpp
LIBS += -L/usr/lib/x86_64-linux-gnu \
    -lgslcblas \
    -lgsl \
    -lblas \
    -lQGLViewer-qt5 \
    -lglut \
    -lGLU \
    -lgomp



# ------------------------------ for CHOLMOD : ------------------------------#
QMAKE_LIBDIR +=$${EXT_DIR}/libcholmod/CHOLMOD/Lib \
    $${EXT_DIR}/libcholmod/AMD/Lib \
    $${EXT_DIR}/libcholmod/COLAMD/Lib \
    $${EXT_DIR}/libcholmod/CCOLAMD/Lib \
    $${EXT_DIR}/libcholmod/CAMD/Lib

LIBS += -lcholmod \
    -lamd \
    -lcolamd \
    -lccolamd \
    -lcamd \
    -llapack \
   # -lgfortran \
   # -lgfortranbegin \
   # -lgfortran \
    -lm

INCLUDEPATH += $${EXT_DIR}/libcholmod/CHOLMOD/Include \
    $${EXT_DIR}/libcholmod/UFconfig \
    /usr/include/

