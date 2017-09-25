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
	if(string[len] == '\n') string[len] = '\0';
}

int criaColuna(int numCampos, FILE *arquivo, int *tamSlot){
	char nomeColuna[50], tipo, buffer[100];
	int tamCol = 1, tamChar;
	

	printf("Digite o nome da %d coluna (max 30 caracteres): ", numCampos);
	

	do{
		if(!fgets(nomeColuna, 50, stdin)) printf("Erro na leitura");
	} while(strlen(nomeColuna) <= 1);

	trataEnter(nomeColuna);	
	if(strlen(nomeColuna) > 30) nomeColuna[30] = '\0';

	tamCol += strlen(nomeColuna);
	printf("\n\n%d\n\n", tamCol);

	printf("Digite o tipo da coluna %s: \nC - Char\nI - Integer\nF - Float\n", nomeColuna);
	do{
		scanf(" %c", &tipo);
	} while ((tipo != 'c') && (tipo != 'i') && (tipo != 'f'));

	if(tipo == 'c'){
		printf("Digite a quantidade de caracteres da coluna %s: ", nomeColuna);	// CALCULAR
		do{
			scanf(" %d", &tamChar);
		} while(tamChar <= 0 || tamChar > 500);
		
		*tamSlot += tamChar;
		
	} else {
		*tamSlot += 4;
		tamChar = 0;
	}
	
		sprintf(buffer,",%s!%c:", nomeColuna, tipo);
		tamCol += 7;
		fwrite(buffer, sizeof(char), strlen(buffer), arquivo);
		fwrite(&tamChar, sizeof(int), 1, arquivo);
		printf("\n\n\n%sOPA\n\n\n", nomeColuna);
	return tamCol;
}


int calculaBitmap(int tamSlot){
	
}	

int criarTabela(FILE *arquivo){
	int len, numCampos = 1, controlCampos = 1, tam = 8;
	char nomeTable[50], espaco = ' ';	
	int tamSlot = 0;

	clean_stdin();
	do {
		printf("Digite o nome da sua tabela(tamanho max 30): ");
		if(!fgets(nomeTable, 50, stdin)) printf("Erro na leitura");
	} while (strlen(nomeTable) <= 1);

	trataEnter(nomeTable);
	if(strlen(nomeTable) > 30) nomeTable[30] = '\0';
	tam += strlen(nomeTable);
	
	printf("Tabela - %s\n\n", nomeTable);
	printf("Criacao de Colunas(nao e necessario inserir a coluna id)\n\n");
	
	
	for(int i = 0; i < 8; i++){
		fwrite(&espaco, sizeof(char), 1, arquivo); 
	}
	fwrite(nomeTable, sizeof(char), strlen(nomeTable), arquivo);
	tam += criaColuna(numCampos, arquivo, &tamSlot);
	
	do {
		printf("Deseja criar mais algum campo?\n1 - Sim\n0 - Nao\n");
		
		do {
			scanf(" %d", &controlCampos);
		} while ((controlCampos != 1) && (controlCampos != 0));
		
		if(controlCampos){
			tam += criaColuna(++numCampos, arquivo, &tamSlot);
		} 
			
		
	} while(controlCampos == 1);

	

	fseek(arquivo, 0, SEEK_SET);
	fwrite(&tam, sizeof(int), 1, arquivo);
	fwrite(&tamSlot, sizeof(int), 1, arquivo);
		
	if(tamSlot + 9 > 4096) {
		printf("Tabela não possível de ser implementada");
		return -1;
	};
	
	fclose(arquivo);
	
		
	return ;

};

void main(){
	int initMenu, menu, setTable; 
	
	FILE *rt, *wt;
	
	rt = fopen("tabela.dat", "r");
	
	if(!rt) {	//se nao existe o arquivo tabela.dat ainda, entao cria
		wt = fopen("tabela.dat", "w+b");
		setTable = criarTabela(wt);
		if (setTable == -1) {
			printf("Tabela impossivel ser criada, excede tamanho maximo");
			return;
		}
	} else {	//caso exista, então vai para o menu inicial
		do {
			initMenu = init();
			if(!initMenu) return;	
		
		} while((initMenu != 1) && (initMenu != 9)); 
		
		if(initMenu == 9) {	//se quiser excluir os dados antigos e trabalhar numa tabela nova
			wt = fopen("tabela.dat", "w+b");
			setTable = criarTabela(wt);
			if (setTable == -1) {
				printf("Tabela impossivel de ser criada, excede tamanho maximo");
				return;
			}
		} else {	//caso queira trabalhar com a tabela ja criada
		 	wt = fopen("tabela.dat", "r+b");	
		}
	}
	
	rt = fopen("tabela.dat", "r+b");
	
	do {
		menu = mainMenu();
			
		

	} while(menu != 0);
	
	if(wt == NULL) printf("\n\ntambeieh");

	FILE *opa = fopen("tabela.dat", "rb");
	fseek(opa, 9, SEEK_SET);
	char amigo[50];

	fread(amigo, sizeof(char), 5, opa);
	printf("\n\n%s\n\n", amigo);
}
