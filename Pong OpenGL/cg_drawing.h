#ifndef CG_DRAWING_EXTRAS_H
#define CG_DRAWING_EXTRAS_H

#include "ofMain.h"

inline void cube_unit(float p) {
    glBegin(GL_QUADS);

    //frente
    glVertex3f(-p, -p, p);
    glVertex3f(-p, p, p);
    glVertex3f(p, p, p);
    glVertex3f(p, -p, p);

    //tras
    glVertex3f(-p, -p, -p);
    glVertex3f(p, -p, -p);
    glVertex3f(p, p, -p);
    glVertex3f(-p, p, -p);

    //cima
    glVertex3f(-p, -p, -p);
    glVertex3f(-p, -p, p);
    glVertex3f(p, -p, p);
    glVertex3f(p, -p, -p);

    //baixo
    glVertex3f(-p, p, p);
    glVertex3f(-p, p, -p);
    glVertex3f(p, p, -p);
    glVertex3f(p, p, p);

    //esq
    glVertex3f(-p, -p, p);
    glVertex3f(-p, -p, -p);
    glVertex3f(-p, p, -p);
    glVertex3f(-p, p, p);

    //dir
    glVertex3f(p, -p, p);
    glVertex3f(p, p, p);
    glVertex3f(p, p, -p);
    glVertex3f(p, -p, -p);

    glEnd();

}
inline void ball_unit_with_texture(ofImage img) {

    glEnable(GL_TEXTURE);
    img.bind();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


    GLfloat p = 0.5;
    glBegin(GL_QUADS);

    //frente
    glNormal3f(0, 0, 1);
    glTexCoord2d(0, 1 / 3.);
    glVertex3f(-p, p, p);
    glTexCoord2d(0, 2 / 3.);
    glVertex3f(-p, -p, p);
    glTexCoord2d(1 / 4., 2 / 3.);
    glVertex3f(p, -p, p);
    glTexCoord2d(1 / 4., 1 / 3.);
    glVertex3f(p, p, p);

    //dir
    glNormal3f(1, 0, 0);
    glTexCoord2d(1 / 4., 1 / 3.);
    glVertex3f(p, p, p);
    glTexCoord2d(1 / 4., 2 / 3.);
    glVertex3f(p, -p, p);
    glTexCoord2d(2. / 4., 2 / 3.);
    glVertex3f(p, -p, -p);
    glTexCoord2d(2. / 4., 1 / 3.);
    glVertex3f(p, p, -p);

    //tras
    glNormal3f(0, 0, -1);
    glTexCoord2d(3. / 4., 1 / 3.);
    glVertex3f(-p, p, -p);
    glTexCoord2d(2. / 4., 1 / 3.);
    glVertex3f(p, p, -p);
    glTexCoord2d(2. / 4., 2 / 3.);
    glVertex3f(p, -p, -p);
    glTexCoord2d(3. / 4., 2 / 3.);
    glVertex3f(-p, -p, -p);

    //esq
    glNormal3f(-1, 0, 0);
    glTexCoord2d(1., 1 / 3.);
    glVertex3f(-p, p, p);
    glTexCoord2d(3. / 4., 1 / 3.);
    glVertex3f(-p, p, -p);
    glTexCoord2d(3. / 4., 2 / 3.);
    glVertex3f(-p, -p, -p);
    glTexCoord2d(1, 2 / 3.);
    glVertex3f(-p, -p, p);

    //baixo
    glNormal3f(0, -1, 0);
    glTexCoord2d(3. / 4., 1.);
    glVertex3f(-p, -p, p);
    glTexCoord2d(3 / 4., 2 / 3.);
    glVertex3f(-p, -p, -p);
    glTexCoord2d(1. / 2., 2 / 3.);
    glVertex3f(p, -p, -p);
    glTexCoord2d(1. / 2., 1.);
    glVertex3f(p, -p, p);

    //cima
    glNormal3f(0, 1, 0);
    glTexCoord2d(3. / 4., 1. / 3);
    glVertex3f(-p, p, -p);
    glTexCoord2d(3. / 4., 0);
    glVertex3f(-p, p, p);
    glTexCoord2d(1. / 2., 0.);
    glVertex3f(p, p, p);
    glTexCoord2d(1. / 2., 1. / 3.);
    glVertex3f(p, p, -p);

    glEnd();

    img.unbind();
    glDisable(GL_TEXTURE);

}

