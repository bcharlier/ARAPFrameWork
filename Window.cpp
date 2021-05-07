#include "Window.h"

#include <QtGui>

Window::Window()
{
    if (this->objectName().isEmpty())
        this->setObjectName("window");
    this->resize(929, 891);

    viewer = new ARAPViewer(this);

    QWidget * mesherWidget = new QWidget(this);

    QGridLayout * mesherLayout = new QGridLayout();

    mesherLayout->addWidget(viewer, 0, 1, 1, 1);

    mesherWidget->setLayout(mesherLayout);

    QGroupBox * viewerGroupBox = new QGroupBox("ARAP Viewer");

    QGridLayout * gridLayout = new QGridLayout(viewerGroupBox);
    gridLayout->setObjectName("gridLayout");

    gridLayout->addWidget(mesherWidget, 0, 1, 1, 1);

    viewerGroupBox->setLayout(gridLayout);

    this->setCentralWidget(viewerGroupBox);

    initDisplayDockWidgets();
    initActions ();
    initMenus ();
    initToolBars ();

    this->setWindowTitle("ARAP Framework");

}

void Window::initDisplayDockWidgets(){
    QDockWidget * displayDockWidget = new QDockWidget("Options");

    QWidget * contents = new QWidget();

    QVBoxLayout * contentLayout = new QVBoxLayout();

    QPushButton * saveCameraPushButton = new QPushButton("Save camera");
    contentLayout->addWidget(saveCameraPushButton);
    connect(saveCameraPushButton, SIGNAL(clicked()), this, SLOT(saveCamera()));

    QPushButton * loadCameraPushButton = new QPushButton("Load camera");
    contentLayout->addWidget(loadCameraPushButton);
    connect(loadCameraPushButton, SIGNAL(clicked()), this, SLOT(openCamera()));

    QPushButton * invertNormalsPushButton = new QPushButton("Invert Normals");
    contentLayout->addWidget(invertNormalsPushButton);
    connect(invertNormalsPushButton, SIGNAL(clicked()), viewer, SLOT(invertNormals()));

    contents->setLayout(contentLayout);
    displayDockWidget->setWidget(contents);

    QGroupBox * deformationGroupBox = new QGroupBox("Deformation");
    deformationGroupBox->setCheckable(true);
    deformationGroupBox->setChecked(false);
    connect (deformationGroupBox, SIGNAL(clicked(bool)), viewer, SLOT(setDeformation(bool)));

    QVBoxLayout * deformationGroupBoxLayout = new QVBoxLayout(deformationGroupBox);

    QPushButton * resetPushButton = new QPushButton("Reset");
    deformationGroupBoxLayout->addWidget(resetPushButton);
    connect(resetPushButton, SIGNAL(clicked()), viewer, SLOT(reset()));

    QLabel * sphereRadiusLabel = new QLabel("Sphere scale");
    deformationGroupBoxLayout->addWidget(sphereRadiusLabel);
    QDoubleSpinBox * sphereRadiusSpinBox = new QDoubleSpinBox();
    sphereRadiusSpinBox->setSingleStep(0.01);
    sphereRadiusSpinBox->setValue(1.);
    sphereRadiusSpinBox->setDecimals( 2 );
    connect (sphereRadiusSpinBox, SIGNAL(valueChanged(double)), viewer, SLOT(setSphereScale(double)));

    deformationGroupBoxLayout->addWidget(sphereRadiusSpinBox);

    QLabel * manipulatorScaleLabel = new QLabel("Manipulator scale");
    deformationGroupBoxLayout->addWidget(manipulatorScaleLabel);
    QDoubleSpinBox * manipulatorScaleSpinBox = new QDoubleSpinBox();
    manipulatorScaleSpinBox->setSingleStep(0.01);
    manipulatorScaleSpinBox->setValue(1.);
    manipulatorScaleSpinBox->setDecimals( 2 );
    connect (manipulatorScaleSpinBox, SIGNAL(valueChanged(double)), viewer, SLOT(setManipulatorScale(double)));

    deformationGroupBoxLayout->addWidget(manipulatorScaleSpinBox);

    QLabel * arapLabel = new QLabel("ARAP interation number");
    deformationGroupBoxLayout->addWidget(arapLabel);
    QSpinBox * arapSpinBox = new QSpinBox();
    arapSpinBox->setValue( viewer->getARAPIteration() );
    connect (arapSpinBox, SIGNAL(valueChanged(int)), viewer, SLOT(setARAPIteration(int)));

    deformationGroupBoxLayout->addWidget(arapSpinBox);

    contentLayout->addWidget(deformationGroupBox);
    contentLayout->addStretch(0);

    this->addDockWidget(Qt::RightDockWidgetArea, displayDockWidget);
}


void Window::saveMesh(){

    QString fileName = QFileDialog::getSaveFileName(this, "Save mesh file as ", "./data/", "Data (*.off)");

    // In case of Cancel
    if ( fileName.isEmpty() ) {
        return;
    }

    if(!fileName.endsWith(".off")) fileName.append(".off");

    viewer->saveOFF(fileName);

}

