#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include "time.h"

struct Jogador {
    int x, y;
    int x_ant, y_ant;
};

#define AREA_MIN_X 10
#define AREA_MAX_X 70
#define AREA_MIN_Y 5
#define AREA_MAX_Y 20

float distanciaTotal = 0;

void iniciarJogo(struct Jogador *jogador);
void desenharJogador(struct Jogador *jogador);
void limparPosicaoAnterior(struct Jogador *jogador);
void desenharVagoes(float linhaY, int espacoX);
void limparVagoes(int linhaY);
void desenharBorda();
void verificarColisao(struct Jogador *jogador, int linhaY, int espacoX, int *jogoEncerrado);
void imprimirDistancia(int metros);
float selecionarDificuldade();
void exibirTelaInicial();

int main() {
    exibirTelaInicial();
    float velocidadeVagoes = selecionarDificuldade();

    struct Jogador jogador;
    int jogoEncerrado = 0;
    int tecla = 0;
    float linhaY = AREA_MIN_Y + 1;
    int espacoX = rand() % (AREA_MAX_X - AREA_MIN_X - 8) + AREA_MIN_X + 2;
    int contadorAtualizacao = 0;

    screenInit(0);
    desenharBorda();
    keyboardInit();
    timerInit(100);
    srand(time(NULL));

    iniciarJogo(&jogador);
    screenUpdate();

    while (!jogoEncerrado && tecla != 10) {
        if (keyhit()) {
            tecla = readch();
            jogador.x_ant = jogador.x;
            jogador.y_ant = jogador.y;

            if ((tecla == 'e' || tecla == 'E') && jogador.x < AREA_MAX_X - 1) jogador.x++;
            if ((tecla == 'q' || tecla == 'Q') && jogador.x > AREA_MIN_X + 1) jogador.x--;

            limparPosicaoAnterior(&jogador);
            desenharJogador(&jogador);
            screenUpdate();
        }

        if (timerTimeOver() == 1) {
            linhaY += velocidadeVagoes;
            distanciaTotal += 0.1;

            if ((int)linhaY != contadorAtualizacao) {
                limparVagoes(contadorAtualizacao);
                contadorAtualizacao = (int)linhaY;
            }

            if (linhaY > AREA_MAX_Y) {
                limparVagoes(AREA_MAX_Y);
                linhaY = AREA_MIN_Y + 1;
                espacoX = rand() % (AREA_MAX_X - AREA_MIN_X - 8) + AREA_MIN_X + 2;
            }

            desenharVagoes(linhaY, espacoX);
            desenharJogador(&jogador);
            imprimirDistancia((int)distanciaTotal);
            verificarColisao(&jogador, (int)linhaY, espacoX, &jogoEncerrado);
            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}

void iniciarJogo(struct Jogador *jogador) {
    jogador->x = (AREA_MIN_X + AREA_MAX_X) / 2;
    jogador->y = AREA_MAX_Y - 2;
    jogador->x_ant = jogador->x;
    jogador->y_ant = jogador->y;
    screenClear();
    desenharBorda();
    screenUpdate();
}

void desenharJogador(struct Jogador *jogador) {
    screenSetColor(GREEN, BLACK);
    screenGotoxy(jogador->x, jogador->y);
    printf("🏃");
}

void limparPosicaoAnterior(struct Jogador *jogador) {
    screenGotoxy(jogador->x_ant, jogador->y_ant);
    printf(" ");
}

void desenharVagoes(float linhaY, int espacoX) {
    screenSetColor(RED, BLACK);
    int yInteiro = (int)linhaY;
    if (yInteiro > AREA_MIN_Y && yInteiro < AREA_MAX_Y) {
        for (int x = AREA_MIN_X + 2; x < AREA_MAX_X; x++) {
            if (x < espacoX || x > espacoX + 3) {
                screenGotoxy(x, yInteiro);
                printf("▅");
            }
        }
    }
}

void limparVagoes(int linhaY) {
    if (linhaY > AREA_MIN_Y && linhaY < AREA_MAX_Y) {
        for (int x = AREA_MIN_X + 2; x < AREA_MAX_X; x++) {
            screenGotoxy(x, linhaY);
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

void verificarColisao(struct Jogador *jogador, int linhaY, int espacoX, int *jogoEncerrado) {
    if (jogador->y == linhaY && (jogador->x < espacoX || jogador->x > espacoX + 3)) {
        *jogoEncerrado = 1;
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

void exibirTelaInicial() {
    screenClear();
    screenSetColor(YELLOW, BLACK);
    screenGotoxy(AREA_MIN_X + 15, AREA_MIN_Y - 3);
    printf("Train-Rush");
    screenSetColor(WHITE, BLACK);
    screenGotoxy(AREA_MIN_X + 10, AREA_MIN_Y);
    printf("Pressione qualquer tecla para iniciar...");
    screenUpdate();
    getchar();
}
