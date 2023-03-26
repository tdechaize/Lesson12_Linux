/*
=====================================================================
OpenGL Lesson 12 :  Creating Another OpenGL Window with glut on Linux
=====================================================================

  Authors Name: Jeff Molofee ( NeHe )

  This code was created by Jeff Molofee '99 (ported to Linux/glut by Richard Campbell '99)

  If you've found this code useful, please let me know.

  Visit me at www.demonews.com/hosted/nehe
 (email Richard Campbell at ulmont@bellsouth.net)

  Disclaimer:

  This program may crash your system or run poorly depending on your
  hardware.  The program and code contained in this archive was scanned
  for virii and has passed all test before it was put online.  If you
  use this code in project of your own, send a shout out to the author!

=====================================================================
                NeHe Productions 1999-2004
=====================================================================
*/

 /*******************************************************************
 *  Project: $(project)
 *  Function : Main program
 ***************************************************************
 *  $Author: Jeff Molofee 2000 ( NeHe )
 *  $Name:  $
 ***************************************************************
 *
 *  Copyright NeHe Production
 *
 *******************************************************************/

/**         Comments manageable by Doxygen
*
*  Modified smoothly by Thierry DECHAIZE
*
*  Paradigm : obtain one source (only one !) compatible for multiple free C Compilers
*    and provide for all users an development environment on Linux (64 bits compatible),
*    the great Code::Blocks manager (version 20.03), and don't use glaux.lib or glaux.dll.
*
*	a) gcc 11.3.0 (32 and 64 bits) version officielle : commande pour l'installation sur Linux Mint
*       -> sudo apt-get install freeglut3 libglew-dev gcc g++ mesa-common-dev build-essential libglew2.2 libglm-dev binutils libc6 libc6-dev gcc-multilib g++-multilib; option -m32 to 32 bits ; no option to 64 bits
*	b) CLANG version 14.0.6 (32 et 64 bits), adossé aux environnements gcc : commande pour l'installation sur Linux Mint
*       -> sudo apt-get install llvm clang ; options pour la compilation et l'édition de liens -> --target=i686-pc-linux-gnu (32 bits) --target=x86_64-pc-linux-gnu (64 bits)
*	c) Mingw 32 ou 64 bits version gcc version 10-win32 20220113 : commande pour l'installation sur Linux Mint (NOT YET ACTIVED - Work in progress : to verify portability of these code)
*        -> sudo apt-get install mingw64    (ATTENTION, il s'agit d'une cross génération : l'exécutable créé ne fonctionne que sur Windows !!!)
*
*  Date : 2023/03/26
*
* \file            lesson12.c
* \author          Jeff Molofee ( NeHe ) originely, ported to Linux/glut by Richard Campbell, and after by Thierry Dechaize on Linux Mint
* \version         2.0.1.0
* \date            26 mars 2023
* \brief           Ouvre une simple fenêtre Wayland on Linux et dessine des cubes rattachés à une liste unique d'objets, dont la texture des faces est issue d'un fichier BMP avec OpenGL + glut.
* \details         Ce programme gére plusieurs événements : le clic sur le bouton "Fermé" de la fenêtre, la sortie du programme par la touche ESCAPE ou par les touches "q" ou "Q" [Quit]
* \details                                                  les touches "f" ou "F" qui active ou non le "Full Screen" et les touches spéciales UP, DOWN, LEFT, RIGTH qui agissent
* \details                                                  sur les rotations respectivement selon l'orientation sur TOUs les membres de la liste des objets représentés.
*
*/

#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	    // Header File For The OpenGL32 Library
#include <GL/glu.h>	    // Header File For The GLu32 Library

#define _XOPEN_SOURCE   600  // Needed because use of function usleep depend of these two define ...!!! However function usleep appear like "... warning: implicit declaration of ..."
#include <unistd.h>     // Header file for sleeping (function usleep)
#include <stdio.h>      // Header file needed by use of printf in this code
#include <string.h>     // Header file needed by use of strcmp in this code
#include <stdlib.h>     // Header file needed by use of malloc/free function in this code

