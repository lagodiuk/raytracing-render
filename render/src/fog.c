#include <stdlib.h>
#include <float.h>
#include <math.h>

#include <render.h>

static inline Float
exponential_fog_density(const Float distance,
                        const void * fog_data);

void
set_no_fog(Scene * const scene) {
    if(scene->fog_parameters) {
        free(scene->fog_parameters);
    }
    scene->fog_density = NULL;
}

void
set_exponential_fog(Scene * const scene,
                    const Float k) {
    scene->fog_density = exponential_fog_density;
    
    Float * k_p = malloc(sizeof(Float));
    *k_p = k;
    
    if(scene->fog_parameters) {
        free(scene->fog_parameters);
    }
    scene->fog_parameters = k_p;
}

static inline Float
exponential_fog_density(const Float distance,
                        const void * fog_data) {
    
    Float * k = (Float *) fog_data;
    return 1 - exp(- (*k) * distance);
}