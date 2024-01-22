#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include <locale.h>

#define MAX_PONTOS 400
#define MAX_RETAS 100
#define MAX_NUMERO_POLIGONO 100
#define MAX_PONTOS_POLIGONO 200
#define TOLERANCIA 4

// Estrutura - pontos gerais
typedef struct
{
    float x;
    float y;
} Ponto;

// Estrutura - lista de pontos
typedef struct
{
    Ponto pontos[MAX_PONTOS]; // Array chamado pontos
    int quantidadePontoLista; // Número de pontos dessa lista
} ListaPontos;

// Estrutura - retas
typedef struct
{
    Ponto pontoInicio;
    Ponto pontoFim;
} Reta;

// EStrutura - lista de retas
typedef struct
{
    Reta retas[MAX_RETAS];
    int quantidadeRetaLista;
} ListaRetas;

// EStrutura - Poligonos
typedef struct
{
    Ponto pontos[MAX_PONTOS_POLIGONO];
    int quantidadePontosPoligono;
} Poligono;

// EStrutura - lista poligonos
typedef struct
{
    Poligono poligonos[MAX_NUMERO_POLIGONO];
    int quantidadePoligonoLista;
} ListaPoligonos;

ListaPontos *listaPonto;
ListaRetas *listaReta;
ListaPoligonos *listaPoligono;
/// ListaPoligonos *listaPoligonos;

int desenhando = 0; // Flag para indicar se o usuário está desenhando
int opcaoEscolhida = 0;
int desenhandoPoligono = 0; // Flag para indicar se estamos desenhando um polígono
Ponto pontosTemporarios[MAX_PONTOS];
int quantidadePontosTemporarios = 0;

ListaPontos *criarListaPontos()
{
    ListaPontos *listaPontos = (ListaPontos *)malloc(sizeof(ListaPontos));
    if (listaPontos != NULL)
    {
        listaPontos->quantidadePontoLista = 0;
    }
    return listaPontos;
}

ListaRetas *criarListaRetas()
{
    ListaRetas *listaRetas = (ListaRetas *)malloc(sizeof(ListaRetas));
    if (listaRetas != NULL)
    {
        listaRetas->quantidadeRetaLista = 0;
    }
    return listaRetas;
}

ListaPoligonos *criarListaPoligonos()
{
    ListaPoligonos *listaPoligonos = (ListaPoligonos *)malloc(sizeof(ListaPoligonos));
    if (listaPoligonos != NULL)
    {
        listaPoligonos->quantidadePoligonoLista = 0;
    }
    return listaPoligonos;
}

// Matrizes necessárias
float **matrizPontos(float x, float y)
{
    float **mP = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++)
    {
        mP[i] = (float *)malloc(sizeof(float));
    }
    mP[0][0] = x;
    mP[1][0] = y;
    mP[2][0] = 1.0;
    return mP;
}

float **matrizTransforma(float tx, float ty)
{
    float **mT = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++)
    {
        mT[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++)
        {
            if (i == j)
                mT[i][j] = 1.0;
            else
                mT[i][j] = 0;
        }
    }
    mT[0][2] = tx;
    mT[1][2] = ty;
    return mT;
}

float **matrizRotaciona()
{
    float **mR = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++)
    {
        mR[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++)
        {
            if (i == j)
                mR[i][j] = 1.0;
            else
                mR[i][j] = 0;
        }
    }
    // Valores para rotacao de 30 graus
    mR[0][0] = 0.8660;  // Valor cos
    mR[0][1] = -0.5000; // Valor - sen
    mR[1][0] = 0.5000;  // Valor + sen
    mR[1][1] = 0.8660;  // Valor cos
    return mR;
}

