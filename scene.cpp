#include "scene.h"
//#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <cmath>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//-------picture variable --------

//---------------------------------




string convert(float number)
{
	ostringstream buff;
	buff << number;
	return buff.str();
}

string intConvert(int number)
{
	ostringstream buff;
	buff << number;
	return buff.str();
}

inline const char * const BoolToString(bool b)
{
	return b ? "true" : "false";
}
object::object()
{

}
object::object(int shape,int id) 
{
	this->shape = shape;
	this->id = id;

}
object::object(int id, float position0, float position1, float position2, float angle0, float angle1, float angle2, float size, int shape, int material)
{
	this->id = id;
	this->position[0] = position0;
	this->position[1] = position1;
	this->position[2] = position2;
	this->angle[0] = angle0;
	this->angle[1] = angle1;
	this->angle[2] = angle2;
	this->size = size;
	this->shape = shape;
	this->material = material;
}
void object::draw()
{

		glPushMatrix();	
		//glMatrixMode(GL_MODELVIEW);
			//glLoadIdentity();
		glScalef(size, size, size);
		glTranslatef(position[0], position[1], position[2]);
		glRotatef(angle[0], 1.0, 0.0, 0.0);
		glRotatef(angle[1], 0.0, 1.0, 0.0);
		glRotatef(angle[2], 0.0, 0.0, 1.0);
		if (isSelected)
		{
			glPushMatrix();
			glColor3f(0, 1, 0);
			glutWireCube(2);
			glPopMatrix();
		}
		

glPushMatrix();	
	switch (material)
	{
	case 0://emerald
	{
		ambient[0] = 0.0215;
		ambient[1] = 0.1745;
		ambient[2] = 0.0215;
		ambient[3] = 1.0;

		diffuse[0] = 0.0215;
		diffuse[4] = 0.1745;
		diffuse[4] = 0.0215;
		diffuse[4] = 1.0;

		specular[0] = 0.0215;
		specular[1] = 0.1745;
		specular[2] = 0.0215;
		specular[3] = 1.0;

		emission[0] = emission[1] = emission[2] = emission[3] = 0.0;

		shiny = 0.6;
	}
	break;
	case 1: // blackRubber
	{
		ambient[0] = ambient[1] = ambient[2] = 0.0;
		ambient[3] = 1.0;

		diffuse[0] = 0.01;
		diffuse[0] = 0.01;
		diffuse[0] = 0.01;
		diffuse[0] = 1.0;

		specular[0] = 0.5;
		specular[1] = 0.5;
		specular[2] = 0.5;
		specular[3] = 1.0;

		emission[0] = emission[1] = emission[2] = emission[3] = 0.0;

		shiny = 0.6;
	}
	break;
	case 2://jade
	{
		ambient[0] = 0.135;
		ambient[1] = 0.2225;
		ambient[2] = 0.1575;
		ambient[3] = 1.0;

		diffuse[0] = 0.54;
		diffuse[4] = 0.89;
		diffuse[4] = 0.63;
		diffuse[4] = 1.0;

		specular[0] = 0.316628;
		specular[1] = 0.316228;
		specular[2] = 0.316228;
		specular[3] = 1.0;

		emission[0] = emission[1] = emission[2] = emission[3] = 0.0;

		shiny = 0.1;
	}
	break;
	case 3://ruby
	{
		ambient[0] = 0.1745;
		ambient[1] = 0.01175;
		ambient[2] = 0.01175;
		ambient[3] = 1.0;

		diffuse[0] = 0.61424;
		diffuse[4] = 0.04136;
		diffuse[4] = 0.04136;
		diffuse[4] = 1.0;

		specular[0] = 0.727811;
		specular[1] = 0.626959;
		specular[2] = 0.626959;
		specular[3] = 1.0;

		emission[0] = emission[1] = emission[2] = emission[3] = 0.0;

		shiny = 0.6;
	}
	break;
	case 4://copper
	{
		ambient[0] = 0.19125;
		ambient[1] = 0.0735;
		ambient[2] = 0.0225;
		ambient[3] = 1.0;

		diffuse[0] = 0.7038;
		diffuse[4] = 0.27048;
		diffuse[4] = 0.0828;
		diffuse[4] = 1.0;

		specular[0] = 0.256777;
		specular[1] = 0.137622;
		specular[2] = 0.086014;
		specular[3] = 1.0;

		emission[0] = emission[1] = emission[2] = emission[3] = 0.0;

		shiny = 0.1;
	}
	break;
	case 5:
	{

	}
	break;
	}

	//material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
	
		switch (shape)
		{
		case 0:
			glutSolidCube(1);
			break;
		case 1:
			glutSolidSphere(1, 20, 20);
			break;
		case 2:
			glutSolidTeapot(1);
			break;
		case 3:
			glutSolidCone(1, 2, 20, 20);
			break;
		case 4:
			glutSolidTetrahedron();
			break;
		}


		glPopMatrix();
		glPopMatrix();
}

int object::getid()
{
	return id;
}
void object::rotation(float angle, int axis)
{
	if (axis == 0)
	{
		this->angle[0] = this->angle[0] + angle;
	}
	if (axis == 1)
	{
		this->angle[1] = this->angle[1] + angle;
	}
	if (axis == 2)
	{
		this->angle[2] = this->angle[2] + angle;
	}
}

void object::translate(float distance, int direction)
{
	if (direction == 0)
	{
		position[0] = position[0] + distance;
	}
	if (direction == 1)
	{
		this->position[1] = this->position[1] + distance;
	}
	if (direction == 2)
	{
		this->position[2] = this->position[2] + distance;
	}
}

void object::scale(float size)
{
	this->size = size;
}

void object::scaleUp(float scale)
{
 this->size = this->size * scale;
}
void object::changeMaterial(int i)
{
 this->material = i;
}

float object::TdDistance(float x, float y, float z)
{
 float distance = sqrtf(powf((position[0]-x), 2) + powf((position[1]-y), 2) + powf((position[2] - z), 2));
 return distance;
}

void object::changeSelect()
{

	isSelected = !isSelected;
	
}
string object::toString()
{
	return convert(id) + " " + convert(position[0]) + " " + convert(position[1]) + " " + convert(position[2]) + " " + convert(angle[0]) + " " + convert(angle[1]) + " " + convert(angle[2]) + " " + convert(size) + " " + intConvert(shape) + " " + intConvert(material) ;
}

float distance()
{
	return 1.0;
}

bool object::check(double Rd[3], double R0[3])
{
	double A = Rd[0] * Rd[0] + Rd[1] * Rd[1] + Rd[2] * Rd[2];
	double* R0Pc = new double[3];
	R0Pc[0] = R0[0] - this->position[0]; // x
	R0Pc[1] = R0[1] - this->position[1]; // y
	R0Pc[2] = R0[2] - this->position[2]; // z

	double B = 2 * (R0Pc[0] * Rd[0] + R0Pc[1] * Rd[1] + R0Pc[2] * Rd[2]);
	double C = (R0Pc[0] * R0Pc[0] + R0Pc[1] * R0Pc[1] + R0Pc[2] * R0Pc[2]) - (this->size * this->size); //size

	double discriminent = B*B - 4 * A * C;

	if (discriminent >= 0){
		double t1 = (-B + sqrt(discriminent)) / (2 * A);
		double t2 = (-B - sqrt(discriminent)) / (2 * A);
		return true;

	}
	return false;
}


void object::selectTrue()
{
	isSelected = true;
}
void object::selectFalse()
{
	isSelected = false;
}