#include "logger.h"     //  added by Thierry DECHAIZE : logger for trace and debug if needed ... only in mode Debug !!!

/* ascii code for the escape key */
#define ESCAPE 27

char *level_debug;    // added by Thierry DECHAIZE, needed in test of logging activity (only with DEBUG mode)

/* The number of our GLUT window */
int window;

/* Indicator of Full Screen */
int nFullScreen=0;

/* floats for x rotation, y rotation, z rotation */
float xrot, yrot, zrot;

/* storage for one texture  */
GLuint texture[1];

GLuint cube;             // storage for the display list
GLuint top;              // storage for the 2nd display list
GLuint xloop;            // loop for x axis.
GLuint yloop;            // loop for y axis.

GLfloat xrot;            // rotates cube on the x axis.
GLfloat yrot;            // rotates cube on the y axis.

// colors for boxes.
static GLfloat boxcol[5][3]=
{
    {1.0f,0.0f,0.0f},{1.0f,0.5f,0.0f},{1.0f,1.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,1.0f,1.0f}
};

// colors for tops of boxes.
static GLfloat topcol[5][3]=
{
    {.5f,0.0f,0.0f},{0.5f,0.25f,0.0f},{0.5f,0.5f,0.0f},{0.0f,0.5f,0.0f},{0.0f,0.5f,0.5f}
};

/* Image type - contains height, width, and data */
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;

// build the display list.

/**	            This function construct the list of all objects with instructions OpenGL
*
* \brief      Fonction BuildList: fonction construisant la liste des objets à afficher avec des instructions OpenGL
* \details    Aucun paramètre en entrée, et rien en retour (GLVoid).
* \return     GLvoid              aucun retour.
*
*/

GLvoid BuildList()
{
    cube = glGenLists(2);              // generate storage for 2 lists, and return a pointer to the first.
    glNewList(cube, GL_COMPILE);       // store this list at location cube, and compile it once.

    // cube without the top;
    glBegin(GL_QUADS);			// Bottom Face

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad

    // Front Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad

    // Back Face
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad

    // Right face
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad

    // Left Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad

    glEnd();
    glEndList();

    top = cube + 1;                    // since we generated 2 lists, this is where the second is...1 GLuint up from cube.
    glNewList(top, GL_COMPILE);        // generate 2nd list (top of box).

    glBegin(GL_QUADS);
    // Top Face
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glEnd();

    glEndList();
}

/*
 * getint and getshort are help functions to load the bitmap byte by byte on
 * SPARC platform.
 * I've got them from xv bitmap load routine because the original bmp loader didn't work
 * I've tried to change as less code as possible.
 */

static unsigned int getint(fp)
     FILE *fp;
{
  int c, c1, c2, c3;

  // get 4 bytes
  c = getc(fp);
  c1 = getc(fp);
  c2 = getc(fp);
  c3 = getc(fp);

  return ((unsigned int) c) +
    (((unsigned int) c1) << 8) +
    (((unsigned int) c2) << 16) +
    (((unsigned int) c3) << 24);
}

