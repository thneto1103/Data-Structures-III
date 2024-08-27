#include "functions.c"
#include <unistd.h>
//extern int root;
//
void driver(FILE* file_dados, FILE* file_indice){

    int promovido;
    int root;

    Cabecalho reg_cabecalho;

    char* promoKey = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\0";
    int promoRRN;

    char chave[55];

    Registro* registros = (Registro*) malloc(500*sizeof(Registro));
    leitura_binario(registros, file_dados);
    fclose(file_dados);

    if (file_indice != NULL){ // (SE O ARQUIVO EXISTE)
        printf("O arquivo existe.\n");
        set_cabecalho(file_indice, &reg_cabecalho, '1', 0, 1);
        //root = getroot(file_indice, &reg_cabecalho);
        root = 0;
    }
    else{
        // CRIACAO DO ARQUIVO BINARIO DE INDICE
        printf("Arquivo inexistente. Criando arquivo...");
        file_indice = fopen("arvB.bin", "wb+");
        printf("\nArquivo criado.");

        // INSERINDO A CHAVE DO RRN 0 DO ARQUIVO BIN DE DADOS NA RAIZ DA ARVORE
        set_cabecalho(file_indice, &reg_cabecalho, '1', 0, 1);

        strcpy(chave, registros[0].nomeTecnologiaOrigem);
        strcat(chave, registros[0].nomeTecnologiaDestino);

        int filhos_nulos[TAM_ORDEM];
        memset(filhos_nulos, -1, TAM_ORDEM);

        root = criarNo(file_indice, promoKey, filhos_nulos, &reg_cabecalho);
        //set_cabecalho(file_indice, &reg_cabecalho, '1', 0, 1);
    }
    //----------------------FIM DA INICIALIZACAO ------------------------------    
    
    // INSERINDO TODAS AS OUTRAS CHAVES NA ARVORE
    // O PROGRAMA ESTÁ LERDO POR CAUSA DESSA PARTE ABAIXO
    int i = 1;
    while (registros[i].nomeTecnologiaOrigem != NULL){
        strcpy(chave, registros[i].nomeTecnologiaOrigem);
        strcat(chave, registros[i].nomeTecnologiaDestino);
        i++;
        // *verificar se insert está sendo executada mesmo só sendo utilizada pelo if()
        printf("%d, ", root);
        promovido = insert(root, chave, &promoRRN, &promoKey, &reg_cabecalho, file_indice);
        if (promovido){ // SE  PROMOTION
            int filhos_iniciais[TAM_ORDEM];
            filhos_iniciais[0] = root;
            filhos_iniciais[1] = promoRRN; // SERA QUE A ORDEM DE *root e promoRRN é invertida?????????????
            root = criarNo(file_indice, chave, filhos_iniciais, &reg_cabecalho);
            //set_cabecalho(file_indice, &reg_cabecalho, '1', root, root+1);
        }
        //set_cabecalho(file_indice, &reg_cabecalho, '1', root, root+1);
    }
    set_cabecalho(file_indice, &reg_cabecalho, '1', root, root+1);
} // >>>>>>>>>>>>>>>>>>>> FIM DA FUNCAO DRIVER

int criarNo(FILE* file_indice, char* chave, int filhos[TAM_ORDEM], Cabecalho* reg_cabecalho){

    Node novoNo;
    //int RRN = 0;
    
    int RRN = conta_registros(file_indice);
    
    setNode(&novoNo);
    for(int i = 0; i < 55; i++){
        novoNo.chaves[0].chave[i] = chave[i];
    }
    //novoNo.chaves[0].chave = chave;
    
    novoNo.filhos[0] = filhos[0];
    novoNo.filhos[1] = filhos[1];

    novoNo.nroChavesNo = 1;
    novoNo.alturaNo = 1;

    writeNode(file_indice, RRN, &novoNo);
    fseek(file_indice, 1, SEEK_SET);
    set_cabecalho(file_indice, reg_cabecalho, '1', RRN, RRN+1);
    return RRN;
} 
// >>>>>>>>>>>>> FIM DA criarNo()