inline void cube_unit_with_texture(ofImage img) {

    glEnable(GL_TEXTURE);
    img.bind();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

    GLfloat p = 0.5;
    GLfloat tile = 2.0; 

    glBegin(GL_QUADS);

    // frente
    glNormal3f(0, 0, 1);
    glTexCoord2d(0, 0);
    glVertex3f(-p, p, p);
    glTexCoord2d(0, tile);
    glVertex3f(-p, -p, p);
    glTexCoord2d(tile, tile);
    glVertex3f(p, -p, p);
    glTexCoord2d(tile, 0);
    glVertex3f(p, p, p);

    // direita
    glNormal3f(1, 0, 0);
    glTexCoord2d(0, 0);
    glVertex3f(p, p, p);
    glTexCoord2d(0, tile);
    glVertex3f(p, -p, p);
    glTexCoord2d(tile, tile);
    glVertex3f(p, -p, -p);
    glTexCoord2d(tile, 0);
    glVertex3f(p, p, -p);

    // trás
    glNormal3f(0, 0, -1);
    glTexCoord2d(0, 0);
    glVertex3f(-p, p, -p);
    glTexCoord2d(tile, 0);
    glVertex3f(p, p, -p);
    glTexCoord2d(tile, tile);
    glVertex3f(p, -p, -p);
    glTexCoord2d(0, tile);
    glVertex3f(-p, -p, -p);

    // esquerda
    glNormal3f(-1, 0, 0);
    glTexCoord2d(0, 0);
    glVertex3f(-p, p, p);
    glTexCoord2d(tile, 0);
    glVertex3f(-p, p, -p);
    glTexCoord2d(tile, tile);
    glVertex3f(-p, -p, -p);
    glTexCoord2d(0, tile);
    glVertex3f(-p, -p, p);

    // baixo
    glNormal3f(0, -1, 0);
    glTexCoord2d(0, 0);
    glVertex3f(-p, -p, p);
    glTexCoord2d(tile, 0);
    glVertex3f(-p, -p, -p);
    glTexCoord2d(tile, tile);
    glVertex3f(p, -p, -p);
    glTexCoord2d(0, tile);
    glVertex3f(p, -p, p);

    // cima
    glNormal3f(0, 1, 0);
    glTexCoord2d(0, 0);
    glVertex3f(-p, p, -p);
    glTexCoord2d(0, tile);
    glVertex3f(-p, p, p);
    glTexCoord2d(tile, tile);
    glVertex3f(p, p, p);
    glTexCoord2d(tile, 0);
    glVertex3f(p, p, -p);

    glEnd();

    img.unbind();
    glDisable(GL_TEXTURE);
}

inline void player_unit_with_texture(ofImage img) {

    glEnable(GL_TEXTURE);
    img.bind();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    GLfloat p = 0.5; 
    GLfloat depth = p / 2;  

    glBegin(GL_QUADS);

    //frente
    glNormal3f(0, 0, 1);
    glTexCoord2d(0, 1 / 3.);
    glVertex3f(-depth, p, p);
    glTexCoord2d(0, 2 / 3.);
    glVertex3f(-depth, -p, p);
    glTexCoord2d(1 / 4., 2 / 3.);
    glVertex3f(depth, -p, p);
    glTexCoord2d(1 / 4., 1 / 3.);
    glVertex3f(depth, p, p);

    //dir
    glNormal3f(1, 0, 0);
    glTexCoord2d(1 / 4., 1 / 3.);
    glVertex3f(depth, p, p);
    glTexCoord2d(1 / 4., 2 / 3.);
    glVertex3f(depth, -p, p);
    glTexCoord2d(2. / 4., 2 / 3.);
    glVertex3f(depth, -p, -p);
    glTexCoord2d(2. / 4., 1 / 3.);
    glVertex3f(depth, p, -p);

    //tras
    glNormal3f(0, 0, -1);
    glTexCoord2d(3. / 4., 1 / 3.);
    glVertex3f(-depth, p, -p);
    glTexCoord2d(2. / 4., 1 / 3.);
    glVertex3f(depth, p, -p);
    glTexCoord2d(2. / 4., 2 / 3.);
    glVertex3f(depth, -p, -p);
    glTexCoord2d(3. / 4., 2 / 3.);
    glVertex3f(-depth, -p, -p);

    //esq
    glNormal3f(-1, 0, 0);
    glTexCoord2d(1., 1 / 3.);
    glVertex3f(-depth, p, p);
    glTexCoord2d(3. / 4., 1 / 3.);
    glVertex3f(-depth, p, -p);
    glTexCoord2d(3. / 4., 2 / 3.);
    glVertex3f(-depth, -p, -p);
    glTexCoord2d(1, 2 / 3.);
    glVertex3f(-depth, -p, p);

    //baixo
    glNormal3f(0, -1, 0);
    glTexCoord2d(3. / 4., 1.);
    glVertex3f(-depth, -p, p);
    glTexCoord2d(3 / 4., 2 / 3.);
    glVertex3f(-depth, -p, -p);
    glTexCoord2d(1. / 2., 2 / 3.);
    glVertex3f(depth, -p, -p);
    glTexCoord2d(1. / 2., 1.);
    glVertex3f(depth, -p, p);

    //cima
    glNormal3f(0, 1, 0);
    glTexCoord2d(3. / 4., 1. / 3);
    glVertex3f(-depth, p, -p);
    glTexCoord2d(3. / 4., 0);
    glVertex3f(-depth, p, p);
    glTexCoord2d(1. / 2., 0.);
    glVertex3f(depth, p, p);
    glTexCoord2d(1. / 2., 1. / 3.);
    glVertex3f(depth, p, -p);

    glEnd();

    img.unbind();
    glDisable(GL_TEXTURE);
}


#endif
