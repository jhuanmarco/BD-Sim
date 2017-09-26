#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<math.h>

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
	clean_stdin();
	return menu;
}	

int mainMenu(){
	int menu;
	printf("Main Menu:\n\n1 - Adicionar Registro\n2 - Remover Registro\n3 - Printar Registros\n0 - Exit\n");
	scanf(" %d", &menu);
	clean_stdin();
	return menu;
}

void trataEnter(char string[]){
	int len;
	len = strlen(string) - 1;
	if(string[len] == '\n') string[len] = '\0';
}


void ondeEsta(FILE *cade){
	int aqui = ftell(cade);
	printf("\n\n\n\nTA NO %d\n\n\n\n", aqui);
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

	printf("Digite o tipo da coluna %s: \nC - Char\nI - Integer\nF - Float\n", nomeColuna);
	do{
		scanf(" %c", &tipo);
	} while ((tipo != 'c') && (tipo != 'i') && (tipo != 'f'));

	if(tipo == 'c'){
		printf("Digite a quantidade de caracteres da coluna %s: ", nomeColuna);	// CALCULAR
		do{
			scanf(" %d", &tamChar);
		} while(tamChar <= 0 || tamChar > 4000);
		
		*tamSlot += tamChar;
		
	} else {
		*tamSlot += 4;
		tamChar = 0;
	}
	
		sprintf(buffer,",%s!%c:", nomeColuna, tipo);
		tamCol += 7;
		fwrite(buffer, sizeof(char), strlen(buffer), arquivo);
		fwrite(&tamChar, sizeof(int), 1, arquivo);
	return tamCol;
}

int calculaBitmap(int tamSlot, int *quantidadeSlots){ // ta ok
	int tamanhoFinal, qntSlots, qntBits, sobrou;

	qntSlots = 4091/tamSlot;
	
	qntBits = 4091%tamSlot;
	
	sobrou = qntBits % 4;
	qntBits /= 4; //espaco em int
	
	qntBits *= 32;//cada int armazena 4 bytes, cada 4bytes tenho 32 posicoes

	while(qntSlots > qntBits) {
		qntSlots--;
		sobrou = sobrou + tamSlot%4;
		qntBits = qntBits + ((tamSlot/4) * 32);	
	}
	
	*quantidadeSlots = qntSlots;

	return qntBits;
}	

