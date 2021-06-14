#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>
#define _GNU_SOURCE	// getline()
#define SIZE 200000

/*-------------------------------------------------------------------------------------------------*/
// STRUCTS SEPARADAS

typedef struct books
{
	int code;
	char title[46];
	char author[31];
    int edition;
	int year;
}
Books;

typedef struct title
{
	char title[46];
	int year[30];
}
Title;

typedef struct author
{
	char name[31];
	int qtd;

}
Author;

typedef struct edition
{
	int year;
	int qtd;
}
Edition;

/*-------------------------------------------------------------------------------------------------*/
// data read and array data alocation
long readInput(struct books bookList[])
{
	FILE *file = fopen("teste10k.txt", "r");
	if (file == NULL)
	{
		printf("Erro ao abrir o arquivo\n");
		exit(1);
	}
	long idx = 0;	// index
	size_t len = 40;
	char *linha = malloc(len);
	char *sub;

	while (getline(&linha, &len, file) > 0)
	{

		sub = strtok(linha, ";");
		bookList[idx].code = atoi(sub);

		sub = strtok(NULL, ";");
		strcpy(bookList[idx].title, sub);

		sub = strtok(NULL, ";");
		strcpy(bookList[idx].author, sub);

		sub = strtok(NULL, ";");
		bookList[idx].edition = atoi(sub);

		sub = strtok(NULL, ";");
		bookList[idx].year = atoi(sub);

		idx++;
	}

	fclose(file);

	return idx;
}

/*-------------------------------------------------------------------------------------------------*/
void authorFileGenerator(Books bookList[])
{
// author array alocation and file output
	Author *author = (Author*) malloc(SIZE* sizeof(Author));	//create a author array

	if (author == NULL)
	{
		printf("Erro ao alocar vetor de autores\n");
		exit(1);
	}
	for (int i = 0; i < SIZE; i++)
	{
		if (i==0)
		{
			strcpy(author[i].name, bookList[i].author);
			author[i].qtd = 1;
		}
		else
		{
			for (int j = 0; j < SIZE; j++)
			{
				if (strcmp(bookList[i].author, author[j].name) == 0)
				{
					author[j].qtd++;
					break;
				}
				else
				{
					if (strlen(author[j].name) == 0)
					{
						strcpy(author[j].name, bookList[i].author);
						author[j].qtd = 1;
						break;
					}
				}
			}
		}
	}

    FILE *file = fopen("author.txt", "w");

    if(file == NULL){
        printf("Erro ao gerar arquico dos autores\n");
        exit(1);
    }
    else
    {
        for(int i = 0; i<SIZE ; i++){
            if(strlen(author[i].name) == 0){
                exit(1);
            }
            fprintf(file,"%s %i\n",author[i].name,author[i].qtd);
        }
    }
    fclose(file);

}

/*-------------------------------------------------------------------------------------------------*/
void editionFileGenerator(Books bookList[]){

// editions file output and resume

	int qtdYear =0 ,bigger =0, qtd=0 ,aux = 0; //var to resume

	Edition *edition = (Edition*) malloc(SIZE *sizeof(Edition)); //alloc a edition array
	
    if(edition == NULL){
		exit(1);
	}
	printf("vetor de resume alocado \n");
	for (int i = 0 ; i<SIZE; i++){
    	if(i==0){
			edition[i].year = bookList[i].year;
        	edition[i].qtd = 1;
    	}
    	else{
        	for(int j = 0 ;j<SIZE; j++){
				if(bookList[i].year == edition[j].year){
                	edition[j].qtd++;
                    break;
            	}
				if(!edition[j].year){
					edition[j].year = bookList[i].year;
                    edition[j].qtd = 1;
                    break;
				}
        	}
    	}
	}
	FILE *file = fopen("edition.txt", "w");
	if(file == NULL){
		printf("Erro ao gerar arquivo edicoes\n");
		return;
	}
	else{
		for(int aux = 0 ; aux < SIZE; aux++){
			if(!edition[aux].year){
				break;
				}
			fprintf(file,"%i %i\n",edition[aux].year,edition[aux].qtd);
			if(edition[aux].qtd > qtd){
                bigger = edition[aux].year;
                qtd = edition[aux].qtd;
            }
			qtdYear = aux;
		}
		fclose(file);
	}
	printf("\n----------------------------------- R E S U M O -----------------------------------\n\n");
	printf("Ao total os livros registrados foram publicados em %i anos diferentes.\n",qtdYear+1);
	printf("O ano que possui mais publicações foi %i com um total de %i livros publicados.\n",bigger,qtd);
	printf("\n-----------------------------------------------------------------------------------\n");
	return;

}

