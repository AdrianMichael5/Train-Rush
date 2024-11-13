#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include "time.h"

// Estrutura para o jogador
typedef struct {
    int x, y;
} Jogador;

// Funções auxiliares
void iniciarJogo(Jogador *jogador);
void desenharJogador(Jogador *jogador);
void desenharVagoes(int linhaY, int espacoX);
void limparLinha(int linhaY);
void verificarColisao(Jogador *jogador, int linhaY, int espacoX, int *jogoEncerrado);
void imprimirPontuacao(int pontuacao);

int main() {
    Jogador jogador;
    int jogoEncerrado = 0;
    int pontuacao = 0;
    int tecla = 0;
    long temporizador = 0;
    int contadorAtualizacao = 0; // Variável de controle para a velocidade dos vagões

    // Inicializações
    screenInit(1);
    keyboardInit();
    timerInit(100);
    srand(time(NULL));

    iniciarJogo(&jogador);
    screenUpdate();

    int linhaY = 1; // Posição inicial da linha dos vagões
    int espacoX = rand() % (MAXX - 8) + 4; // Posição aleatória do espaço na linha

    while (!jogoEncerrado && tecla != 10) { // Até o jogador apertar "Enter" ou perder
        if (keyhit()) {
            tecla = readch();
            if ((tecla == 'e' || tecla == 'E') && jogador.x < MAXX - 2) jogador.x++; // Mover para a direita
            if ((tecla == 'q' || tecla == 'Q') && jogador.x > 1) jogador.x--; // Mover para a esquerda
        }

        if (timerTimeOver() == 1) {
            contadorAtualizacao++;
            if (contadorAtualizacao % 2 == 0) { // Diminui a velocidade dos vagões (ajustável)
                limparLinha(linhaY - 1); // Limpa a linha anterior para remover os vagões antigos
                linhaY++; // Move a linha de vagões para baixo

                if (linhaY >= MAXY) { // Quando os vagões saem da tela
                    linhaY = 1; // Reinicia no topo
                    espacoX = rand() % (MAXX - 8) + 4; // Gera um novo espaço aleatório
                    pontuacao++; // Incrementa a pontuação
                }

                desenharVagoes(linhaY, espacoX);
                desenharJogador(&jogador);
                verificarColisao(&jogador, linhaY, espacoX, &jogoEncerrado);
                imprimirPontuacao(pontuacao);
                screenUpdate();
            }
            temporizador++;
        }
    }

    // Mensagem de final de jogo
    screenSetColor(RED, BLACK);
    screenGotoxy(MAXX / 2 - 5, MAXY / 2);
    printf("VOCÊ PERDEU!");

    // Limpeza
    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}

void iniciarJogo(Jogador *jogador) {
    jogador->x = MAXX / 2;
    jogador->y = MAXY - 2;
    screenClear(); // Garante que a tela esteja limpa no início do jogo
}

void desenharJogador(Jogador *jogador) {
    screenSetColor(GREEN, BLACK);
    screenGotoxy(jogador->x, jogador->y);
    printf("A"); // Representa o jogador
}

void desenharVagoes(int linhaY, int espacoX) {
    screenSetColor(RED, BLACK);
    for (int x = 1; x <= MAXX; x++) {
        // Aumenta o espaço para o jogador passar
        if (x < espacoX || x > espacoX + 3) { // Deixa um espaço maior (4 caracteres de largura)
            screenGotoxy(x, linhaY);
            printf("V");
        }
    }
}

void limparLinha(int linhaY) {
    if (linhaY > 0 && linhaY < MAXY) {
        for (int x = 1; x <= MAXX; x++) {
            screenGotoxy(x, linhaY);
            printf(" ");
        }
    }
}

void verificarColisao(Jogador *jogador, int linhaY, int espacoX, int *jogoEncerrado) {
    if (jogador->y == linhaY && (jogador->x < espacoX || jogador->x > espacoX + 3)) {
        *jogoEncerrado = 1; // O jogador colidiu com um vagão
    }
}

void imprimirPontuacao(int pontuacao) {
    screenSetColor(WHITE, BLACK);
    screenGotoxy(2, 2);
    printf("Vagões passados: %d", pontuacao);
}