/****************************************************************************
*****************************************************************************/

#include "ARAPViewer.h"

#include <QGLContext>

#if QT_VERSION >= 0x040000
# include <QKeyEvent>
#endif


using namespace qglviewer;

static unsigned int max_operation_saved = 10;

ARAPViewer::ARAPViewer(QWidget *parent) : QGLViewer(parent) {}

ARAPViewer::~ARAPViewer(){}

void ARAPViewer::init()
{
    // Absolutely needed for MouseGrabber
    setMouseTracking(true);

    setBackgroundColor(QColor(255,255,255));

    restoreStateFromFile();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    manipulator = new SimpleManipulator;
    connect(manipulator , SIGNAL(moved()) , this , SLOT(updateFromCMInterface()));
    connect(manipulator , SIGNAL(mouseReleased()) , this , SLOT(manipulatorReleased()));

    rselection = new RectangleSelection;
    connect( rselection , SIGNAL(add(QRectF , bool )) , this , SLOT(addToSelection(QRectF , bool)) );
    connect( rselection , SIGNAL(remove(QRectF)) , this , SLOT(removeFromSelection(QRectF)) );
    connect( rselection , SIGNAL(apply()) , this , SLOT(computeManipulatorForDeformation()) );

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    initLightsAndMaterials();

    setKeyDescription(Qt::Key_D, "Change display mode");

    displayMode = LIGHTED;

    sphereScale = 0.5;
    manipulatorScale = 1.;
    deformation = false;

    meshInterface = MMInterface< Vec3Df >(QGLContext::fromOpenGLContext(this->context()));

}

void ARAPViewer::initLightsAndMaterials() {

    GLTools::initLights();
    GLTools::setDefaultMaterial();
    GLTools::setSunriseLight();

}



void ARAPViewer::mousePressEvent(QMouseEvent* e )
{

    if( deformation ){
        if( ( e->modifiers() & Qt::ShiftModifier ) )
        {

            if( rselection->isInactive() )
            {
                rselection->activate();
            }
            rselection->mousePressEvent( e , camera() );
            update();
            return;

        }

        else if ((e->button() == Qt::MidButton) && (e->modifiers() & Qt::ControlModifier))
        {
            manipulator->clear();
            manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);
            meshInterface.make_selected_fixed_handles();
        }

        else if ((e->button() == Qt::LeftButton) && (e->modifiers() & Qt::ControlModifier))
        {
            bool found;
            qglviewer::Vec point = camera()->pointUnderPixel(e->pos(), found);
            if( found ){
                manipulator->clear();
                manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);
                meshInterface.make_fixed_handles(Vec3Df(point[0], point[1], point[2]), sphereScale*3.);
            }
        }
        else if ((e->button() == Qt::RightButton) && (e->modifiers() & Qt::ControlModifier))
        {
            bool found;
            qglviewer::Vec point = camera()->pointUnderPixel(e->pos(), found);
            if( found ){
                manipulator->clear();
                manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);
                meshInterface.select(Vec3Df(point[0], point[1], point[2]), sphereScale*3. );
            }
        }
    }
    QGLViewer::mousePressEvent(e);
}

void ARAPViewer::mouseMoveEvent(QMouseEvent* e  )
{

    if( deformation && ! rselection->isInactive() )
    {

        if( rselection->isInactive() )
        {
            rselection->activate();
        }
        rselection->mouseMoveEvent( e , camera() );
        update();
        return;
    }


    QGLViewer::mouseMoveEvent(e);
}

void ARAPViewer::mouseReleaseEvent(QMouseEvent* e  )
{

    if( deformation && ! rselection->isInactive() )
    {

        if( rselection->isInactive() )
        {
            rselection->activate();
        }
        rselection->mouseReleaseEvent( e , camera() );
        rselection->deactivate();
        update();
        return;

    }

    QGLViewer::mouseReleaseEvent(e);
}

void ARAPViewer::manipulatorReleased(){
    saveCurrentState();
}

void ARAPViewer::saveCurrentState(){

    if( Q.size() == max_operation_saved )
        Q.pop_front();
    Q.push_back(meshInterface.get_modified_vertices());

}


