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
    m_projection_ortho = Matrix4x4(Vector4D(1,0,0,0),
                             Vector4D(0,1,0,0),
                             Vector4D(0,0,0,0),
                             Vector4D(0,0,0,0));

    mouse_x = 0;
    mouse_left = false;
    mouse_middle = false;
    mouse_right = false;

    reset_view();
    setModeModelRotate();
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
    fov = 40;
    n = 50;
    f = 200;
    m_cube.resetTransform();
    m_cube.appendTranslationTransform(
                Matrix4x4(Vector4D(  1,  0,    0, 200  ),
                          Vector4D(  0,  1,    0, 200  ),
                          Vector4D(  0,  0,    1, 100  ),
                          Vector4D(  0,  0,    0,   1  )));
    update();
}

void Renderer::setModeModelRotate()
{
    mode_model_rotate = true;
    mode_model_scale = false;
    mode_model_translate = false;
    update();
}

void Renderer::setModeModelScale()
{
    mode_model_rotate = false;
    mode_model_scale = true;
    mode_model_translate = false;
    update();
}

void Renderer::setModeModelTranslate()
{
    mode_model_rotate = false;
    mode_model_scale = false;
    mode_model_translate = true;
    update();
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
void Renderer::drawObject(std::vector<Line3D> linesGnomonCube, Matrix4x4 cube_gnomon)
{
    for(std::vector<Line3D>::iterator it = linesGnomonCube.begin(); it != linesGnomonCube.end(); ++it) {

        Line3D line = *it;
        // Get the points and apply the model matrix
        Point3D p1 = cube_gnomon * line.getP1();
        Point3D p2 = cube_gnomon * line.getP2();

        Matrix4x4 pp1 = Matrix4x4();
        pp1[0][0] = p1[0];
        pp1[1][1] = p1[1];
        pp1[2][2] = p1[2];
        pp1[3][3] = 1;
        Matrix4x4 pp2 = Matrix4x4();
        pp2[0][0] = p2[0];
        pp2[1][1] = p2[1];
        pp2[2][2] = p2[2];
        pp2[3][3] = 1;

        // Fill this in: Apply the view matrix
//        p1 = view_matrix * p1;
//        p2 = view_matrix * p2;

        // Fill this in: Do clipping here... near/far
        if (  (pp1[2][2] < n && pp2[2][2] < n) // don't draw if both line endings are oob
           || (pp1[2][2] > f && pp2[2][2] > f))
        {
            continue;
        }
        pp1 = clipNearPlane(pp1,pp2);
        pp2 = clipNearPlane(pp2,pp1);

        // Apply the projection matrix
        // projection matrix provides the illusion of perspective
        pp1 = m_projection * pp1;
        pp2 = m_projection * pp2;

        //homogenize
        p1 = homogenize(pp1);
        p2 = homogenize(pp2);

        // clipping for viewport..?
        if ( (p1[0] < viewport_x_min && p2[0] < viewport_x_min)
             ||(p1[0] > viewport_x_max && p2[0] > viewport_x_max)
             ||(p1[1] < viewport_y_min && p2[1] < viewport_y_min)
             ||(p1[1] > viewport_y_max && p2[1] > viewport_y_max))
        {
            continue;
        }
        p1 = clipLine(p1,p2);
        p2 = clipLine(p2,p1);

        draw_line(Point2D(p1[0], p1[1]), Point2D(p2[0], p2[1]));
    }

    // Draw viewport
    draw_line(m_viewport[0], Point2D(m_viewport[0][0], m_viewport[1][1]));
    draw_line(m_viewport[0], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[0][0], m_viewport[1][1]));
}

