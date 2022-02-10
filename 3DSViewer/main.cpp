/*********************************************************************************************\
|	
|	Mark M. Lanning
|	ME 519
|	
|	3DS Loader - Main.cpp
|		simple 3DS texture Loading and mapping program
|
\*********************************************************************************************/
// Press 'v' toggels the wireframe mode on and off
// Press SPACE to stop the rotation of the objects
//*********************************************************************************************/

#include <Windows.h>
#include "bmp.h"
//includes for working with text to the OpenGL screen
#include <iostream>	//for posting stuff to the console window
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <gl\gl.h>			// Header File For The OpenGL32 Library

#include <GL\glut.h>

#include <gl\gl.h>			// Header File For The OpenGL32 Library

#include <gl\glu.h>			// Header File For The GLu32 Library

#include <stdio.h>
#include "TextureObjects.h"	// Header File For the texture loader library
#include "main.h"
#include "3ds.h"			// Include the 3DS header file.
#include <scrnsave.h>
#include <commctrl.h>
#include <processenv.h>



//#pragma comment(lib, "scrnsave.lib")	//This lib is for Multi-byte, not Unicode
//#pragma comment(lib, "ComCtl32.lib")
using namespace std;


BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}


//3DS Variable Information


//#define FILE_TEXTURE_3DS "skull.3DS"
char *FILE_TEXTURE_3DS;

CLoad3DS g_Load3ds;	// This is 3DS class.  This should go in a good model class.
t3DModel g_3DModel;	// This holds the 3D Model info that we load in
UINT g_Texture[10] = {0};// This holds the texture info, referenced by an ID

int   g_ViewMode	  = GL_TRIANGLES;// We want the default drawing mode to be normal
bool  g_RotateObject  = true;	// Turn lighting on initially
float g_RotateX		  = 0.0f;	// This is the current value at which the model is rotated
float g_RotationSpeed = 0.8f;	// This is the speed that our model rotates.  (-speed rotates left)


//set up a structure for the texture system
CTextures Textures;

float	xy_aspect;				//aspect ratio variable

const bool DEBUG = false;		//Specify if information is written about what is going on to the console window



/******************************************* Load3DSObject *******************************************/
void Load_3DS_Object()
{// Load the *.3DS file.  We just pass in an address to our t3DModel structure and the file name string we want to load
	g_Load3ds.Import3DS(&g_3DModel, FILE_TEXTURE_3DS);			// Load our .3DS file into our model structure

	// Depending on how many textures we found, load each one
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
		{
			// We pass in our global texture array, the name of the texture, and an ID to reference it.	
			Textures.LoadTexture(g_3DModel.pMaterials[i].strFile, g_Texture[i]);				
		}

		// Set the texture ID for this material
		g_3DModel.pMaterials[i].texureId = i;
	}
}



/******************************************* DrawObject *******************************************/
void Draw_3DS_Object(int pX, int pY, int pZ, int pSize)
{
	// We want the model to rotate around the axis so we give it a rotation
	// value, then increase/decrease it. You can rotate right of left with the arrow keys.
	glPushMatrix();

	glRotatef(g_RotateX, 0, 1.0f, 0);							// Rotate the object around the Y-Axis	
	if (g_RotateObject)	{	g_RotateX += g_RotationSpeed;	}	// Increase the speed of rotation
	
	// We have a model that has a certain amount of objects and textures.  We want to go through each object 
	// in the model, bind it's texture map to it, then render it by going through all of it's faces (Polygons).  	
	
	// Since we know how many objects our model has, go through each of them.
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if(g_3DModel.pObject.size() <= 0) break;

		// Get the current object that we are displaying
		t3DObject *pObject = &g_3DModel.pObject[i];
			
		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture) 
		{
			// Turn on texture mapping and turn off color
			glEnable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);

			// Bind the texture map to the object by it's materialID
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} 
		else 
		{
			// Turn off texture mapping and turn on color
			glDisable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);
		}

		// This determines if we are in wireframe or normal mode
		glBegin(g_ViewMode);					// Begin drawing with our selected mode (triangles or lines)

			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Go through each corner of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the index for each point of the face
					int index = pObject->pFaces[j].vertIndex[whichVertex];
			
					// Give OpenGL the normal for this vertex.
					glNormal3f(pObject->pNormals[index].x, pObject->pNormals[index].y, pObject->pNormals[index].z);
				
					// If the object has a texture associated with it, give it a texture coordinate.
					if(pObject->bHasTexture) 
					{

						// Make sure there was a UVW map applied to the object or else it won't have tex coords.
						if(pObject->pTexVerts) 
						{
							glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
						}
					} 
					else 
					{
						// if the size is at > 1 and material ID != -1, then it is a valid material.
						if(g_3DModel.pMaterials.size() && pObject->materialID >= 0) 
						{
							// Get and set the color that the object is, since it must not have a texture
							BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;

							// Assign the current color to this model
							glColor3ub(pColor[0], pColor[1], pColor[2]);
						}
					}

					// Pass in the current vertex of the object (Corner of current face)
					glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
				}
			}

		glEnd();			// End the model drawing
	}

	glPopMatrix();
}


