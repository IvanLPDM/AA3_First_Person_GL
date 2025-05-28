#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>


//Constantes
#define M_PI 3.141592f

//Enums
enum DayState { AMANECER, MEDIODIA, ATARDECER, NOCHE };

//Variables
//Sun
float angle_sun = 0;
float radius_sun = 8;
float vel_sun = 0.5;
float rotation_sun = 0;

//Camara
float camX = 0.0f, camY = 0.5f, camZ = 2.0f;  // Posición
float camYaw = 0.0f, camPitch = 0.0f;        // Rotación
float camSpeed = 0.05f;

int lastMouseX = 600, lastMouseY = 600;  // Centro inicial
bool firstMouse = true;

bool warpPointer = false;

DayState currentState = AMANECER;



// Función para inicializar la configuración de OpenGL
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Establece el color de fondo en negro
    glEnable(GL_DEPTH_TEST);

    // Habilita la iluminación
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Habilita el uso del color del material con glColor
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Define la luz
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 5.0f, 5.0f, 5.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Material por defecto (puedes cambiarlo más adelante por objeto si quieres)
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glutSetCursor(GLUT_CURSOR_NONE); //Raton invisible
}


//Movimiento del raton------------
void keyboard(unsigned char key, int x, int y) {
    float frontX = cos(camYaw) * cos(camPitch);
    float frontY = sin(camPitch);
    float frontZ = sin(camYaw) * cos(camPitch);

    float len = sqrt(frontX * frontX + frontY * frontY + frontZ * frontZ);
    frontX /= len;
    frontY /= len;
    frontZ /= len;

    float rightX = -sin(camYaw);
    float rightZ = cos(camYaw);

    switch (key) {
    case 'w':
        camX += frontX * camSpeed;
        camY += frontY * camSpeed;
        camZ += frontZ * camSpeed;
        break;
    case 's':
        camX -= frontX * camSpeed;
        camY -= frontY * camSpeed;
        camZ -= frontZ * camSpeed;
        break;
    case 'd':
        camX += rightX * camSpeed;
        camZ += rightZ * camSpeed;
        break;
    case 'a':
        camX -= rightX * camSpeed;
        camZ -= rightZ * camSpeed;
        break;
    case 27:  // ESC
        exit(0);
        break;
    }
    glutPostRedisplay();  // Actualiza la ventana
}

void passiveMouseMotion(int x, int y) {
    int centerX = 600;
    int centerY = 600;

    if (warpPointer) {
        warpPointer = false;  // Evita bucle infinito
        return;
    }

    float sensitivity = 0.002f;
    int dx = x - centerX;
    int dy = centerY - y;

    camYaw += dx * sensitivity;
    camPitch += dy * sensitivity;

    // Limitar pitch
    if (camPitch > M_PI / 2.0f - 0.01f) camPitch = M_PI / 2.0f - 0.01f;
    if (camPitch < -M_PI / 2.0f + 0.01f) camPitch = -M_PI / 2.0f + 0.01f;

    warpPointer = true;
    glutWarpPointer(centerX, centerY);  // Vuelve al centro

    glutPostRedisplay();
}

//Escena--------------------------
void drawHouse(float x, float y) {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f); 
    glTranslatef(x, 0.2f, y);
    glutSolidCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);  
    glTranslatef(x, 0.4f, y); 
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glRotatef(45, 0.0f, 0.0f, 1.0f);
    glutSolidCone(0.5, 0.5, 4, 4);
    glPopMatrix();
}

void drawTree(float x, float y) {
    glPushMatrix();
    glColor3f(0.55f, 0.27f, 0.07f);  
    glTranslatef(x, 0.0f, y);  
    glRotatef(-90, 1.0f, 0.0f, 0.0f);  
    glutSolidCone(0.1f, 1.0f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0f, 1.0f, 0.0f); 
    glTranslatef(x, 1.0f, y);  
    glutSolidSphere(0.3, 20, 20);  
    glPopMatrix();
}

