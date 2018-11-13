#include "dao.h"
#include <wiringPi.h>

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
	
	cnxData = getDAOConfig();
	conexao = mysql_init(NULL);
	resultado = NULL;
	
	if (mysql_real_connect(conexao, cnxData.host, cnxData.usuario, cnxData.senha, cnxData.banco, 0, NULL, 0)) {
		
		if(!mysql_query(conexao, query)) {
			resultado = mysql_store_result(conexao);
		}
			
		mysql_close(conexao);
		conexao = mysql_init(NULL);
		
	} else {
		printf("Falha!\n");
		printf("Erro %d %s\n", mysql_errno(conexao),mysql_error(conexao));
	}
	
	return resultado;
}

int autorizado(char *codigo, char *id) {
	
	int retorno;
	char query[LMB_DAO_MAX_STR];
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	retorno = 0;
	
	sprintf(query, "SELECT * FROM cadastro WHERE id_cartao = '%s';\n", codigo);
	resultado = queryDAO(query);
	
	if (resultado != NULL && mysql_num_rows(resultado)) {
		row = mysql_fetch_row(resultado);
		sprintf(id, "%s", row[0]);
		retorno = 1;
	}
	
	return retorno;
	
}
	
int logAcesso(char *id){
	
	int retorno;
	
	
	char query[LMB_DAO_MAX_STR];
	
	retorno = 0;
	sprintf(query, "INSERT INTO log( hex, dh_acesso) values ('%s', now());\n", id); 
	queryDAO(query);
	
	wiringPiSetup();
	pinMode(0,OUTPUT); // led verde
	pinMode(2,OUTPUT); //trava
	
	
	digitalWrite(0,HIGH);
	digitalWrite(2,LOW);
	delay(1000);
	digitalWrite(0,LOW);
	digitalWrite(2,HIGH);
	delay(1000);
	
	return retorno;
}
