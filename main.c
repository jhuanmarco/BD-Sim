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

void  SetBit( int A[],  int k ){
      A[k/32] |= 1 << (k%32);  // Set the bit at the k-th position in A[i]
}


void  ClearBit( int A[],  int k ){
      A[k/32] &= ~(1 << (k%32));
}


int TestBit( int A[],  int k ){
      return ( (A[k/32] & (1 << (k%32) )) != 0 ) ;     
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


int calculaBitmap(int tamSlot, int *quantidadeSlots){
	int tamanhoFinal, qntSlots, qntBits;

	qntSlots = 4091/tamSlot;
	qntBits = 4091%tamSlot;
	
	qntBits /= 4; //espaco em int
	qntBits *= 32;//cada int armazena 4 bytes, cada 4bytes tenho 32 posicoes

	while(qntSlots > qntBits) {
		qntSlots--;
		qntBits = qntBits + ((tamSlot/4) * 32);	
	}
	
	*quantidadeSlots = qntSlots;

	return qntBits;
}	

void criaPagina(char caminho[], int tamSlot){
	FILE *arquivo = fopen(caminho, "r+b");
	int qntBits, deslocamento = -1, zero = 0, qntSlots; //quantidade de slots que a pagina tera
	fseek(arquivo, 0, SEEK_END);

	char liberaEspaco = ' ', barra = '/';
	for(int i = 0; i < 4096; i++) {
		fwrite(&liberaEspaco, sizeof(char), 1, arquivo);
	}
	
	qntBits = calculaBitmap(tamSlot, &qntSlots);
	deslocamento -= -5*((qntBits/ 32) * 4); //para obter a quantidade de inteiros que preciso salvar
	
	fseek(arquivo, deslocamento, SEEK_END);
	for(int i = 0; i < qntBits/32; i++ ){
			fwrite(&zero, sizeof(int), 1, arquivo);
	}
	
	fwrite(&qntSlots, sizeof(int), 1, arquivo);
	fwrite(&barra, sizeof(char), 1, arquivo);

	fclose(arquivo);
}

int criarTabela(FILE *arquivo){
	int len, numCampos = 1, controlCampos = 1, tam = 8;
	char nomeTable[50], espaco = ' ';	
	int tamSlot = 0;

	
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
		
	if(tamSlot > 4087) {
		printf("Tabela não possível de ser implementada");
		return -1;
	};
	
	fclose(arquivo);
	criaPagina("tabela.dat",tamSlot);
		
	return 0;

};



void adicionaRegistro(char caminho[]){
	FILE *arquivo = fopen(caminho, "r+b");
	char nomeTabela[50];
	int tamHeader, tamSlot, i = -1, j=-1;
	int numPagina, numSlot;
	char camposNome[99][99];
	int tamanhoCampos[99];
	char tipoCampos[99];
	char verificador, ignora;

	fread(&tamHeader, sizeof(int),1, arquivo);
	fread(&tamSlot, sizeof(int), 1, arquivo);
	
	do{
		i++;
		fread(&nomeTabela[i], sizeof(char), 1, arquivo);
	} while(nomeTabela[i] != ',');
	
	nomeTabela[i] = '\0';

	i = -1;
			
	do{
		i++;
		j=-1;

		do{
			j++;
			fread(&camposNome[i][j], sizeof(char), 1, arquivo);
		} while (camposNome[i][j] != '!');
		
		camposNome[i][j] = '\0';
		printf("\n\n%s\n\n", camposNome[i]);
		
		fread(&tipoCampos[i], sizeof(char), 1, arquivo);
		fread(&ignora, sizeof(char), 1, arquivo);
		fread(&tamanhoCampos[i], sizeof(int), 1, arquivo);
		fread(&verificador, sizeof(char), 1, arquivo); 

	} while(verificador == ','); 

	i++; //i possui a quantidade de campos

	printf("\n\n A tabela %s possui %d campos:\n", nomeTabela, i);
	for(j = 0; j < i; j++){
		printf("nome %s\n tipo %c\n tamanho %d\n", camposNome[j], tipoCampos[j], tamanhoCampos[j]); 
	}

	printf("\n\n");
}

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

		switch(menu){
			case 1:
				adicionaRegistro("tabela.dat");
			break;
		}

	} while(menu != 0);
	
	if(wt == NULL) printf("\n\ntambeieh");

	FILE *opa = fopen("tabela.dat", "rb");
	fseek(opa, 9, SEEK_SET);
	char amigo[50];

	fread(amigo, sizeof(char), 5, opa);
	printf("\n\n%s\n\n", amigo);
}