/******************************************* myIdle *******************************************/
void myIdle( ) 
{
	//ask for a redraw
	glutPostRedisplay();
}


/******************************************* myDisplay *******************************************/
void myDisplay(void)
{
	// Clear The Screen And The Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Set the view so y is up, view point is 2,2,2 and lookat is 0,0,0 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 30, 60,	0.0, 0.0, 0.0,		0.0, 1.0, 0.0);		//(Camera POS  |  ObjectAT  |  UP Vector)
	
	//Draw object to the screen
	Draw_3DS_Object(0,0,0,20);

	
	//swap the drawing buffer with the display one
	glutSwapBuffers();
	
}




/******************************************* myResize *******************************************/
void myResize( int width, int height ) 
{
	//get the viewport of GLUI and then set the viewport back to that after the resize
	glViewport(0, 0, width, height);

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	//figure out aspect ratio
	xy_aspect = (float)width / (float)height;

	// Set the clipping volume
	gluPerspective(45, xy_aspect, 20, 300);

	//post some type of debug information
	if ( DEBUG ) {	cout << "Window Changed to (x,y): "<< width << ", " << height << endl ;   }

	//request a refresh to occur
	glutPostRedisplay();
}


/******************************************* myKeyboard *******************************************/
void myKeyboard (unsigned char key, int x, int y)
{
	switch(key)
	{
		//Exit the program if the escape key or q key are pressed
		case 27:			//Escape
		case 'q':			//Q - Key
			exit(0);
			break;
		
		case 'v':
			{
				if(g_ViewMode == GL_TRIANGLES)				// We our drawing mode is at triangles
				{	g_ViewMode = GL_LINE_STRIP;		}		// Go to line stips
				else 
				{	g_ViewMode = GL_TRIANGLES;		}		// Go to triangles
			}
			break;
		case 'V':
			{
				if(g_ViewMode == GL_TRIANGLES)				// We our drawing mode is at triangles
				{	g_ViewMode = GL_LINE_STRIP;		}		// Go to line stips
				else 
				{	g_ViewMode = GL_TRIANGLES;		}		// Go to triangles
			}
			break;


		case ' ':				//Spin the Object?
			g_RotateObject = !g_RotateObject;	//flip the spin flag

		default:
			break;
	}

	//if (DEBUG) {	cout << "Keypress Occured: " << key << endl;		}
	 cout << "Keypress Occured: " << key << endl; 

}


/******************************************* main *******************************************/
void main(int argc, char** argv)
{	
	//Show header information to the screen
	cout << "ษออออออออออออออออออออออออออออออป" << endl;
	cout << "บ                              บ" << endl;
	cout << "บ     Program: 3DS Loader      บ" << endl;
	cout << "บ                              บ" << endl;
	cout << "ศออออออออออออออออออออออออออออออผ" << endl;
	
	//set up GLUT's OpenGL 
	glutInit(&argc, argv);
	if (argc < 2){
		fprintf(stderr, "\nUsage: %s inputfile\n", *argv);
		exit(1);
	}
	FILE_TEXTURE_3DS = *++argv;
	 

	//setup double buffer 
	//int max_width = glutGet(GLUT_SCREEN_WIDTH); //using these values for full screen instead of windowed mode
	//int max_height = glutGet(GLUT_SCREEN_HEIGHT);//using these values for full screen instead of windowed mode
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	
	glutInitWindowPosition(0, 0);
	//glutInitWindowSize(1920, 1080);  
	//glutInitWindowSize(max_width, max_height);  //windowed settings for max width & height
	glutCreateWindow("3DS TextureObjects - 3DS Texture Loader");
	glutFullScreen();
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	

	//Load the 3DS object
	Load_3DS_Object();

	// Turn on a lighting and enable it, we will just use the default values in this case
	// We also want color, so we turn that on
	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color

	//set up the handlers
	glutDisplayFunc(myDisplay);			//Display handler


	glutReshapeFunc(myResize);			//Resize handler
	glutIdleFunc(myIdle);				//Idle handler
	glutKeyboardFunc(myKeyboard);		//Keyboard handler
	
	//Start the OpenGL
	glutMainLoop();

}
