#ifndef WINDOW_H
#define WINDOW_H


#include <QMainWindow>
#include "ARAPViewer.h"

class Window : public QMainWindow
  {
    Q_OBJECT

  public:
    Window();

  private:
        ARAPViewer * viewer;
        QDockWidget * mesherDockWidget;
        void initActions ();
        void initFileActions ();
        void initMenus();
        void initToolBars();
        QActionGroup * fileActionGroup;
        QToolBar * fileToolBar;
        void initDisplayDockWidgets();
        QGroupBox * displayGroupBox;

        QTabWidget * contents;

  private slots:

        void saveMesh();
        void openMesh();
        void openMeshModel();
        void openConstraints();
        void saveCamera();
        void openCamera();

  };


#endif // WINDOW_H