float **matrizMultiplica(float **m1, float **m2, int l, int c)
{
    float **r = (float **)malloc(l * sizeof(float *));
    for (int i = 0; i < l; i++)
    {
        r[i] = (float *)malloc(c * sizeof(float));
        for (int j = 0; j < c; j++)
        {
            r[i][j] = 0;
            for (int k = 0; k < l; k++)
            {
                r[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return r;
}

float **matrizTranslada(float **mP, float **mT)
{
    float **r = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++)
    {
        r[i] = (float *)malloc(sizeof(float));
    }
    r = matrizMultiplica(mT, mP, 3, 1);
    return r;
}

void matrizFree(float **matriz, int linhas)
{
    for (int i = 0; i < linhas; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}

void imprimirMatriz(float **matriz, int linhas, int colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            printf("%.2f ", matriz[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Função para transladar um ponto em uma lista
void transladarPonto(ListaPontos *listaPontos, int indice, float mx, float my)
{
    if (listaPontos == NULL || listaPontos->quantidadePontoLista == 0)
    {
        printf("Lista vazia ou ocorreu um erro inesperado.\n");
    }
    else
    {
        float dx = 1.0;
        float dy = 3.0;

        // float **mT = criarMatrizT(mx - listaPontos->pontos[indice].x, my - listaPontos->pontos[indice].y);
        float **mT = matrizTransforma(dx, dy);
        float **mP = matrizPontos(listaPontos->pontos[indice].x, listaPontos->pontos[indice].y);

        printf("Matriz de Translação (mT):\n");
        imprimirMatriz(mT, 3, 3);

        printf("Matriz Ponto Original (mP):\n");
        imprimirMatriz(mP, 3, 1);

        float **r = matrizTranslada(mP, mT);

        printf("Resultado da Translação (r = mT * mP):\n");
        imprimirMatriz(r, 3, 1);

        listaPontos->pontos[indice].x = r[0][0];
        listaPontos->pontos[indice].y = r[1][0];
    }
}

// Função para transladar uma reta em uma lista
void transladarReta(ListaRetas *listaRetas, int indice, float mx, float my)
{
    if (listaRetas == NULL || listaRetas->quantidadeRetaLista == 0)
    {
        printf("Lista vazia ou ocorreu um erro inesperado.\n");
    }
    else
    {
        printf("Transladando Reta\n");

        // Modificar os valores desejados de translação
        float tx = 1.0; // Valor desejado de translação em x
        float ty = 3.0; // Valor desejado de translação em y

        // Criar a matriz de translação
        float **mT = matrizTransforma(tx, ty);

        printf("Matriz de Translação (mT):\n");
        imprimirMatriz(mT, 3, 3);

        // Criar matrizes para os pontos de início e fim
        float **mPInicio = matrizPontos(listaRetas->retas[indice].pontoInicio.x, listaRetas->retas[indice].pontoInicio.y);
        float **mPFim = matrizPontos(listaRetas->retas[indice].pontoFim.x, listaRetas->retas[indice].pontoFim.y);

        printf("Matriz Ponto Inicial (mPInicio):\n");
        imprimirMatriz(mPInicio, 3, 1);

        printf("Matriz Ponto Final (mPFim):\n");
        imprimirMatriz(mPFim, 3, 1);

        // Transladar os pontos de início e fim
        float **rInicio = matrizTranslada(mPInicio, mT);
        float **rFim = matrizTranslada(mPFim, mT);

        printf("Resultado da Translação (rInicio = mT * mPInicio):\n");
        imprimirMatriz(rInicio, 3, 1);

        printf("Resultado da Translação (rFim = mT * mPFim):\n");
        imprimirMatriz(rFim, 3, 1);

        listaRetas->retas[indice].pontoInicio.x = rInicio[0][0];
        listaRetas->retas[indice].pontoInicio.y = rInicio[1][0];

        listaRetas->retas[indice].pontoFim.x = rFim[0][0];
        listaRetas->retas[indice].pontoFim.y = rFim[1][0];

        // Liberar a memória alocada para as matrizes e vetores temporários
        matrizFree(mPInicio, 3);
        matrizFree(rInicio, 3);
        matrizFree(mPFim, 3);
        matrizFree(rFim, 3);

        // Liberar a memória alocada para a matriz de translação
        matrizFree(mT, 3);
    }
}

void imprimirPontos(ListaPontos *listaPontos)
{
    printf("\nQuantidade de pontos na lista: %d\n", listaPontos->quantidadePontoLista);

    for (int i = 0; i < listaPontos->quantidadePontoLista; i++)
    {
        printf("Ponto %d: (%.2f, %.2f)\n", i + 1, listaPontos->pontos[i].x, listaPontos->pontos[i].y);
    }
}

void imprimirRetas(ListaRetas *listaRetas)
{
    printf("\nQuantidade de retas na lista: %d\n", listaRetas->quantidadeRetaLista);

    for (int i = 0; i < listaRetas->quantidadeRetaLista; i++)
    {
        printf("Reta %d: (%.2f, %.2f) - Ponto final (%.2f, %.2f)\n", i + 1, listaRetas->retas[i].pontoInicio.x, listaRetas->retas[i].pontoInicio.y, listaRetas->retas[i].pontoFim.x, listaRetas->retas[i].pontoFim.y);
    }
}

void imprimirPoligonos(ListaPoligonos *listaPoligonos)
{
    printf("\nQuantidade de polígonos na lista: %d\n", listaPoligonos->quantidadePoligonoLista);

    for (int i = 0; i < listaPoligonos->quantidadePoligonoLista; i++)
    {
        printf("Polígono %d:\n", i + 1);
        for (int j = 0; j < listaPoligonos->poligonos[i].quantidadePontosPoligono; j++)
        {
            printf("   Ponto %d: (%.2f, %.2f)\n", j + 1, listaPoligonos->poligonos[i].pontos[j].x, listaPoligonos->poligonos[i].pontos[j].y);
        }
    }
    printf("Fim da lista de polígonos\n");
}

// Validações de listas vazias/cheias
int listaPontosVazia(ListaPontos *listaPontos)
{
    return listaPontos->quantidadePontoLista == 0 ? 1 : 0;
}

int listaPontosCheia(ListaPontos *listaPontos)
{
    return listaPontos->quantidadePontoLista == MAX_PONTOS ? 1 : 0;
}

int listaRetasVazia(ListaRetas *listaRetas)
{
    return listaRetas->quantidadeRetaLista == 0 ? 1 : 0;
}

int listaRetasCheia(ListaRetas *listaRetas)
{
    return listaRetas->quantidadeRetaLista == MAX_RETAS ? 1 : 0;
}

int listaPoligonosVazia(ListaPoligonos *listaPoligonos)
{
    return listaPoligonos->quantidadePoligonoLista == 0 ? 1 : 0;
}

int listaPoligonosCheia(ListaPoligonos *listaPoligonos)
{
    return listaPoligonos->quantidadePoligonoLista == MAX_NUMERO_POLIGONO ? 1 : 0;
}

void opcaoMenu(int opcao)
{
    opcaoEscolhida = opcao;
}

void menuTela()
{
    glutCreateMenu(opcaoMenu);

    glutAddMenuEntry("Desenhar Ponto", 1);
    glutAddMenuEntry("Desenhar Segmento de Reta", 2);
    glutAddMenuEntry("Desenhar Polilinha", 3);

    glutAddMenuEntry("Selecionar/Excluir Ponto", 4);
    glutAddMenuEntry("Selecionar/Excluir Segmento de Reta", 5);
    glutAddMenuEntry("Selecionar/Excluir Polilinha", 6);

    glutAddMenuEntry("Transladar Ponto", 7);
    glutAddMenuEntry("Transladar Segmento de Reta", 8);
    glutAddMenuEntry("Transladar Polilinha", 9);

    glutAddMenuEntry("Rotacionar Ponto", 10); // No ponto é em relação a origem do eixo de coordenadas.
    glutAddMenuEntry("Rotacionar Segmento de Reta", 11);
    glutAddMenuEntry("Rotacionar Polilinha", 12);

    glutAddMenuEntry("Escalar Segmento de Reta", 13);
    glutAddMenuEntry("Escalar Polilinha", 14);

    glutAddMenuEntry("Exportar formas", 15);
    glutAddMenuEntry("Importar formas", 16);
    glutAddMenuEntry("Sair", 17);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void adicionarPonto(ListaPontos *listaPontos, float x, float y)
{
    if (listaPontos == NULL)
        printf("Poxa, ocorreu um erro ao adiconar Ponto na lista\n");
    else if (listaPontosCheia(listaPontos) == 1)
        printf("Que pena, atingiu a quantidade máxima da lista\n");
    else
    {
        listaPontos->pontos[listaPontos->quantidadePontoLista].x = x;
        listaPontos->pontos[listaPontos->quantidadePontoLista].y = y;
        listaPontos->quantidadePontoLista++;
    }
}

void adicionarReta(ListaRetas *listaRetas, Ponto ponto1, Ponto ponto2)
{
    if (listaRetas == NULL)
        printf("Poxa, ocorreu um erro ao adiconar Reta na lista\n");
    else if (listaRetasCheia(listaRetas) == 1)
        printf("Que pena, atingiu a quantidade máxima da lista\n");
    else
    {
        listaRetas->retas[listaRetas->quantidadeRetaLista].pontoInicio = ponto1;
        listaRetas->retas[listaRetas->quantidadeRetaLista].pontoFim = ponto2;
        listaRetas->quantidadeRetaLista++;
    }
}

void adicionarPoligono(ListaPoligonos *listaPoligonos, Poligono poligono)
{
    if (listaPoligonos == NULL)
    {
        printf("Que pena, Erro ao adicionar polígono na lista\n");
        return;
    }
    else if (listaPoligonosCheia(listaPoligonos) == 1)
    {
        printf("Sua lista de poligonos atingiu a quantidade máxima que pena\n");
    }
    else
    {
        printf("Adicionando polígono à lista.\n");
        for (int i = 0; i < poligono.quantidadePontosPoligono; i++)
        {
            listaPoligonos->poligonos[listaPoligonos->quantidadePoligonoLista].pontos[i] = poligono.pontos[i];
        }
        listaPoligonos->poligonos[listaPoligonos->quantidadePoligonoLista] = poligono;
        listaPoligonos->quantidadePoligonoLista++;
    }
}

void desenhaListaPontos(ListaPontos *listaPontos)
{
    // printf("Desenhando Pontos...\n");
    glPointSize(6.0);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < listaPontos->quantidadePontoLista; i++)
    {
        glVertex2f(listaPontos->pontos[i].x, listaPontos->pontos[i].y);
    }
    glEnd();
}

int pontoNaReta(float mx, float my, Reta reta)
{
    // Adicione esses prints para debug
    printf("\n\nmx: %.2f, my: %.2f\n", mx, my);
    printf("reta.pontoInicio.x: %.2f, reta.pontoInicio.y: %.2f\n", reta.pontoInicio.x, reta.pontoInicio.y);
    printf("reta.pontoFim.x: %.2f, reta.pontoFim.y: %.2f\n", reta.pontoFim.x, reta.pontoFim.y);

    // Verifica se o ponto (mx, my) está dentro da tolerância da reta
    if (mx >= fmin(reta.pontoInicio.x, reta.pontoFim.x) - TOLERANCIA &&
        mx <= fmax(reta.pontoInicio.x, reta.pontoFim.x) + TOLERANCIA &&
        my >= fmin(reta.pontoInicio.y, reta.pontoFim.y) - TOLERANCIA &&
        my <= fmax(reta.pontoInicio.y, reta.pontoFim.y) + TOLERANCIA)
    {
        printf("Ponto está dentro da tolerância da reta.\n");
        return 1; // Ponto está dentro da tolerância da reta
    }

    printf("Ponto NÃO está dentro da tolerância da reta.\n");
    return 0; // Ponto não está dentro da tolerância da reta
}

void excluirPonto(ListaPontos *listaPontos, int indice)
{
    if (listaPontos == NULL || listaPontosVazia(listaPontos) == 1)
        printf("\nPoxa! Lista vazia ou ocorreu um erro inesperado.\n");
    else if (indice < 0 || indice >= listaPontos->quantidadePontoLista)
        printf("\nÍndice inválido para exclusão.\n");
    else
    {
        printf("Excluindo ponto: (%.2f, %.2f)\n", listaPontos->pontos[indice].x, listaPontos->pontos[indice].y);

        for (int i = indice; i < (listaPontos->quantidadePontoLista - 1); i++)
        {
            listaPontos->pontos[i] = listaPontos->pontos[i + 1];
        }
        listaPontos->quantidadePontoLista--;

        printf("Ponto excluído com sucesso.\n");
    }
}

void excluirReta(ListaRetas *listaRetas, int indice)
{
    printf("\nSó na função mesmo\n");
    if (listaRetas == NULL || listaRetasVazia(listaRetas) == 1)
        printf("\nLista vazia ou ocorreu um erro inesperado.\n");
    else if (indice < 0 || indice >= listaRetas->quantidadeRetaLista)
        printf("\nÍndice inválido para exclusão.\n");
    else
    {
        printf("Excluindo reta: (%.2f, %.2f) - Ponto final (%.2f, %.2f)\n",
               listaRetas->retas[indice].pontoInicio.x,
               listaRetas->retas[indice].pontoInicio.y,
               listaRetas->retas[indice].pontoFim.x,
               listaRetas->retas[indice].pontoFim.y);

        for (int i = indice; i < (listaRetas->quantidadeRetaLista - 1); i++)
        {
            listaRetas->retas[i] = listaRetas->retas[i + 1];
        }
        listaRetas->quantidadeRetaLista--;

        printf("Reta excluída com sucesso.\n");
    }
}

int selecionarPonto(ListaPontos *listaPontos, float px, float py)
{
    if (listaPontos == NULL || listaPontos->quantidadePontoLista == 0)
    {
        printf("Que pena! Lista vazia ou ocorreu um erro inesperado.\n");
        return -1; // Adiciona um valor de retorno indicando erro
    }
    else
    {
        for (int i = 0; i < listaPontos->quantidadePontoLista; i++)
        {
            if (px <= listaPontos->pontos[i].x + TOLERANCIA && px >= listaPontos->pontos[i].x - TOLERANCIA)
            {
                if (py <= listaPontos->pontos[i].y + TOLERANCIA && py >= listaPontos->pontos[i].y - TOLERANCIA)
                {
                    printf("Ponto selecionado: (%.2f, %.2f), Índice: %d\n", listaPontos->pontos[i].x, listaPontos->pontos[i].y, i);
                    return i;
                }
            }
        }

        printf("Nenhum ponto selecionado nas coordenadas (%.2f, %.2f)\n", px, py);
        return -1; // Adiciona um valor de retorno indicando que nenhum ponto foi encontrado
    }
}

int selecionarReta(ListaRetas *listaRetas, float mx, float my)
{
    if (listaRetas == NULL || listaRetas->quantidadeRetaLista == 0)
    {
        printf("Lista de retas vazia ou ocorreu um erro inesperado.\n");
        return -1; // Adiciona um valor de retorno indicando erro
    }
    else
    {
        for (int i = 0; i < listaRetas->quantidadeRetaLista; i++)
        {
            // Verifica se (mx, my) está dentro da tolerância da reta
            if (pontoNaReta(mx, my, listaRetas->retas[i]))
            {
                printf("Reta selecionada: (%.2f, %.2f) - Ponto final (%.2f, %.2f), Índice: %d\n",
                       listaRetas->retas[i].pontoInicio.x,
                       listaRetas->retas[i].pontoInicio.y,
                       listaRetas->retas[i].pontoFim.x,
                       listaRetas->retas[i].pontoFim.y, i);
                return i;
            }
        }

        printf("Nenhuma reta selecionada nas coordenadas (%.2f, %.2f)\n", mx, my);
        return -1; // Adiciona um valor de retorno indicando que nenhuma reta foi encontrada
    }
}

// Função para verificar se um ponto (mx, my) está dentro da tolerância do polígono
int pontoNoPoligono(float mx, float my, Poligono poligono)
{
    int i, j, c = 0;
    for (i = 0, j = poligono.quantidadePontosPoligono - 1; i < poligono.quantidadePontosPoligono; j = i++)
    {
        if (((poligono.pontos[i].y > my) != (poligono.pontos[j].y > my)) &&
            (mx < (poligono.pontos[j].x - poligono.pontos[i].x) * (my - poligono.pontos[i].y) / (poligono.pontos[j].y - poligono.pontos[i].y) + poligono.pontos[i].x))
        {
            c = !c;
        }
    }
    return c;
}

// Função para excluir um polígono da lista
void excluirPoligono(ListaPoligonos *listaPoligonos, int indice)
{
    if (listaPoligonos == NULL || listaPoligonosVazia(listaPoligonos) == 1)
    {
        printf("\nLista vazia ou ocorreu um erro inesperado.\n");
    }
    else if (indice < 0 || indice >= listaPoligonos->quantidadePoligonoLista)
    {
        printf("\nÍndice inválido para exclusão.\n");
    }
    else
    {
        printf("Excluindo polígono:\n");
        for (int i = indice; i < (listaPoligonos->quantidadePoligonoLista - 1); i++)
        {
            listaPoligonos->poligonos[i] = listaPoligonos->poligonos[i + 1];
        }
        listaPoligonos->quantidadePoligonoLista--;

        printf("Polígono excluído com sucesso.\n");

        glutPostRedisplay(); // Solicita a atualização da tela
    }
}

// Função para selecionar um polígono com base em coordenadas (mx, my)
int selecionarPoligono(ListaPoligonos *listaPoligonos, float mx, float my)
{
    if (listaPoligonos == NULL || listaPoligonos->quantidadePoligonoLista == 0)
    {
        printf("Lista de polígonos vazia ou ocorreu um erro inesperado.\n");
        return -1; // Adiciona um valor de retorno indicando erro
    }
    else
    {
        for (int i = 0; i < listaPoligonos->quantidadePoligonoLista; i++)
        {
            // Verifica se (mx, my) está dentro da tolerância do polígono
            if (pontoNoPoligono(mx, my, listaPoligonos->poligonos[i]))
            {
                printf("Polígono selecionado. Índice: %d\n", i);
                return i;
            }
        }

        printf("Nenhum polígono selecionado nas coordenadas (%.2f, %.2f)\n", mx, my);
        return -1; // Adiciona um valor de retorno indicando que nenhum polígono foi encontrado
    }
}

void desenhaListaRetas(ListaRetas *listaRetas)
{
    // printf("Desenhando Retas...\n");
    glLineWidth(3.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    for (int i = 0; i < listaRetas->quantidadeRetaLista; i++)
    {
        glVertex2f(listaRetas->retas[i].pontoInicio.x, listaRetas->retas[i].pontoInicio.y);
        glVertex2f(listaRetas->retas[i].pontoFim.x, listaRetas->retas[i].pontoFim.y);
    }
    glEnd();
}

void desenhaPoligono(ListaPoligonos *listaPoligonos)
{
    // printf("Desenhando polígonos...\n");
    glLineWidth(3.0);
    for (int i = 0; i < listaPoligonos->quantidadePoligonoLista; i++)
    {
        glBegin(GL_LINE_STRIP);
        glColor3f(0, 0, 1);

        for (int j = 0; j < listaPoligonos->poligonos[i].quantidadePontosPoligono; j++)
        {
            glVertex2f(listaPoligonos->poligonos[i].pontos[j].x, listaPoligonos->poligonos[i].pontos[j].y);
        }

        glEnd();
    }
    if (desenhandoPoligono && quantidadePontosTemporarios > 0)
    {
        glBegin(GL_LINE_STRIP);
        glColor3f(0, 0, 1); // Cor azul

        for (int i = 0; i < quantidadePontosTemporarios; i++)
        {
            glVertex2f(pontosTemporarios[i].x, pontosTemporarios[i].y);
        }

        glEnd();
    }
}

void iniciarDesenhoPoligono()
{
    desenhandoPoligono = 1;
    quantidadePontosTemporarios = 0;
    glutPostRedisplay(); // Solicita a atualização da tela
}

void mouseClick(int botao, int state, int x, int y)
{
    static int aguardandoSegundoPonto = 0;
    static Ponto pontoInicial;
    static int qtd_p = 0;
    static Ponto aux[2];
    static Ponto auxPoligono[MAX_PONTOS_POLIGONO];

    printf("\nOpção escolhida: %d\n", opcaoEscolhida);

    switch (opcaoEscolhida)
    {
    case 0:
        printf("Selecione uma opção, por favor\n");
        break;
    case 1:
        printf("-> Desenho de pontos\n");
        if (botao == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            desenhando = 1;
            adicionarPonto(listaPonto, x, 600 - y);
            printf("Adicionado %d ponto: (%.2f, %.2f)\n", listaPonto->quantidadePontoLista, listaPonto->pontos[listaPonto->quantidadePontoLista - 1].x, listaPonto->pontos[listaPonto->quantidadePontoLista - 1].y);
        }
        break;
    case 2:
        printf("-> Desenho de retas\n");
        if (botao == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if (qtd_p == 0)
            {
                aux[0].x = x;
                aux[0].y = 600 - y;
                qtd_p++;
                printf("Adicionado ponto inicial: (%.2f, %.2f)\n", aux[0].x, aux[0].y);
            }
            else if (qtd_p == 1)
            {
                aux[1].x = x;
                aux[1].y = 600 - y;
                adicionarReta(listaReta, aux[0], aux[1]);

                printf("\Qnuantidade: %d\n", listaReta->quantidadeRetaLista);
                qtd_p = 0;
                printf("Adicionada Reta com ponto final: (%.2f, %.2f)\n", aux[1].x, aux[1].y);

                glutPostRedisplay();
            }
        }
        break;
    case 3:
        printf("-> Desenho de Polilinha\n");
        if (botao == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if (!desenhandoPoligono)
            {
                printf("\nEntrou aqui");
                iniciarDesenhoPoligono();
            }

            if (quantidadePontosTemporarios < MAX_PONTOS_POLIGONO)
            {
                printf("\nEntrou aqui: quantidadePontosTemporarios < MAX_PONTOS_POLYGON");
                pontosTemporarios[quantidadePontosTemporarios].x = x;
                pontosTemporarios[quantidadePontosTemporarios].y = glutGet(GLUT_WINDOW_HEIGHT) - y; // Invertendo a coordenada y
                quantidadePontosTemporarios++;

                // Adiciona o ponto temporário ao polígono e desenha a forma
                if (quantidadePontosTemporarios > 1)
                {
                    printf("Outro if de temporarios\n");
                    glBegin(GL_LINE_STRIP);
                    glColor3f(0, 0, 1);

                    for (int i = 0; i < quantidadePontosTemporarios; i++)
                    {
                        glVertex2f(pontosTemporarios[i].x, pontosTemporarios[i].y);
                        printf("valores: %d", pontosTemporarios[i].x);
                    }

                    glEnd();

                    glutSwapBuffers(); // Troca os buffers para exibir
                }
            }
        }

        break;
    case 4:
        printf("-> Selecionar/Excluir Ponto\n");
        if (botao == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            excluirPonto(listaPonto, selecionarPonto(listaPonto, x, 600 - y));
            glutPostRedisplay();
        }
        break;
    case 5:
        printf("-> Selecionar/Excluir Segmento de Reta\n");
        if (botao == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            excluirReta(listaReta, selecionarReta(listaReta, x, 600 - y));
            glutPostRedisplay();
        }
        break;
    case 6:
        printf("-> Selecionar/Excluir Polilinha\n");

        if (botao == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            // Verifica se há algum polígono nas coordenadas do clique direito
            int indicePoligonoSelecionado = selecionarPoligono(listaPoligono, x, glutGet(GLUT_WINDOW_HEIGHT) - y);

            if (indicePoligonoSelecionado != -1)
            {
                // Exclui o polígono selecionado
                excluirPoligono(listaPoligono, indicePoligonoSelecionado);
            }
        }

        break;
    case 7:
        printf("-> Transladar Ponto\n");
        {
            // Verifica se o índice foi selecionado corretamente
            if (selecionarPonto(listaPonto, x, 600 - y) != -1)
            {
                transladarPonto(listaPonto, selecionarPonto(listaPonto, x, 600 - y), x, y);
                glutPostRedisplay();
            }
        }
        break;
    case 8:
        printf("-> Transladar Segmento de Reta");
        if (botao == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            // Verifica se o índice foi selecionado corretamente
            int indiceRetaSelecionada = selecionarReta(listaReta, x, 600 - y);
            if (indiceRetaSelecionada != -1)
            {
                transladarReta(listaReta, indiceRetaSelecionada, x, y);
                glutPostRedisplay();
            }
        }
        break;
    case 9:
        printf("-> Transladar Polilinha\n");
        break;
    case 10:
        printf("-> Rotacionar Ponto\n");
        break;
    case 11:
        printf("-> Rotacionar Segmento de Reta\n");
        break;
    case 12:
        printf("-> Rotacionar Polilinha\n");
        break;
    case 13:
        printf("-> Escalar Segmento de Reta\n");
        break;
    case 14:
        printf("-> Escalar Polilinha\n");
        break;
    case 15:
        printf("-> Exportar formas\n");
        if (state == GLUT_DOWN && botao == GLUT_LEFT_BUTTON)
        {
            salvarFormas();
        }
        break;
    case 16:
        printf("-> Importar formas\n");
        if (state == GLUT_DOWN && botao == GLUT_LEFT_BUTTON)
        {
            carregarFormas();
            glutPostRedisplay(); // Atualiza a tela após a importação
        }

        break;
    case 17:
        printf("-> Sair");
        exit(0);
        break;
    default:
        break;
    }
}

void keyboardFunc(unsigned char key, int x, int y)
{
    if (key == 13 || key == 32) // Enter ou Espaço
    {
        if (desenhandoPoligono && quantidadePontosTemporarios > 2)
        {
            Poligono novoPoligono;
            for (int i = 0; i < quantidadePontosTemporarios; i++)
            {
                novoPoligono.pontos[i] = pontosTemporarios[i];
            }
            novoPoligono.quantidadePontosPoligono = quantidadePontosTemporarios;

            adicionarPoligono(listaPoligono, novoPoligono);

            desenhandoPoligono = 0;
            quantidadePontosTemporarios = 0;

            glutPostRedisplay(); // Solicita a atualização da tela
        }
    }
}

void salvarFormas()
{
    FILE *arquivo = fopen("formas.txt", "w");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    fprintf(arquivo, "Pontos:\n");
    for (int i = 0; i < listaPonto->quantidadePontoLista; i++)
    {
        fprintf(arquivo, "%.2f %.2f\n", listaPonto->pontos[i].x, listaPonto->pontos[i].y);
    }

    fprintf(arquivo, "\nRetas:\n");
    for (int i = 0; i < listaReta->quantidadeRetaLista; i++)
    {
        fprintf(arquivo, "%.2f %.2f %.2f %.2f\n", listaReta->retas[i].pontoInicio.x, listaReta->retas[i].pontoInicio.y, listaReta->retas[i].pontoFim.x, listaReta->retas[i].pontoFim.y);
    }

    fprintf(arquivo, "\nPolígonos:\n");
    for (int i = 0; i < listaPoligono->quantidadePoligonoLista; i++)
    {
        fprintf(arquivo, "Número de Lados: %d\n", listaPoligono->poligonos[i].quantidadePontosPoligono);
        for (int j = 0; j < listaPoligono->poligonos[i].quantidadePontosPoligono; j++)
        {
            fprintf(arquivo, "%.2f %.2f\n", listaPoligono->poligonos[i].pontos[j].x, listaPoligono->poligonos[i].pontos[j].y);
        }
    }

    fclose(arquivo);

    printf("Formas salvas no arquivo formas.txt.\n");
}

void limparListas()
{
    // Limpar lista de pontos
    if (listaPonto != NULL)
    {
        free(listaPonto);
        listaPonto = criarListaPontos();
    }

    // Limpar lista de retas
    if (listaReta != NULL)
    {
        free(listaReta);
        listaReta = criarListaRetas();
    }

    // Limpar lista de polígonos
    if (listaPoligono != NULL)
    {
        free(listaPoligono);
        listaPoligono = criarListaPoligonos();
    }
}

void carregarFormas()
{
    FILE *arquivo = fopen("formas.txt", "r");

    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return;
    }

    // Limpar listas existentes antes de carregar
    limparListas();

    char linha[256];
    int numeroLados; // Declare a variável numeroLados aqui

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        if (strcmp(linha, "Pontos:\n") == 0)
        {
            while (fgets(linha, sizeof(linha), arquivo) != NULL && linha[0] != '\n')
            {
                Ponto ponto;
                sscanf(linha, "%f %f", &ponto.x, &ponto.y);
                adicionarPonto(listaPonto, ponto.x, ponto.y);
            }
        }
        else if (strcmp(linha, "Retas:\n") == 0)
        {
            while (fgets(linha, sizeof(linha), arquivo) != NULL && linha[0] != '\n')
            {
                Reta reta;
                sscanf(linha, "%f %f %f %f", &reta.pontoInicio.x, &reta.pontoInicio.y, &reta.pontoFim.x, &reta.pontoFim.y);
                adicionarReta(listaReta, reta.pontoInicio, reta.pontoFim);
            }
        }
    }

    fclose(arquivo);

    printf("Formas carregadas do arquivo formas.txt.\n");
}

int init(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0); // Cor de fundo
    glMatrixMode(GL_PROJECTION);      // Matriz de projeção
    gluOrtho2D(0.0, 800.0, 0.0, 600.0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT); // Desenha o fundo - limpa tela

    // Adicionais
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // fim adicionais

    // Desenha os pontos
    desenhaListaPontos(listaPonto); // Certifique-se de que a variável 'lista' está acessível aqui
    desenhaListaRetas(listaReta);
    desenhaPoligono(listaPoligono);

    glFlush(); // Desenha os comandos
}

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "Portuguese_Brazil.1252");
    glutInit(&argc, argv);                      // Inicializa o glut
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB); // Modo de display
    glutInitWindowSize(800, 600);               // Largura e altura da janela de exibição
    glutCreateWindow("Paint - Versão final");   // Criação da janela de exibição

    // Inicializa as listas
    listaPonto = criarListaPontos();
    listaReta = criarListaRetas();
    listaPoligono = criarListaPoligonos();

    // Adiciona alguns pontos para teste
    listaPonto->pontos[0].x = 500.0;
    listaPonto->pontos[0].y = 200.0;
    listaPonto->quantidadePontoLista++;

    listaPonto->pontos[1].x = 150.0;
    listaPonto->pontos[1].y = 350.0;
    listaPonto->quantidadePontoLista++;

    // Adiciona alguns pontos para as retas para teste
    Ponto pontoInicial1 = {400.0, 6.0};
    Ponto pontoFinal1 = {400.0, 790.0};

    Ponto pontoInicial2 = {5.0, 300.0};
    Ponto pontoFinal2 = {790.0, 300.0};

    Ponto pontoInicial3 = {3.0, 2.0};
    Ponto pontoFinal3 = {50.0, 40.0};

    listaReta->retas[listaReta->quantidadeRetaLista].pontoInicio = pontoInicial1;
    listaReta->retas[listaReta->quantidadeRetaLista].pontoFim = pontoFinal1;
    listaReta->quantidadeRetaLista++;

    listaReta->retas[listaReta->quantidadeRetaLista].pontoInicio = pontoInicial2;
    listaReta->retas[listaReta->quantidadeRetaLista].pontoFim = pontoFinal2;
    listaReta->quantidadeRetaLista++;

    listaReta->retas[listaReta->quantidadeRetaLista].pontoInicio = pontoInicial3;
    listaReta->retas[listaReta->quantidadeRetaLista].pontoFim = pontoFinal3;
    listaReta->quantidadeRetaLista++;

    // Adiciona alguns polígonos para teste
    Poligono poligono1;
    poligono1.pontos[0].x = 600.0;
    poligono1.pontos[0].y = 100.0;

    poligono1.pontos[1].x = 200.0;
    poligono1.pontos[1].y = 100.0;

    poligono1.pontos[2].x = 200.0;
    poligono1.pontos[2].y = 10.0;

    poligono1.pontos[3].x = 600.0;
    poligono1.pontos[3].y = 10.0;

    poligono1.pontos[4].x = 600.0;
    poligono1.pontos[4].y = 100.0;
    poligono1.quantidadePontosPoligono = 5;

    adicionarPoligono(listaPoligono, poligono1);

    // Imprime os pontos
    imprimirPontos(listaPonto);
    imprimirRetas(listaReta);
    imprimirPoligonos(listaPoligono);

    // Verifica se a lista está vazia ou cheia
    printf("Lista vazia - Pontos: %s\n", listaPontosVazia(listaPonto) ? "Sim" : "Nao");
    printf("Lista cheia - Pontos: %s\n", listaPontosCheia(listaPonto) ? "Sim" : "Nao");

    printf("Lista vazia - Retas: %s\n", listaRetasVazia(listaReta) ? "Sim" : "Nao");
    printf("Lista cheia - Retas: %s\n", listaRetasCheia(listaReta) ? "Sim" : "Nao");

    printf("Lista vazia - Polilinha: %s\n", listaPoligonosVazia(listaPoligono) ? "Sim" : "Nao");
    printf("Lista cheia - Polilinha: %s\n", listaPoligonosCheia(listaPoligono) ? "Sim" : "Nao");

    menuTela();

    init(); // Executa a função de inicialização

    glutDisplayFunc(display); // Deixa como a função callback de exibição
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboardFunc); // Função para lidar com pressionamento de tecla

    glutMainLoop(); // Mostra tudo e espera
    return 0;
}
