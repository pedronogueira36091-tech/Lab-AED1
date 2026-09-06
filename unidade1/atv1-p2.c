#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define TAM_CELULA     40 

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float   raio;
    Color   cor;
} Bola;

void inicializarBola(Bola *b) {
    b->pos = (Vector2){ GetRandomValue(50, LARGURA_JANELA - 50),
                         GetRandomValue(50, ALTURA_JANELA - 50) };
    b->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                         (float)GetRandomValue(-4, 4) };
    b->raio = (float)GetRandomValue(10, 25);
    b->cor  = (Color){ GetRandomValue(100, 255), GetRandomValue(100, 255),
                        GetRandomValue(100, 255), 255 };
}

int **criarMatriz(int linhas, int colunas) {
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    if (matriz == NULL) return NULL;

    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)malloc(colunas * sizeof(int));
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = 0; 
        }
    }
    return matriz;
}

void liberarMatriz(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

void desenharMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            // Se visitada (1): azul claro/brilhante; Se não visitada (0): escuro
            Color cor = (matriz[i][j] == 1) ? (Color){ 60, 140, 220, 255 }
                                            : (Color){ 15, 30, 55, 255 };
            DrawRectangle(j * TAM_CELULA, i * TAM_CELULA,
                          TAM_CELULA - 2, TAM_CELULA - 2, cor);
        }
    }
}

Bola *criarBolas(int quantidade) {
    Bola *bolas = (Bola *)malloc(quantidade * sizeof(Bola));
    if (bolas == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        inicializarBola(bolas + i);
    }
    return bolas;
}

void atualizarBola(Bola *b, int **grade, int linhas, int colunas) {
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    if (b->pos.x - b->raio < 0 || b->pos.x + b->raio > LARGURA_JANELA)
        b->vel.x *= -1;
    if (b->pos.y - b->raio < 0 || b->pos.y + b->raio > ALTURA_JANELA)
        b->vel.y *= -1;

    int col = (int)(b->pos.x / TAM_CELULA);
    int lin = (int)(b->pos.y / TAM_CELULA);

    if (lin >= 0 && lin < linhas && col >= 0 && col < colunas) {
        grade[lin][col] = 1; 
    }
}

int contarCelulasVisitadas(int **grade, int linhas, int colunas) {
    int visitadas = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (grade[i][j] == 1) {
                visitadas++;
            }
        }
    }
    return visitadas;
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA,
               "Ponteiros e Alocacao Dinamica - Mapa de Calor");
    SetTargetFPS(60);

    int linhas   = ALTURA_JANELA / TAM_CELULA;
    int colunas  = LARGURA_JANELA / TAM_CELULA;
    int totalCelulas = linhas * colunas;
    int **grade  = criarMatriz(linhas, colunas);

    int quantidadeBolas = 12;
    Bola *bolas = criarBolas(quantidadeBolas);

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) {
            int novaQtd = quantidadeBolas + 1;
            Bola *temp = (Bola *)realloc(bolas, novaQtd * sizeof(Bola));
            if (temp != NULL) {
                bolas = temp;
                quantidadeBolas = novaQtd;
                inicializarBola(&bolas[quantidadeBolas - 1]);
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE) && quantidadeBolas > 0) {
            int novaQtd = quantidadeBolas - 1;
            if (novaQtd == 0) {
                free(bolas);
                bolas = NULL;
                quantidadeBolas = 0;
            } else {
                Bola *temp = (Bola *)realloc(bolas, novaQtd * sizeof(Bola));
                if (temp != NULL) {
                    bolas = temp;
                    quantidadeBolas = novaQtd;
                }
            }
        }

        for (int i = 0; i < quantidadeBolas; i++) {
            atualizarBola(bolas + i, grade, linhas, colunas);
        }

        int celulasVisitadas = contarCelulasVisitadas(grade, linhas, colunas);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            desenharMatriz(grade, linhas, colunas);

            for (int i = 0; i < quantidadeBolas; i++) {
                DrawCircleV(bolas[i].pos, bolas[i].raio, bolas[i].cor);
            }

            DrawText("ESPACO: +Bola | BACKSPACE: -Bola", 10, 10, 18, WHITE);
            DrawText(TextFormat("Células visitadas: %d / %d", celulasVisitadas, totalCelulas),
                     10, 35, 18, GREEN);
            DrawText(TextFormat("Total de bolas: %d", quantidadeBolas),
                     10, 60, 18, YELLOW);
            DrawText("Pressione ESC para sair", 10, ALTURA_JANELA - 25, 16, WHITE);

        EndDrawing();
    }

    free(bolas);
    liberarMatriz(grade, linhas);

    CloseWindow();
    return 0;
}