/*
 * ./Cadastro.c
 * 
 * Copyright 2017  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
 
#ifndef LMB_CARTAO_H
#define LMB_CARTAO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <PCSC/winscard.h>
#include <PCSC/reader.h>
#include <mysql/mysql.h>


typedef struct ctxCarta {
	SCARDCONTEXT contexto;
	LPTSTR mszReaders;
}TCtxCartao;


int full(char *codigo);


#define LMB_DAO_MAX_STR 255

typedef struct CnxData{
	char host[LMB_DAO_MAX_STR];
	char usuario[LMB_DAO_MAX_STR];
	char senha[LMB_DAO_MAX_STR];
	char banco[LMB_DAO_MAX_STR];
} TCnxData;

TCnxData getDAOConfig();

MYSQL_RES* queryDAO(char *query);
MYSQL_RES* queryDAONEW(char *query);

#endif // LMB_CADASTRO


int checkState(char *funcao, long rv, char *msg){
	
	int retorno;
	
	retorno = 1;
	
	if (SCARD_S_SUCCESS != rv) {
		retorno = 0;
		sprintf(msg, "%s: %s\n", funcao, pcsc_stringify_error(rv));
	}
	
	//printf("check: %i\n",retorno);
	return retorno;
}


int full(char *codigo) {
	
	int retorno;
	
	long rv;

	SCARDCONTEXT hContext;
	LPTSTR mszReaders;
	SCARDHANDLE hCard;
	DWORD dwReaders, dwActiveProtocol, dwRecvLength;

	SCARD_IO_REQUEST pioSendPci;
	BYTE pbRecvBuffer[1024];
	BYTE cmd[] = { 0xFF, 0xCA, 0x00, 0x00, 0x04 };

	unsigned int i;
 
	retorno = 1;
 
	 rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	if (checkState("SCardEstablishContext", rv, codigo)) {
		
		rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
		retorno = checkState("SCardListReaders", rv, codigo);

		mszReaders = calloc(dwReaders, sizeof(char));
		rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
		retorno = checkState("SCardListReaders", rv, codigo);
		

		rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
		if(!checkState("SCardConnect", rv, codigo)){
			retorno = 2;
		}


		switch (dwActiveProtocol) {
			case SCARD_PROTOCOL_T0:
				pioSendPci = *SCARD_PCI_T0;
			break;

			case SCARD_PROTOCOL_T1:
				pioSendPci = *SCARD_PCI_T1;
			break;
		}

		// CMD
		dwRecvLength = sizeof(pbRecvBuffer);
		rv = SCardTransmit(hCard, &pioSendPci, cmd, sizeof(cmd), NULL, pbRecvBuffer, &dwRecvLength);
		
		
		if (SCARD_S_SUCCESS != rv) {
			//printf("%s\n", pcsc_stringify_error(rv)); //Invalid handle.
		}

		char *aux = (char *) malloc(sizeof(char)*255);
		aux[0] = '\0';
		for(i=0; i<dwRecvLength; i++) {
			sprintf(aux, "%02X", pbRecvBuffer[i]);
			strcat((char *) codigo, (char *) aux);
	
		}
		free(aux);
		
		// Disconect
		rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
		if (SCARD_S_SUCCESS != rv) {
			//printf("%s\n", pcsc_stringify_error(rv)); //Invalid handle.
		}
			
		#ifdef SCARD_AUTOALLOCATE
			rv = SCardFreeMemory(hContext, mszReaders);
			if (SCARD_S_SUCCESS != rv) {
				printf("%s\n", pcsc_stringify_error(rv));
			}
		#else
			free(mszReaders);
		#endif
		rv = SCardReleaseContext(hContext);

		if (SCARD_S_SUCCESS != rv) {
			printf("%s\n", pcsc_stringify_error(rv));
		}
		
	}
	
	return retorno;
}

TCnxData getDAOConfig(){
	
	TCnxData retorno;
	
	sprintf(retorno.host, "localhost");
	sprintf(retorno.usuario, "root");
	sprintf(retorno.senha, "1234");
	sprintf(retorno.banco, "cadb");
	
	return retorno;
}

MYSQL_RES* queryDAO(char *query){
	
	TCnxData cnxData;
	MYSQL *conexao;
	MYSQL_RES *resultado;
	MYSQL_ROW linha;
	//FILE *arq;
	//char conteudo;
	
	cnxData = getDAOConfig();
	conexao = mysql_init(NULL);
	resultado = NULL;
	
	if (mysql_real_connect(conexao, cnxData.host, cnxData.usuario, cnxData.senha, cnxData.banco, 0, NULL, 0)) {
		
		if(!mysql_query(conexao, query)) {
			resultado = mysql_store_result(conexao);
			//arq = fopen("arquivo_texto.txt","a");
			while (linha = mysql_fetch_row(resultado)) {
							printf(" %s;", linha[1]);
							printf("%s;", linha[0]);
							printf("%s;", linha[2]);
							printf("%s\n", linha[3]);
							//conteudo = scanf("%s; %s; %s; %s\n",linha[1],linha[0],linha[2],linha[3]);
							//fwrite(arq,conteudo);
														
					}
		}
		//fclose(arq);
		mysql_close(conexao);
		conexao = mysql_init(NULL);
		
	} else {
		printf("Falha!\n");
		printf("Erro %d %s\n", mysql_errno(conexao),mysql_error(conexao));
	}
	
	return resultado;
}

MYSQL_RES* queryDAONEW(char *query){
	
	TCnxData cnxData;
	MYSQL *conexao;
	MYSQL_RES *resultado;
	MYSQL_ROW linha;

	
	cnxData = getDAOConfig();
	conexao = mysql_init(NULL);
	resultado = NULL;
	
	if (mysql_real_connect(conexao, cnxData.host, cnxData.usuario, cnxData.senha, cnxData.banco, 0, NULL, 0)) {
		
		if(!mysql_query(conexao, query)) {
			resultado = mysql_store_result(conexao);
			while (linha = mysql_fetch_row(resultado)) {
							printf("\nNome: %s\n", linha[1]);
							printf("ID: %s\n", linha[0]);
							printf("Matricula: %s\n", linha[2]);
							printf("--------------------------------\n");
							
					}
		}
		mysql_close(conexao);
		conexao = mysql_init(NULL);
		
	} else {
		printf("Falha!\n");
		printf("Erro %d %s\n", mysql_errno(conexao),mysql_error(conexao));
	}
	
	return resultado;
}

int main(int argc, char **argv)
{
	int mat=0;
	int op=0;
	char *codigo;
	char nome[80]; 
	char query[LMB_DAO_MAX_STR];
		
	codigo = (char *) malloc(sizeof(char)*255);
	
	printf("CADASTRAMENTO DE CARTÃO\n");
	printf("1 - Cadastrar\n");
	printf("2 - Buscar\n");
	printf("3 - Excluir\n");
	printf("4 - Listar\n");
	printf("5 - Gerar Relatorio\n");
	printf("0 - Sair\n");
	scanf("%i", &op);
	
	while(op != 0) {	
		
	switch (op)
  {
     case 1 :
		printf("\n**Coloque o cartão sob o leitor**\n\n");
		printf("Nome: ");
		scanf("%s", nome);
		printf("Matricula: ");
		scanf("%i\n", &mat);
		full(codigo);
		printf(" %s - %i - %s ", nome, mat, codigo);
		sprintf(query, "INSERT INTO cadastro(id_cartao, nome, matricula) values ( '%s', '%s', '%i');\n", codigo, nome, mat);
		queryDAONEW(query);
		printf("\nCadastro Realizado com Sucesso!\n\n");
		op=6;
	
     break;
 
     case 2 :
		printf ("Buscar Usuario\n");
		printf("Matricula: ");
		scanf("%i\n", &mat);
		sprintf(query, "SELECT * FROM `cadastro` WHERE matricula = %i;\n",mat);
		queryDAONEW(query);
		op=6;
			
     break;
		
     case 3 :
        printf ("Excluir Usuario\n");
		printf("Nome: ");
		scanf("%s\n", nome);
		sprintf(query, "DELETE FROM `cadastro` WHERE Nome = '%s';\n",nome);
		queryDAONEW(query);
		printf("\nCadastro Excluido com Sucesso!\n\n");
		op=6;
		
     break;
 
     case 4 :
        printf ("Listar Usuarios\n");
		sprintf(query, "SELECT * FROM `cadastro`;\n");
		queryDAONEW(query);
		op = 6;
		
     break;
 
     case 5 :
        printf ("Gerar Relatorio\n");
        sprintf(query, "select 'Nome do Aluno','ID do Cartão','Matrícula', 'Data de Acesso' UNION ALL select B.nome,B.id_cartao,B.matricula,A.dh_acesso from log A INNER JOIN cadastro B ON A.HEX = B.ID_CARTAO\n");
		queryDAO(query);
		op = 6;
     break;
	
	 default :
        printf("\nCADASTRAMENTO DE CARTÃO\n");
		printf("1 - Cadastrar\n");
		printf("2 - Buscar\n");
		printf("3 - Excluir\n");
		printf("4 - Listar\n");
		printf("5 - Gerar Relatorio\n");
		printf("0 - Sair\n");
		scanf("%i", &op);
     break;
	
  }

 }
  return 0;

}

