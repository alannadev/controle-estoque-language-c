#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <conio.h>

/*Variavéis que serão utilizadas durante todo o programa*/

FILE *arquivo;
char nome_arquivo[13] = "produtos.txt"; 
typedef struct {
        int chave_primaria;
        char descricao[100];
        int quantidade;
        float valorCompra;
        float valorVenda;
} PRODUTO;


/*Funções e procedimentos secundários*/

/* OPÇÕES DE BUSCA - (char opcaoBusca)
I - Busca pro Id
D - Busca por descrição

Caso a busca seja por descrição, o parâmetro para o ID deve ser -1
*/
void buscaProdutoFiltro(int id,PRODUTO *produto,char opcaoBusca);
void insereProdutoArquivo(FILE *arquivo, PRODUTO *produto,int gerarChavePrimaria);
void buscaLinhaArquivo(FILE *arquivo,PRODUTO *produto);
void mostraMenuPrincipal();
int  geraChavePrimaria(); /*Busca o último id do arquivo e retorna o posterior, semelhante a sequence dos bancos de dado*/
int  qtdLinhasArquivo();
int  tamanhoArquivo(const char* file_name);


/*Procedimentos para design da exibição*/
void clear();
void exibeTitulo();
void insereTraco(int qtd,int quebraLinha);

/*Procedimentos - CRUD*/

void criar();
void atualizar();

void deletar(int id); /*Para a deleção de todos os produtos, basta passar 0 no ID*/
void buscarProdutos(int mostraMenu);
void buscarProdutoPorDescricao();


void criar()
{
    int continua = 1;

    while(continua)
    {
        PRODUTO produto;

        printf("\nINSERÇÃO DE PRODUTOS\n");
        insereTraco(20,1);
    
        printf("DESCRIÇÃO (máx 100 caracteres): \t");
        scanf(" %[^\n]",produto.descricao);
        setbuf(stdout, 0);
        fflush(stdout);

        printf("QUANTIDADE: \t");
        scanf("%d",&produto.quantidade);

        printf("VALOR COMPRA (R$) \t");
        scanf("%f",&produto.valorCompra);

        printf("VALOR VENDA (R$) \t");
        scanf("%f",&produto.valorVenda);

        arquivo =  fopen(nome_arquivo,"a+");

        insereProdutoArquivo(arquivo,&produto,1);

        insereTraco(20,1);
        printf("Continuar cadastrando?\n 1\t SIM\n 0 \t NÃO\n");
        scanf("%d",&continua);

        fclose(arquivo);
    }

    mostraMenuPrincipal();
}

void insereProdutoArquivo(FILE *arquivo, PRODUTO *produto,int gerarChavePrimaria)
{
    int chavaPrimaria;
    if (gerarChavePrimaria)
        chavaPrimaria = geraChavePrimaria();
    else 
    chavaPrimaria = produto->chave_primaria;

    fprintf(arquivo,"%d; %s; %d; %.2f; %.2f\n",chavaPrimaria,produto->descricao,produto->quantidade,produto->valorCompra,produto->valorVenda);
    fflush(arquivo);
}

