# BD Simulator

Simulação de gerência de paginas e registro utilizando estrutura de Bitmap
## Funcionamento
Cada pagina contém 4096 Bytes, sendo:
- 5 + X Bytes de cada paginas reservado para a configuração
- 1 Byte para indicar se há outra pagina seguinte
- 4 Bytes para denotar a quantidade de slots que a pagina possui
- X Bytes denotando o bitmap

O bitmap trabalha de forma que cada inteiro tem 32 posicoes, logo, caso precise alocar 32 registros apenas 01 inteiro será reservado para tal(ou 4bytes), caso eu aloque 64 registros por paginas será necessario destinar 8bytes para tal operacao

Tanto o byte de verificacao, quanto os 4 bytes de quantidade de slots e até mesmo os N bytes para o bitmap estao alocados no final de cada pagina

Espaço disponivel para registros em cada pagina = 4096 - 5 - X, onde X é o tamanho em bytes do bitmap

## Header
O Header é escrito no inicio do arquivo, ele especifica a forma que o simulador deve ler cada página

Header: XYnomeTabela,nomeCampo!C:Z,...
- X - Tamanho do proprio header para deslocamento futuro
- Y - Tamanho de cada slot
- , - Para demarcar separação de cada campo
- ! - Para demarcar termino do nome do campo
- C - Tipo da informação (char, int ou float)
- : - Para indicar inicio do tamanho
- Z - Tamanho (caso int ou float = 0, caso char pode variar de acordo com que o usuário digitar)

## Compilação
Criando o arquivo executável:
```sh
$ gcc -o exe main.c
```
Executando:
```sh
$ ./exe
```
Onde exe é o nome do executável, podendo ser alterado conforme necessário

**Free Software, Hell Yeah!**
