#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Estruturas
 */
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

typedef struct {
    char nome[30];
    char cor[10];
    char *missao;   // apontador para string alocada dinamicamente
    int missaoId;   // id da missão sorteada (útil para checagem)
} Jogador;

/*
 * Protótipos
 */
void limparBufferEntrada(void);

Territorio* criarMapa(int qtd);
void cadastrarTerritorios(Territorio* mapa, int total);
void exibirTerritorios(const Territorio* mapa, int total);

void atacar(Territorio* atacante, Territorio* defensor);

char* criarMissaoAleatoria(char* missoes[], int totalMissoes, int *saidaMissaoId);
void exibirMissao(const char *missao);
int verificarMissao(int missaoId, const char *corJogador, const Territorio* mapa, int tamanho, const Jogador* jogadores, int numJogadores);

Jogador* criarJogadores(int num);
void liberarMemoria(Territorio* mapa, Jogador* jogadores, int numJogadores);

/*
 * Missões disponíveis (descrições)
 * Obs: a checagem faz-se pelo missaoId, não por parsing de string.
 */
#define NUM_MISSOES 5

int main(void) {
    srand((unsigned int)time(NULL));

    printf("=== MISSÕES E BATALHAS (versão simplificada) ===\n");

    // 1) criar mapa dinamicamente
    int qtdTerritorios;
    printf("Digite o número total de territórios: ");
    if (scanf("%d", &qtdTerritorios) != 1 || qtdTerritorios <= 0) {
        printf("Quantidade inválida.\n");
        return 1;
    }
    limparBufferEntrada();

    Territorio *mapa = criarMapa(qtdTerritorios);
    if (mapa == NULL) {
        printf("Erro de alocação.\n");
        return 1;
    }

    // cadastrar
    cadastrarTerritorios(mapa, qtdTerritorios);

    // 2) criar jogadores (fixo 2 jogadores para este exercício)
    int numJogadores = 2;
    Jogador *jogadores = criarJogadores(numJogadores);
    if (jogadores == NULL) {
        free(mapa);
        return 1;
    }

    // pedir nome e cor para cada jogador e sortear missão
    char *missoes[NUM_MISSOES] = {
        "Conquistar 3 territorios (controlar 3 territorios).",
        "Eliminar todas as tropas da cor 'vermelho' (zerar tropas vermelhas).",
        "Ter pelo menos 5 tropas a mais que o oponente (diferença >=5).",
        "Controlar a maioria dos territorios (mais da metade).",
        "Controlar pelo menos 2 territorios."
    };

    for (int i = 0; i < numJogadores; ++i) {
        printf("\n--- Dados do Jogador %d ---\n", i + 1);
        printf("Nome do jogador: ");
        fgets(jogadores[i].nome, sizeof(jogadores[i].nome), stdin);
        jogadores[i].nome[strcspn(jogadores[i].nome, "\n")] = '\0';

        printf("Cor do jogador (ex: vermelho, azul, verde): ");
        fgets(jogadores[i].cor, sizeof(jogadores[i].cor), stdin);
        jogadores[i].cor[strcspn(jogadores[i].cor, "\n")] = '\0';

        // atribuir missão aleatória e armazenar dinamicamente
        jogadores[i].missao = criarMissaoAleatoria(missoes, NUM_MISSOES, &jogadores[i].missaoId);
        printf("\nMissao atribuida (apenas exibida uma vez):\n");
        exibirMissao(jogadores[i].missao);
    }

    // 3) loop principal: alterna turnos entre jogadores até missão ser cumprida ou user sair
    int jogadorAtual = 0;
    int opcao;
    int jogoAtivo = 1;

    while (jogoAtivo) {
        printf("\n========================================\n");
        printf("Turno de: %s (cor: %s)\n", jogadores[jogadorAtual].nome, jogadores[jogadorAtual].cor);
        printf("1 - Exibir mapa\n");
        printf("2 - Atacar\n");
        printf("0 - Sair do jogo\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            limparBufferEntrada();
            printf("Entrada invalida.\n");
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                exibirTerritorios(mapa, qtdTerritorios);
                break;
            case 2: {
                exibirTerritorios(mapa, qtdTerritorios);
                int idAtacante, idDefensor;
                printf("Escolha o numero do territorio atacante: ");
                if (scanf("%d", &idAtacante) != 1) { limparBufferEntrada(); break; }
                printf("Escolha o numero do territorio defensor: ");
                if (scanf("%d", &idDefensor) != 1) { limparBufferEntrada(); break; }
                limparBufferEntrada();

                if (idAtacante < 1 || idAtacante > qtdTerritorios || idDefensor < 1 || idDefensor > qtdTerritorios) {
                    printf("Indices invalidos.\n");
                } else if (idAtacante == idDefensor) {
                    printf("Um territorio nao pode atacar a si mesmo.\n");
                } else {
                    Territorio *at = &mapa[idAtacante - 1];
                    Territorio *de = &mapa[idDefensor - 1];

                    // valida se o territorio atacante pertence ao jogador atual
                    if (strcmp(at->cor, jogadores[jogadorAtual].cor) != 0) {
                        printf("Erro: o territorio atacante nao pertence a voce (cor diferente).\n");
                    } else {
                        atacar(at, de);
                        // exibir situação pós-ataque
                        printf("\n--- Situacao apos o ataque ---\n");
                        printf("%s (%s) - Tropas: %d\n", at->nome, at->cor, at->tropas);
                        printf("%s (%s) - Tropas: %d\n", de->nome, de->cor, de->tropas);
                    }
                }
                break;
            }
            case 0:
                jogoAtivo = 0;
                break;
            default:
                printf("Opcao invalida.\n");
        }

        // Ao final do turno, verificar missões (verifica todos jogadores - requisitos)
        for (int i = 0; i < numJogadores; ++i) {
            if (verificarMissao(jogadores[i].missaoId, jogadores[i].cor, mapa, qtdTerritorios, jogadores, numJogadores)) {
                printf("\n========================================\n");
                printf("MISSÃO CUMPRIDA!\nJogador '%s' (cor: %s) cumpriu a missao:\n", jogadores[i].nome, jogadores[i].cor);
                exibirMissao(jogadores[i].missao);
                printf("Vencedor: %s\n", jogadores[i].nome);
                jogoAtivo = 0;
                break;
            }
        }

        // alterna jogador
        if (jogoAtivo) jogadorAtual = (jogadorAtual + 1) % numJogadores;
    }

    // 4) liberar memoria
    liberarMemoria(mapa, jogadores, numJogadores);
    return 0;
}

