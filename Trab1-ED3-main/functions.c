#include "header.h"

// %%%%%%%%%%%%%%%%%%% FUNCOES AUXILIARES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void leitura_binario(Registro *registros, FILE *arq){

    int i = 0;
    char byte;

    fseek(arq, 13, SEEK_SET);

    //while (fread(&registros[i].removido, sizeof(char), 1, arq) == 1) {
    while (fread(&registros[i].removido, sizeof(char), 1, arq) != 0) { 
    	
        fread(&registros[i].grupo, sizeof(int), 1, arq);
        fread(&registros[i].popularidade, sizeof(int), 1, arq);
        fread(&registros[i].peso, sizeof(int), 1, arq);
        fread(&registros[i].tamanhoTecnologiaOrigem, sizeof(int), 1, arq);

        registros[i].nomeTecnologiaOrigem = (char *)malloc(registros[i].tamanhoTecnologiaOrigem*sizeof(char)+1);
        fread(registros[i].nomeTecnologiaOrigem, sizeof(char), registros[i].tamanhoTecnologiaOrigem, arq);


        fread(&registros[i].tamanhoTecnologiaDestino, sizeof(int), 1, arq);

        registros[i].nomeTecnologiaDestino = (char *)malloc(registros[i].tamanhoTecnologiaDestino*sizeof(char));
        fread(registros[i].nomeTecnologiaDestino, sizeof(char), registros[i].tamanhoTecnologiaDestino, arq);

        registros[i].nomeTecnologiaOrigem[registros[i].tamanhoTecnologiaOrigem] = '\0';
        registros[i].nomeTecnologiaDestino[registros[i].tamanhoTecnologiaDestino] = '\0';


        fseek(arq, 76 - 21 - registros[i].tamanhoTecnologiaOrigem - registros[i].tamanhoTecnologiaDestino, SEEK_CUR);
        i++;
    }
    return;
}

char* conteudoEntreAspas(const char* input) { // FUNCAO DO ChatGPT
    
    const char* inicio = strchr(input, '"'); // Encontra a primeira aspa
    if (inicio == NULL) {
        return NULL; // Não há aspas
    }

    inicio++; // Avança para o próximo caractere após a primeira aspa

    const char* fim = strchr(inicio, '"'); // Encontra a segunda aspa
    if (fim == NULL) {
        return NULL; // Não há segunda aspa
    }

    // Calcula o tamanho do conteúdo entre as aspas
    size_t tamanho = fim - inicio;

    // Aloca memória para o conteúdo entre as aspas
    char* resultado = (char*)malloc(tamanho + 1);
    if (resultado == NULL) {
        return NULL; // Falha na alocação de memória
    }

    // Copia o conteúdo entre as aspas para o resultado
    strncpy(resultado, inicio, tamanho);
    resultado[tamanho] = '\0'; // Adiciona um caractere nulo no final

    return resultado;
}

Registro campo_compativel(FILE* arq_indice, Registro registro, char* campo, char* input){

    if (!strcmp(campo, "peso")){
        if (atoi(input) == registro.peso){
            return registro;
        }
    }

    if (!strcmp(campo, "popularidade")){
        if (atoi(input) == registro.popularidade){
            return registro;
        }
    }

    if (!strcmp(campo, "grupo")){
        if (atoi(input) == registro.grupo){
            return registro;
        }
    }

    if (!strcmp(campo, "nomeTecnologiaOrigem")){
        //scan_quote_string(input);
        if (!strcmp(conteudoEntreAspas(input), registro.nomeTecnologiaOrigem)){
            return registro;
        }
    }

    if (!strcmp(campo, "nomeTecnologiaDestino")){
        //scan_quote_string(input);
        if (!strcmp(conteudoEntreAspas(input), registro.nomeTecnologiaDestino)){
            return registro;
        }
    }

    if (!strcmp(campo, "nomeTecnologiaOrigemDestino")){
        // fazer busca no arquivo de indice
        return registro; // não é essa linha
    }
}



// %%%%%%%%%%%%%%%%%%%% FIM DAS FUNCOES AUXILIARES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




// %%%%%%%%%%%%%%%%%%%%% FUNCIONALIDADES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void funcionalidade3(FILE* arq, FILE* arq_indice, char vet_nomes_campos[][21], char** vet_valores_campos, int n){

    Registro* registros; // ponteiro para criacao de uma lista de registros
    registros = malloc(TAM*sizeof(Registro));

    leitura_binario(registros, arq); // lendo o arquivo binario de DADOS configurando a lista de registros

    int i = 0;
    for (int k = 0; k < n; k++){
        while(registros[i].nomeTecnologiaOrigem != NULL){
            // comparando os campos fornecidos com os campos esperados
            campo_compativel(arq_indice, registros[i], vet_nomes_campos[k], vet_valores_campos[k]);
            i++;
        }
        i = 0;
    }

    free(registros);
    fclose(arq);
}

void funcionalidade6(FILE* file_dados, FILE* file_indice){

    int n; // NUMERO DE CAMPOS A SEREM LIDOS
    scanf("%d", &n);
    char entrada[n][21]; // VETOR DE CAMPOS
    char** valores_campos = (char**) malloc(n*sizeof(char)); // VETOR DE VALORES DE CAMPOS
    for(int j = 0; j < n; j++){
        valores_campos[j] = (char*) malloc(21);
    }

    for (int i = 0; i < n; i++){
        scanf("%s", entrada[i]);
        scanf("%s", valores_campos[i]);
    }
    // LEMBRAR QUE A BUSCA ESTÁ SENDO FEITA NA FUNCAO CAMPO COMPATIVEL, CHAMADA DENTRO DA FUNC 3
    funcionalidade3(file_dados, file_indice,entrada, valores_campos, n);

    
    // LIBERANDO MEMÓRIA
    for(int j = 0; j < n; j++){
        free(valores_campos[j]);
    }
    free(valores_campos);
}