#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#include "sim.h"
#include "render.h"

static void * do_run_sim(void *null) {
    sim_run();

    return NULL;
}

int main(void) {
    pthread_t sim;
    pthread_create(&sim, NULL, do_run_sim, NULL);

    render_run();

    return 0;
}
