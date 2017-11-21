#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "GL\glut.h"
#include "point3.h"
#include "Facette.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

#define WIDTH  600
#define HEIGHT 600

#define KEY_ESC 27

float angleX = 0.0f; //angle de rotation en Y de la scene
float angleY = 0.0f; //angle de rotation en X de la scene

float pasDeplacement = 1.25;


//position lumiere
float xLitePos = 0;
float yLitePos = 10;
float zLitePos = -100;

float tx = 0.0;
float ty = 0.0;
float tz = 0.0;

pair<vector<point3>, vector<int>> structure;
vector<point3> normales;
vector<point3> normalesSommets;

/* initialisation d'OpenGL*/


pair<vector<point3>, vector<int>> lireFichier(string filename)
{
	string readLine;
	int delimiterPos_1, delimiterPos_2, delimiterPos_3, delimiterPos_4;
	pair<vector<point3>, vector<int>> structure;

	// Open file for reading
	ifstream in(filename.c_str());

	// Check if file is in OFF format
	getline(in, readLine);
	if (readLine != "OFF")
	{
		cout << "Le fichier n'est pas au format OFF." << endl;
		return structure;
	}
	else
	{
	cout << "Le fichier est au format OFF" << endl;
	}

	int nbSommets, nbFacettes, nbAretes;
	getline(in, readLine);
	delimiterPos_1 = readLine.find(" ", 0);
	nbSommets = atoi(readLine.substr(0, delimiterPos_1 + 1).c_str());
	delimiterPos_2 = readLine.find(" ", delimiterPos_1);
	nbFacettes = atoi(readLine.substr(delimiterPos_1, delimiterPos_2 + 1).c_str());
	delimiterPos_3 = readLine.find(" ", delimiterPos_2);
	nbAretes = atoi(readLine.substr(delimiterPos_2, delimiterPos_3 + 1).c_str());

	vector<point3> sommets = vector<point3>();
	vector<int> facettes = vector<int>();

	for (int n = 0; n < nbSommets; n++)
	{
		float x, y, z;
		getline(in, readLine);
		delimiterPos_1 = readLine.find(" ", 0);
		x = atof(readLine.substr(0, delimiterPos_1).c_str());
		delimiterPos_2 = readLine.find(" ", delimiterPos_1 + 1);
		y = atof(readLine.substr(delimiterPos_1, delimiterPos_2).c_str());
		delimiterPos_3 = readLine.find(" ", delimiterPos_2 + 1);
		z = atof(readLine.substr(delimiterPos_2, delimiterPos_3).c_str());
		point3 sommet = point3(x, y, z);
		sommets.push_back(sommet);
	}

	for (int n = 0; n < nbFacettes; n++)
	{
		float x, y, z, nbSom;
		getline(in, readLine);
		delimiterPos_1 = readLine.find(" ", 0);
		nbSom = atof(readLine.substr(0, delimiterPos_1).c_str());
		delimiterPos_2 = readLine.find(" ", delimiterPos_1 + 1);
		x = atof(readLine.substr(delimiterPos_1, delimiterPos_2).c_str());
		delimiterPos_3 = readLine.find(" ", delimiterPos_2 + 1);
		y = atof(readLine.substr(delimiterPos_2, delimiterPos_3).c_str());
		delimiterPos_4 = readLine.find(" ", delimiterPos_3 + 1);
		z = atof(readLine.substr(delimiterPos_3, delimiterPos_4).c_str());

		facettes.push_back(x);
		facettes.push_back(y);
		facettes.push_back(z);
	}

	
	structure = make_pair(sommets, facettes);

	return structure;
}

float valAbsolue(float nb)
{
	float result = nb;

	if (result < 0)
		result *= -1;

	return result;
}

vector<point3> calculNormales(vector<point3> sommets, vector<int> facettes)
{
	vector<point3> normalesCalculee = vector<point3>();

	for (int i = 0; i < facettes.size() - 3; i = i + 3)
	{
		point3 vecteurAB = sommets[facettes[i + 1]] - sommets[facettes[i]];
		point3 vecteurAC = sommets[facettes[i + 2]] - sommets[facettes[i]];

		point3 normale = point3();
		normale.x = ((vecteurAB.y * vecteurAC.z) - (vecteurAB.z * vecteurAC.y));
		normale.y = ((vecteurAB.z * vecteurAC.x) - (vecteurAB.x * vecteurAC.z));
		normale.z = ((vecteurAB.x * vecteurAC.y) - (vecteurAB.y * vecteurAC.x));

		normalesCalculee.push_back(normale.normalize());
	}

	return normalesCalculee;
}

vector<point3> calculNormalesSommets(vector<point3> sommets, vector<int> facettes, vector<point3> normales)
{
	vector<point3> normalesSommetsCalculees = vector<point3>();

	for (int i = 0; i < sommets.size(); i++)
	{
		if (i % 100 == 0)
		{
			cout << i << endl;
		}
		vector<point3> normalesSommet = vector<point3>();

		for (int j = 0; j < facettes.size() - 3; j = j + 3)
		{
			if (facettes[j] == i || facettes[j + 1] == i || facettes[j + 2] == i)
			{
				normalesSommet.push_back(normales[j/3]);
			}
		}

		point3 sommeNormales = point3(0, 0, 0);

		for (auto n : normalesSommet)
		{
			sommeNormales = sommeNormales + n;
		}

		normalesSommetsCalculees.push_back(sommeNormales / normalesSommet.size());

	}

	return normalesSommetsCalculees;
}