void Renderer::paintGL()
{
    draw_init(width(), height());

    flTanThetaOver2 = tan(((((float) fov/2) * 3.14159) / 180)); // tan(fov/2) in radians

    m_projection = Matrix4x4(Vector4D(20/aspectRatio/flTanThetaOver2,0,0,0),
                             Vector4D(0,20/aspectRatio/flTanThetaOver2,0,0),
                             Vector4D(0,0, (f+n)/(f-n), -(2*f*n)/(f-n)),
                             Vector4D(0,0, 1,0));

    // CUBE CUBE CUBE CUBERT
    std::vector<Line3D> demoLinesCube = m_cube.getLines();
    Matrix4x4 model_matrix_cube = m_cube.getTransform();
    drawObject(demoLinesCube, model_matrix_cube);

    // GNOMON GNOMON GNOMON GNOMON GNOMON
    std::vector<Line3D> linesGnomonCube = m_cube.gnomon.getLines();
    Matrix4x4 cube_gnomon = m_cube.gnomon.getTransform();
    drawObject(linesGnomonCube, cube_gnomon);

    draw_complete();
}

Matrix4x4 Renderer::clipNearPlane(Matrix4x4 p, Matrix4x4 q)
{
    // line from q to p.. p is out, clip p to near plane
    if (p[2][2] < n)
    {
        float d = (float) p[2][2] / n;

        Vector3D qp = Vector3D(p[0][0] - q[0][0], p[1][1] - q[1][1], n);
        p[0][0] = q[0][0] + qp[0]*d;
        p[1][1] = q[1][1] + qp[1]*d;
        p[2][2] = n;
    }

    // line from q to p is outside far plane
    else if (p[2][2] > f)
    {
        float d = (float) p[2][2] / n;

        Vector3D qp = Vector3D(p[0][0] - q[0][0], p[1][1] - q[1][1], n);
        p[0][0] = q[0][0] + qp[0]*d;
        p[1][1] = q[1][1] + qp[1]*d;
        p[2][2] = f;

    }
    return p;
}

Point3D Renderer::homogenize(Matrix4x4 point)
{
    return Point3D(point[0][0]/point[3][2], point[1][1]/point[3][2], point[2][2]/point[3][2]);
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
    aspectRatio = (float) width / height;
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " released.\n";

    if (event->button() == 1)
    {
        mouse_left = true;
    }
    if (event->button() == 4)
    {
        mouse_middle = true;
    }
    if (event->button() == 2)
    {
        mouse_right = true;
    }
    mouse_x = 0;
}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " released.\n";
    if (event->button() == 1)
    {
        mouse_left = false;
    }
    else if (event->button() == 4)
    {
        mouse_middle = false;
    }
    else if (event->button() == 2)
    {
        mouse_right = false;
    }
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    update();
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";
    if (mouse_x == 0)
    {
        mouse_x = event->x();
        return;
    }

    magnitude = mouse_x - event->x();

    if (mode_model_scale)
    {
        if (mouse_left)
        {
            m_cube.scale_factor_x -= ((float) magnitude) / 100;
            if (m_cube.scale_factor_x < 0)
                m_cube.scale_factor_x = 0.0;
        }
        if (mouse_middle)
        {
            m_cube.scale_factor_y -= ((float) magnitude) / 100;
            if (m_cube.scale_factor_y < 0)
                m_cube.scale_factor_y = 0.0;
        }
        if (mouse_right)
        {
            m_cube.scale_factor_z -= ((float) magnitude) / 100;
            if (m_cube.scale_factor_z < 0)
                m_cube.scale_factor_z = 0.0;
        }
    }

    if (mode_model_rotate)
    {
        if (mouse_left)
        {
            m_cube.appendRotationTransform(rotation(magnitude, 'x'));
        }
        if (mouse_middle)
        {
            m_cube.appendRotationTransform(rotation(magnitude, 'y'));
        }
        if (mouse_right)
        {
            m_cube.appendRotationTransform(rotation(magnitude, 'z'));
        }
    }

    if (mode_model_translate)
    {
        if (mouse_left)
        {
            m_cube.appendTranslationTransform(translation(Vector3D(magnitude, 0, 0)));
        }
        if (mouse_middle)
        {
            m_cube.appendTranslationTransform(translation(Vector3D(0, magnitude, 0)));
        }
        if (mouse_right)
        {
            m_cube.appendTranslationTransform(translation(Vector3D(0, 0, magnitude)));
        }

    }
    mouse_x = event->x();
}
