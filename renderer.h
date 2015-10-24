#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>

#include "algebra.h"

using namespace std;

// The "main" OpenGL widget
class Renderer : public QOpenGLWidget, protected QOpenGLFunctions {

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Renderer(QWidget *parent = 0);

    // destructor
    virtual ~Renderer();

	// A useful function that forces this widget to rerender. If you
	// want to render a new frame, do not call paintGL
	// directly. Instead call this, which will cause an paintGL
	// call when the time is right.
	void invalidate();

	// *** Fill in these functions (in viewer.cpp) ***

	// Set the parameters of the current perspective projection using
	// the semantics of gluPerspective().
	void set_perspective(double fov, double aspect,
		       double near, double far);

	// Restore all the transforms and perspective parameters to their
	// original state. Set the viewport to its initial size.
	void reset_view();
    void setModeModelRotate();
    void setModeModelScale();
    void setModeModelTranslate();

    Point3D clipPoint(Point3D point);
    Point3D clipLine(Point3D p1, Point3D p2);
    Matrix4x4 clipNearAndFarPlanes(Matrix4x4 p, Matrix4x4 q);
    Point3D homogenize(Matrix4x4 p);

    void drawObject(std::vector<Line3D> linesGnomonCube, Matrix4x4 cube_gnomon);
protected:

	// Events we implement
	// Note that we could use Qt's "signals and slots" mechanism
	// instead, but for many classes there's a convenient member
	// function one just needs to define that'll be called with the
	// event.

	// override fundamental drawing functions

	// Called when OpenGL is first initialized
	void initializeGL();

	// Called when the window needs to be redrawn
	void paintGL();

	// Called when the window is resized
	void resizeGL(int w, int h);

	// override mouse event functions

	// Called when a mouse button is pressed
	virtual void mousePressEvent(QMouseEvent * event);

	// Called when a mouse button is released
	virtual void mouseReleaseEvent(QMouseEvent * event);

	// Called when the mouse moves
	virtual void mouseMoveEvent(QMouseEvent * event);protected:


private:

	// *** Fill me in ***
    // You will want to declare some more matrices here
    Matrix4x4 m_projection;
    Matrix4x4 m_projection_ortho;
    Point2D m_viewport[2];
    double viewport_x_min;
    double viewport_x_max;
    double viewport_y_min;
    double viewport_y_max;
    float aspectRatio;
    float flTanThetaOver2;
    float fov;

    float n;
    float f;

    int mouse_x;
    bool mouse_left;
    bool mouse_middle;
    bool mouse_right;

    bool mode_model_rotate;
    bool mode_model_translate;
    bool mode_model_scale;

    int magnitude;

    Cube m_cube;
};

#endif
