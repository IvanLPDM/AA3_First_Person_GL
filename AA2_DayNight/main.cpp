#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

//Constantes
#define M_PI 3.141592f

//Enums
enum DayState { AMANECER, MEDIODIA, ATARDECER, NOCHE };

//Variables
//Sun
float angle_sun = 0;
float radius_sun = 8;
float vel_sun = 0.3;
float rotation_sun = 0;

//Camara
float camX = 0.0f, camY = 0.5f, camZ = 2.0f;  // Posición
float camYaw = 0.0f, camPitch = 0.0f;        // Rotación
float camSpeed = 0.05f;

int lastMouseX = 600, lastMouseY = 600;  // Centro inicial
bool firstMouse = true;

bool warpPointer = false;

//Lantern
bool linternaEncendida = false;
float lastSunIntensity = 1.0f;

DayState currentState = AMANECER;

bool shownText = false;

struct Objeto {
    float x, z;
    bool interactuado = false;
};

std::vector<Objeto> objetosInteractuables;


struct AABB {
    float xMin, xMax;
    float zMin, zMax;
};

std::vector<AABB> collisionBoxes;


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

bool checkCollision(float nextX, float nextZ) {
    float halfSize = 0.1f;
    for (const auto& box : collisionBoxes) {
        if (nextX + halfSize > box.xMin &&
            nextX - halfSize < box.xMax &&
            nextZ + halfSize > box.zMin &&
            nextZ - halfSize < box.zMax) {
            return true;
        }
    }
    return false;
}

void interactuarConObjetoCercano() {
    float rangoInteraccion = 2.0f;
    for (auto& obj : objetosInteractuables) {
        float dx = camX - obj.x;
        float dz = camZ - obj.z;
        float distancia = sqrt(dx * dx + dz * dz);

        if (distancia < rangoInteraccion && !obj.interactuado) {
            obj.interactuado = true;

            std::cout << "¡Interacción realizada con objeto en (" << obj.x << ", " << obj.z << ")!" << std::endl;
            shownText = true;
            break;
        }
    }
}


//Movimiento del raton------------
void keyboard(unsigned char key, int x, int y) {
    float frontX = cos(camYaw) * cos(camPitch);
    float frontZ = sin(camYaw) * cos(camPitch);
    float rightX = -sin(camYaw);
    float rightZ = cos(camYaw);

    float nextX = camX;
    float nextZ = camZ;

    switch (key) {
    case 'w':
        nextX += frontX * camSpeed;
        nextZ += frontZ * camSpeed;
        break;
    case 's':
        nextX -= frontX * camSpeed;
        nextZ -= frontZ * camSpeed;
        break;
    case 'd':
        nextX += rightX * camSpeed;
        nextZ += rightZ * camSpeed;
        break;
    case 'e':
    case 'E':
        interactuarConObjetoCercano();
        break;
    case 'a':
        nextX -= rightX * camSpeed;
        nextZ -= rightZ * camSpeed;
        break;
    case 27:  // ESC
        exit(0);
        break;
    }

    // Solo mover si no hay colisión
    if (!checkCollision(nextX, nextZ)) {
        camX = nextX;
        camZ = nextZ;
    }

    glutPostRedisplay();
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Solo permite encender la linterna si es de noche 
        if (lastSunIntensity < 0.2f) {
            linternaEncendida = !linternaEncendida;
            if (linternaEncendida) {
                glEnable(GL_LIGHT1);
            }
            else {
                glDisable(GL_LIGHT1);
            }
        }
    }
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

    // Collision
    collisionBoxes.push_back({ 1.75f, 2.25f, 1.75f, 2.25f });
    collisionBoxes.push_back({ 0.75f, 1.25f, 0.75f, 1.25f });
    collisionBoxes.push_back({ -1.25f, -0.75f, -1.25f, -0.75f });
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

    //Collision
    collisionBoxes.push_back({ -0.15f, 0.15f, -0.15f, 0.15f });
    collisionBoxes.push_back({ -1.65f, -1.35f, -1.65f, -1.35f });
    collisionBoxes.push_back({ 2.05f, 2.35f, 2.25f, 2.55f });
}

void drawStone(float x, float y) {
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    glTranslatef(x, 0.0f, y); 
    glutSolidSphere(0.3, 20, 20);  
    glPopMatrix();

    //Collision
    collisionBoxes.push_back({ 0.2f, 0.8f, 0.2f, 0.8f });
    collisionBoxes.push_back({ -0.5f, 0.1f, 0.0f, 0.4f });


    objetosInteractuables.push_back({ x, y });
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
    float radians = angle_sun * M_PI / 180.0f;
    float intensity = 0.5f * (sin(radians) + 1.0f);
    lastSunIntensity = intensity; // Guardamos para saber si es de noche

    // Cielo
    float r = 0.1f + 0.4f * intensity;
    float g = 0.1f + 0.5f * intensity;
    float b = 0.2f + 0.6f * intensity;
    glClearColor(r, g, b, 1.0f);

    // Sol
    GLfloat light_diffuse[] = { intensity, intensity, intensity, 1.0f };
    GLfloat light_ambient[] = { 0.1f * intensity, 0.1f * intensity, 0.1f * intensity, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    //Lantern
    if (lastSunIntensity < 0.2f) {
        if (linternaEncendida) {
            float frontX = cos(camYaw) * cos(camPitch);
            float frontY = sin(camPitch);
            float frontZ = sin(camYaw) * cos(camPitch);

            float pos[] = { camX, camY, camZ, 1.0f };
            float dir[] = { frontX, frontY, frontZ };

            glLightfv(GL_LIGHT1, GL_POSITION, pos);
            glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);

            GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
            glLightfv(GL_LIGHT1, GL_SPECULAR, white);
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0f);
            glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10.0f);
        }
    }
    else {
        // apagar linterna automáticamente
        if (linternaEncendida) {
            glDisable(GL_LIGHT1);
            linternaEncendida = false;
        }
    }
}

//MiniMap----------------------
void drawMinimap() {
    // Cambia a un segundo viewport
    glViewport(1000, 720, 200, 200);  

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-5, 5, -5, 5);  

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING); 
    glDisable(GL_DEPTH_TEST);

    // Fondo
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-5, -5);
    glVertex2f(5, -5);
    glVertex2f(5, 5);
    glVertex2f(-5, 5);
    glEnd();

    float px = camZ;
    float pz = camX;
    float angle = camYaw;

    float size = 0.3f;
    glPushMatrix();
    glTranslatef(px, pz, 0);
    glRotatef(-angle * 180.0 / M_PI, 0, 0, 1);  

    glColor3f(0.0f, 1.0f, 1.0f); 
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, size);
    glVertex2f(-size * 0.5f, -size * 0.5f);
    glVertex2f(size * 0.5f, -size * 0.5f);
    glEnd();
    glPopMatrix();

    // Dibujar objetos estáticos
    glColor3f(1.0f, 0.0f, 0.0f);  // Marcadores de objetos
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(2, 2);
    glVertex2f(1, 1);
    glVertex2f(-1, -1);
    glVertex2f(0, 0);     // árbol
    glVertex2f(-1.5, -1.5);
    glVertex2f(2.2, 2.4);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);  
}

void drawText(const char* text, int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1200, 0, 1200);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2i(x, y);

    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
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

    if (shownText)
    {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText("Es una piedra normal", 400, 400);
        glEnable(GL_LIGHTING);
        shownText = false;
    }

    drawMinimap();

    glFlush(); 
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
    glutMouseFunc(mouseClick);
        

    glutMainLoop();  // Inicia el ciclo principal de GLUT
    return 0;
}