void atualizar()
{
    int idProduto, opcao;
    PRODUTO produto;
    exibeTitulo();
    buscarProdutos(0);
    printf("\n\n\nINSIRA O ID DO PRODUTO QUE SEJA ALTERAR: \t");
    scanf("%d",&idProduto);
    printf("\nINFORMAÇÃO QUE SERÁ ATUALIZADA\n");
    insereTraco(20,1);
    printf("1- DESCRIÇÃO\n2- VALOR DE COMPRA\n3- VALOR DE VENDA\n4- QUANTIDADE\n");
    scanf("%d",&opcao);

    buscaProdutoFiltro(idProduto,&produto,'I');

    if (opcao == 1)
    {
        printf("Nova descrição:\t");
        scanf(" %[^\n]",produto.descricao);
    }

    else if (opcao ==2)
    {
        printf("Novo valor de compra:\t");
        scanf("%f",&produto.valorCompra);
    }

    else  if (opcao ==3)
    {
        printf("Novo valor de venda:\t");
        scanf("%f",&produto.valorVenda);
    }
        
    else if (opcao == 4)
    {
        printf("Nova quantidade:\t");
        scanf("%d",&produto.quantidade);
    }


    deletar(idProduto);
   
    arquivo = fopen(nome_arquivo,"a+");
    insereProdutoArquivo(arquivo,&produto,0);
    fclose(arquivo);
    insereTraco(20,1);
    printf("PRODUTO ATUALIZADO\n");
    insereTraco(20,1);
    printf("ID: %d\nDescrição: %s \nQuantidade: %d\nValor compra: %.2f\nValor venda: %.2f\n", produto.chave_primaria,produto.descricao,produto.quantidade,produto.valorCompra,produto.valorVenda);

    mostraMenuPrincipal();
}

void deletar(int id)
{
    exibeTitulo();

    int idProduto = id;
    int tamanhoArray = 0;
    PRODUTO *produtos = (PRODUTO*)calloc(0,sizeof(PRODUTO));
    PRODUTO produto;

    if (id == 0)
    {
        buscarProdutos(0);
        printf("INSIRA O ID DO PRODUTO QUE DESEJA EXCLUIR: \t");
        scanf("%d",&idProduto);
        insereTraco(20,1);
    }

    arquivo = fopen(nome_arquivo,"r+");

    while(!feof(arquivo))
    {
       PRODUTO produto;
       buscaLinhaArquivo(arquivo,&produto);
       tamanhoArray++;
       produtos = (PRODUTO*) realloc(produtos, tamanhoArray*sizeof(PRODUTO));
       produtos[tamanhoArray-1] = produto;
    } 

        
    fclose(arquivo);

    arquivo = fopen(nome_arquivo,"w+");
     for (int i=0;i<tamanhoArray;i++)
     {
         produto = produtos[i];

         if (produto.chave_primaria != idProduto)
         {
              insereProdutoArquivo(arquivo,&produto,0);
         }
     }  

      fclose(arquivo); 

    if (id == 0)
    buscarProdutos(1);

}    
    
void buscaProdutoFiltro(int id,PRODUTO *produto,char opcaoBusca)
{
    PRODUTO produtoLocal;  
    
    arquivo = fopen(nome_arquivo,"r+");
    char *ret;
    while(!feof(arquivo))
    {
    buscaLinhaArquivo(arquivo,&produtoLocal);
     if(opcaoBusca == 'I')
     {
          if (produtoLocal.chave_primaria == id)
          {
                 *produto = produtoLocal;
                 break;
          }
          
    }
    else if (opcaoBusca == 'D')
    {
          int i = 0;
          int j = 0;
          while(produtoLocal.descricao[i])
          {
              produtoLocal.descricao[i] = toupper(produtoLocal.descricao[i]);

              i++;
          }
          while(produto->descricao[j])
          {
              produto->descricao[j] = toupper(produto->descricao[j]);
              j++;
          }

          char *ret = strstr(produtoLocal.descricao,produto->descricao);
          if (ret)
          {
             printf("ID: %d\nDescrição: %s \nQuantidade: %d\nValor compra: %.2f\nValor venda: %.2f\n", produtoLocal.chave_primaria,produtoLocal.descricao,produtoLocal.quantidade,produtoLocal.valorCompra,produtoLocal.valorVenda);
             insereTraco(20,1);
          }
    }

     }
   
    fclose(arquivo); 
}

void buscarProdutoPorDescricao()
{

    PRODUTO produto; 
    int resposta = 1;
    while(resposta)
    {
    printf("Insira a descrição do produto para a busca\n");
    scanf(" %[^\n]",produto.descricao);
    insereTraco(20,1);

    buscaProdutoFiltro(-1,&produto,'D'); 


    
    printf("Realizar outra busca?\n 1\t SIM\n 0 \t NÃO\n");
    scanf("%d",&resposta);
    clear();
    }
    clear();
    mostraMenuPrincipal();   
}

