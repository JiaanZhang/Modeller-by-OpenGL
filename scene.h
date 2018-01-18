#include <string>
using namespace std;
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
class object
{
public:
	object();
	object(int shape, int id);
	object(int id, float position0, float position1, float position2, float angle0, float angle1, float angle2, float size, int shape, int material);
	void draw();
	void rotation(float angle, int axis);
	void translate(float distance, int axis);
	void scale(float size);
	void changeSelect();
	void selectTrue();
	void selectFalse();
	int id;
	int getid();
	string toString();
	bool check(double Rd[3], double R0[3]);
	void scaleUp(float scale);
	void changeMaterial(int i);
	float TdDistance(float x, float y, float z);

private:
	float position[3] = { 0.0,0.0,0.0 };
	float angle[3] = { 0,0,0 };
	float size = 1.0;
	bool isSelected = true;
	int shape = 2;
	bool pick = false;
	int material = 2;

	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emission[4];
	float shiny;
};

class Point
{
};

float distance(float a[3], float b[3]);
string convert(float number);
inline const char * const BoolToString(bool b);
