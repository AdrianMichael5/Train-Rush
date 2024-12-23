#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include "time.h"

#define AREA_MIN_X 10
#define AREA_MAX_X 70
#define AREA_MIN_Y 5
#define AREA_MAX_Y 20
#define MAX_JOGADORES 10

typedef struct TopScore {
    char *iniciais;          
    int score;               
    struct TopScore *prox;   
} TopScore;

struct Jogador {
    int posicao_x, posicao_y;
    int posicao_x_anterior, posicao_y_anterior;
};

float distancia_total = 0;

void iniciarJogo(struct Jogador *jogador);
void desenharJogador(struct Jogador *jogador);
void limparPosicaoAnterior(struct Jogador *jogador);
void desenharVagoes(float posicao_linha_y, int espaco_x);
void limparVagoes(int linha_y);
void desenharBorda();
void verificarColisao(struct Jogador *jogador, int linha_y, int espaco_x, int *jogo_encerrado);
void imprimirDistancia(int metros);
float selecionarDificuldade();

void adicionarTopScore(TopScore **lista, char *iniciais, int score);
void salvarTopScores(const char *arquivo, TopScore *lista);
TopScore* carregarTopScores(const char *arquivo);
void exibirTopScores(TopScore *lista);
void liberarLista(TopScore *lista);

