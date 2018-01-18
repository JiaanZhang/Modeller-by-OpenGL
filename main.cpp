#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "scene.h"
#include <fstream>
#include <vector>
#include <cmath>
//#include <GL/glut.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

using namespace std;



#define X 0
#define Y 1
#define Z 2
#define W 3


//--------Main program setting -----
float width = 600;
float height = 600;
float camPos[] = { 100.0f, 100.0f,100.0f };	//where the camera is
float camTarget[] = { 1, 1, 1 };
int camera_toggle = 1;
float camera_move = 3;
float camera_rot = 10;
float angle = 0;
float dis;
bool caleft = false;
bool caright =false;
//-------------------------------



//-----operate detail-------

float trans_dis = 1.0;
float roat_angle = 20;
int id = 0;

//------------------------

//-----vector of the object-----

std::vector<object> object_list;
std::vector<object>::iterator object_iter;

//---------------------------


//------Object Select -----
bool Select_object = false;
int current_Select = -1;

//------------------------


//------mouse ray ---------
double* m_start = new double[3];
double* m_end = new double[3];
//-----------------------


//-----Texture ---------
//GLubyte* image4;
//int width_2, height_2, max_2;
//GLuint myTex1[1];
//----------------------


//-------Light setup -----------
float light0_pos[] = {-7,6,2,0};
float light1_pos[] = {6, 7, -2, 0};

//Lighting values for light0
float amb0[4]  = {1, 1, 1, 1};
float diff0[4] = {1, 1, 1, 1};
float spec0[4] = {1, 1, 1, 1};

//Lighting values for light1
 float amb1[4]  = {1, 1, 1, 1};
 float diff1[4] = {0.2, 0.2, 0.2, 1};
float spec1[4] = {0, 0, 0.2, 1};

//-----------------------------
float ambient[4] = {0.0215f,0.1745f,0.0215f,1.0f};
float diffuse[4] = {0.0215f,0.1745f,0.0215f,1.0f};
float specular[4] = {0.0215f,0.1745f,0.0215f,1.0f};
float emission[4] = {0.0f,0.0f,0.0f,0.0f};
float shiny = 0.6;

GLubyte* marble_tex;
GLuint textures[1];
int wid_1, hei_1, max_1;



bool checklight(double Rd[3], double R0[3],float light[3])
{
	double A = Rd[0] * Rd[0] + Rd[1] * Rd[1] + Rd[2] * Rd[2];
	double* R0Pc = new double[3];
	R0Pc[0] = R0[0] - light[0]; // x
	R0Pc[1] = R0[1] - light[1]; // y
	R0Pc[2] = R0[2] - light[2]; // z

	double B = 2 * (R0Pc[0] * Rd[0] + R0Pc[1] * Rd[1] + R0Pc[2] * Rd[2]);
	double C = (R0Pc[0] * R0Pc[0] + R0Pc[1] * R0Pc[1] + R0Pc[2] * R0Pc[2]) - (1.5* 1.5); //size

	double discriminent = B*B - 4 * A * C;

	if (discriminent >= 0){
		double t1 = (-B + sqrt(discriminent)) / (2 * A);
		double t2 = (-B - sqrt(discriminent)) / (2 * A);
		return true;

	}
	return false;
}



GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
	GLubyte* img;
	FILE *fd;
	int n, m;
	int  k, nm;
	char c;
	int i;
	char b[100];
	float s;
	int red, green, blue;
	
	fd = fopen(file, "r");
	fscanf(fd,"%[^\n] ",b);
	if(b[0]!='P'|| b[1] != '3')
	{
		printf("%s is not a PPM file!\n",file); 
		exit(0);
	}
	printf("%s is a PPM file\n", file);
	fscanf(fd, "%c",&c);
	while(c == '#') 
	{
		fscanf(fd, "%[^\n] ", b);
		printf("%s\n",b);
		fscanf(fd, "%c",&c);
	}
	ungetc(c,fd); 
	fscanf(fd, "%d %d %d", &n, &m, &k);

	printf("%d rows  %d columns  max value= %d\n",n,m,k);

	nm = n*m;

	img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);


	s=255.0/k;


	for(i=0;i<nm;i++) 
	{
		fscanf(fd,"%d %d %d",&red, &green, &blue );
		img[3*nm-3*i-3]=red*s;
		img[3*nm-3*i-2]=green*s;
		img[3*nm-3*i-1]=blue*s;
	}

	*width = n;
	*height = m;
	*max = k;

	return img;
}