void ARAPViewer::restaureLastState(){

    if( Q.size() > 0 ){

        if(Q.size() > 1)
            Q.pop_back();

        setTopositions(Q.back());

        if(Q.size() > 1)
            Q.pop_back();
    }

}


void ARAPViewer::setTopositions(const vector<Vec3Df> & positions){

    manipulator->clear();
    manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);

    meshInterface.setToPosition(positions);
    updateFromCMInterface(positions);

}

void ARAPViewer::addToSelection( QRectF const & zone , bool moving )
{
    if(manipulator->getEtat()) manipulator->deactivate();

    meshInterface.select(zone , moving);
}

void ARAPViewer::removeFromSelection( QRectF const & zone )
{
    if(manipulator->getEtat()) manipulator->deactivate();
    meshInterface.unselect(zone);
}

void ARAPViewer::computeManipulatorForDeformation()
{
    meshInterface.computeManipulatorForSelection( manipulator );
}

void ARAPViewer::clear(){

    mesh = Mesh();

    meshInterface.clear();

    manipulator->deactivate();

    Q.clear();

}


void ARAPViewer::openOBJ(const QString & filename){

    clear();

    std::vector<Vec3Df> & V = mesh.getVertices();
    std::vector<Triangle> & T = mesh.getTriangles();

    FileIO::objLoader(filename.toStdString(), V, T);

    updateViewer();

}

void ARAPViewer::openOFFModel (const QString & filename) {

    model_mesh.clear();

    std::vector<Vec3Df> & vertices = model_mesh.getVertices();
    std::vector<Triangle> & triangles = model_mesh.getTriangles();

    FileIO::openOFF(filename.toStdString(), vertices, triangles);

    model_mesh.update();

}


void ARAPViewer::openOFF (const QString & filename) {

    clear();

    std::vector<Vec3Df> & vertices = mesh.getVertices();
    std::vector<Triangle> & triangles = mesh.getTriangles();

    FileIO::openOFF(filename.toStdString(), vertices, triangles);

    updateViewer();

}

void ARAPViewer::openConstraints (const QString & filename) {

    //    clear();


    std::ifstream myfile;
    myfile.open(filename.toStdString().c_str());
    if (!myfile.is_open())
    {
        std::cout << filename.toStdString() << " cannot be opened" << std::endl;
        return;
    }

    int v_id;
    float x, y, z;

    std::vector<std::pair<int, Vec3Df> > constraints;
    while ( myfile.good() )
    {
        myfile >> v_id;
        if( myfile.good() ){
            myfile >> x; myfile >> y; myfile >> z;

            constraints.push_back(std::make_pair(v_id, Vec3Df(x,y,z)));
        }
    }
    myfile.close();

    meshInterface.changedConstraints(constraints);

    updateFromCMInterface(meshInterface.get_modified_vertices());
}

void ARAPViewer::saveCamera(const QString &filename){
    std::ofstream out (filename.toUtf8());
    if (!out)
        exit (EXIT_FAILURE);

    out << camera()->position() << " " <<
           camera()->viewDirection() << " " <<
           camera()->upVector() << " " <<
           camera()->fieldOfView();
    out << std::endl;
    out.close ();
}

std::istream & operator>>(std::istream & stream, qglviewer::Vec & v)
{
    stream >>
              v.x >>
              v.y >>
              v.z;

    return stream;
}
void ARAPViewer::openCamera(const QString &filename){

    std::ifstream file;
    //file.open(filename.toStdString());

    qglviewer::Vec pos;
    qglviewer::Vec view;
    qglviewer::Vec up;
    float fov;

    file >> pos >>
            view >>
            up >>
            fov;

    camera()->setPosition(pos);
    camera()->setViewDirection(view);
    camera()->setUpVector(up);
    camera()->setFieldOfView(fov);

    camera()->computeModelViewMatrix();
    camera()->computeProjectionMatrix();

    update();
}

void ARAPViewer::saveOFF(const QString & filename) {

    FileIO::saveOFF(filename.toStdString(), mesh.getVertices(), mesh.getTriangles());
}


