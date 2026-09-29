#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Definições de constantes para configurações da janela e do jogo
#define LARGURA_JANELA  800
#define ALTURA_JANELA   600
#define RAIO_JOGADOR    20.0f
#define MAX_ENTIDADES   30
#define TOTAL_INIMIGOS  5
#define TOTAL_ITENS     6
#define ARQUIVO_PLACAR  "placar.txt"
#define ARQUIVO_SAVE    "save.bin"

// Tipo que define a categoria de cada objeto no jogo
typedef enum {
    ENTIDADE_JOGADOR,
    ENTIDADE_INIMIGO,
    ENTIDADE_ITEM
} TipoEntidade;

// Union: compartilha memória entre dano (inimigo) e valor (item)
typedef union {
    int dano;
    int valor;
} ExtraEntidade;

// Struct que armazena todas as propriedades de cada elemento do jogo
typedef struct {
    TipoEntidade  tipo;
    Vector2       pos;
    float         raio;
    int           vida;
    Color         cor;
    ExtraEntidade extra;
} Entidade;

// Vetor global de ponteiros para guardar as entidades ativas
Entidade *vetorEntidades[MAX_ENTIDADES];
int totalEntidades = 0;

// Aloca memória dinamicamente e inicializa os atributos de uma entidade
Entidade *criarEntidade(TipoEntidade tipo, Vector2 pos) {
    Entidade *e = (Entidade *)malloc(sizeof(Entidade));
    if (e == NULL) return NULL; // Retorna NULL se a alocação de memória falhar

    e->tipo = tipo;
    e->pos  = pos;
    e->raio = (tipo == ENTIDADE_JOGADOR) ? RAIO_JOGADOR
            : (tipo == ENTIDADE_INIMIGO) ? 15.0f : 8.0f;

    switch (tipo) {
        case ENTIDADE_JOGADOR:
            e->vida = 100;
            e->cor  = BLUE;
            break;
        case ENTIDADE_INIMIGO:
            e->vida       = 40;
            e->cor        = MAROON;
            e->extra.dano = GetRandomValue(5, 15); // Define dano aleatório
            break;
        case ENTIDADE_ITEM:
            e->vida        = 1;
            e->cor         = GOLD;
            e->extra.valor = GetRandomValue(5, 20); // Define valor do item
            break;
    }
    return e;
}

// Insere uma entidade criada no vetor de controle
void adicionarEntidade(Entidade *e) {
    if (e == NULL || totalEntidades >= MAX_ENTIDADES) return;
    vetorEntidades[totalEntidades] = e;
    totalEntidades++;
}

// Libera a memória de uma entidade e move a última do vetor para cobrir a lacuna
void removerEntidade(int indice) {
    if (indice < 0 || indice >= totalEntidades) return;
    free(vetorEntidades[indice]);
    vetorEntidades[indice] = vetorEntidades[totalEntidades - 1];
    totalEntidades--;
}

// Libera a memória de todas as entidades do vetor
void liberarTodasEntidades(void) {
    for (int i = 0; i < totalEntidades; i++) free(vetorEntidades[i]);
    totalEntidades = 0;
}

// Calcula colisão usando a distância entre os centros dos dois círculos
bool colidiu(Entidade *a, Entidade *b) {
    float dx = a->pos.x - b->pos.x;
    float dy = a->pos.y - b->pos.y;
    float distancia = sqrtf(dx * dx + dy * dy);
    return distancia <= (a->raio + b->raio);
}

// Desenha a entidade na tela usando as funções da raylib
void desenharEntidade(Entidade *e) {
    DrawCircleV(e->pos, e->raio, e->cor);
    // Exibe a vida num texto em cima do inimigo
    if (e->tipo == ENTIDADE_INIMIGO) {
        DrawText(TextFormat("%d", e->vida), e->pos.x - 8, e->pos.y - 26, 14, BLACK);
    }
}