static void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	structure = lireFichier("buddha.off");
	//structure = lireFichier("max.off");
	//structure = lireFichier("triceratops.off");
	normales = calculNormales(structure.first, structure.second);
	normalesSommets = calculNormalesSommets(structure.first, structure.second, normales);
}

void traceMaillage(vector<point3> sommets, vector<int> facettes, point3 position, float coefDivision)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < facettes.size() - 3; i = i + 3)
	{
		glColor3f(0.7, 0.7, 0.7);
		glBegin(GL_TRIANGLES);
		glVertex3f((sommets[facettes[i]].x - position.x) / coefDivision, (sommets[facettes[i]].y - position.y) / coefDivision, (sommets[facettes[i]].z - position.z) / coefDivision);
		glVertex3f((sommets[facettes[i + 1]].x - position.x) / coefDivision, (sommets[facettes[i + 1]].y - position.y) / coefDivision, (sommets[facettes[i + 1]].z - position.z) / coefDivision);
		glVertex3f((sommets[facettes[i + 2]].x - position.x) / coefDivision, (sommets[facettes[i + 2]].y - position.y) / coefDivision, (sommets[facettes[i + 2]].z - position.z) / coefDivision);
		glEnd();
	}
}

void traceNormales()
{
	for (int i = 0 ; i < normalesSommets.size() ; i++)
	{
		glBegin(GL_LINES);
			glColor3f(0.f, 1.f, 0.f);
			glVertex3f(structure.first[i].x, structure.first[i].y, structure.first[i].z);
			glVertex3f((structure.first[i].x + normalesSommets[i].x)*2, (structure.first[i].y + normalesSommets[i].y)*2, (structure.first[i].z + normalesSommets[i].z)*2);
		glEnd();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //effacement du buffer

														//Description de la scene
	glLoadIdentity();
	//rotation de la scene suivant les mouvements de la souris
	glRotatef(-angleY, 0.0f, 0.0f, 1.0f);
	glRotatef(angleX, 0.0f, 1.0f, 0.0f);
	glTranslatef(tx, ty, tz);

	glRotatef(-angleY, 0.0f, 0.0f, 1.0f);
	glRotatef(angleX, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float maxCoordonnee = 0;

	for(auto p : structure.first)
	{
		positionX += p.x;
		positionY += p.y;
		positionZ += p.z;

		if (valAbsolue(p.x) > maxCoordonnee)
			maxCoordonnee = valAbsolue(p.x);
		if (valAbsolue(p.y) > maxCoordonnee)
			maxCoordonnee = valAbsolue(p.y);
		if (valAbsolue(p.z) > maxCoordonnee)
			maxCoordonnee = valAbsolue(p.z);
	}

	positionX /= structure.first.size();
	positionY /= structure.first.size();
	positionZ /= structure.first.size();

	point3 centre = point3(positionX, positionY, positionZ);

	traceMaillage(structure.first, structure.second, centre, maxCoordonnee);

	traceNormales();

	glPopMatrix();


	//affiche les axes du repere
	glColor3f(0.0, 1.0, 0.0); //Y vert
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 20, 0);
	glEnd();

	glColor3f(0.0, 0.0, 1.0); //Z bleu
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 20);
	glEnd();

	glColor3f(1.0, 0.0, 0.0); //X rouge
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(20, 0, 0);
	glEnd();

	glutSwapBuffers();// echange des buffers
}

/* Au cas ou la fenetre est modifiee ou deplacee */
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -2, 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GLvoid gestionSouris(int x, int y)
{
	angleX = x * 720 / WIDTH; //gere l'axe Oy
	angleY = -(y * 180 / HEIGHT - 90) * 4; //gere l'axe Ox

	glutPostRedisplay();
}

GLvoid gestionFleche(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		tz += int(pasDeplacement); glutPostRedisplay();
		break;

	case GLUT_KEY_DOWN:
		tz -= int(pasDeplacement); glutPostRedisplay();
		break;
		glutPostRedisplay();
	}

}

GLvoid window_key_down(unsigned char key, int x, int y)  //appuie des touches
{
	switch (key)
	{

		//deplacement de la camera
	case 'z':
		ty += int(pasDeplacement); glutPostRedisplay();
		break;

	case 's':
		ty -= int(pasDeplacement); glutPostRedisplay();
		break;

	case 'q':
		tx -= int(pasDeplacement); glutPostRedisplay();
		break;

	case 'd':
		tx += int(pasDeplacement); glutPostRedisplay();
		break;

		//sortie
	case KEY_ESC:
		exit(0);
		break;

	default:
		printf("La touche %d non active.\n", key);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInitWindowSize(400, 400);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Des triangles !");
	init();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(&window_key_down);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(gestionSouris);
	glutSpecialFunc(&gestionFleche);
	glutMainLoop();
	return 0;
}