void Window::openMesh(){

    QString selectedFilter, openFileNameLabel;


    QString fileFilter = "Known Filetypes (*.obj *.off);;OBJ (*.obj);;OFF (*.off)";

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select an input mesh"),
                                                    openFileNameLabel,
                                                    fileFilter,
                                                    &selectedFilter);

    // In case of Cancel
    if ( fileName.isEmpty() ) {
        return;
    }

    viewer->openMesh(fileName);
}

void Window::openMeshModel(){

    QString selectedFilter, openFileNameLabel;


    QString fileFilter = "Known Filetypes (*.obj *.off);;OBJ (*.obj);;OFF (*.off)";

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select an input mesh"),
                                                    openFileNameLabel,
                                                    fileFilter,
                                                    &selectedFilter);

    // In case of Cancel
    if ( fileName.isEmpty() ) {
        return;
    }

    viewer->openModel(fileName);
}

void Window::openConstraints(){
    QString selectedFilter, openFileNameLabel;


    QString fileFilter = "Known Filetypes (*.off);;OFF (*.off)";

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select an input mesh"),
                                                    openFileNameLabel,
                                                    fileFilter,
                                                    &selectedFilter);

    // In case of Cancel
    if ( fileName.isEmpty() ) {
        return;
    }

    viewer->openConstraints(fileName);
}

void Window::initActions () {
    initFileActions ();
}

void Window::initFileActions () {

    QAction * fileOpenMeshAction = new QAction (QPixmap ("./Icons/fileopen.png"), "Open Mesh", this);
    fileOpenMeshAction->setShortcut (tr ("Ctrl+O"));
    connect (fileOpenMeshAction, SIGNAL (triggered ()) , this, SLOT (openMesh ()));

    QAction * fileOpenMeshModelAction = new QAction (QPixmap ("./Icons/fileopen.png"), "Open Model", this);
    fileOpenMeshModelAction->setShortcut (tr ("Ctrl+O"));
    connect (fileOpenMeshModelAction, SIGNAL (triggered ()) , this, SLOT (openMeshModel ()));

    QAction * fileOpenConstraintsAction = new QAction (QPixmap ("./Icons/fileopen.png"), "Open Constraints", this);
    // fileOpenConstraintsAction->setShortcut (tr ("Ctrl+SHIFT+O"));
    connect (fileOpenConstraintsAction, SIGNAL (triggered ()) , this, SLOT (openConstraints()));

    QAction * fileSaveMeshAction = new QAction (QPixmap ("./Icons/filesave.png"), "Save Mesh", this);
    fileSaveMeshAction->setShortcut (tr ("Ctrl+S"));
    connect (fileSaveMeshAction, SIGNAL (triggered ()) , this, SLOT (saveMesh ()));

    QAction * fileQuitAction = new QAction (QPixmap("./Icons/exit.png"), "Quit", this);
    fileQuitAction->setShortcut (tr ("Ctrl+Q"));
    connect (fileQuitAction, SIGNAL (triggered()) , qApp, SLOT (closeAllWindows()));

    fileActionGroup = new QActionGroup (this);

    fileActionGroup->addAction (fileOpenMeshAction);
    fileActionGroup->addAction (fileOpenMeshModelAction);
    fileActionGroup->addAction (fileOpenConstraintsAction);
    fileActionGroup->addAction (fileSaveMeshAction);

    fileActionGroup->addAction (fileQuitAction);
}


void Window::initMenus () {
    QMenu * fileMenu = menuBar ()->addMenu (tr ("File"));;
    fileMenu->addActions (fileActionGroup->actions ());

    QMenu * helpMenu = menuBar ()->addMenu (tr ("Help"));
    QAction *  helpAction = new QAction ("Control", this);
    helpAction->setShortcut (tr ("Ctrl+H"));
    connect (helpAction, SIGNAL (triggered ()), viewer, SLOT (help ()));
    helpMenu->addAction (helpAction);
}

void Window::initToolBars () {
    fileToolBar = new QToolBar (this);
    fileToolBar->addActions (fileActionGroup->actions ());
    addToolBar (fileToolBar);
}

void Window::saveCamera(){
    QString fileName = QFileDialog::getSaveFileName(this, "Save camera file as ", "./data/", "Data (*.txt)");

    // In case of Cancel
    if ( fileName.isEmpty() ) {
        return;
    }

    if(!fileName.endsWith(".txt"))
        fileName.append(".txt");

    viewer->saveCamera(fileName);
}

void Window::openCamera(){

    QString selectedFilter, openFileNameLabel;

    QString fileFilter = "Known Filetypes (*.txt);; Data (*.txt)";

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select an input camera"),
                                                    openFileNameLabel,
                                                    fileFilter,
                                                    &selectedFilter);

    // In case of Cancel
    if ( fileName.isEmpty() ) {
        return;
    }

    viewer->openCamera(fileName);
}

