#include "renderer.h"

#include <QTextStream>
#include "draw.h"

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // Setup orthogonal projection
    m_projection = Matrix4x4(Vector4D(1,0,0,0),
                             Vector4D(0,1,0,0),
                             Vector4D(0,0,0,0),
                             Vector4D(0,0,0,0));

    // Setup a demo transform, translate and rotate
    m_demoTriangle.appendTransform(
                Matrix4x4(Vector4D(0.8660, 0,-0.5   , 10 ),
                          Vector4D(0     , 1, 0     , 100),
                          Vector4D(0.5   , 0, 0.8660, 0  ),
                          Vector4D(0     , 0, 0     , 1  )));
}

// destructor
Renderer::~Renderer()
{
	// Nothing to do here right now.
}

// Force a rerender
void Renderer::invalidate()
{
	update();
}

void Renderer::set_perspective(double fov, double aspect,
                             double near, double far)
{
    // Fill me in!
}

void Renderer::reset_view()
{
    // Fill me in!
}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // You might not have anything in here, might have viewport & matrix setup...
}


// called by the Qt GUI system, to allow OpenGL drawing commands
//void Renderer::paintGL()
//{
//	// Here is where your drawing code should go.

//	draw_init(width(), height());

//	/* A few of lines are drawn below to show how it's done. */

//	set_colour(Colour(0.1, 0.1, 0.1));

//	draw_line(Point2D(0.1*width(), 0.1*height()),
//		Point2D(0.9*width(), 0.9*height()));
//	draw_line(Point2D(0.9*width(), 0.1*height()),
//		Point2D(0.1*width(), 0.9*height()));

//	draw_line(Point2D(0.1*width(), 0.1*height()),
//		Point2D(0.2*width(), 0.1*height()));
//	draw_line(Point2D(0.1*width(), 0.1*height()),
//		Point2D(0.1*width(), 0.2*height()));

//	draw_complete();
	    
//}

void Renderer::paintGL()
{
    draw_init(width(), height());

    std::vector<Line3D> demoLines = m_demoTriangle.getLines();
        Matrix4x4 model_matrix = m_demoTriangle.getTrasform();

        for(std::vector<Line3D>::iterator it = demoLines.begin(); it != demoLines.end(); ++it) {

            Line3D line = *it;
            // Get the points and apply the model matrix
            Point3D p1 = model_matrix * line.getP1(), p2 = model_matrix * line.getP2();

            // Fill this in: Apply the view matrix

            // Fill this in: Do clipping here...

            // Apply the projection matrix
            p1 = m_projection * p1;
            p2 = m_projection * p2;

            // Fill this in: Do clipping here (maybe)

            draw_line(Point2D(p1[0], p1[1]), Point2D(p2[0], p2[1]));
        }

    // Draw viewport
    draw_line(m_viewport[0], Point2D(m_viewport[0][0], m_viewport[1][1]));
    draw_line(m_viewport[0], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[0][0], m_viewport[1][1]));

    draw_complete();
}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int width, int height)
{
    m_viewport[0] = Point2D(0.05*width, 0.05*height);
    m_viewport[1] = Point2D(0.95*width, 0.95*height);
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
}


// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " released.\n";
}


// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";
}

