#include <stdio.h>
#include <string.h>

#define max_territorios 5
#define max_string 100

//criando a struct
struct territorio {
    char nome[30];
    char cor[10];
    int tropas;
};
//limpando o buffer
void limparbufferdeentrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
//função principal
int main() {
    struct territorio exercito[max_territorios];
    int total_territorio = 0;

    printf("Vamos cadastrar os %d territórios:\n", max_territorios);

    // criando o laço for para imprimir todos os territorios
    for (total_territorio = 0; total_territorio < max_territorios; total_territorio++) {
        printf("\n--- Cadastrando território %d ---\n", total_territorio + 1);

    //cadastro dos territorios
        printf("Nome do território: ");
        fgets(exercito[total_territorio].nome, sizeof(exercito[total_territorio].nome), stdin);
        exercito[total_territorio].nome[strcspn(exercito[total_territorio].nome, "\n")] = '\0'; // remove \n

        printf("Cor do exército: ");
        fgets(exercito[total_territorio].cor, sizeof(exercito[total_territorio].cor), stdin);
        exercito[total_territorio].cor[strcspn(exercito[total_territorio].cor, "\n")] = '\0';

        printf("Total de tropas: ");
        scanf("%d", &exercito[total_territorio].tropas);
        limparbufferdeentrada();
    }
    
    //imprimindo todo o laço
    printf("\n--- Territórios cadastrados ---\n");
    for (int i = 0; i < max_territorios; i++) {
        printf("\nTerritório %d\n", i + 1);
        printf("Nome: %s\n", exercito[i].nome);
        printf("Cor: %s\n", exercito[i].cor);
        printf("Tropas: %d\n", exercito[i].tropas);
    }

    return 0;
}