void ARAPViewer::updateViewer(){

    mesh.update();

    std::vector<Vec3Df> & vertices = mesh.getVertices();
    Vec3Df center;
    double radius;
    MeshTools::computeAveragePosAndRadius(vertices, center, radius);

    updateCamera(center, radius);

    meshInterface.clear();
    meshInterface.setMode(REALTIME);
    meshInterface.loadAndInitialize(mesh.getVertices(), mesh.getTriangles());

    manipulator->clear();
    manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);

    saveCurrentState();

    double average = meshInterface.getAverage_edge_halfsize();
    sphereScale = camera()->sceneRadius()*0.01 /average;
    if(sphereScale > 1.) sphereScale = 1.;

}

void ARAPViewer::updateCamera(const Vec3Df & center, float radius){
    camera()->setSceneCenter(Vec(center[0], center[1], center[2]));
    camera()->setSceneRadius(radius*2.);
    manipulator->setDisplayScale(manipulatorScale*radius/3.);

    camera()->showEntireScene();
}

void ARAPViewer::reset(){

    meshInterface.setToPosition(meshInterface.get_vertices());

    setTopositions(meshInterface.get_modified_vertices());

}

void ARAPViewer::updateFromCMInterface(){

    meshInterface.changed(manipulator);

    updateFromCMInterface(meshInterface.get_modified_vertices());

}

void ARAPViewer::updateFromCMInterface( std::vector< Vec3Df > const & copoints ){

    std::vector<Vec3Df> & points = mesh.getVertices();

    for( unsigned int i = 0 ; i < copoints.size() ; i ++ ){
        points[i] = copoints[i];
    }

    mesh.recomputeNormals();

    update();
}

void ARAPViewer::draw(){

    if(displayMode == LIGHTED || displayMode == LIGHTED_WIRE){

        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_LIGHTING);

    }  else if(displayMode == WIRE){

        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        glDisable (GL_LIGHTING);

    }  else if(displayMode == SOLID ){
        glDisable (GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    }

    glColor3f(1.,1.,1.);
    mesh.draw(meshInterface.get_selected_vertices(), meshInterface.get_fixed_vertices());

    if(displayMode == SOLID || displayMode == LIGHTED_WIRE){
        glEnable (GL_POLYGON_OFFSET_LINE);
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth (1.0f);
        glPolygonOffset (-1.0, 1.0);

        glColor3f(0.,0.,0.);
        mesh.draw();
        model_mesh.draw();

        glDisable (GL_POLYGON_OFFSET_LINE);
        glEnable (GL_LIGHTING);
    }

    if(deformation){
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        glEnable( GL_DEPTH);
        glEnable( GL_DEPTH_TEST );
        glEnable(GL_BLEND);
        glEnable(GL_LIGHTING);
        meshInterface.drawSelectedVertices();

        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable( GL_DEPTH_TEST );
        manipulator->draw();
        glEnable( GL_DEPTH_TEST );

        glEnable(GL_BLEND);
        rselection->draw();
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }
}

void ARAPViewer::changeDisplayMode(){
    if(displayMode == LIGHTED)
        displayMode = LIGHTED_WIRE;
    else if(displayMode == LIGHTED_WIRE)
        displayMode = SOLID;
    else if(displayMode == SOLID)
        displayMode = WIRE;
    else
        displayMode = LIGHTED;
    update();
}

void ARAPViewer::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_D : changeDisplayMode(); break;
    case Qt::Key_A :
        if(deformation && e->modifiers() & Qt::ControlModifier){
            manipulator->clear();
            manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);
            if( e->modifiers() & Qt::ShiftModifier) meshInterface.fixe_all();
            else meshInterface.select_all();
        }
        update(); break;
    case Qt::Key_U :
        if(deformation && e->modifiers() & Qt::ControlModifier){
            manipulator->clear();
            manipulator->setDisplayScale(manipulatorScale*camera()->sceneRadius()/9.);
            if( e->modifiers() & Qt::ShiftModifier) meshInterface.unfixe_all();
            else meshInterface.unselect_all();
        }
        update(); break;
    case Qt::Key_Z :
        if(e->modifiers() & Qt::ControlModifier) restaureLastState(); update(); break;
    default : QGLViewer::keyPressEvent(e);
    }
}