static unsigned int getshort(fp)
     FILE *fp;
{
  int c, c1;

  //get 2 bytes
  c = getc(fp);
  c1 = getc(fp);

  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.

/**	            This function load image stored in file BMP (quick and dirty bitmap loader !! bug on SOLARIS/SPARC, two functions added below correct this code)
*
* \brief      Fonction ImageLoad : fonction chargeant une image stockée dans un fichier BMP
* \details    En entrée, le nom du fichier stockant l'image, en retour l'image chargée en mémoire.
* \param	  *filename			Le nom du fichier stockant l'image					*
* \param	  *image			l'image chargée en mémoire					*
* \return     int               un entier de type booléen (0 / 1).
*
*/

int ImageLoad(char *filename, Image *image) {
    FILE *file;
#ifdef __x86_64__
    unsigned int size;                  // size of the image in bytes
#else
    unsigned long size;                 // size of the image in bytes
#endif

#ifdef __x86_64__
    unsigned int i;                     // standard counter (4 bytes stored value)
#else
    unsigned long i;                    // standard counter (4 bytes stored value)
#endif

    unsigned short int planes;          // number of planes in image (must be 1)
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // used to convert bgr to rgb color.

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function ImageLoad.");
#endif // defined DEBUG

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL) {
      printf("File Not Found : %s\n",filename);
      return 0;
    }

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // No 100% errorchecking anymore!!!

    // read the width
    image->sizeX = getint (file);
    printf("Width of %s: %lu\n", filename, image->sizeX);

    // read the height
    image->sizeY = getint (file);
    printf("Height of %s: %lu\n", filename, image->sizeY);

    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    planes = getshort(file);
    if (planes != 1) {
	  printf("Planes from %s is not 1: %u\n", filename, planes);
	  return 0;
    }

    // read the bpp
    bpp = getshort(file);
    if (bpp != 24) {
      printf("Bpp from %s is not 24: %u\n", filename, bpp);
      return 0;
    }

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	  printf("Error allocating memory for color-corrected image data");
	  return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	  printf("Error reading image data from %s.\n", filename);
	  return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	  temp = image->data[i];
	  image->data[i] = image->data[i+2];
	  image->data[i+2] = temp;
    }

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function ImageLoad.");
#endif // defined DEBUG

    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures

/**	            This function load textures used for object with instructions OpenGL
*
* \brief      Fonction LoadGLTextures : fonction chargeant la texture à appliquer à un objet avec des instructions OpenGL
* \details    Aucun paramètre en entrée, et rien en retour (GLVoid).
* \return     GLvoid              aucun retour.
*
*/

void LoadGLTextures() {
    // Load Texture
    Image *image1;

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function LoadGLTextures.");
#endif // defined DEBUG

    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	  printf("Error allocating space for image");
	  exit(0);
    }

    if (!ImageLoad("../../Data/lesson12/cube.bmp", image1)) {
	  exit(1);
    }

    // Create Texture
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale linearly (use mipmaps) when image smaller than texture

    // 2d texture, 3 components (red, green, blue), x size from image, y size from image,
    // rgb color data, unsigned byte data, and finally the data itself.
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1->sizeX, image1->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function LoadGLTextures.");
#endif // defined DEBUG
};

/* A general OpenGL initialization function.  Sets all of the initial parameters. */

/**	            This function initialize the basics characteristics of the scene with instructions OpenGL (background, depth, type of depth, reset of the projection matrix, ...)
*
* \brief      Fonction InitGL : fonction gerant les caractéristiques de base de la scéne lorsque avec des instructions OpenGL (arrière plan, profondeur, type de profondeur, ...)
* \details    En retour les deux paramètres des nouvelles valeurs de largeur et de hauteur de la fenêtre redimensionnée.
* \param	  Width			    la largeur de la fenêtre lors de l'initialisation					*
* \param	  Height			la hauteur de la fenêtre lors de l'initialisation					*
* \return     void              aucun retour.
*
*/

void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function InitGL.");
#endif // defined DEBUG

    LoadGLTextures();				// Load The Texture(s)
    BuildList();                                // set up our display lists.
    glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);	// Clear The Background Color To Blue
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function InitGL.");
#endif // defined DEBUG
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */

/**	            This function manage the new dimension of the scene when resize of windows with instructions OpenGL
*
* \brief      Fonction ReSizeGLScene : fonction gerant les nouvelles dimensions de la scéne lorsque l'utilisateur agit sur un redimensionnement de la fenêtre
* \details    En retour les deux paramètres des nouvelles valeurs de largeur et de hauteur de la fenêtre redimensionnée.
* \param	  Width			    la  nouvelle largeur de la fenêtre redimensionnée					*
* \param	  Height			la  nouvelle hauteur de la fenêtre redimensionnée					*
* \return     void              aucun retour.
*
*/

void ReSizeGLScene(int Width, int Height)
{
#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function ResizeGLScene.");
#endif // defined DEBUG

    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
	Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function ResizeGLScene.");
#endif // defined DEBUG
}

