#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <render.h>
#include <color.h>
#include <obj_loader.h>

#define MAX_VERTEX_COUNT 50000

void
parse_vertex(const char * str,
             Point3d * v);

void
parse_norm_vector(const char * str,
                  Vector3d * v);

void
parse_face(char * str,
           Point3d v[],
           Vector3d vn[],
           void (* face_handler)(Queue * vertexes,
                                 Queue * norm_vectors,
                                 void * args),
           void * args);

void
parse_face_str(char * str,
               int * v_index,
               int * vt_index,
               int * vn_index);

void
load_obj(const char * filename,
         void (* face_handler)(Queue * vertexes,
                               Queue * norm_vectors,
                               void * args),
         void * args) {
    
    Point3d vertexes[MAX_VERTEX_COUNT];
    int vertexes_cnt = 0;    
    
    Vector3d norm_vectors[MAX_VERTEX_COUNT];
    int norm_vectors_cnt = 0;
    
    FILE * fp = fopen(filename, "r");
    
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    
    while ((read = getline(&line, &len, fp)) > 0) {
        
        if((line[0] != 'v') && (line[0] != 'f'))
            continue;
        
        if((line[0] == 'v') && (line[1] == ' '))
            parse_vertex(&line[2], &vertexes[vertexes_cnt++]);
        
        if((line[0] == 'v') && (line[1] == 'n'))
            parse_norm_vector(&line[3], &norm_vectors[norm_vectors_cnt++]);
        
        if((line[0] == 'f') && (line[1] == ' '))
            parse_face(&line[2], vertexes, norm_vectors, face_handler, args);
    }
    
    if (line)
        free(line);
}

void
parse_vertex(const char * str,
             Point3d * v) {
    sscanf(str, "%lf %lf %lf", &v->y, &v->z, &v->x);
}

void
parse_norm_vector(const char * str,
                  Vector3d * v) {
    sscanf(str, "%lf %lf %lf", &v->y, &v->z, &v->x);
}

void
parse_face(char * str,
           Point3d v[],
           Vector3d vn[],
           void (* face_handler)(Queue * vertexes,
                                 Queue * norm_vectors,
                                 void * args),
           void * args) {
    
    Queue * tokens = new_queue();
    
    char * token = NULL;
    token = strtok(str, " \n");
    while(token) {
        add(token, tokens);
        token = strtok(NULL, " \n");
    }
    
    Queue * vertexes = new_queue();
    Queue * norm_vectors = new_queue();
    
    int vertex_index = 0;
    int texture_index = 0;
    int norm_index = 0;
    while(!is_empty(tokens)) {
        token = (char *) get(tokens);
        
        parse_face_str(token, &vertex_index, &texture_index, &norm_index);

        add(&v[vertex_index - 1], vertexes);
        
        if(norm_index > 0)
            add(&vn[norm_index - 1], norm_vectors);
    }
    
    face_handler(vertexes, norm_vectors, args);
    
    release_queue(tokens);
    release_queue(vertexes);
    release_queue(norm_vectors);
}

void
parse_face_str(char * str,
               int * v_index,
               int * vt_index,
               int * vn_index) {
    
    int str_len = strlen(str);
    
    int i = 0;
    while((str[i] != '/')
          && (str[i] != ' ')
          && (str[i] != '\0'))
        i++;
    str[i] = '\0';
    
    if(strlen(str) > 0)
        *v_index = atoi(str);
    
    i++;
    if(i >= str_len)
        return;
    
    str += i;
    str_len = strlen(str);
    i = 0;
    while((str[i] != '/')
          && (str[i] != ' ')
          && (str[i] != '\0'))
        i++;
    str[i] = '\0';
    
    if(strlen(str) > 0)
        *vt_index = atoi(str);
    
    i++;
    if(i >= str_len)
        return;
    
    str += i;
    str_len = strlen(str);
    i = 0;
    while((str[i] != '/')
          && (str[i] != ' ')
          && (str[i] != '\0'))
        i++;
    str[i] = '\0';
    
    if(strlen(str) > 0)
        *vn_index = atoi(str);
}

void
scene_face_handler(Queue * vertexes,
                   Queue * norm_vectors,
                   void * arg) {
    SceneFaceHandlerParams * params = (SceneFaceHandlerParams *) arg;
    
    Scene * scene = params->scene;
    Float scale = params->scale;
    Float dx = params->dx;
    Float dy = params->dy;
    Float dz = params->dz;
    Color default_color = params->default_color;
    Material default_material = params->default_material;
    
    Point3d * p1 = (Point3d *) get(vertexes);
    Point3d * p2 = (Point3d *) get(vertexes);
    Point3d * p3 = NULL;
    
    Vector3d * v1 = (Vector3d *) get(norm_vectors);
    Vector3d * v2 = (Vector3d *) get(norm_vectors);
    Vector3d * v3 = NULL;
    
    while(!is_empty(vertexes)) {
        p3 = (Point3d *) get(vertexes);
        v3 = (Vector3d *) get(norm_vectors);
        
        if(v1 && v2 && v3)
            add_object(scene, new_triangle_with_norms(
                                                      point3d(p1->x * scale + dx, p1->y * scale + dy, p1->z * scale + dz),
                                                      point3d(p2->x * scale + dx, p2->y * scale + dy, p2->z * scale + dz),
                                                      point3d(p3->x * scale + dx, p3->y * scale + dy, p3->z * scale + dz),
                                                      *v1,
                                                      *v2,
                                                      *v3,
                                                      default_color,
                                                      default_material));
        else
            add_object(scene, new_triangle(
                                           point3d(p1->x * scale + dx, p1->y * scale + dy, p1->z * scale + dz),
                                           point3d(p2->x * scale + dx, p2->y * scale + dy, p2->z * scale + dz),
                                           point3d(p3->x * scale + dx, p3->y * scale + dy, p3->z * scale + dz),
                                           default_color,
                                           default_material));
        
        p2 = p3;
        v2 = v3;
    }
}