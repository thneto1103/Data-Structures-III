#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_ORDEM 4
#define TAM_MAX 55
#define TAM_STR 100
#define TAM 500

typedef struct Cabecalho
{
	char status;
	int noRaiz; // RRN do no raiz
	int RRNproxNo;
} Cabecalho; // tamanho = 1 + 8 + lixo = 9 + 196 = 205


typedef struct key{
	char chave[55];
	int RRN; // Pri, referencia para o registro correspondente no arquivo de dados, -1 se não houver
} key; // tamanho = 55 + 4 = 59

typedef struct Node{
	int nroChavesNo;
	int alturaNo;
	int RRNdoNo;
	int filhos[TAM_ORDEM]; // Pi, RRN dos nós filhos 
	key chaves[TAM_ORDEM - 1]; // Ci
} Node; // tamanho de cada nó (registro do arq de indice) = 4 * 3 + 4 * TAM_ORDEM + 59 * (TAM_ORDEM - 1) = 12 + 16 + 177 = 205

typedef struct Registro{
    char removido;
    int grupo;
    int popularidade;
    int peso;
    int tamanhoTecnologiaOrigem;
    char* nomeTecnologiaOrigem;
    int tamanhoTecnologiaDestino;
    char* nomeTecnologiaDestino;
} Registro; // tamanho 1 + 5 * 4 + 2*tam_variavel = aprox. 30

// >>>>>>>>>>>>>>>>>>>>>> PROTOTIPOS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void leitura_binario(Registro *registros, FILE *arq);

char* conteudoEntreAspas(const char* input); // FUNCAO DO ChatGPT

Registro campo_compativel(FILE* arq_indice, Registro registro, char* campo, char* input);

void funcionalidade3(FILE* arq, FILE* arq_indice, char vet_nomes_campos[][21], char** vet_valores_campos, int n);

Registro funcionalidade4(FILE* arq, int n);

void funcionalidade6(FILE* file_dados, FILE* file_indice);

int busca(int RRN, char* key, int *foundRRN, int *foundPos, FILE* file_indice);

void driver(FILE* file_dados, FILE* file_indice);

int criarNo(FILE* file_indice, char* chave, int filhos[TAM_ORDEM], Cabecalho* reg_cabecalho);

int insert(int currentRRN, char key[],int *promoChild, char** promoKey, Cabecalho* reg_cabecalho, FILE* file_indice); // lembrar de colocar o arquivo como parametro

int ins_page(char* key, int child, Node* page);

void split(FILE* file_indice, char* newkey, int newkeyChildRRN, Node *page, char* promoKey,int* promoChild, Node *newpage, Cabecalho* reg_cabecalho);

int getroot(FILE* file_indice, Cabecalho* reg_cabecalho);

void setNode(Node* node);

int search(char* key, Node *page, int *pos);

void set_cabecalho(FILE* file_indice, Cabecalho* reg_cabecalho, char status, int noRaiz, int RRNproxNo);

int consultar_cabecalho(Cabecalho *cabecalho, FILE *arquivoIndice, int n);

int conta_registros(FILE* arq);

void writeNode(FILE* file_indice, int RRN, Node* node);

void readNode(FILE* file_indice, int RRN, Node* node);

int getpage(FILE* file_indice);

// >>>>>>>>>>>>>>>>>>>> FIM DOS PROTÓTIPOS DE FUNCAO <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