void criaPagina(char caminho[], int tamSlot){ //ta ok
	FILE *arquivo = fopen(caminho, "r+b");
	int qntBits, deslocamento = -1, zero = 0, qntSlots; //quantidade de slots que a pagina tera
	
	char liberaEspaco = ' ', barra = '/', compara;
	
	
	fseek(arquivo, -1, SEEK_END);
	fread(&compara, sizeof(char), 1, arquivo);
	
	
	if(compara == '/') {
		compara = '*';
		fseek(arquivo, -1, SEEK_END); 
		fwrite(&compara, sizeof(char),1, arquivo);
	}
	
	for(int i = 0; i < 4096; i++) {
		fwrite(&liberaEspaco, sizeof(char), 1, arquivo);
	}
	
	
	qntBits = calculaBitmap(tamSlot, &qntSlots);
	
	qntBits = qntBits/32; //em inteiros
	
	deslocamento = -5 -(qntBits * 4); //para obter a quantidade de bytes que preciso deslocar para salvar
	
	fseek(arquivo, deslocamento, SEEK_END);
	
	for(int i = 0; i < qntBits; i++ ){
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
	
	printf("\nTabela - %s\n", nomeTable);
	printf("Criacao de Colunas.\n\n");
	
	
	for(int i = 0; i < 8; i++){
		fwrite(&espaco, sizeof(char), 1, arquivo); 
	}
	fwrite(nomeTable, sizeof(char), strlen(nomeTable), arquivo);
	tam += criaColuna(numCampos, arquivo, &tamSlot);
	
	do {
		printf("\nDeseja criar mais algum campo?\n1 - Sim\n0 - Nao\n");
		
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
	
	
	criaPagina("tabela.dat",tamSlot);
	fclose(arquivo);
	
		
	return 0;

};

void  SetBit( int A[],  int k ){
      A[k/32] |= 1 << (k%32);  // Set the bit at the k-th position in A[i]
}


void  ClearBit( int A[],  int k ){
      A[k/32] &= ~(1 << (k%32));
}


int TestBit( int A[],  int k ){
      int i = ( (A[k/32] & (1 << (k%32) )) != 0 ) ;     	
      return i;
}

int buscaEndereco(int *endPagina, char caminho[], int tamHeader){

	int qntSlots, intPonteiro = 0, tamSlot, vetor,  deslocaFrente, controle = 0, test = 0, deslocPonteiro,i,j;
	FILE *arquivo = fopen(caminho, "r+b");
	FILE *arquivo2;	
	char testaPonteiro;
	
	
	fseek(arquivo, 4, SEEK_SET);
	fread(&tamSlot, sizeof(int), 1, arquivo);
	*endPagina = 0;	
	deslocPonteiro = 4091;
	
	//encontra final da pagina
	
	deslocaFrente = -1;
	
	do{
		
		*endPagina = *endPagina + 1;
		
		fseek(arquivo, (tamHeader + (deslocPonteiro * (*endPagina)) + (controle*5)), SEEK_SET);
		controle++;
		
		fread(&qntSlots, sizeof(int), 1, arquivo);
		
		fread(&testaPonteiro, sizeof(char), 1, arquivo);
		
		if(testaPonteiro == '*') intPonteiro = 1; //tem proxima pagina
		
		fseek(arquivo, -1, SEEK_CUR); //pois vai para o final
		
		do{
			
			deslocaFrente++; //0
			
			fseek(arquivo, (-4*2), SEEK_CUR);
			fread(&vetor, sizeof(int), 1, arquivo);
			
			i = 0;
			do{
				test = TestBit(&vetor, i);
				if(test == 0) { // se test = 0  tem lugar disponivel
					SetBit(&vetor, i);
					fseek(arquivo, -4, SEEK_CUR);
					fwrite(&vetor, sizeof(int), 1, arquivo);
					fclose(arquivo);
					return i;
					
					
				} 
			
				qntSlots--;
				i++;
			
			}while((qntSlots != 0) && (i < 32));
		
			
		}while(qntSlots > 0); //caso nao tenha mais slots na pagina
			
			deslocaFrente = deslocaFrente*4+5;
			fseek(arquivo, deslocaFrente, SEEK_CUR);
			
	}while(intPonteiro);
	
	*endPagina++;
	i=0;
	
	criaPagina("tabela.dat", tamSlot);
	arquivo2 = fopen(caminho, "r+a");
	
	fseek(arquivo2, -9, SEEK_END);
	fread(&vetor, sizeof(int), 1, arquivo2);
	SetBit(&vetor, i);
	fseek(arquivo2, -4, SEEK_CUR);
	fwrite(&vetor, sizeof(int), 1, arquivo2);
	
	fclose(arquivo2);
	
	return i;
}

void adicionaRegistro(char caminho[]){
	FILE *arquivo = fopen(caminho, "r+b");
	char nomeTabela[50], tipoCampos[99];
	char camposNome[99][99];
	char *str = malloc(sizeof(char) * 4096);
	char verificador, ignora;
	int endBitMap, endPagina, aux, aux2, tamHeader, tamSlot, i=-1, j=-1;
	int tamanhoCampos[99];
	float auxf;
	
	fread(&tamHeader, sizeof(int),1, arquivo);
	fread(&tamSlot, sizeof(int), 1, arquivo);
	
	do{ //lendo cabecalho
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
		fread(&tipoCampos[i], sizeof(char), 1, arquivo);
		fread(&ignora, sizeof(char), 1, arquivo);
		fread(&tamanhoCampos[i], sizeof(int), 1, arquivo);
		fread(&verificador, sizeof(char), 1, arquivo); 

	} while(verificador == ','); 

	i++; //i possui a quantidade de campos, aqui terminou de ler o cabeçalho
	

	printf("\n%s:\nA tabela %s possui %d campos:\n", nomeTabela,nomeTabela, i);	
	
	endBitMap = buscaEndereco(&endPagina, "tabela.dat", tamHeader);  //endColuna possui onde deve ser inserido e endPagina possui a pagina
	
	fseek(arquivo,tamHeader+(4096*(endPagina-1))+(endBitMap*tamSlot),SEEK_SET); //arquivo esta no lugar da insercao
	
	
	
	for(j = 0; j < i; j++){
		if(tipoCampos[j] == 'c'){
			
			do{
				printf("\nDigite a Coluna '%s' (CHAR - Max %d Caractereres):", camposNome[j], tamanhoCampos[j]); 	
				fgets(str, (tamanhoCampos[j] + 2),stdin);
			} while(strlen(str) <= 1);			

			trataEnter(str);			
			aux2 = strlen(str);
					
			if(tamanhoCampos[j] > aux2){
				while(tamanhoCampos[j] > aux2){
					str[aux2] = ' ';
					aux2++;
				} 
				str[aux2] = '\0';
			} else {
				str[aux2-1] = '\0';
			}
			
			fwrite(str, sizeof(char), tamanhoCampos[j], arquivo);
			
		} else if(tipoCampos[j] == 'i'){
			printf("\nDigite a Coluna '%s' (INT):", camposNome[j]);
			scanf(" %d", &aux);
			fwrite(&aux, sizeof(int), 1, arquivo);
		} else if(tipoCampos[j] == 'f'){
			printf("\nDigite a Coluna '%s' (FLOAT):", camposNome[j]);
			scanf(" %f", &auxf);
			fwrite(&auxf, sizeof(int), 1, arquivo);
		}
	}
	
	printf("\n\nCadastro realizado com sucesso, RID(%d,%d)\n\n", endPagina, endBitMap+1);
	fclose(arquivo);

}




void removeRegistro(char caminho[]){
	FILE *arquivo = fopen(caminho, "r+b");
	int pagina, bitMap, tamHeader, tamSlot, qntSlots, contPaginas = 1, deslocamento, controle = 0, numInt, numVetor, aux;
	char separador;
	
	
	fseek(arquivo, 0, SEEK_SET);
	fread(&tamHeader, sizeof(int), 1, arquivo); 
	fread(&tamSlot, sizeof(int), 1, arquivo); 
	
	fseek(arquivo, -5, SEEK_END);
	fread(&qntSlots, sizeof(int), 1, arquivo);
	
	fseek(arquivo, 0, SEEK_SET);
	
	deslocamento = tamHeader + 4095;
	fseek(arquivo, deslocamento, SEEK_SET);
	deslocamento = 4095;
	
	while(separador == '*'){
		contPaginas++;
		fseek(arquivo, deslocamento, SEEK_CUR);
		fread(&separador, sizeof(char), 1, arquivo);
	}	
		
	do{
		printf("\nDigite o RID da informação\nPagina: ");
		scanf(" %d", &pagina);
		printf("BitMap: ");
		scanf(" %d", &bitMap);
	} while(pagina <= 0 || bitMap <= 0);
	
	if(bitMap > qntSlots){
		printf("\nSlot Inexistente\n");
		return;
	}
	
	if(pagina > contPaginas){
		printf("\nPagina Inexistente\n");
		return;
	}
	
	controle = pagina-1;
	deslocamento = tamHeader + controle*5 + pagina*4091;
	fseek(arquivo,deslocamento,SEEK_SET);
	
	numInt = bitMap/32;
	deslocamento = (numInt*4) + 4;
	deslocamento *= -1;
	
	fseek(arquivo,deslocamento,SEEK_CUR);
	fread(&numInt, sizeof(int), 1, arquivo);
	
	numVetor = bitMap%32;
	aux = TestBit(&numInt, numVetor-1);
	if(aux == 0) {
		printf("\nSlot já vazio\n");
		return;
	} 
	
	ClearBit(&numInt, numVetor-1);
	fseek(arquivo,-4,SEEK_CUR);
	fwrite(&numInt, sizeof(int), 1, arquivo);
	
	fclose(arquivo);
	printf("\nSlot limpo com sucesso\n");
}

void printa(int pag, int slot, char caminho[]){
	char nomeTabela[50], tipoCampos[99];
	char camposNome[99][99];
	char *charAux = malloc(sizeof(char)*4095); 
	char ignora, verificador;
	int i=-1, j=-1, k, tamHeader, tamslot, deslocamento, intAux;
	int tamanhoCampos[99];
	float floatAux;
	
	
	FILE *arquivo = fopen(caminho, "r+b");
	
	fread(&tamHeader, sizeof(int), 1, arquivo);
	fread(&tamslot, sizeof(int), 1, arquivo);
	
	do{ //lendo cabecalho
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
		fread(&tipoCampos[i], sizeof(char), 1, arquivo);
		fread(&ignora, sizeof(char), 1, arquivo);
		fread(&tamanhoCampos[i], sizeof(int), 1, arquivo);
		fread(&verificador, sizeof(char), 1, arquivo); 

	} while(verificador == ','); 
	
	i++;

	deslocamento = tamHeader + (pag*4096) + (slot*tamslot);
	fseek(arquivo, deslocamento, SEEK_SET);
	
	for(j = 0; j < i; j++){
		printf("%s - ", camposNome[j]);
		if(tipoCampos[j] == 'c'){
			for(k = 0; k < tamanhoCampos[j]; k++){
				fread(&charAux[k], sizeof(char), 1, arquivo);			
			}
			charAux[k] = '\0';
			printf("%s\n",charAux); 
		} else if(tipoCampos[j] == 'i'){
		
			fread(&intAux, sizeof(int), 1, arquivo);
			printf("%d\n",intAux); 
			
		} else if(tipoCampos[j] == 'f'){
		
			fread(&floatAux, sizeof(int), 1, arquivo);
			printf("%f\n",floatAux); 	
		}	
	}
	
	
}

void printaRegistros(char caminho[]){
	int i=-1, j=-1, k;
	int tamHeader, tamSlot, deslocamento, controle = 0, auxSlots, qntSlots, deslocamentoInterno, vetor, test, testaProx, pagBusca, slotBusca;
	char prox;
	char nomeTabela[50];
	
	FILE *arquivo = fopen(caminho, "r+b");
	
	fread(&tamHeader, sizeof(int), 1, arquivo);
	fread(&tamSlot, sizeof(int), 1, arquivo);
	
	printf(" \n\n%d\n\n", tamHeader);
	
	
	
	do{
		i++;
		fread(&nomeTabela[i], sizeof(char), 1, arquivo);
	} while(nomeTabela[i] != ',');
	
	
	
	nomeTabela[i] = '\0';
	printf(" \n\n%s\n\n", nomeTabela);

	fseek(arquivo, -5, SEEK_END);
	fread(&qntSlots, sizeof(int), 1, arquivo);
	
	printf(" \n\n%d\n\n", qntSlots);
	controle = 0; // == pag 1
	
	fseek(arquivo, 0, SEEK_SET);
	
	ondeEsta(arquivo);
	
	do {	
		auxSlots = qntSlots;
		printf(" \n\n%d\n\n", auxSlots);
		deslocamento = tamHeader + ((controle+1)*4096);
		ondeEsta(arquivo);
		fseek(arquivo, deslocamento, SEEK_SET);
		ondeEsta(arquivo);
		fseek(arquivo, -1, SEEK_CUR);
		ondeEsta(arquivo);
		fread(&prox, sizeof(char), 1, arquivo);
		ondeEsta(arquivo);
		
		if(prox == '*') {
			testaProx = 1;
		} else { 
			testaProx = 0;
		}
		// para baixo
		do {	
			fseek(arquivo, -1, SEEK_CUR);
			deslocamentoInterno = -2*4;
			fseek(arquivo, deslocamentoInterno, SEEK_CUR); //aponta o int para ler
			fread(&vetor, sizeof(int), 1, arquivo);
			
			k = 0;
			do{
				test = TestBit(&vetor, k);
				printf(" %d\n", test);
				if(test == 1) { // se test = 1 printa
					pagBusca = controle + 1;
					slotBusca = k +1;			
					printf("RID(%d,%d):\n", pagBusca, slotBusca);
					printa(controle, k, caminho);					
				}
				
				auxSlots--;
				k++;
			} while((k < 32) && (auxSlots < qntSlots)); 
			
		} while (auxSlots < qntSlots);
		
		controle ++;
		
	} while (testaProx == 1); 
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
			case 2: 
				removeRegistro("tabela.dat");
			break;
			case 3:
				printaRegistros("tabela.dat");
			break;
			case 0:
			
			break;
		}

	} while(menu != 0);
	
	fclose(rt);
}