/* ---------------------------
 * Implementação das funções
 * --------------------------- */

/* limpa buffer stdin */
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* aloca e retorna mapa (calloc) */
Territorio* criarMapa(int qtd) {
    Territorio *m = (Territorio*) calloc(qtd, sizeof(Territorio));
    return m;
}

/* cadastra territórios com leitura de stdin */
void cadastrarTerritorios(Territorio* mapa, int total) {
    for (int i = 0; i < total; ++i) {
        printf("\n--- Cadastro do Territorio %d ---\n", i + 1);
        printf("Nome do territorio: ");
        fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor do exercito (ex: vermelho, azul, verde): ");
        fgets(mapa[i].cor, sizeof(mapa[i].cor), stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Numero de tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1) {
            limparBufferEntrada();
            printf("Valor invalido. Digite um inteiro: ");
        }
        limparBufferEntrada();
    }
}

/* imprime lista de territorios */
void exibirTerritorios(const Territorio* mapa, int total) {
    printf("\n=== MAPA (%d territorios) ===\n", total);
    for (int i = 0; i < total; ++i) {
        printf("%2d) %s | Cor: %s | Tropas: %d\n", i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("=============================\n");
}

/* simula ataque com dados (1..6) e atualiza territorios via ponteiros */
void atacar(Territorio* atacante, Territorio* defensor) {
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("Erro: nao se pode atacar territorio da mesma cor.\n");
        return;
    }
    if (atacante->tropas <= 1) {
        printf("Erro: atacante precisa ter mais de 1 tropa para atacar.\n");
        return;
    }

    printf("\n--- BATALHA: %s (%s) -> %s (%s) ---\n", atacante->nome, atacante->cor, defensor->nome, defensor->cor);
    int dadoAt = (rand() % 6) + 1;
    int dadoDf = (rand() % 6) + 1;
    printf("Dado atacante: %d  |  Dado defensor: %d\n", dadoAt, dadoDf);

    if (dadoAt > dadoDf) {
        printf("Atacante venceu! Territorio conquistado.\n");
        // defensor muda de dono
        strcpy(defensor->cor, atacante->cor);
        // transfere metade das tropas do atacante para o defensor
        int mov = atacante->tropas / 2;
        defensor->tropas = mov;
        atacante->tropas -= mov;
    } else {
        printf("Defensor resistiu. Atacante perde 1 tropa.\n");
        atacante->tropas = (atacante->tropas > 0) ? atacante->tropas - 1 : 0;
    }
}

/* cria string de missao dinamicamente e retorna ponteiro. também fornece missaoId via saidaMissaoId */
char* criarMissaoAleatoria(char* missoes[], int totalMissoes, int *saidaMissaoId) {
    int id = rand() % totalMissoes;
    const char *orig = missoes[id];
    size_t len = strlen(orig);
    char *copia = (char*) malloc(len + 1);
    if (copia == NULL) return NULL;
    strcpy(copia, orig);
    *saidaMissaoId = id;
    return copia;
}

/* exibe missão (passagem por valor da string) */
void exibirMissao(const char *missao) {
    printf("%s\n", missao);
}

/*
 * verificarMissao:
 * - missaoId: id da missao (0..NUM_MISSOES-1)
 * - corJogador: cor do jogador que tem a missao
 * - mapa/tamanho: estado atual do mapa
 * - jogadores/numJogadores: ajuda para missões envolvendo oponente
 *
 * Retorna 1 se cumprida, 0 caso contrario.
 *
 * Implementação simplificada:
 * 0 -> Conquistar 3 territorios (player controla >= 3)
 * 1 -> Eliminar todas as tropas da cor 'vermelho' (total tropas dessa cor == 0)
 * 2 -> Ter pelo menos 5 tropas a mais que o oponente (dif >=5)
 * 3 -> Controlar a maioria dos territorios (> total/2)
 * 4 -> Controlar pelo menos 2 territorios (>=2)
 */
int verificarMissao(int missaoId, const char *corJogador, const Territorio* mapa, int tamanho, const Jogador* jogadores, int numJogadores) {
    int countControlados = 0;
    int totalTropasDaCor = 0;
    int tropasJogador = 0;
    int tropasOponente = 0;

    // Conta controle e tropas por cor
    for (int i = 0; i < tamanho; ++i) {
        if (strcmp(mapa[i].cor, corJogador) == 0) {
            ++countControlados;
            tropasJogador += mapa[i].tropas;
        } else {
            tropasOponente += mapa[i].tropas;
        }
        // total tropas da cor 'vermelho'
        if (strcmp(mapa[i].cor, "vermelho") == 0) {
            totalTropasDaCor += mapa[i].tropas;
        }
    }

    switch (missaoId) {
        case 0: // conquistar 3 territorios
            return (countControlados >= 3) ? 1 : 0;
        case 1: // eliminar tropas da cor 'vermelho'
            return (totalTropasDaCor == 0) ? 1 : 0;
        case 2: // ter pelo menos 5 tropas a mais que o oponente
            return ((tropasJogador - tropasOponente) >= 5) ? 1 : 0;
        case 3: // controlar maioria
            return (countControlados > (tamanho / 2)) ? 1 : 0;
        case 4: // controlar pelo menos 2 territorios
            return (countControlados >= 2) ? 1 : 0;
        default:
            return 0;
    }
}

/* cria dinamicamente espaço para n jogadores */
Jogador* criarJogadores(int num) {
    Jogador *v = (Jogador*) calloc(num, sizeof(Jogador));
    return v;
}

/* libera toda memoria: mapa e as strings de missao dos jogadores e vetor de jogadores */
void liberarMemoria(Territorio* mapa, Jogador* jogadores, int numJogadores) {
    if (mapa) free(mapa);
    if (jogadores) {
        for (int i = 0; i < numJogadores; ++i) {
            if (jogadores[i].missao) free(jogadores[i].missao);
        }
        free(jogadores);
    }
    printf("\nMemoria liberada. Obrigado por jogar!\n");
}



