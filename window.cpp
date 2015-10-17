#include "window.h"

#include <QMenuBar>
#include <QApplication>

Window::Window(QWidget * parent) :
    QMainWindow(parent)
{
	setWindowTitle("CPSC453: Boxes vs the World!");

	// Create the main drawing object
	renderer = new Renderer();
	renderer->setMinimumSize(300, 300);
	setCentralWidget(renderer);

	// Create the actions to be used by the menus
    createFileActions();
    createModeActions();

	// Create the menus
	// Setup the file menu
	mFileMenu = menuBar()->addMenu(tr("&File"));
	mFileMenu->addAction(mQuitAction);  // add quitting
    mFileMenu->addAction(mResetAction);

    // Setup the Draw menu
    mModeMenu = menuBar()->addMenu(tr("&Mode"));
    mModeMenu->addAction(mModeModelRotate);
    mModeMenu->addAction(mModeModelScale);
    mModeMenu->addAction(mModeModelTranslate);
    mModeGroup = new QActionGroup(this);
    mModeGroup->setExclusive(true);
    mModeGroup->addAction(mModeModelRotate);
    mModeGroup->addAction(mModeModelScale);
    mModeGroup->addAction(mModeModelTranslate);
}
// helper function for creating actions
void Window::createFileActions()
{
    // Reset the view
    mResetAction = new QAction(tr("&Reset"), this);
    mResetAction->setShortcut(QKeySequence(Qt::Key_A));
    connect(mResetAction, SIGNAL(triggered()), this, SLOT(resetView()));

    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createModeActions()
{
    // rotate
    mModeModelRotate = new QAction(tr("&Rotate"), this);
    mModeModelRotate->setShortcut(QKeySequence(Qt::Key_R));
    mModeModelRotate->setCheckable(true);
    mModeModelRotate->setChecked(true);
    connect(mModeModelRotate, SIGNAL(triggered()), this, SLOT(modeModelRotate()));
    // scaling
    mModeModelScale = new QAction(tr("&Scale"), this);
    mModeModelScale->setShortcut(QKeySequence(Qt::Key_S));
    mModeModelScale->setCheckable(true);
    connect(mModeModelScale, SIGNAL(triggered()), this, SLOT(modeModelScale()));
    // translating
    mModeModelTranslate = new QAction(tr("&Translate"), this);
    mModeModelTranslate->setShortcut(QKeySequence(Qt::Key_T));
    mModeModelTranslate->setCheckable(true);
    connect(mModeModelTranslate, SIGNAL(triggered()), this, SLOT(modeModelTranslate()));
}

void Window::resetView()
{
    renderer->reset_view();
}

void Window::modeModelRotate()
{
    renderer->setModeModelRotate();
}
void Window::modeModelScale()
{
    renderer->setModeModelScale();
}
void Window::modeModelTranslate()
{
    renderer->setModeModelTranslate();
}

// destructor
Window::~Window()
{
    delete renderer;
}
