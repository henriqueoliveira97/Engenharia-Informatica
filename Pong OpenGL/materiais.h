#ifndef MATERIALS_H
#define MATERIALS_H
#include "ofMain.h"

inline void setMaterial(int materialID) {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;

	switch (materialID) {
	case 0: //Cinzento
		ambient[0] = 0.2; ambient[1] = 0.2; ambient[2] = 0.2; ambient[3] = 1.0;
		diffuse[0] = 0.3; diffuse[1] = 0.3; diffuse[2] = 0.3; diffuse[3] = 1.0;
		specular[0] = 0.4; specular[1] = 0.4; specular[2] = 0.4; specular[3] = 1.0;
		shininess = 120.0;  
		break;

	case 1: //Vermelho Suave
		ambient[0] = 0.5; ambient[1] = 0.1; ambient[2] = 0.1; ambient[3] = 1.0; 
		diffuse[0] = 0.7; diffuse[1] = 0.2; diffuse[2] = 0.2; diffuse[3] = 1.0; 
		specular[0] = 0.8; specular[1] = 0.3; specular[2] = 0.3; specular[3] = 1.0; 
		shininess = 120.0; 
		break;

	case 2: //Amarelo
		ambient[0] = 0.3; ambient[1] = 0.3; ambient[2] = 0.0; ambient[3] = 1.0;
		diffuse[0] = 1.0; diffuse[1] = 1.0; diffuse[2] = 0.0; diffuse[3] = 1.0;
		specular[0] = 1.0; specular[1] = 1.0; specular[2] = 0.3; specular[3] = 1.0;
		shininess = 120.0;
		break;

	case 3: //Dourado
		ambient[0] = 0.4; ambient[1] = 0.3; ambient[2] = 0.1; ambient[3] = 1.0;
		diffuse[0] = 1.0; diffuse[1] = 0.8; diffuse[2] = 0.3; diffuse[3] = 1.0;
		specular[0] = 1.0; specular[1] = 0.9; specular[2] = 0.6; specular[3] = 1.0;
		shininess = 120.0;
		break;
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}


#endif