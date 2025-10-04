#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definição da struct Territorio
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// Função para limpar o buffer do teclado
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função para cadastrar territórios
void cadastrarTerritorios(Territorio* mapa, int total) {
    for (int i = 0; i < total; i++) {
        printf("\n--- Cadastro do Território %d ---\n", i + 1);

        printf("Nome do território: ");
        fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor do exército: ");
        fgets(mapa[i].cor, sizeof(mapa[i].cor), stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Número de tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }
}

// Função para exibir territórios
void exibirTerritorios(Territorio* mapa, int total) {
    printf("\n=== Territórios Cadastrados ===\n");
    for (int i = 0; i < total; i++) {
        printf("\nTerritório %d\n", i + 1);
        printf("Nome: %s\n", mapa[i].nome);
        printf("Cor: %s\n", mapa[i].cor);
        printf("Tropas: %d\n", mapa[i].tropas);
        printf("--------------------------\n");
    }
}

// Função para simular ataque
void atacar(Territorio* atacante, Territorio* defensor) {
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("\nErro: Não é possível atacar um território da mesma cor!\n");
        return;
    }

    if (atacante->tropas <= 1) {
        printf("\nErro: O território atacante precisa ter mais de 1 tropa para atacar!\n");
        return;
    }

    printf("\n=== Batalha entre %s (Atacante) e %s (Defensor) ===\n",
           atacante->nome, defensor->nome);

    // Rolagem de dados
    int dadoAtacante = (rand() % 6) + 1; // 1 a 6
    int dadoDefensor = (rand() % 6) + 1; // 1 a 6

    printf("Dado do atacante: %d\n", dadoAtacante);
    printf("Dado do defensor: %d\n", dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("\nO atacante venceu a batalha!\n");

        // O defensor muda de dono
        strcpy(defensor->cor, atacante->cor);

        // O atacante move metade das tropas para o território conquistado
        int tropasMovidas = atacante->tropas / 2;
        defensor->tropas = tropasMovidas;
        atacante->tropas -= tropasMovidas;

    } else {
        printf("\nO defensor resistiu ao ataque!\n");
        atacante->tropas -= 1; // atacante perde 1 tropa
    }
}

// Função para liberar memória
void liberarMemoria(Territorio* mapa) {
    free(mapa);
    printf("\nMemória liberada com sucesso.\n");
}

// Função principal
int main() {
    srand(time(NULL)); // garante aleatoriedade

    int qtdTerritorios;
    printf("Digite o número total de territórios: ");
    scanf("%d", &qtdTerritorios);
    limparBufferEntrada();

    // Alocação dinâmica
    Territorio* mapa = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro ao alocar memória!\n");
        return 1;
    }

    // Cadastro inicial
    cadastrarTerritorios(mapa, qtdTerritorios);

    int opcao;
    do {
        printf("\n===== MENU =====\n");
        printf("1 - Exibir territórios\n");
        printf("2 - Realizar ataque\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                exibirTerritorios(mapa, qtdTerritorios);
                break;
            case 2: {
                exibirTerritorios(mapa, qtdTerritorios);

                int idAtacante, idDefensor;
                printf("Escolha o número do território atacante: ");
                scanf("%d", &idAtacante);
                printf("Escolha o número do território defensor: ");
                scanf("%d", &idDefensor);
                limparBufferEntrada();

                if (idAtacante < 1 || idAtacante > qtdTerritorios ||
                    idDefensor < 1 || idDefensor > qtdTerritorios) {
                    printf("Erro: índices inválidos!\n");
                } else if (idAtacante == idDefensor) {
                    printf("Erro: um território não pode atacar a si mesmo!\n");
                } else {
                    atacar(&mapa[idAtacante - 1], &mapa[idDefensor - 1]);
                }
                break;
            }
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    liberarMemoria(mapa);
    return 0;
}