int main() {
    float velocidade_vagoes = selecionarDificuldade();

    struct Jogador jogador;
    int jogo_encerrado = 0;
    int tecla = 0;
    float linha_y = AREA_MIN_Y + 1;
    int espaco_x = rand() % (AREA_MAX_X - AREA_MIN_X - 8) + AREA_MIN_X + 2;
    int contador_atualizacao = 0;

    TopScore *listaTopScores = carregarTopScores("topscores.txt");

    screenInit(0);
    desenharBorda();
    keyboardInit();
    timerInit(100);
    srand(time(NULL));

    printf("Bem-vindo ao Train Rush!\n");
    exibirTopScores(listaTopScores);

    iniciarJogo(&jogador);
    screenUpdate();

    while (!jogo_encerrado && tecla != 10) {
        if (keyhit()) {
            tecla = readch();
            jogador.posicao_x_anterior = jogador.posicao_x;
            jogador.posicao_y_anterior = jogador.posicao_y;

            if ((tecla == 'e' || tecla == 'E') && jogador.posicao_x < AREA_MAX_X - 1) jogador.posicao_x++;
            if ((tecla == 'q' || tecla == 'Q') && jogador.posicao_x > AREA_MIN_X + 1) jogador.posicao_x--;

            limparPosicaoAnterior(&jogador);
            desenharJogador(&jogador);
            screenUpdate();
        }

        if (timerTimeOver() == 1) {
            linha_y += velocidade_vagoes;
            distancia_total += 0.1;

            if ((int)linha_y != contador_atualizacao) {
                limparVagoes(contador_atualizacao);
                contador_atualizacao = (int)linha_y;
            }

            if (linha_y > AREA_MAX_Y) {
                limparVagoes(AREA_MAX_Y);
                linha_y = AREA_MIN_Y + 1;
                espaco_x = rand() % (AREA_MAX_X - AREA_MIN_X - 8) + AREA_MIN_X + 2;
            }

            desenharVagoes(linha_y, espaco_x);
            desenharJogador(&jogador);
            imprimirDistancia((int)distancia_total);
            verificarColisao(&jogador, (int)linha_y, espaco_x, &jogo_encerrado);
            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    printf("Jogo finalizado!\n");
    printf("Pontuação final: %.2f\n", distancia_total);

    char *iniciais = malloc(4 * sizeof(char));
    printf("Digite suas iniciais (3 letras): ");
    scanf("%3s", iniciais);

    adicionarTopScore(&listaTopScores, iniciais, (int)distancia_total);
    salvarTopScores("topscores.txt", listaTopScores);
    exibirTopScores(listaTopScores);

    liberarLista(listaTopScores);

    return 0;
}

void iniciarJogo(struct Jogador *jogador) {
    jogador->posicao_x = (AREA_MIN_X + AREA_MAX_X) / 2;
    jogador->posicao_y = AREA_MAX_Y - 2;
    jogador->posicao_x_anterior = jogador->posicao_x;
    jogador->posicao_y_anterior = jogador->posicao_y;
    screenClear();
    desenharBorda();
    screenUpdate();
}

void desenharJogador(struct Jogador *jogador) {
    screenSetColor(GREEN, BLACK);
    screenGotoxy(jogador->posicao_x, jogador->posicao_y);
    printf("🏃");
}

void limparPosicaoAnterior(struct Jogador *jogador) {
    screenGotoxy(jogador->posicao_x_anterior, jogador->posicao_y_anterior);
    printf(" ");
}

void desenharVagoes(float posicao_linha_y, int espaco_x) {
    screenSetColor(RED, BLACK);
    int linha_y_inteira = (int)posicao_linha_y;
    if (linha_y_inteira > AREA_MIN_Y && linha_y_inteira < AREA_MAX_Y) {
        for (int x = AREA_MIN_X + 2; x < AREA_MAX_X; x++) {
            if (x < espaco_x || x > espaco_x + 3) {
                screenGotoxy(x, linha_y_inteira);
                printf("▅");
            }
        }
    }
}

void limparVagoes(int linha_y) {
    if (linha_y > AREA_MIN_Y && linha_y < AREA_MAX_Y) {
        for (int x = AREA_MIN_X + 2; x < AREA_MAX_X; x++) {
            screenGotoxy(x, linha_y);
            printf(" ");
        }
    }
}

void desenharBorda() {
    screenSetColor(YELLOW, BLACK);
    for (int y = AREA_MIN_Y; y <= AREA_MAX_Y; y++) {
        screenGotoxy(AREA_MIN_X, y);
        printf("🟨");
        screenGotoxy(AREA_MAX_X, y);
        printf("🟨");
    }
}

void verificarColisao(struct Jogador *jogador, int linha_y, int espaco_x, int *jogo_encerrado) {
    if (jogador->posicao_y == linha_y && (jogador->posicao_x < espaco_x || jogador->posicao_x > espaco_x + 3)) {
        *jogo_encerrado = 1;
    }
}

void imprimirDistancia(int metros) {
    screenSetColor(WHITE, BLACK);
    screenGotoxy(AREA_MIN_X + 2, AREA_MIN_Y - 1);
    printf("Distância: %d metros", metros);
    screenUpdate();
}

float selecionarDificuldade() {
    int escolha = 0;
    while (escolha < 1 || escolha > 4) {
        screenClear();
        screenSetColor(WHITE, BLACK);
        screenGotoxy(AREA_MIN_X + 15, AREA_MIN_Y - 2);
        printf("Bem-vindo ao Train-Rush!");
        screenGotoxy(AREA_MIN_X + 10, AREA_MIN_Y + 1);
        printf("Escolha a dificuldade:");
        screenGotoxy(AREA_MIN_X + 10, AREA_MIN_Y + 3);
        printf("1 - Facil");
        screenGotoxy(AREA_MIN_X + 10, AREA_MIN_Y + 4);
        printf("2 - Media");
        screenGotoxy(AREA_MIN_X + 10, AREA_MIN_Y + 5);
        printf("3 - Dificil");
        screenGotoxy(AREA_MIN_X + 10, AREA_MIN_Y + 6);
        printf("4 - Impossivel");

        screenGotoxy(AREA_MIN_X + 10, AREA_MIN_Y + 8);
        printf("Escolha (1-4): ");
        scanf("%d", &escolha);
    }

    switch (escolha) {
        case 1: return 0.3;
        case 2: return 0.5;
        case 3: return 0.7;
        case 4: return 1.0;
        default: return 0.5;
    }
}

void adicionarTopScore(TopScore **lista, char *iniciais, int score) {
    TopScore *novo = malloc(sizeof(TopScore));
    novo->iniciais = iniciais;
    novo->score = score;
    novo->prox = NULL;

    if (*lista == NULL || (*lista)->score < score) {
        novo->prox = *lista;
        *lista = novo;
        return;
    }

    TopScore *atual = *lista;
    while (atual->prox != NULL && atual->prox->score >= score) {
        atual = atual->prox;
    }

    novo->prox = atual->prox;
    atual->prox = novo;
}


void salvarTopScores(const char *arquivo, TopScore *lista) {
    FILE *fp = fopen(arquivo, "w");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de top scores!\n");
        return;
    }
    TopScore *atual = lista;
    while (atual != NULL) {
        fprintf(fp, "%s %d\n", atual->iniciais, atual->score);
        atual = atual->prox;
    }
    fclose(fp);
}

TopScore* carregarTopScores(const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL) {
        return NULL; 
    }
    TopScore *lista = NULL;
    char iniciais[4];
    int score;

    while (fscanf(fp, "%s %d", iniciais, &score) != EOF) {
        char *iniciais_dinamicas = malloc(4 * sizeof(char));
        strcpy(iniciais_dinamicas, iniciais);
        adicionarTopScore(&lista, iniciais_dinamicas, score);
    }
    fclose(fp);
    return lista;
}

void exibirTopScores(TopScore *lista) {
    printf("\n==== Top Scores ====\n");
    TopScore *atual = lista;
    while (atual != NULL) {
        printf("%s - %d pontos\n", atual->iniciais, atual->score);
        atual = atual->prox;
    }
}

void liberarLista(TopScore *lista) {
    TopScore *atual = lista;
    while (atual != NULL) {
        TopScore *temp = atual;
        atual = atual->prox;
        free(temp->iniciais);
        free(temp);
    }
}