void drawStone(float x, float y) {
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    glTranslatef(x, 0.0f, y); 
    glutSolidSphere(0.3, 20, 20);  
    glPopMatrix();
}


void drawObjects()
{
    // Dibujar la esfera en movimiento
    glPushMatrix();
    float x = radius_sun * cos(angle_sun * M_PI / 180.0f);
    float y = radius_sun * sin(angle_sun * M_PI / 180.0f);
    glTranslatef(x, y, 0.0f);  // Mover la esfera en un círculo
    glRotatef(rotation_sun, 0.0f, 0.0f, 1);
    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(1, 20, 20);
    glPopMatrix();

    //movimiento de la luz con el sol
    GLfloat light_position[] = { x, y, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    //Ground
    glColor3f(0.2f, 0.6f, 0.2f);  // Color verde para el plano
    glBegin(GL_QUADS);
    glVertex3f(-4.0f, -0.05f, -4.0f);
    glVertex3f(-4.0f, -0.05f, 4.0f);
    glVertex3f(4.0f, -0.05f, 4.0f);
    glVertex3f(4.0f, -0.05f, -4.0f);
    glEnd();

    drawHouse(2,2);
    drawHouse(1,1);
    drawHouse(-1,-1);
    drawTree(0, 0);
    drawTree(-1.5, -1.5);
    drawTree(2.2, 2.4);
    drawStone(.5, .5);
    drawStone(-0.2, 0.2);
}

//Estado del día
void updateDayState() {

    float intensity = 0;

    if (angle_sun >= 45 && angle_sun < 135)
    {
        currentState = MEDIODIA;
        intensity = 1;
    }
    else if (angle_sun >= 135 && angle_sun < 225)
    {
        currentState = ATARDECER;
        intensity = .6;
    }
    else if (angle_sun >= 225 && angle_sun < 315)
    {
        currentState = NOCHE;
        intensity = 0;
    }
    else
    {
        currentState = AMANECER;
        intensity = .4;
    }

    float r = 0.2f + 0.5f * intensity;
    float g = 0.2f + 0.6f * intensity;
    float b = 0.3f + 0.7f * intensity;

    glClearColor(r, g, b, 1.0f);

    // Ajustar intensidad general
    GLfloat light_diffuse[] = { intensity, intensity, intensity, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);



    switch (currentState) {
    case AMANECER:
        printf("Estado del día: Amanecer\n");
        break;
    case MEDIODIA:
        printf("Estado del día: Mediodía\n");
        break;
    case ATARDECER:
        printf("Estado del día: Atardecer\n");
        break;
    case NOCHE:
        printf("Estado del día: Noche\n");
        break;
    }
}


// Función para dibujar la escena
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //Limpiar Buffer

    //Perspectiva
    glViewport(0, 0, 1200, 1200);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 10.0);  // Proyección en perspectiva

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Mouse Movement
    float frontX = cos(camYaw) * cos(camPitch);
    float frontY = sin(camPitch);
    float frontZ = sin(camYaw) * cos(camPitch);

    gluLookAt(camX, camY, camZ,
        camX + frontX, camY + frontY, camZ + frontZ,
        0.0f, 1.0f, 0.0f);

    drawObjects();

    

    glFlush();  //Final
}

void update(int value) {


    angle_sun += vel_sun;

    if (angle_sun >= 360.0f)
    {
        angle_sun -= 360.0f;
    }

    rotation_sun += 1.0f;
    if (rotation_sun > 360.0f) rotation_sun -= 360.0f;

    updateDayState();

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


// Función principal para inicializar y ejecutar el programa
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 1200);  // Tamaño de la ventana
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Base");

    init();
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);  // Iniciar update
    glutPassiveMotionFunc(passiveMouseMotion); //Movimiento del ratón

    glutMainLoop();  // Inicia el ciclo principal de GLUT
    return 0;
}