/* EXERCÍCIO 1: Grava no placar.txt em modo texto ("a"), incluindo o nome do jogador[cite: 1] */
void salvarPlacarTexto(const char *nome, int pontuacao) {
    FILE *arquivo = fopen(ARQUIVO_PLACAR, "a"); // "a": adiciona ao final sem apagar o que existe[cite: 1]
    if (arquivo == NULL) return; // Trata erro se o arquivo não puder ser aberto[cite: 1]

    fprintf(arquivo, "%s %d\n", nome, pontuacao); // Escreve o nome e a pontuação[cite: 1]
    fclose(arquivo); // Garante o fechamento do arquivo[cite: 1]
}

/* EXERCÍCIO 1: Lê o arquivo em modo texto ("r") par a par (nome pontuacao) e retorna a maior[cite: 1] */
int lerMelhorPontuacao(void) {
    FILE *arquivo = fopen(ARQUIVO_PLACAR, "r"); // "r": apenas leitura[cite: 1]
    if (arquivo == NULL) return 0;

    int melhor = 0, valor = 0;
    char nomeLido[16];

    // Lê a string e o inteiro enquanto houver linhas no arquivo[cite: 1]
    while (fscanf(arquivo, "%15s %d", nomeLido, &valor) == 2) {
        if (valor > melhor) melhor = valor; // Atualiza se encontrar um valor maior
    }
    fclose(arquivo);
    return melhor;
}

// Salva o estado completo das entidades no arquivo binário ("wb")[cite: 1]
bool salvarJogoBinario(void) {
    FILE *arquivo = fopen(ARQUIVO_SAVE, "wb"); // "wb": escrita em modo binário[cite: 1]
    if (arquivo == NULL) return false;

    // Primeiro grava a quantidade de entidades para saber quantas ler depois[cite: 1]
    fwrite(&totalEntidades, sizeof(int), 1, arquivo);
    // Grava as structs uma a uma[cite: 1]
    for (int i = 0; i < totalEntidades; i++) {
        fwrite(vetorEntidades[i], sizeof(Entidade), 1, arquivo);
    }

    fclose(arquivo);
    return true;
}

