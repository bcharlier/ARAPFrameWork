/****************************************************************************


*****************************************************************************/

#ifndef MESHERVIEWER_H
#define MESHERVIEWER_H
#include "openglincludeQtComp.h"

#include "Mesh.h"


enum Mode {MESHING, MESH_DEFORMATION, IMAGE_DEFORMATION};

enum DisplayMode{ WIRE=0, SOLID=1, LIGHTED_WIRE=2, LIGHTED=3 };

#include <QColor>
#include <QOpenGLFunctions>
#include <QGLViewer/qglviewer.h>
#include <QString>

#include "Manipulator/RectangleSelection.h"
#include "Vec3D.h"

#include "GLUtilityMethods.h"

#include "MeshManipInterface.h"

using namespace qglviewer;

class ARAPViewer : public QGLViewer, public QOpenGLFunctions
{
    Q_OBJECT
public :
    ARAPViewer(QWidget *parent);
    ~ARAPViewer();

    void openOBJ(const QString & fileName);
    void openOFF (const QString & filename);
    void openOFFModel (const QString & filename);
    void saveOFF (const QString & filename);

    void openCamera (const QString & filename);
    void saveCamera(const QString & filename);
    void openConstraints (const QString & filename) ;

    void save (const QString & filename);
    void open (const QString & filename);

    void setTopositions(const vector<Vec3Df> & positions);

    unsigned int getARAPIteration(){ return meshInterface.getIterationNb(); }
protected :
    virtual void init();
    virtual void draw();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual QString helpString() const;

    void initLightsAndMaterials();
    void drawNormals();
    void updateViewer();
    void clear();

    void updateCamera(const Vec3Df & center, float radius);

    void updateFromCMInterface( std::vector< Vec3Df > const & copoints );

    void changeDisplayMode();

    void restaureLastState();

    DisplayMode displayMode;
    MMInterface< Vec3Df > meshInterface;
    SimpleManipulator * manipulator;
    RectangleSelection * rselection;

    std::deque< vector<Vec3Df> > Q;

    Mesh mesh;

    Mesh model_mesh;
    double sphereScale;
    double manipulatorScale;

    bool deformation;

public slots :
    void manipulatorReleased();
    void updateFromCMInterface();
    void addToSelection(QRectF const &, bool);
    void removeFromSelection(QRectF const &);
    void computeManipulatorForDeformation();
    void saveCurrentState();
    void setSphereScale(double _sphereScale){sphereScale = _sphereScale; meshInterface.set_sphere_scale(sphereScale); update();}
    void setManipulatorScale(double _mScale){manipulatorScale = _mScale; manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);update();}

    void setARAPIteration(int itNb){ meshInterface.setIterationNb(itNb); }
    void invertNormals(){ mesh.invertNormal(); update(); }
    void setDeformation(bool _deformation){ deformation = _deformation; update();}
    void reset();

};


#endif // MESHERVIEWER_H