/* The main drawing function. */

/**	            This function generate the scene with injstructions OpenGL
*
* \brief      Fonction DrawGLScene : fonction generant l'affichage attendu avec des instructions OpenGL
* \details    Aucun paramètre dans ce cas de figure car tout est géré directement à l'intérieur de cette fonction d'affichage.
* \return     void              aucun retour.
*
*/

void DrawGLScene()
{
#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function DrawGLScene.");
#endif // defined DEBUG

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer

    glBindTexture(GL_TEXTURE_2D, texture[0]);

    for (yloop=1; yloop<6; yloop++) { // 5 rows of cubes.
        for (xloop=0; xloop<yloop; xloop++) {
	      glLoadIdentity();

	      glTranslatef(1.4f+(((float)xloop)*2.8f)-(((float)yloop)*1.4f),((6.0f-((float)yloop))*2.4f)-7.0f,-20.0f);

	      glRotatef(45.0f - (2.0f*yloop)+xrot, 1.0f, 0.0f, 0.0f);
	      glRotatef(45.0f + yrot, 0.0f, 1.0f, 0.0f);

	      glColor3fv(boxcol[yloop-1]);
	      glCallList(cube);

	      glColor3fv(topcol[yloop-1]);
	      glCallList(top);
        }
    }

    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function DrawGLScene.");
#endif // defined DEBUG
}

/* The function called whenever a key is pressed. */

/**	            This function inform caller with key pressed.
*
* \brief      Fonction keyPressed : fonction permettant de connaître quelle touche a été tapée au clavier
* \details    Le premier paarmètre en retour de cette fonction correspond à la touche précédemment appuyée.
* \param	  key			    la touche appuyée 					*
* \param	  x             	une position x au niveau du clavier *
* \param	  y             	une position y au niveau du clavier *
* \return     void              aucun retour.
*
*/

void keyPressed(unsigned char key, int x, int y)
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key){

	//Mode plein écran : il suffit de taper au clavier sur la touche F majuscule ou f minuscule en mode flip-flop
		case 'f' :
		case 'F' :
			if (nFullScreen==0)
			{
				glutFullScreen();
				nFullScreen=1;
				break;
			}
			if (nFullScreen==1)
			{
		        glutReshapeWindow(640, 480);
				glutPositionWindow (0, 0);
				nFullScreen=0;
				break;
			}
	//Quitter
		case 'q' :
		case 'Q' :
		case ESCAPE  : // Touche ESCAPE (code ascii 27) : il s'agit de sortir proprement de ce programme
            glutDestroyWindow(window);
			exit (0);
			break;

		default :
//			printf(" Touche: %c\n Souris a: %d %d \n",key,x,y);
			break;

	}
}

/* The function called whenever a normal key is pressed. */

/**	            This function inform caller with key pressed.
*
* \brief      Fonction specialKeyPressed : fonction permettant de connaître quelle touche spéciale a été tapée au clavier
* \details    Le premier paarmètre en retour de cette fonction correspond à la touche précédemment appuyée.
* \param	  key			    la touche appuyée 					*
* \param	  x             	une position x au niveau du clavier *
* \param	  y             	une position y au niveau du clavier *
* \return     void              aucun retour.
*
*/

void specialKeyPressed(int key, int x, int y)
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {
    case GLUT_KEY_UP:
	xrot -= 0.2f;
	break;

    case GLUT_KEY_DOWN:
	xrot += 0.2f;
	break;

    case GLUT_KEY_LEFT:
	yrot += 0.2f;
	break;

    case GLUT_KEY_RIGHT:
	yrot -= 0.2f;
	break;

    default:
	printf ("Special key %d pressed. No action there yet.\n", key);
	break;
    }
}

/* Main function : GLUT runs as a console application starting with program call main()  */

