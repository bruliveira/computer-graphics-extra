#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Inicializando os ângulos de rotação e escala
float alpha = 0, beta = 0, delta = 1;
bool keyUpPressionada = false;
bool keyDownPressionada = false;
bool keyLeftPressionada = false;
bool keyRightPressionada = false;
bool salvaImagem = false;

void Ilumina()
{
    float lightPosition[4] = {0.0f, 10.0f, -5.0f, 1.0f};
    float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Iluminação ambiente, especular e difusa
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

void objetoBule()
{
    // Cores - objetoBule
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

void objetoToro()
{
    // Cores - objetoToro
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

void objetoBola()
{
    // Cores - objetoBola
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
    glEnable(GL_DEPTH_TEST);          // Remoção de superf�cie oculta

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

void cenarioDesenho(float espessura, int op)
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

void update(int value)
{
    if (keyRightPressionada)
    {
        beta += 1.5; // Incrementa o ângulo de rotação em torno do eixo y
    }
    if (keyLeftPressionada)
    {
        beta -= 1.5; // Decrementa o ângulo de Rotação em torno do eixo y
    }
    if (keyUpPressionada)
    {
        alpha += 1.5; // Incrementa o ângulo de Rotação em torno do eixo x
    }
    if (keyDownPressionada)
    {
        alpha -= 1.5; // Decrementa o ângulo de Rotação em torno do eixo x
    }

    // Pode deixar automatico, tira os ifs e utiliza apenas esses
    // alpha += 1.5;
    // beta += 1.5;

    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

void keyPressedSpecial(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_F1: // Salva a imagem quando a tecla F1 for pressionada
        salvaImagem = true;
        break;
    case GLUT_KEY_RIGHT:
        keyRightPressionada = true;
        break;
    case GLUT_KEY_LEFT:
        keyLeftPressionada = true;
        break;
    case GLUT_KEY_UP:
        keyUpPressionada = true;
        break;
    case GLUT_KEY_DOWN:
        keyDownPressionada = true;
        break;
    }
    glutPostRedisplay();
}

void keyReleasedSpecial(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        keyRightPressionada = false;
        break;
    case GLUT_KEY_LEFT:
        keyLeftPressionada = false;
        break;
    case GLUT_KEY_UP:
        keyUpPressionada = false;
        break;
    case GLUT_KEY_DOWN:
        keyDownPressionada = false;
        break;
    }
}

void exportar(const char *filename, int width, int height)
{
    // Armazenar os pixels
    unsigned char *pixels = (unsigned char *)malloc(3 * width * height);
    if (pixels == NULL)
    {
        printf("Erro ao alocar mem�ria para os pixels.\n");
        return;
    }

    // Lendo os pixels do framebuffer
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Inverte a ordem dos pixels no eixo y (OpenGL armazena os pixels de baixo para cima)
    unsigned char *flipped_pixels = (unsigned char *)malloc(3 * width * height);
    for (int y = 0; y < height; ++y)
    {
        memcpy(flipped_pixels + (height - y - 1) * width * 3, pixels + y * width * 3, width * 3);
    }

    // Salva os pixels em um arquivo de imagem
    stbi_write_png(filename, width, height, 3, flipped_pixels, width * 3);

    // Libera a memória alocada
    free(pixels);
    free(flipped_pixels);

    printf("Imagem salva como %s\n", filename);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    // Chão
    cenarioDesenho(0.1, 1);

    // Parede
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    cenarioDesenho(0.1, 0);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glRotatef(270.0, 0.0, 0.0, 1.0);
    cenarioDesenho(0.1, 0);
    glPopMatrix();

    objetoBule();
    objetoToro();
    objetoBola();

    glFlush();

    if (salvaImagem)
    {
        exportar("cena1.png", 800, 800);
        salvaImagem = false;
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