int insert(int currentRRN, char key[55], int* promoChild, char** promoKey, Cabecalho* reg_cabecalho, FILE* file_indice){

    // PROMOTION = 1
    // NO PROMOTION = 0
    // ERROR = -1
    Node page, newpage; 
    int PBRRN = 0; // RRN do nó promovido de um nível inferior
    int found, promovido;
    int pos;
    
    //char PBkey[55];
    char* PBkey = (char*)malloc(55*sizeof(char)); // chave promovida
    memset(PBkey, '$', 55);
    PBkey[55] = '\0';

    if (currentRRN == -1){
        strcpy(*promoKey, key);
        *promoChild = -1;
        return 1;
    }

    readNode(file_indice, currentRRN, &page);
    //found = search(key, &page, &pos);
    found = 0;
    
    if (found){
        printf("Erro!");
        return 0;
    }
    int value = insert(page.filhos[pos], key, &PBRRN, &PBkey, reg_cabecalho, file_indice); // definiu o PBRRN anteriormente??

    if (!value){
        return 0;
    }
    if (page.nroChavesNo < TAM_ORDEM - 1){
        ins_page(PBkey, PBRRN, &page);
        writeNode(file_indice, currentRRN, &page);
        return 0;
    }
    else{
        split(file_indice, PBkey, PBRRN, &page, *promoKey, promoChild, &newpage, reg_cabecalho);
        writeNode(file_indice, currentRRN, &page);
        writeNode(file_indice, *promoChild, &newpage);
        return 1;
    }
    free(PBkey);
}  // >>>> FIM DO INSERT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int ins_page(char* key, int child, Node* page){
    int i;

    for (i = page->nroChavesNo; key < page->chaves[i - 1].chave && i > 0; i--){
        page->chaves[i] = page->chaves[i-1];
        page->filhos[i+1] = child;
    }
    page->nroChavesNo++;
    strcpy(page->chaves[i].chave, key);
    page->filhos[i+1] = child;
}

void split(FILE* file_indice, char* newkey, int newkeyChildRRN, Node* page, char* promoKey, int* promoChild, Node *newpage, Cabecalho* reg_cabecalho){
    
    // newkey = nova chave a ser inserida
    // newkeyChildRRN = filho a direita da nova chave a ser inserida
    // page = página de disco corrente
    // promoKey = chave promovida
    // promoChild = filho a direita da chave promovida
    // newpage = nova página de disco

    //set_cabecalho();

    typedef struct tmpPage{
        int nroChavesNo;
        int alturaNo;
        int RRNdoNo;
        int filhos[TAM_ORDEM+1]; // Pi, RRN dos nós filhos
        key chaves[TAM_ORDEM]; // Ci
    } TmpPage;

    char* empty_key = (char*) malloc(55*sizeof(char));
    memset(empty_key, '$', 55);
    empty_key[54] = '\0';


    TmpPage tmpPage;

    tmpPage.alturaNo = page->alturaNo;

    int i;
    for (i = 0; i < TAM_ORDEM - 1; i++){
        strcpy(tmpPage.chaves[i].chave, page->chaves[i].chave);
        tmpPage.filhos[i] = page->filhos[i];
    }
    tmpPage.filhos[i] = page->filhos[i];
    //tmpPage.nroChavesNo = TAM_ORDEM - 1;

    for (i = TAM_ORDEM - 1; strcmp(newkey, tmpPage.chaves[i - 1].chave) < 0 && i > 0; i--){
        strcpy(tmpPage.chaves[i].chave, tmpPage.chaves[i - 1].chave);
        tmpPage.filhos[i + 1] = tmpPage.filhos[i];
    }

    strcpy(tmpPage.chaves[i].chave, newkey);
    tmpPage.filhos[i+1] = newkeyChildRRN;

    //*promoChild = getpage(file_indice);
    *promoChild = conta_registros(file_indice);
    setNode(newpage);

    for (i=0; i < (TAM_ORDEM - 1)/2; i++) {
        strcpy(page->chaves[i].chave, tmpPage.chaves[i].chave);
        page->filhos[i] = tmpPage.filhos[i];
        strcpy(newpage->chaves[i].chave, tmpPage.chaves[i+ 1 + (TAM_ORDEM-1)/2].chave);
        newpage->filhos[i] = tmpPage.filhos[i+ 1 + (TAM_ORDEM-1)/2];
        strcpy(page->chaves[i+(TAM_ORDEM-1)/2].chave, empty_key);
        page->filhos[i + 1 + (TAM_ORDEM-1)/2] = -1;
    }

    page->filhos[(TAM_ORDEM - 1)/2] = tmpPage.filhos[TAM_ORDEM/2 -1];
    newpage->filhos[(TAM_ORDEM - 1)/2] = tmpPage.filhos[i + TAM_ORDEM/2];
    newpage->nroChavesNo = (TAM_ORDEM - 1) - (TAM_ORDEM - 1)/2;
    page->nroChavesNo = (TAM_ORDEM - 1)/2;
    //strcpy(promoKey, tmpPage.chaves[(TAM_ORDEM - 1)/2].chave); // ???? verificar essa linha
    promoKey = tmpPage.chaves[(TAM_ORDEM -1)/2].chave; // :::::??::::::::
}

int getroot(FILE* file_indice, Cabecalho* reg_cabecalho){
    // buffer, sizeof, numero, ponteiro
    int root;
    long lseek(); // @@@@&@&@&@&@&@&@&@&&@ ISSO É MESMO NECESSÁRIO???

    root = consultar_cabecalho(reg_cabecalho, file_indice, 1);

    /*lseek(file_indice, 0l, SEEK_SET);
    if (fread(&root, 4, 1, file_indice) == 0){
        printf("Erro ao buscar o RRN da raiz");
        exit(1);
    }*/
    return root;
}
// retorna a posição da página se ela não estiver ocupada 
int getpage(FILE* file_indice){
    int fd = fileno(file_indice);
    off_t posicao_atual = lseek(fd, 0, SEEK_END);
    off_t posicao_disponivel = posicao_atual + 205;
    return posicao_disponivel;
}