void setup_texture(){

	glGenTextures(1, textures);
	marble_tex = LoadPPM("marble.ppm", &wid_1, &hei_1, &max_1);
	//setup second texture (using marble image)
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	//set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//create a texture using the "tex" array
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid_1, hei_1, 0, GL_RGB, GL_UNSIGNED_BYTE, marble_tex);
	
}


void read()
{
	int id, shape, material;
	float position0, position1, position2, angle0, angle1, angle2, size;
	ifstream input("output.txt");
	string line;

	while (input >> id >> position0 >> position1 >> position2 >> angle0 >> angle1 >> angle2 >> size >> shape >> material)
	{
		cout << id << position0 << position1 << position2 << angle0 << angle1 << angle2 << size << shape << material << endl;
		object o3 = object(id, position0, position1, position2, angle0, angle1, angle2, size, shape, material);
		object_list.push_back(o3);
	}
	/*
	while (getline(input, line))
	{
	input >> a >> b >> c >> d >> e >> f >> g >> h >> i;
	cout << a << b << c << d << e << f << g << h << i << endl;
	}
	*/
}

void write()
{
	ofstream out("output.txt");

	for (object_iter = object_list.begin(); object_iter != object_list.end(); ++object_iter)
	{
		object o3 = *object_iter;
		out << o3.toString() << endl;
	}
}


void drawFloor() {
	glPushMatrix();
    glBegin(GL_QUADS);
    glNormal3d(0, 1, 0);
	float f_amb[] = {0.05375,0.05,0.06625,1.0};
	float f_diff[] = {0.18275,0.17,0.22525,1.0};
	float f_spec[] = {0.332741,0.328634,0.346435,1.0};
	float f_shiny = 0.3 * 128;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, f_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, f_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, f_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, f_shiny);
    // Front
    glVertex3f(-50, -1, 50);
    glVertex3f(50, -1, 50);
    glVertex3f(50, 1, 50);
    glVertex3f(-50, 1, 50);
    
    // Top
    glVertex3f(-50,1,50);
    glVertex3f(50,1,50);
    glVertex3f(50,1,-50);
    glVertex3f(-50,1,-50);
    
    // Bottom
    glVertex3f(-50,-1,50);
    glVertex3f(50,-1,50);
    glVertex3f(50,-1,-50);
    glVertex3f(-50,-1,-50);
    
    // Left side
    glVertex3f(-50,1,50);
    glVertex3f(-50,-1,50);
    glVertex3f(-50,-1,-50);
    glVertex3f(-50,1,-50);
    
    // Right side
    glVertex3f(50,1,50);
    glVertex3f(50,-1,50);
    glVertex3f(50,-1,-50);
    glVertex3f(50,1,-50);
    
    // Back side
    glVertex3f(-50,1,-50);
    glVertex3f(-50,-1,-50);
    glVertex3f(50,-1,-50);
    glVertex3f(50,1,-50);
    glEnd();
    glPopMatrix();
}

float Distance(float center1, float center2, float loop1, float loop2)
{
 float distance = sqrtf(powf((loop1 - center1), 2) + powf((loop2 - center2), 2));
 return distance;
}