/**         Comments manageable by Doxygen
*
* \brief      Programme Main obligatoire pour les programmes sous Linux (OpenGL en mode console).
* \details    Programme principal de lancement de l'application qui appelle ensuite toutes les fonctions utiles OpenGL et surtout glut.
* \param      argc         le nombre de paramètres de la ligne de commande.
* \param      argv         un pointeur sur le tableau des différents paramètres de la ligne de commande.
* \return     int          un entier permettant de connaître la statut du lancement du programme.
*
*/

int main(int argc, char **argv)
{

   if (getenv("LEVEL")) {                 // LEVEL is set
       level_debug=getenv("LEVEL");           // Added by Thierry DECHAIZE : récupérer la valeur de la variable d'environnement LEVEL si elle existe
       }
    else {
       snprintf(level_debug,2,"%s"," ");
    }

#ifdef DEBUG
    printf("Niveau de trace : %s.\n",level_debug);

    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Enter within main, before call of glutInit.");
#endif // defined DEBUG

    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */

/**	            This Code initialize the context of windows on Wayland with glut.
*
* \brief      Appel de la fonction glutInit : fonction Glut d'initialisation
* \details    En entrée de cette fonction, les paramètres de la ligne de commande.
* \param	  argc			    le nombre de paramètres mis à disposition			*
* \param	  argv              Hle tableau des différents paramètres mis à disposition *
* \return     int               un integer.
*
*/

    glutInit(&argc, argv);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutInitDisplayMode.");
#endif // defined DEBUG

    /* Select type of Display mode:
     Double buffer
     RGBA color
     Alpha components supported
     Depth buffer */

/**	            This Code initialize le mode d'affichage défini avec une fonction glut.
*
* \brief      Appel de la fonction glutInitDisplayMode(: fonction Glut initialisant le mode d'affichage.
* \details    En entrée de cette fonction, des paramètres séparés par des "ou logique".
* \param	  const         une succession deparamètres séparés par des "ou logique".
* \return     void          aucun retour de fonction.
*
*/

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutInitWindowSize");
#endif // defined DEBUG

    /* get a 640 x 480 window */

/**	            This Code initialize the dimensions (width & height) of the window into screen.
*
* \brief      Appel de la fonction glutInitWindowSize : fonction Glut initialisant la position de la fenêtre dans l'écran.
* \details    En entrée de cette fonction, les deux paramètres X et Y correspondant à la taille de la fenêtre dans l'écran (deux dimensions)
* \param	  X			    la largeur de la fenêtre en x
* \param	  Y			    la hauteur de la fenêtre en y
* \return     void          aucun retour de fonction.
*
*/

    glutInitWindowSize(640, 480);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutInitWindowPosition");
#endif // defined DEBUG

    /* the window starts at the upper left corner of the screen */

/**	            This Code initialize the position of the window into screen.
*
* \brief      Appel de la fonction glutInitWindowPosition : fonction Glut initialisant la position de la fenêtre dans l'écran.
* \details    En entrée de cette fonction, les deux paramètres X et Y de positionnement de la fenêtre dans l'écran (deux dimensions)
* \param	  X			    le positionnement de la fenêtre en x
* \param	  Y			    le positionnement de la fenêtre en y
* \return     void          aucun retour de fonction.
*
*/

    glutInitWindowPosition(0, 0);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutCreateWindow");
#endif // defined DEBUG

    /* Open a window */

/**	            This Code create windows on Wayland with glut.
*
* \brief      Appel de la fonction glutCreateWindow : fonction Glut créant une fenêtre Wayland avec glut.
* \details    En entrée de cette fonction, l'identification de la fenêtre (.id. son nom).
* \param	  tittle			le nom de la fenêtre
* \return     int               l'identifiant entier de la fenêtre créee.
*
*/

    window = glutCreateWindow("Jeff Molofee's GL Tutorial : draw list of cubes textured (issued BMP) - NeHe '99");

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutDisplayFunc");
#endif // defined DEBUG

    /* Register the function to do all our OpenGL drawing. */

/**	            This Code rely the internal function DrawGLScene at the display function of glut.
*
* \brief      Appel de la fonction glutDisplayFunc : fonction Glut permettant d'activer la fonction interne d'affichage.
* \details    En entrée de cette fonction, l'adresse de la fonction interne appellée .
* \param	  &function			l'adresse de la fonction interne d'affichage.
* \return     void              aucun retour de fonction.
*
*/

    glutDisplayFunc(&DrawGLScene);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutIdleFunc");
#endif // defined DEBUG

    /* Even if there are no events, redraw our gl scene. */

/**	            This Code rely the internal function DrawGLScene at the Idle Function of glut.
*
* \brief      Appel de la fonction glutIdleFunc : fonction d'attente de Glut permettant d'activer la fonction interne d'affichage.
* \details    En entrée de cette fonction, l'adresse de la fonction interne appellée .
* \param	  &function			l'adresse de la fonction interne d'affichage.
* \return     void              aucun retour de fonction.
*
*/

    glutIdleFunc(&DrawGLScene);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutReshapeFunc");
#endif // defined DEBUG

    /* Register the function called when our window is resized. */

/**	            This Code rely the internal function ResizeGLScene at the reshape function of glut.
*
* \brief      Appel de la fonction glutReshapeFunc : fonction Glut permettant d'activer la fonction interne de changement des dimensions d'affichage.
* \details    En entrée de cette fonction, l'adresse de la fonction interne appellée.
* \param	  &ResizeGLScene	l'adresse de la fonction interne traitant des changements de dimension de l'affichage.
* \return     void              aucun retour de fonction.
*
*/

    glutReshapeFunc(&ReSizeGLScene);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutKeyboardFunc");
#endif // defined DEBUG

    /* Register the function called when the keyboard is pressed. */

/**	            This Code rely the internal function keyPressed at the keyboard function of glut (normal touchs).
*
* \brief      Appel de la fonction glutKeyboardFunc : fonction Glut permettant de recupérer la touche appuyée sur le clavier (touches normales).
* \details    En entrée de cette fonction, l'adresse de la fonction interne gérant les appuis sur le clavier.
* \param	  &keyPressed			l'adresse de la fonction interne gérant les appuis sur le clavier (touches normales).
* \return     void                  aucun retour de fonction.
*
*/

    glutKeyboardFunc(&keyPressed);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutSpecialFunc");
#endif // defined DEBUG

    /* Register the function called when special keys (arrows, page down, etc) are pressed. */

/**	            This Code rely the internal function specialKeyPressed at the special keyboard function of glut (special touchs).
*
* \brief      Appel de la fonction glutSpecialFunc : fonction Glut permettant de recupérer la touche appuyée sur le clavier (touches spéciales).
* \details    En entrée de cette fonction, l'adresse de la fonction interne gérant les appuis sur le clavier.
* \param	  &specialKeyPressed			l'adresse de la fonction interne gérant les appuis sur le clavier (touches spéciales).
* \return     void                          aucun retour de fonction.
*
*/

    glutSpecialFunc(&specialKeyPressed);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of InitGL");
#endif // defined DEBUG

    /* Initialize our window. */

/**	            This Code initialize les paramètres d'affichage OpenGL.
*
* \brief      Appel de la fonction InitGL : fonction d'initialisation de la taille de la fenêtre d'affichage OpenGL (la même que celle de glut).
* \details    En entrée de cette fonction, les deux paramètres X et Y correspondant à la taille de la fenêtre OpenGL dans l'écran (deux dimensions)
* \param	  X			    la largeur de la fenêtre en x
* \param	  Y			    la hauteur de la fenêtre en y
* \return     void          aucun retour de fonction.
*
*/

    InitGL(640, 480);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step within main, before call of glutMainLoop");
#endif // defined DEBUG

    /* Start Event Processing Engine */

/**	            This Code run the permanently wait loop of events.
*
* \brief      Appel de la fonction glutMainLoop : fonction lancant la boucle d'attente des événements sous glut.
* \details    Aucun paramètre en entrée ni en sortie.
* \return     void          aucun retour de fonction.
*
*/

    glutMainLoop();

    return 1;
}