void buscarProdutos(int mostraMenu)
{
    arquivo = fopen(nome_arquivo,"r");
    int qtdProduto = 1;
    PRODUTO produto;

if (tamanhoArquivo(nome_arquivo) == 0)
{
    
    printf("\n\nALERTA: ARQUIVO VAZIO. INSIRA INFORMAÇÕES NO MENU ABAIXO\a\n\n\n"); 
    mostraMenuPrincipal();
    
}
else
{
   while(!feof(arquivo))
     {
    
     buscaLinhaArquivo(arquivo,&produto);
    
     printf("\nProduto %d\n",qtdProduto);
 
     insereTraco(20,1);

    printf("ID: %d\nDescrição: %s \nQuantidade: %d\nValor compra: %.2f\nValor venda: %.2f\n", produto.chave_primaria,produto.descricao,produto.quantidade,produto.valorCompra,produto.valorVenda);
     qtdProduto++;

     }
}

    if (mostraMenu)
        mostraMenuPrincipal();

    fclose(arquivo);
    
}

void mostraMenuPrincipal(void)
{
    int opcao;
    exibeTitulo();
    printf("\nOPÇÕES\n");
    insereTraco(38,1);

    printf("| 1 |\tInserção\n");
    insereTraco(38,1);
    printf("| 2 |\tDeleção\n");
    insereTraco(38,1);
    printf("| 3 |\tAtualização\n");
    insereTraco(38,1);
    printf("| 4 |\tExibir todos\n");
    insereTraco(38,1);
    printf("| 5 |\tBusca por descrição\n");
    insereTraco(38,1);
    

    scanf("%d",&opcao);
    clear();

    switch(opcao)
    {
        case 1: criar();break;
        case 2: deletar(0);break;
        case 3: atualizar();break;
        case 4: buscarProdutos(1);break;
        case 5: buscarProdutoPorDescricao();break; 
        default: exit(0);
    }
}


void buscaLinhaArquivo(FILE *arquivo,PRODUTO *produto)
{
    fscanf(arquivo, "%d;%[^;];%d;%f;%f\n",&produto->chave_primaria,&produto->descricao,&produto->quantidade,&produto->valorCompra,&produto->valorVenda);
}

int geraChavePrimaria()
{
    PRODUTO produto;
    FILE * arquivo;
    
    arquivo = fopen(nome_arquivo,"r");

    if (tamanhoArquivo(nome_arquivo) == 0)
    {
        return 1;
    }

     while(!feof(arquivo))
     {
       buscaLinhaArquivo(arquivo, &produto);
     }

    fclose(arquivo);

    return produto.chave_primaria + 1;

}


/*Funções de arquivo*/
int qtdLinhasArquivo()
{
    arquivo = fopen(nome_arquivo,"r");
    int qtdLinha; 

     while(!feof(arquivo))
     {
         if (fgetc(arquivo) == '\n')
            qtdLinha ++;
     }

     return  qtdLinha + 1;
}

int tamanhoArquivo(const char* file_name)
{
    FILE *file = fopen(file_name, "r");

    if(file == NULL)
        return 0;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fclose(file);

    return size;

    fclose(file);
}

int main ()
{
    system("chcp 65001"); //atualizar padrão para UTF8
    setlocale(LC_ALL, "Portuguese_Brasil");

    mostraMenuPrincipal();

   return 0;
}

/*Funções para exibição*/

void insereTraco(int qtd,int quebraLinha)
{
    int i;

    for(i=0;i<qtd;i++)
    {
        printf("-");
    }

if (quebraLinha == 1)
    printf("\n");
}

void clear()
{
       #if defined(__linux__) || defined(__unix__) || defined(__Apple__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}
void exibeTitulo()
{
    insereTraco(38,1);
    printf("SISTEMA DE GERENCIAMENTO PARA ESTOQUE\n");
    insereTraco(38,1);
}