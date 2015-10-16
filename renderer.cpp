#include "renderer.h"
#include "a2.h"

#include <QTextStream>
#include "draw.h"

QTextStream cout(stdout);

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
                Matrix4x4(Vector4D(2, 0,-0.5   , 10 ),
                          Vector4D(0     , 2, 0     , 10),
                          Vector4D(0.5   , 0, 0.8660, 0  ),
                          Vector4D(0     , 0, 0     , 1  )));

    m_cube.appendTransform(
                Matrix4x4(Vector4D(  2,  0, -0.5, 200  ),
                          Vector4D(  0,  2,    0, 100  ),
                          Vector4D(0.5,  0,    2,   0  ),
                          Vector4D(  0,  0,    0,   1  )));
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

// clips a single point to edges of viewport
Point3D Renderer::clipPoint(Point3D point)
{
    if (point[0] < viewport_x_min)
    {
        point[0] = viewport_x_min;
    }
    if (point[0] > viewport_x_max)
    {
        point[0] = viewport_x_max;
    }
    if (point[1] < viewport_y_min)
    {
        point[1] = viewport_y_min;
    }
    if (point[1] > viewport_y_max)
    {
        point[1] = viewport_y_max;
    }
    return point;
}

// clips point 1 with respect to point 2 to viewport space
Point3D Renderer::clipLine(Point3D p1, Point3D p2)
{
    float slope = ((float) p2[1] - p1[1]) / ((float) p2[0] - p1[0]);

    if (p1[0] < viewport_x_min)
    {
        p1[1] = p1[1] + (viewport_x_min - p1[0]) * slope;
        p1[0] = viewport_x_min;
    }
    if (p1[0] > viewport_x_max)
    {
        p1[1] = p1[1] - (p1[0] - viewport_x_max) * slope;
        p1[0] = viewport_x_max;
    }
    if (p1[1] < viewport_y_min)
    {
        p1[0] = p1[0] + ((viewport_y_min - p1[1]) / slope);
        p1[1] = viewport_y_min;
    }
    if (p1[1] > viewport_y_max)
    {
        p1[0] = p1[0] - ((p1[1] - viewport_y_max) / slope);
        p1[1] = viewport_y_max;
    }
    return p1;
}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    draw_init(width(), height());

//    m_cube.appendTransform(rotation(1, 'z'));
    m_cube.appendTransform(rotation(1, 'y'));
//    m_cube.appendTransform(rotation(1, 'x'));

    std::vector<Line3D> demoLines = m_demoTriangle.getLines();
    Matrix4x4 model_matrix = m_demoTriangle.getTransform();

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
        p1 = clipLine(p1,p2);
        p2 = clipLine(p2,p1);

        draw_line(Point2D(p1[0], p1[1]), Point2D(p2[0], p2[1]));
    }
    std::vector<Line3D> demoLinesCube = m_cube.getLines();
    Matrix4x4 model_matrix_cube = m_cube.getTransform();

    for(std::vector<Line3D>::iterator it = demoLinesCube.begin(); it != demoLinesCube.end(); ++it) {

        Line3D line = *it;
        // Get the points and apply the model matrix
        Point3D p1 = model_matrix_cube * line.getP1(), p2 = model_matrix_cube * line.getP2();

        // Fill this in: Apply the view matrix

        // Fill this in: Do clipping here...

        // Apply the projection matrix
        p1 = m_projection * p1;
        p2 = m_projection * p2;

        p1 = clipLine(p1,p2);
        p2 = clipLine(p2,p1);

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
    m_viewport[0] = Point2D(10, 10);
    m_viewport[1] = Point2D(width-10, height-10);
    viewport_x_min = 10;
    viewport_x_max = width-10;
    viewport_y_min = 10;
    viewport_y_max = height-10;
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
    update();
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";
}

