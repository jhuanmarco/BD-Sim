#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void clean_stdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int init(){
	int menu;
	printf("BD Sim:\n\n1 - Manter Arquivos\n9 - Novos Arquivos\n0 - Exit\n"); //caso 9, o banco é refeito
	scanf(" %d", &menu);
	return menu;
}	

int mainMenu(){
	int menu;
	printf("Main Menu:\n\n1 - Adicionar Registro\n2 - Remover Registro\n3 - Printar Registros\n0 - Exit\n");
	scanf(" %d", &menu);
	return menu;
}

void trataEnter(char string[]){
	int len;
	len = strlen(string) - 1;
	string[len] = '\0';
}

void criaCampo(int numCampos, FILE *arquivo){
	printf("Digite o nome do %d campo ", numCampos);


}

void criarTabela(FILE *arquivo){
	int len, numCampos = 1, controlCampos = 1;
	char nomeTable[50];	
	
	clean_stdin();
	printf("Digite o nome da sua tabela(tamanho max 30): ");
	
	if(!fgets(nomeTable, 50, stdin)) printf("Erro na leitura");
	trataEnter(nomeTable);
	
	printf("Criacao de Campos(nao e necessario inserir o campo id):\n\n");
	printf("Tabela - %s\n", nomeTable);
	
	criaCampo(numCampos, arquivo);
	
	do {
		printf("Deseja criar mais algum campo?\n1 - Sim\n0 - Nao\n");
		
		do {
			scanf(" %d", &controlCampos);
		} while ((controlCampos != 1) && (controlCampos != 0));
		
		if(controlCampos){
			criaCampo(++numCampos, arquivo);
		} 
			
		
	} while(controlCampos == 1);


};

void main(){
	int initMenu, menu; 
	
	FILE *rt, *wt;
	
	rt = fopen("tabela.dat", "r");
	
	if(!rt) {	//se nao existe o arquivo tabela.dat ainda, entao cria
		wt = fopen("tabela.dat", "w+b");
		criarTabela(wt);
	} else {	//caso exista, então vai para o menu inicial
		do {
			initMenu = init();
			if(!initMenu) return;	
		
		} while((initMenu != 1) && (initMenu != 9)); 
		
		if(initMenu == 9) {	//se quiser excluir os dados antigos e trabalhar numa tabela nova
			wt = fopen("tabela.dat", "w+b");
			criarTabela(wt);
		} else {	//caso queira trabalhar com a tabela ja criada
		 	wt = fopen("tabela.dat", "r+b");	
		}
	}
	
	rt = fopen("tabela.dat", "r+b");
	
	do {
		menu = mainMenu();
			
		

	} while(menu != 0);
	
	
}
