/*
 CSCI 480 Computer Graphics
 Assignment 1: Height Fields
 C++ starter code

 Created by Yuan Lu 2015 Fall 
 README!!!!:
 1)solved z buffer fighting
 2) use z,x,c to switch among Point, Line, Fill mode
 */

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>

using namespace std;

int g_iMenuId;
int count;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;



typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = TRANSLATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
    int i, j;
    Pic *in = NULL;
    
    if (filename == NULL)
        return;
    
    /* Allocate a picture buffer */
    in = pic_alloc(640, 480, 3, NULL);
    
    printf("File to save to: %s\n", filename);
    
    for (i=479; i>=0; i--) {
        glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                     &in->pix[i*in->nx*in->bpp]);
    }
    
    if (jpeg_write(filename, in))
        printf("File saved Successfully\n");
    else
        printf("Error in Saving\n");
    
    pic_free(in);
}

void myinit(int windowWidth, int windowHeight)
{
    /* setup gl view here */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    //setup view port 
    glViewport(0, 0, windowWidth, windowHeight); // use a screen size of WIDTH x HEIGHT
    
    //set matrixmode to be projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //set perspective 
    gluPerspective(45, 1.333, 0.01, 10000.0);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
}

void display()
{
    /* draw 1x1 cube about origin */
    /* replace this code with your height field implementation */
    /* you may also want to precede it with your*/
    
    //clear the color and depth buffer for every frame 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /*the grid to drawn by 2 loops, first loop through every pixel on the same row
    then move on to the next row (y+!)*/
    //first loop
    for(int i=0;i<g_pHeightData->ny;i++){
        //initalize drawing
        glBegin(GL_TRIANGLE_STRIP);
        //second loop, the row loop
        for(int j=0;j<g_pHeightData->nx;j++) {            
            glColor3f(PIC_PIXEL(g_pHeightData, j, i, 0)/255.0,100,150);
            glVertex3f(j, i, PIC_PIXEL(g_pHeightData, j, i, 0)/5);//draw first vertex, the bottom vertext
            glColor3f(PIC_PIXEL(g_pHeightData, j, i+1, 0)/255.0,100,150);
            glVertex3f(j, i+1, PIC_PIXEL(g_pHeightData, j, i+1, 0)/5);//second vertex, the top vertex
        }
        glEnd();//first row drawing ends
        // now move on to the next row 
    }
    
    //when either lefr or middle mouse button is pressed
    if (g_iLeftMouseButton || g_iMiddleMouseButton) {
        //determine transformation based on the control state
        switch (g_ControlState)
        {
            case TRANSLATE:{
                printf ("Translating! z:%f  \n", g_vLandTranslate[2]);
                glTranslatef(g_vLandTranslate[0]*20, g_vLandTranslate[1]*20, g_vLandTranslate[2]*20 );
                break;
            }
            case ROTATE:{
                printf ("Rotating! z:%f  \n", g_vLandRotate[2]);
                glTranslatef(g_pHeightData->nx/2,g_pHeightData->ny/2,0);
                glRotatef(g_vLandRotate[0], 1.0, 0.0, 0.0);
                glRotatef(g_vLandRotate[1], 0.0, 1.0, 0.0);
                glRotatef(g_vLandRotate[2], 0.0, 0.0, 1.0);
                glTranslatef(-g_pHeightData->nx/2,-g_pHeightData->ny/2,0);   
                break;
            }
            case SCALE:{
                printf ("Scaling! z:%f  \n", g_vLandScale[2]);
                glTranslatef(g_pHeightData->nx/2,g_pHeightData->ny/2,0);
                glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);
                glTranslatef(-g_pHeightData->nx/2,-g_pHeightData->ny/2,0);
                break;
            }
        }
        //after each transformation, clear the arrays
        for (int i=0; i<4; i++) {
            g_vLandRotate[i] = 0.0;
            g_vLandTranslate[i] = 0.0;
            g_vLandScale[i] = 1.0;
        }
    }
    glFlush();

    char buffer[50];
    int n;
    n=sprintf (buffer, "%d.jpg", count);

  printf ("[%s]",buffer);

    saveScreenshot(buffer);
    count++;
}

void menufunc(int value)
{
    switch (value)
    {
        case 0:
            exit(0);
            break;
    }
}

void doIdle()
{
    /* do some stuff... */
    
    /* make the screen update */
    glutPostRedisplay();
}

/* converts mouse drags into information about
 rotation/translation/scaling */
void mousedrag(int x, int y)
{
    int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
    switch (g_ControlState)
    {
        case TRANSLATE:{
            if (g_iLeftMouseButton)
            {
                g_vLandTranslate[0] += vMouseDelta[0]*0.01;
                g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandTranslate[2] += vMouseDelta[1]*0.01;
            }

            break;
        }
        case ROTATE:
            if (g_iLeftMouseButton)
            {
                g_vLandRotate[0] += vMouseDelta[1];
                g_vLandRotate[1] += vMouseDelta[0];
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandRotate[2] += vMouseDelta[1];
            }

            break;
        case SCALE:
            if (g_iLeftMouseButton)
            {
                g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
                g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
            }

            break;
    }
    g_vMousePos[0] = x;
    g_vMousePos[1] = y; 
}

void mouseidle(int x, int y)
{
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            g_iLeftMouseButton = (state==GLUT_DOWN);
            break;
        case GLUT_MIDDLE_BUTTON:
            g_iMiddleMouseButton = (state==GLUT_DOWN);
            break;
        case GLUT_RIGHT_BUTTON:
            g_iRightMouseButton = (state==GLUT_DOWN);
            break;
    }
    
    switch(glutGetModifiers())
    {
        case GLUT_ACTIVE_CTRL:
            g_ControlState = TRANSLATE;
            break;
        case GLUT_ACTIVE_SHIFT:
            g_ControlState = SCALE;
            break;
        default:
            g_ControlState = ROTATE;
            break;
    }
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void MyKeyboardFunc(unsigned char Key, int x, int y)
{
    switch(Key)
    {
        case 'z':  glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
        case 'x':  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
        case 'c':  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        default: break;
    };
}

int main (int argc, char ** argv)
{
    count = 0;
    if (argc<2)
    {
        printf ("usage: %s heightfield.jpg\n", argv[0]);
        exit(1);
    }
    
    g_pHeightData = jpeg_read(argv[1], NULL);
    if (!g_pHeightData)
    {
        printf ("error reading %s.\n", argv[1]);
        exit(1);
    }
    glutInit(&argc,argv);
    

     //init and setup the window
    int windowWidth = 640;
    int windowHeight = 480;
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH );
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment 1");
    
    /* tells glut to use a particular display function to redraw */
    
    
    /* allow the user to quit using the right mouse button menu */
    g_iMenuId = glutCreateMenu(menufunc);
    glutSetMenu(g_iMenuId);
    glutAddMenuEntry("Quit",0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    /* replace with any animate code */
    glutIdleFunc(doIdle);
    /* callback for mouse drags */
    glutMotionFunc(mousedrag);
    /* callback for idle mouse movement */
    glutPassiveMotionFunc(mouseidle);
    /* callback for mouse button changes */
    glutMouseFunc(mousebutton);
    
    //setup keyboard functions 
    glutKeyboardFunc(MyKeyboardFunc);
    
    /* do initialization */
    myinit(windowWidth, windowHeight);
    glTranslatef(-g_pHeightData->nx/2,-g_pHeightData->ny/2,-g_pHeightData->nx*3);
    glColor4f(1.0,1.0,1.0,1.0);
    glutDisplayFunc(display);
    
    glutMainLoop();
    return(0);
}
