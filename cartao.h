#ifndef LMB_CARTAO_H
#define LMB_CARTAO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <PCSC/winscard.h>
#include <PCSC/reader.h>

typedef struct ctxCarta {
	SCARDCONTEXT contexto;
	LPTSTR mszReaders;
}TCtxCartao;

int full(char *codigo);

#endif // LMB_CARTAO_H
