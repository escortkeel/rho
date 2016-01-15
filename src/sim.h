#ifndef RHO_SIM_H
#define RHO_SIM_H

#define RADIUS 0.2

extern double x, y, z;

void tick();

void initGL(int width, int height);
void drawGL(int width, int height);

void sim_run();

#endif