void setNode(Node* node){
    //cria uma chave vazia (preenchida por $)
    char* empty_key = (char*) malloc(55*sizeof(char));
    memset(empty_key, '$', 55);
    empty_key[54] = '\0';
    //seta as informações da variável node
    int j;
    for (int j = 0; j < TAM_ORDEM - 1; j++){
        strcpy(node->chaves[j].chave, empty_key);
        node->filhos[j] = -1;
    }
    node->filhos[TAM_ORDEM - 1] = - 1;
    free(empty_key);
}

int search(char* key, Node *page, int *pos){
    //busca por uma chave e retorna 1 se encontrar
    int i;
    for (i = 0; i < page->nroChavesNo && strcmp(key, page->chaves[i].chave); i++)
        ;
    *pos = i;
    //if (*pos < page->nroChavesNo && key == page->chaves[*pos].chave)
    if (*pos < page->nroChavesNo && strcmp(key, page->chaves[*pos].chave) == 0)
        return 1; 
    else
        return 0;
}

void set_cabecalho(FILE* file_indice, Cabecalho* reg_cabecalho, char status, int noRaiz, int RRNproxNo){
    //escreve um cabeçalho com as informações passadas no input
    reg_cabecalho->status = status;
    reg_cabecalho->noRaiz = noRaiz;
    reg_cabecalho->RRNproxNo = RRNproxNo;

    char lixo_cabecalho[196];
    memset(lixo_cabecalho, '$', 196);
    lixo_cabecalho[195] = '\0';

    fseek(file_indice, 0, SEEK_SET);

    //  ESCRITA DO CABECALHO NO ARQUIVO E INDICE
    fwrite(&reg_cabecalho->status, sizeof(char), 1, file_indice);
    fwrite(&reg_cabecalho->noRaiz, sizeof(int), 1, file_indice);
    fwrite(&reg_cabecalho->RRNproxNo, sizeof(int), 1, file_indice);
    fwrite(lixo_cabecalho, 1, 196, file_indice); // AQUI TEM QUE TER &?
} // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> FIM DA set_cabecalho

int consultar_cabecalho(Cabecalho *cabecalho, FILE *arquivoIndice, int n)
{
    //retorna uma informação do cabeçalho a depender da entrada n 
    switch (n){
    case 0:
        return cabecalho->status;
    case 1:
        return cabecalho->noRaiz;
    case 2:
        return cabecalho->RRNproxNo;
    }
}

int conta_registros(FILE* arq){
    //conta o numero digitos não vazios do registro
    int buffer;
    int counter = -1;
    fseek(arq, 0, SEEK_SET); // tem esse +1 mesmo?
    while(fread(&buffer, sizeof(char), 205, arq) != 0){
        counter++;
    }
    return (counter*205)+205;
}

void writeNode(FILE* file_indice, int RRN, Node* node){
    //escreve o nó no arquivo de índice 
    fseek(file_indice, (RRN+1)*205, SEEK_SET);
    fwrite(&node->nroChavesNo, 4, 1,file_indice);
    fwrite(&node->alturaNo, 4, 1,file_indice);
    fwrite(&node->RRNdoNo, 4, 1,file_indice);
    for (int k = 0; k < TAM_ORDEM; k++){
        fwrite(&node->filhos[k], 4, TAM_ORDEM, file_indice);
        if (k < TAM_ORDEM - 1){
            fwrite(&node->chaves[k].chave, 1, 55, file_indice);
            fwrite(&node->chaves[k].RRN, 4, 1, file_indice);
        }
    }
}

void readNode(FILE* file_indice, int RRN, Node* node){
    //lê o nó no arquivo de índice
    fseek(file_indice, (RRN+1)*205, SEEK_SET);
    fread(&node->nroChavesNo, 4, 1, file_indice);
    fread(&node->alturaNo, 4, 1, file_indice);
    fread(&node->RRNdoNo, 4, 1, file_indice);
    for (int k = 0; k < TAM_ORDEM; k++){
        fread(&node->filhos[k], 4, TAM_ORDEM, file_indice);
        if (k < TAM_ORDEM - 1){
            fread(&node->chaves[k].chave, 1, 55, file_indice);
            fread(&node->chaves[k].RRN, 4, 1, file_indice);
        }
    }
}

int main(){


    FILE* file_dados = fopen("binary.bin", "rb");
    FILE* file_indice = fopen("arvB.bin", "rb+");

    printf("teste inicio\n");
    driver(file_dados, file_indice);
    printf("\nteste final\n");

    //free(root);
    fclose(file_indice);
    fclose(file_dados);

    return 0;
}