void drawLights()
{
	glPushMatrix();
	glColor3f(0, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
	glPushMatrix();
	

		glTranslatef(light0_pos[0], light0_pos[1], light0_pos[2]);
		glScalef(0.1f, 0.1f, 0.1f);
		glutSolidSphere(10, 20, 20);
	glPopMatrix();

	glPushMatrix();
	
		glTranslatef(light1_pos[0], light1_pos[1], light1_pos[2]);
		glScalef(1.0f, 1.0f, 1.0f);
		glutSolidSphere(2, 20, 20);
	glPopMatrix();

	glPopMatrix();

}



void Select_this(int pass_id) {
	for (int i = 0; i < object_list.size(); i++)
	{

		if (object_list[i].getid() == pass_id)
		{
			object_list[i].selectTrue();
		}
		else
		{
			object_list[i].selectFalse();
		}
	}
}

void Move_this(int pass_id, int mode, int axis, float amount) {
	for (int i = 0; i < object_list.size(); i++)
	{
		if (object_list[i].getid() == pass_id)
		{
			if (mode == 0) {
				switch (axis)
				{
				case 0:
					object_list[i].translate(amount, 0);
					break;
				case 1:
					object_list[i].translate(amount, 1);
					break;
				case 2:
					object_list[i].translate(amount, 2);
					break;
				}
			}
			else
			{
				switch (axis)
				{
				case 0:
					object_list[i].rotation(amount, 0);
					break;
				case 1:
					object_list[i].rotation(amount, 1);
					break;
				case 2:
					object_list[i].rotation(amount, 2);
					break;
				}
			}
		}
	}

	}



void getInitialAngle(float dis)
{
 angle = acos((camPos[0] - camTarget[0]) / dis);
}

void eraseItem(int index)
{
	object_list.erase(object_list.begin() + index);
}

void change_material(int pass_id, int mode) {
	for (int i = 0; i < object_list.size(); i++)
	{
		if (object_list[i].getid() == pass_id)
		{
				object_list[i].changeMaterial(mode);}}
	}
	
void scale_it(int pass_id, float amount){

for (int i = 0; i < object_list.size(); i++)
	{
		if (object_list[i].getid() == pass_id)
		{
				object_list[i].scaleUp(amount);
	}
	}
}
	

	void keyboard(unsigned char key, int xIn, int yIn)
	{
		switch (key)
		{	
		case 'q':
		case 27:	//27 is the esc key
			exit(0);
			break;
		case 'z':
			if (Select_object) {
				Move_this(current_Select, 0, 0, trans_dis);
			}
			break;
		case 'x':
			if (Select_object) {
				Move_this(current_Select, 0, 0, -trans_dis);
			}
			break;
		case 'c':
			if (Select_object) {
				Move_this(current_Select, 0, 1, trans_dis);
			}
			break;
		case 'v':
			if (Select_object) {
				Move_this(current_Select, 0, 1, -trans_dis);
			}
			break;
		case 'b':
			if (Select_object) {
				Move_this(current_Select, 0, 2, trans_dis);
			}
			break;
		case 'n':
			if (Select_object) {
				Move_this(current_Select, 0, 2, -trans_dis);
			}
			break;
		case 'Z':
			if (Select_object) {
				Move_this(current_Select, 1, 0, roat_angle);
			}
			break;
		case 'X':
			if (Select_object) {
				Move_this(current_Select, 1, 0, -roat_angle);
			}
			break;
		case 'C':
			if (Select_object) {
				Move_this(current_Select, 1, 1, roat_angle);
			}
			break;
		case 'V':
			if (Select_object) {
				Move_this(current_Select, 1, 1, -roat_angle);
			}
			break;
		case 'B':
			if (Select_object) {
				Move_this(current_Select, 1, 2, roat_angle);
			}
			break;
		case 'N':
			if (Select_object) {
				Move_this(current_Select, 1, 2, -roat_angle);
			}
			break;
		case 'r':
		case 'R':
			 object_list.clear();
			break;
		case '6':
		{
			object n_cube = object(0, id);
			current_Select = id;
			object_list.push_back(n_cube);
			Select_this(current_Select);
			Select_object = true;
			id++;
		}
		break;
		case '7':
		{
			object n_sphere = object(1, id);
			current_Select = id;
			object_list.push_back(n_sphere);
			Select_this(current_Select);
			Select_object = true;
			id++;
		}
		 	break;
		case '8':
		{
			object n_teapot = object(2, id);
			current_Select = id;
			object_list.push_back(n_teapot);
			Select_this(current_Select);
			Select_object = true;
			id++;
		}
		break;
		case '9':
		{
			object n_cone = object(3, id);
			current_Select = id;
			object_list.push_back(n_cone);
			Select_this(current_Select);
			Select_object = true;
			id++;
		}
		break;
		case '0':
		{
			object n_torus = object(4, id);
			current_Select = id;
			object_list.push_back(n_torus);
			Select_this(current_Select);
			Select_object = true;
			id++;
		}
		break;
		case '1':
		{
			if(Select_object){
				change_material(current_Select,0);
			}
		}
		break;
		case '2':
		{
			if(Select_object){
				change_material(current_Select,1);
			}
		}
		break;
		case '3':
		{
			if(Select_object){
				change_material(current_Select,2);
			}
		}
		break;
		case '4':
		{
			if(Select_object){
				change_material(current_Select,3);
			}
		}
		break;
		case '5':
		{
			if(Select_object){
				change_material(current_Select,4);
			}
		}
		break;
		case '-':
		{
			if(Select_object){
				scale_it(current_Select,0.75);
			}
		}
		break;
		case '=':
		{
			if(Select_object){
				scale_it(current_Select,1.25);
			}
		}	
		break;	

		case ',':
			switch(camera_toggle % 4){
				case 1:
				 camPos[0] += camera_move;
				break;
				case 2:
				light0_pos[0] += camera_move;
				break;
				case 3:
				light1_pos[0] += camera_move;
				break;
			}
		break;

		case '.':
			switch(camera_toggle % 4){
				case 1:
				 camPos[0] -= camera_move ;
				break;
				case 2:
				light0_pos[0] -= camera_move;
				break;
				case 3:
				light1_pos[0] -= camera_move;
				break;
			}
		break;


		case ';':
			switch(camera_toggle % 4){
				case 1:
				 camPos[1] += camera_move ;
				break;
				case 2:
				light0_pos[1] += camera_move;
				break;
				case 3:
				light1_pos[1] += camera_move;
				break;
			}
		break;

		case 39:
			switch(camera_toggle % 4){
				case 1:
				 camPos[1] -= camera_move;
				break;
				case 2:
				light0_pos[1] -= camera_move;
				break;
				case 3:
				light1_pos[1] -= camera_move;
				break;
			}
		break;


		case 91:
			switch(camera_toggle % 4){
				case 1:
				 camPos[2] += camera_move ;
				break;
				case 2:
				light0_pos[2] += camera_move;
				break;
				case 3:
				light1_pos[2] += camera_move;
				break;
			}
		break;

		case 93:
			switch(camera_toggle % 4){
				case 1:
				 camPos[2] -= camera_move;
				break;
				case 2:
				light0_pos[2] -= camera_move;
				break;
				case 3:
				light1_pos[2] -= camera_move;
				break;
			}
		break;

		case 'S':
		case 's':
   			write();
   			break;
   		case 'L':
  		case 'l':
   			read();
   			break;
		}

	}

	void special(int key, int xIn, int yIn) {
		switch (key) {
		case GLUT_KEY_DOWN:
			camPos[Y] -= 5.0f;
			camTarget[Y] -= 5.0f;
			break;
		case GLUT_KEY_UP:
			camPos[Y] += 5.0f;
			camTarget[Y] += 5.0f;
			break;
		case GLUT_KEY_LEFT:
  		{
  	 	angle = angle + 0.1;
   		camPos[0] = camTarget[0] + (dis)* cos(angle);
   		camPos[2] = camTarget[2] + (dis)* sin(angle);
 	 }
			break;
		case GLUT_KEY_RIGHT:
  		{
 	  	angle = angle - 0.1;
   		camPos[0] = camTarget[0] + (dis)* cos(angle);
   		camPos[2] = camTarget[2] + (dis)* sin(angle);
 	 }
			break;
		}
	}
	//mouse
void mouse(int btn, int state, int x, int y){
	if (btn == GLUT_LEFT_BUTTON){
		if (state == GLUT_UP){
		}

		if (state == GLUT_DOWN){

			double matModelView[16], matProjection[16]; 
			int viewport[4]; 

			// get matrix and viewport:
			glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
			glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
			glGetIntegerv( GL_VIEWPORT, viewport ); 

			// window pos of mouse, Y is inverted on Windows
			double winX = (double)x; 
			double winY = viewport[3] - (double)y; 

			// get point on the 'near' plane (third param is set to 0.0)
			gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
					viewport, &m_start[0], &m_start[1], &m_start[2]); 

			// get point on the 'far' plane (third param is set to 1.0)
			gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
					viewport, &m_end[0], &m_end[1], &m_end[2]); 
			//----------------------------------------
			//  Ray intersection
			//----------------------------------------
			double* R0 = new double[3];
			double* Rd = new double[3];

			double xDiff = m_end[0] - m_start[0];
			double yDiff = m_end[1] - m_start[1];
			double zDiff = m_end[2] - m_start[2];

			double mag = sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff);
			R0[0] = m_start[0];
			R0[1] = m_start[1];
			R0[2] = m_start[2];

			Rd[0] = xDiff / mag;
			Rd[1] = yDiff / mag;
			Rd[2] = zDiff / mag;
			

	bool check =false;

	float max = 0.0;
	int object_id = -1;

	if (checklight(Rd,R0,light0_pos)){
		camera_toggle = 2;
	}else if( checklight(Rd,R0,light1_pos)){
		camera_toggle = 3;
	}else{
		camera_toggle = 1;
	}

	for (int i = 0; i < object_list.size(); i++)
	{
		if (object_list[i].check(Rd,R0))
		{
			if (object_list[i].TdDistance(camPos[0],camPos[1],camPos[2]) > max){

				max = object_list[i].TdDistance(camPos[0],camPos[1],camPos[2]);
				object_id = object_list[i].getid();
			}

			check = true;
			break;
		}
	}
	if (check == false ){
				Select_object = false;
			current_Select = -1;			
	}

	if (object_id != -1) {
		current_Select = object_id;
			Select_object = true;
	}
	
	Select_this(current_Select);

	}
}
if (btn == GLUT_RIGHT_BUTTON){
		if (state == GLUT_DOWN){

			if( Select_object){

				eraseItem(current_Select);
				current_Select = -1;
				Select_object = false;

			}
}}

}
	