QString ARAPViewer::helpString() const
{
    QString text("<h2>KidPocket Framework</h2>");

    text += "This application allows to deform a mesh using </br> <b>ARAP*</b>.";

    text += "<p>";

    text += "<h3>Interaction</h3>";
    text += "<p>";
    text += "<h4>Basics</h4>";
    text += "<ul>";
    text += "<li><b>H</b>   :   make this help appear.</li>";
    text += "<li><b>D</b>   :   change display mode.</li>";
    text += "<li><b>Ctrl + Q</b>   :   close the application.</li>";
    text += "</ul>";
    text += "<h4>Open</h4>";
    text += "<ul>";
    text += "<li><b>Ctrl + O</b>    :    open a file containing a surface mesh (*.off, *.obj).</li>";
    text += "</ul>";
    text += "<h4>Save</h4>";
    text += "<ul>";
    text += "<li><b>Ctrl + S</b>    :    save deformed surface mesh (*.off).</li>";
    text += "</ul>";
    text += "<h4>Deformation</h4>";
    text += "<ul>";
    text += "<li><b>Ctrl + mouse right</b>    :    select/unselect as moving handles cage vertices</li>";
    text += "<li><b>Ctrl + mouse left</b>    :    change vertices handle type: fixed to moving or moving to fixed</li>";
    text += "<li><b>Ctrl + mouse middle</b>    :    change all moving handle vertices to fixed</li>";
    text += "<li><b>Ctrl + A</b>    :    set all free vertices to moving handles</li>";
    text += "<li><b>Ctrl + Shift + A</b>    :    set all free vertices to fixed handles</li>";
    text += "<li><b>Ctrl + U</b>    :    unselect all moving handles</li>";
    text += "<li><b>Ctrl + Shift + U</b>    :    unselect all fixed handles</li>";
    text += "<li><b>Shift + mouse right click</b>    :    manipulate the selected vertices</li>";
    text += "<li><b>Mouse middle on active manipulator</b>    :    change manipulator type</li>";
    text += "<li><b>Ctrl + Z</b>    :    cancel last deformation</li>";
    text += "</ul>";
    text += "</p>";

    text += "<h3>User guide</h3>";
    text += "<p>";
    text += "<h4>Pipeline</h4>";
    text += "<ul>";
    text += "<li>Load a mesh (*.off).</li>";
    text += "<li>Activate deformation tools.</li>";
    text += "<li>Select cage vertices. <ul><li>Fixed handles: red spheres.</li> <li>Moving handles: green spheres.</li> </ul></li>";
    text += "<li>Compute manipulator (Shift+right click).</li>";
    text += "<li>Operate deformations and iterate last 2 steps until getting the desired result. <b>WARNING:</b> Do not forget to unselect cage vertices before deforming a different area of the model.</li>";
    text += "<li>Save model.</li>";
    text += "</ul>";
    text += "<h4>Remarks</h4>";
    text += "<ul>";
    text += "<li>When you open a file there will be a latence time due to the initialization of ARAP.</li>";
    text += "<li>To deform the model, use the 'Rectangle' selection tool to select the moving handles of cage vertices, by using the mouse while keeping 'Shift' pressed (discussed before).</li>";
    text += "<li>To unselect vertices, use the 'Rectangle' selection tool by using the mouse while keeping 'Ctrl + Shift' pressed.</li>";
    text += "<li>To disable the manipulation tool, right click on it.</li>";
    text += "</ul>";
    text += "</p>";

    text += "<h3>Biblio</h3>";
    text += "<p>";
    text += "(*) : M. Alexa, D. Cohen-Or, D. Levin, As-rigid-as-possible shape 457 interpolation, Proceedings of SIGGRAPH 2000 (2000) 157458 164ISBN 1-58113-208-5.";
    text += "</p>";

    text += "</p>";
    return text;
}
