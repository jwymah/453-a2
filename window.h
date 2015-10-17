#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenu>

#include "renderer.h"

class Window : public QMainWindow
{
    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Window(QWidget *parent = 0);

    // destructor
    ~Window();

protected:

private slots:
    void resetView();
    void modeModelRotate();
    void modeModelScale();
    void modeModelTranslate();

private:
	// The main OpenGL area
    Renderer * renderer;

	// Menu items and actions
	QMenu * mFileMenu;
	QAction * mQuitAction;
    QAction * mResetAction;

    QActionGroup * mModeGroup;
    QMenu * mModeMenu;
    QAction * mModeModelRotate;
    QAction * mModeModelScale;
    QAction * mModeModelTranslate;

	// helper function for creating actions
    void createFileActions();
    void createModeActions();
};

#endif
