#include "cartao.h"


int checkState(char *funcao, long rv, char *msg){
	
	int retorno;
	
	retorno = 1;
	
	if (SCARD_S_SUCCESS != rv) {
		retorno = 0;
		sprintf(msg, "%s: %s\n", funcao, pcsc_stringify_error(rv));
	}
	
	printf("check: %i\n",retorno);
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
			printf("%s\n", pcsc_stringify_error(rv)); //Invalid handle.
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
			printf("%s\n", pcsc_stringify_error(rv)); //Invalid handle.
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



