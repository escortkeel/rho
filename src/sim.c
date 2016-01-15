#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#include "sim.h"

#define TICK_DURATION 0.001f

#define WALL_X 1
#define WALL_Y 1
#define WALL_Z 1

double x = 0;
double y = 0;
double z = 0;
double vx = 0.6;
double vy = 0.2;
double vz = 0.8;

void tick() {
    x = x + vx * TICK_DURATION;
    y = y + vy * TICK_DURATION;
    z = z + vz * TICK_DURATION;

    if(x < -WALL_X + RADIUS || x > WALL_X - RADIUS) {
        vx = -vx;
    }

    if(y < -WALL_Y + RADIUS || y > WALL_Y - RADIUS) {
        vy = -vy;
    }

    if(z < -WALL_Z + RADIUS || z > WALL_Z - RADIUS) {
        vz = -vz;
    }
}

static inline struct timespec dtotimespec(double d) {
    struct timespec t;
    t.tv_sec = d;
    t.tv_nsec = (d - ((double) t.tv_sec)) * 1.0e9;
    return t;
}

static inline double timespectod(struct timespec t) {
    return ((double) t.tv_sec) + 1.0e-9 * ((double) t.tv_nsec);
}

static struct timespec lasttime;

void sim_run() {
    clock_gettime(CLOCK_MONOTONIC, &lasttime);

    while(1) {
        double waited;
        struct timespec now;

        clock_gettime(CLOCK_MONOTONIC, &now);
        waited = timespectod(now) - timespectod(lasttime);

        while(waited < TICK_DURATION) {
            double remaining = TICK_DURATION - waited;

            struct timespec request = dtotimespec(remaining), left;
            nanosleep(&request, &left);

            clock_gettime(CLOCK_MONOTONIC, &now);
            waited = timespectod(now) - timespectod(lasttime);
        }

        lasttime = now;

        tick();
    }
}