// Carrega o estado do jogo lendo o arquivo binário ("rb")[cite: 1]
bool carregarJogoBinario(void) {
    FILE *arquivo = fopen(ARQUIVO_SAVE, "rb"); // "rb": leitura em modo binário[cite: 1]
    if (arquivo == NULL) return false;

    int totalSalvo = 0;
    // Lê quantas entidades foram salvas[cite: 1]
    if (fread(&totalSalvo, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return false;
    }

    liberarTodasEntidades(); // Limpa as entidades atuais antes de carregar as salvas[cite: 1]
    for (int i = 0; i < totalSalvo; i++) {
        Entidade *e = (Entidade *)malloc(sizeof(Entidade)); // Aloca memória para a nova entidade[cite: 1]
        if (fread(e, sizeof(Entidade), 1, arquivo) != 1) { // Lê os dados do arquivo para a struct[cite: 1]
            free(e);
            break;
        }
        adicionarEntidade(e);
    }

    fclose(arquivo);
    return true;
}

int main(void) {
    srand((unsigned int)time(NULL)); // Inicializa gerador de números aleatórios

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 6 - Exercicio 1");
    SetTargetFPS(60);

    // Variável para armazenar o nome do jogador[cite: 1]
    char nomeJogador[16] = "Jogador1";

    // Criando jogador e adicionando ao jogo
    Entidade *jogador = criarEntidade(ENTIDADE_JOGADOR,
                                      (Vector2){ LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f });
    adicionarEntidade(jogador);

    // Gerando inimigos em posições aleatórias
    for (int i = 0; i < TOTAL_INIMIGOS; i++) {
        Vector2 pos = { GetRandomValue(30, LARGURA_JANELA - 30), GetRandomValue(30, ALTURA_JANELA - 30) };
        adicionarEntidade(criarEntidade(ENTIDADE_INIMIGO, pos));
    }
    // Gerando itens em posições aleatórias
    for (int i = 0; i < TOTAL_ITENS; i++) {
        Vector2 pos = { GetRandomValue(30, LARGURA_JANELA - 30), GetRandomValue(30, ALTURA_JANELA - 30) };
        adicionarEntidade(criarEntidade(ENTIDADE_ITEM, pos));
    }

    int pontuacao = 0;
    int melhorPontuacao = lerMelhorPontuacao(); // Busca recorde no arquivo de texto[cite: 1]
    char mensagem[64] = "";
    float tempoMensagem = 0.0f;

    // Loop principal do jogo
    while (!WindowShouldClose()) {

        // Movimentação do jogador
        float vel = 250.0f * GetFrameTime();
        if (IsKeyDown(KEY_RIGHT)) jogador->pos.x += vel;
        if (IsKeyDown(KEY_LEFT))  jogador->pos.x -= vel;
        if (IsKeyDown(KEY_UP))    jogador->pos.y -= vel;
        if (IsKeyDown(KEY_DOWN))  jogador->pos.y += vel;

        // Verifica colisões entre o jogador e as outras entidades
        for (int i = 1; i < totalEntidades; i++) {
            Entidade *e = vetorEntidades[i];
            if (!colidiu(jogador, e)) continue;

            if (e->tipo == ENTIDADE_ITEM) {
                pontuacao += e->extra.valor;
                removerEntidade(i); // Remove item coletado
                i--;
            } else if (e->tipo == ENTIDADE_INIMIGO) {
                jogador->vida -= e->extra.dano;
                if (jogador->vida < 0) jogador->vida = 0;
            }
        }

        // Tecla F5: salva o placar com o nome do jogador no texto[cite: 1]
        if (IsKeyPressed(KEY_F5)) {
            salvarPlacarTexto(nomeJogador, pontuacao);
            if (pontuacao > melhorPontuacao) melhorPontuacao = pontuacao;
            TextCopy(mensagem, "Placar salvo em placar.txt!");
            tempoMensagem = 2.0f;
        }

        // Tecla F6: salva o estado atual do jogo no binário[cite: 1]
        if (IsKeyPressed(KEY_F6)) {
            bool ok = salvarJogoBinario();
            TextCopy(mensagem, ok ? "Jogo salvo em save.bin!" : "Erro ao salvar save.bin!");
            tempoMensagem = 2.0f;
        }

        // Tecla F9: recarrega o jogo salvo do arquivo binário[cite: 1]
        if (IsKeyPressed(KEY_F9)) {
            bool ok = carregarJogoBinario();
            if (ok) jogador = vetorEntidades[0]; // Atualiza o ponteiro para o novo endereço do jogador[cite: 1]
            TextCopy(mensagem, ok ? "Jogo carregado de save.bin!" : "Nenhum save.bin encontrado!");
            tempoMensagem = 2.0f;
        }

        if (tempoMensagem > 0.0f) tempoMensagem -= GetFrameTime(); // Cronômetro do texto explicativo

        //desenha tudo na tela
        BeginDrawing();
            ClearBackground(RAYWHITE);

          
            for (int i = 0; i < totalEntidades; i++) {
                desenharEntidade(vetorEntidades[i]);
            }

           
            DrawText(TextFormat("Jogador: %s | Vida: %d | Pontos: %d | Recorde: %d",
                                nomeJogador, jogador->vida, pontuacao, melhorPontuacao), 10, 10, 20, DARKGRAY);
            DrawText("F5 salva placar (texto) | F6 salva jogo (binario) | F9 carrega jogo (binario)",
                      10, 34, 18, GRAY);
            DrawText("Setas movem o jogador | ESC sai", 10, ALTURA_JANELA - 25, 16, GRAY);

            if (tempoMensagem > 0.0f) {
                DrawText(mensagem, 10, 58, 20, DARKGREEN);
            }

        EndDrawing();
    }

    liberarTodasEntidades();
    CloseWindow();
    return 0;
}