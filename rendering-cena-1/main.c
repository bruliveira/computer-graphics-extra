#include <windows.h>
#include <stdbool.h> // Incluindo a biblioteca para usar o tipo de dados bool
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

float alpha = 0, beta = 0, delta = 1; // Inicializa os ângulos de rotação e escala
bool keyUpPressed = false;
bool keyDownPressed = false;
bool keyLeftPressed = false;
bool keyRightPressed = false;

void Ilumina()
{
    float lightPosition[4] = {0.0f, 10.0f, -5.0f, 1.0f};
    float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Iluminações ambiente, especular e difusa
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); // Luz
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    float global_ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    // Ativando iluminação
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void bule()
{
    // Cores - bule
    float kd[4] = {0.2f, 0.6f, 1.0f, 1.0f};
    float ks[4] = {0.9f, 0.9f, 0.9f, 1.0f};
    float ns = 70.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);

    // Desenho
    glPushMatrix();
    glTranslatef(0.99, 0.8, 1.8);
    glRotatef(60.0, 0.0, 1.0, 0.0);
    glRotatef(beta, 0.0, 1.0, 0.0);  // Rotação em torno do eixo y
    glRotatef(alpha, 1.0, 0.0, 0.0); // Rotação em torno do eixo x
    glutSolidTeapot(0.2);
    glPopMatrix();
}

void toro()
{
    // Cores - toro
    float kd[4] = {0.2f, 1.0f, 0.2f, 1.0f};
    float ks[4] = {0.9f, 0.9f, 0.9f, 1.0f};
    float ns = 100.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);

    // Desenho
    glPushMatrix();
    glTranslatef(1.7, 0.8, 1.7);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(beta, 0.0, 1.0, 0.0);  // Rotação em torno do eixo y
    glRotatef(alpha, 1.0, 0.0, 0.0); // Rotação em torno do eixo x

    glutSolidTorus(0.06, 0.2, 40, 40);
    glPopMatrix();
}

void bola()
{
    // Cores - bola
    float ka[4] = {2.5f, 1.5f, 0.0f, 1.0f};
    float kd[4] = {1.0f, 0.5f, 0.0f, 1.0f};
    float ks[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float ns = 0.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);

    // Desenho
    glPushMatrix();
    glTranslatef(0.9, 0.8, 1.0);
    glRotatef(beta, 0.0, 1.0, 0.0);  // Rotação em torno do eixo y
    glRotatef(alpha, 1.0, 0.0, 0.0); // Rotação em torno do eixo x
    glutSolidSphere(0.3, 30, 30);
    glPopMatrix();
}

void init(void)
{
    glClearColor(0.5, 0.5, 0.5, 1.0); // Cor de fundo
    glEnable(GL_DEPTH_TEST);          // Remoção de superfície oculta

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Carrega a matriz de identidade
    gluLookAt(2.0, 1.6, 2.5,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Definindo a matrix de projeção perspectiva
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100.0, 1.0, 0.1, 100.0);

    Ilumina(); // Iluminação
}

void cenario(float espessura, int op)
{
    float kd[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    float ks[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float ns = 0.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);

    if (op == 1)
    {
        glPushMatrix();
        glTranslatef(1.0, espessura, 1.0);
        glScalef(2.0, espessura, 2.0);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glTranslatef(1.0, 0.0, 0.5);
        glScalef(2.0, espessura, 0.8);
        glutSolidCube(1.0);
        glPopMatrix();
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    // Chão
    cenario(0.1, 1);

    // Parede
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    cenario(0.1, 0);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glRotatef(270.0, 0.0, 0.0, 1.0);
    cenario(0.1, 0);
    glPopMatrix();

    bule();
    toro();
    bola();

    glFlush();
}

int a = 2;
void update(int value)
{
    // Atualize a cena aqui...
    if (keyRightPressed && a == 2)
    {
        beta += 1.5; // Incrementa o ângulo de rotação em torno do eixo y
    }
    if (keyLeftPressed)
    {
        beta -= 1.5; // Decrementa o ângulo de rotação em torno do eixo y
    }
    if (keyUpPressed)
    {
        alpha += 1.5; // Incrementa o ângulo de rotação em torno do eixo x
    }
    if (keyDownPressed)
    {
        alpha -= 1.5; // Decrementa o ângulo de rotação em torno do eixo x
    }

    // Pode deixar automatico, tira os ifs e utiliza apenas esses
    // alpha += 1.5;
    // beta += 1.5;

    glutPostRedisplay();

    glutTimerFunc(25, update, 0);
}

void keyPressedSpecial(int key, int x, int y)
{
    // Ative a rotação automática na direção correspondente quando uma tecla de seta for pressionada
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        keyRightPressed = true;
        break;
    case GLUT_KEY_LEFT:
        keyLeftPressed = true;
        break;
    case GLUT_KEY_UP:
        keyUpPressed = true;
        break;
    case GLUT_KEY_DOWN:
        keyDownPressed = true;
        break;
    }
}

void keyReleasedSpecial(int key, int x, int y)
{
    // Desative a rotação automática na direção correspondente quando uma tecla de seta for liberada
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        keyRightPressed = false;
        break;
    case GLUT_KEY_LEFT:
        keyLeftPressed = false;
        break;
    case GLUT_KEY_UP:
        keyUpPressed = false;
        break;
    case GLUT_KEY_DOWN:
        keyDownPressed = false;
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);                                    // Inicializa o glut
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); // Modo de display e a janela de exibição
    glutInitWindowPosition(200, 0);                           // Seta a posição inicial da janela
    glutInitWindowSize(800, 800);                             // Largura e altura da janela de exibição
    glutCreateWindow("RENDERIZAÇÃO DE CENA 3D - CENA 1");     // Criação da janela de exibição

    init();                   // Executa a função de inicialização
    glutDisplayFunc(display); // Deixa como a função callback de exibição

    glutSpecialFunc(keyPressedSpecial);    // Teclas especiais
    glutSpecialUpFunc(keyReleasedSpecial); // Teclas especiais soltas

    glutTimerFunc(25, update, 0); // Inicia a atualização da cena
    glutMainLoop();               // Mostra tudo e espera
    return 0;
}