/*-------------------------------------------------------------------------------------------------*/
void titleFileGenerator(Books bookList[], long input){

	Title *titles =(Title*) malloc(input * sizeof(Title));//alloc a edition array

    if(titles == NULL){
		printf("\nErro na alocacao\n");
		getchar();
	}
	printf("alocou title\n");
	for (long i=0;i<input;i++){
   	 	if(i==0){
    	    strcpy(titles[i].title,bookList[i].title); //Se for a primeira posicao, insere nela
       		titles[i].year[i]=bookList[i].year;
			printf("inseriu na primeira\n");
   		}
    	else{
        	for(long j=0;j<input;j++){
            	if(strcmp(bookList[i].title,titles[j].title)==0){ //Se o livro ja estiver na lista, verifica um posicao na lista de anos
                	for (int k=0;k<30;k++){
                    	if(bookList[i].year == titles[j].year[k]){
							printf("[%i].year eh igual titles[%i].year[%i]\n",i,j,k);
							i++;
							break;
							//printf("insereriu %s no ano %i\n",titles[j].title,titles[j].year);
							}
                    	if(titles[j].year[k] == 0){ //Se a posicao for nula, é feito a insercao
                        	titles[j].year[k]=bookList[i].year;
                        	break;
                    	}
                    	if(k==29){
                        	printf("Tem mais de 30 edicoes e 30 anos diferentes para livro %s",bookList[i].title);
                            break;
                    	}
                	}
            	}
            	else{
                	if(strlen(titles[j].title) == 0){
                    	strcpy(titles[j].title,bookList[i].title);
                    	titles[j].year[0]=bookList[i].year;
                    	break;
                	}
            	}
        	}
    	}
	}

	FILE *file = fopen("books.txt", "w");
	
	if(file == NULL){
		printf("Erro na abertura do arquivo");
		return;
	}
	else{
		for(long l=0;l<input;l++){
			if(strlen(titles[l].title) == 0)break;
            fprintf(file,"%s",titles[l].title);
			for(int n=0;n<30;n++){
				if(titles[l].year[n] == 0) break;
				else{
                    fprintf(file,";%d",titles[l].year[n]);
				}
                
			}
            fprintf(file,"\n");
		}
		fclose(file);
	}

    printf("Arquivo livro salvo!\n");
	
	long g;

	for(g=0;g<input;g++){
		if(strlen(titles[g].title) == 0){
			break;
		}
	}
	printf("Total de livros Publicados sem repeticao: %li\n",g);


}

/*-------------------------------------------------------------------------------------------------*/
int main()
{

	pid_t pid1, pid2, pid3;	// fork id
	int shmid;	// shared memory id

	Books * bookList; // pointer to books array

	if ((shmid = shmget(4, SIZE* sizeof(Books), IPC_CREAT | 0600)) < 0)
		printf(" Erro ao criar area memoria compartilhada\n");
	printf(" alocado com sucesso \n");

	if ((bookList = shmat(shmid, 0, 0)) < 0)
		printf(" Erro ao alocar memoria compartilhada\n");
	printf("compartilhada com sucesso\n");
	printf("leitor\n");	//leitura()

	long input = readInput(bookList);

	pid1 = fork();
	if (pid1 > 0)
	{
		pid2 = fork();
		if (pid2 > 0)
		{
			pid3 = fork();
			if (pid3 > 0)
			{
				wait(NULL);
				wait(NULL);
				wait(NULL);
				//printf("resumo\n");	//resumo();
			}
			else {
				printf("edicao\n");	//edicao() 
				editionFileGenerator(bookList);
				titleFileGenerator(bookList, input);
			}
		}
		else
		{
			printf("livro\n");	//livro()
			//titleFileGenerator(bookList);
		}
	}
	else
	{
		printf("autor\n");	//autor()
		authorFileGenerator(bookList);
		
	}
}