void passive(int x, int y)
{
 if(x <= 30)
 {
  caleft = true;
 }
 else if (x >= 570)
 {
  caright = true;
 }
 else
 {
  caleft = false;
  caright = false;
 }

 
}

void cameraRotete()
{
 if (caleft)
 {
  angle = angle + 0.01;//left
  camPos[0] = camTarget[0] + (dis)* cos(angle);
  camPos[2] = camTarget[2] + (dis)* sin(angle);
 }
 if (caright)
 {
  angle = angle - 0.01;//left
  camPos[0] = camTarget[0] + (dis)* cos(angle);
  camPos[2] = camTarget[2] + (dis)* sin(angle);
 }
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_FLAT);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 500);


			m_start[0] = 0;
			m_start[1] = 0;
			m_start[2] = 0;

			m_end[0] = 0;
			m_end[1] = 0;
			m_end[2] = 0;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cameraRotete();
	gluLookAt(camPos[0], camPos[1], camPos[2], camTarget[0], camTarget[1], camTarget[2], 0, 1, 0);
	drawFloor();
	glScalef(5.0f, 5.0f, 5.0f);
	

	glPushMatrix();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glVertex3f(m_start[0], m_start[1], m_start[2]);
		glVertex3f(m_end[0], m_end[1], m_end[2]);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		
		glPushMatrix();
			drawLights();
		glPopMatrix();

		glPushMatrix();
			for (vector<object>::iterator it = object_list.begin(); it != object_list.end(); ++it) {
				object o3 = *it;
								
				o3.draw();

			}
		glPopMatrix();		


	glPopMatrix();
 	glPushMatrix();
    	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
		glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
   		glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
	glPopMatrix();


	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w, 0, h);
	gluPerspective(45, (float)((w + 0.0f) / h), 1, 500);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void FPSTimer(int value) { //60fps
	glutTimerFunc(17, FPSTimer, 0);
	glutPostRedisplay();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Modeller");	//creates the window


	dis = Distance(camTarget[0], camTarget[2], camPos[0], camPos[2]);
	 getInitialAngle(dis);


	glutDisplayFunc(display);

	//keyboard callback
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutSpecialFunc(special);
	glutPassiveMotionFunc(passive);


	//resize callback
	glutReshapeFunc(reshape);

	//fps timer callback
	glutTimerFunc(17, FPSTimer, 0);
 		glEnable(GL_TEXTURE_2D);
	//setup_texture();

	init();
	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}