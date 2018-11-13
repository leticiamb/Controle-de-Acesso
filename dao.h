#ifndef LMB_DAO_H
#define LMB_DAO_H

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

#define LMB_DAO_MAX_STR 255

typedef struct CnxData{
	char host[LMB_DAO_MAX_STR];
	char usuario[LMB_DAO_MAX_STR];
	char senha[LMB_DAO_MAX_STR];
	char banco[LMB_DAO_MAX_STR];
} TCnxData;

TCnxData getDAOConfig();

MYSQL_RES* queryDAO(char *query);

int autorizado(char *codigo, char *id);
int logAcesso(char *codigo);

#endif //LMB_DAO_H
