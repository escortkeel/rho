#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "render.h"
#include "sim.h"

#define FOV_COEFF (M_PI / 20)
#define ROT_COEFF (2 * M_PI * 3 / 360)

static bool tracking;
static double lastx, lasty;
static double yawangle, pitchangle;
static double aspect, fov = M_PI / 2;

#define NUM_Z_SEGMENTS 500
#define NUM_THETA_SEGMENTS 500

static inline void defineSpherePoint(double X, double Y, double Z, double R, double z, double theta) {
    double p[3];
    double r2 = pow(R, 2) - pow(z, 2);
    double r = r2 <= 0 ? 0 : sqrt(r2);
    p[0] = r * cos(theta);
    p[1] = r * sin(theta);
    p[2] = z;
    glNormal3dv(p);
    p[0] += X;
    p[1] += Y;
    p[2] += Z;
    glVertex3dv(p);
}

static inline void drawSphere(double X, double Y, double Z, double R) {
    double dz = 2 * R / NUM_Z_SEGMENTS;
    double dtheta = 2 * M_PI / NUM_THETA_SEGMENTS;

    for(int i = 0; i < NUM_Z_SEGMENTS; i++) {
        glBegin(GL_QUAD_STRIP);

        for(int j = 0; j < NUM_THETA_SEGMENTS + 1; j++) {
            defineSpherePoint(X, Y, Z, R, R - i * dz, j * dtheta);
            defineSpherePoint(X, Y, Z, R, R - (i + 1) * dz, j * dtheta);
        }

        glEnd();
    }
}

static int f[4][2] = {{0, 0}, {0, 1}, {1, 0}, {0, 1}};

static inline void drawCuboid(double x1, double y1, double z1, double x2, double y2, double z2) {
    double v[3][2];
    v[0][0] = x1;
    v[0][1] = x2;
    v[1][0] = y1;
    v[1][1] = y2;
    v[2][0] = z1;
    v[2][1] = z2;

    glBegin(GL_QUADS);

    int p[3];

    for(int s = 0; s < 2; s++) {
        for(int m = 0; m < 3; m++) {
            for(int i = 0; i < 4; i++) {
                p[(m + 1) % 3] = f[i][0];
                p[(m + 2) % 3] = f[i][1];

                glNormal3d(m == 0, m == 1, m == 2);
                glVertex3d(v[0][p[0]], v[1][p[1]], v[2][p[2]]);
            }
        }
    }

    glEnd();
}

#define ZNEAR 0.1
#define ZFAR  5

static void updateProjection() {
    double ymax = ZNEAR * tan(fov / 2);
    double xmax = ymax * aspect;

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(ymax, -ymax, xmax, -xmax, ZNEAR, ZFAR);
    glPopMatrix();
}

void initGL(int width, int height) {
    glViewport(0, 0, width, height);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    aspect = ((double) width) / ((double) height);

    updateProjection();
}

void drawGL(int width, int height) {
    aspect = ((double) width) / ((double) height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(-x, -y, -z-2);
    glRotated(yawangle, 0, 1, 0);
    glRotated(pitchangle, 1, 0, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv (GL_LIGHT0, GL_POSITION, light_position);

    glColor4d(1.f, 0.f, 0.f, 1.f);
    drawSphere(x, y, z, RADIUS);

    glColor3d(1, 1, 1);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex3d(1, 1, 1);
    glVertex3d(1, 1, -1);

    glVertex3d(-1, 1, 1);
    glVertex3d(-1, 1, -1);

    glVertex3d(1, -1, 1);
    glVertex3d(1, -1, -1);

    glVertex3d(-1, -1, 1);
    glVertex3d(-1, -1, -1);

    glVertex3d(-1, -1, 1);
    glVertex3d(-1, -1, -1);

    glVertex3d(1, 1, -1);
    glVertex3d(1, -1, -1);
    glVertex3d(1, -1, -1);
    glVertex3d(-1, -1, -1);
    glVertex3d(-1, -1, -1);
    glVertex3d(-1, 1, -1);
    glVertex3d(-1, 1, -1);
    glVertex3d(1, 1, -1);
    glEnd();

    glColor4d(0.f, 0.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glVertex3d(-1,-1,1);
    glVertex3d(1,-1,1);
    glVertex3d(1,-1,-1);
    glVertex3d(-1,-1,-1);

    glVertex3d(-1,1,1);
    glVertex3d(1,1,1);
    glVertex3d(1,1,-1);
    glVertex3d(-1,1,-1);

    glVertex3d(1,-1,1);
    glVertex3d(1,-1,-1);
    glVertex3d(1,1,-1);
    glVertex3d(1,1,1);

    glVertex3d(-1,-1,1);
    glVertex3d(-1,-1,-1);
    glVertex3d(-1,1,-1);
    glVertex3d(-1,1,1);

    glVertex3d(1,-1,-1);
    glVertex3d(-1,-1,-1);
    glVertex3d(-1,1,-1);
    glVertex3d(1,1,-1);
    glEnd();
//    drawCuboid(-1, -1, -1, 1, -1, 1);
}

static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    if(tracking) {
        double dx = x - lastx;
        double dy = y - lasty;

        yawangle = fmax(-180, fmin(180, yawangle + -ROT_COEFF * dx));
        pitchangle = fmax(-90, fmin(90, pitchangle + -ROT_COEFF * dy));

        printf("%f %f\n", yawangle, pitchangle);

        lastx = x;
        lasty = y;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS) {
            tracking = true;

            double x, y;
            glfwGetCursorPos(window, &x, &y);

            lastx = x;
            lasty = y;
        } else if(action == GLFW_RELEASE) {
            tracking = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    double newfov = fov + yoffset * FOV_COEFF;
    fov = fmax(0.001, fmin(M_PI / 1.005, fov + yoffset * FOV_COEFF / pow(fmax(1, newfov),2)));

    updateProjection();
}

void render_run() {
    GLFWwindow* w;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    w = glfwCreateWindow(1000, 1000, "Simple example", NULL, NULL);
    if (!w) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);
    glfwSetKeyCallback(w, key_callback);
    glfwSetCursorPosCallback(w, cursor_position_callback);
    glfwSetMouseButtonCallback(w, mouse_button_callback);
    glfwSetScrollCallback(w, scroll_callback);

    int width, height;
    glfwGetFramebufferSize(w, &width, &height);

    initGL(width, height);

    while (!glfwWindowShouldClose(w)) {
        glfwGetFramebufferSize(w, &width, &height);

        drawGL(width, height);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glfwDestroyWindow(w);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
