#include "controle.h"
#include <wiringPi.h>

int verificar(char *codigo){
	
	int retorno;
	char *id;
	retorno = 1;
	id = (char *) malloc(sizeof(char)*255);
	
	if(autorizado(codigo, id)) {
		logAcesso(id);
		sleep(3);
	} else {
	wiringPiSetup();
	pinMode(1,OUTPUT);
	digitalWrite(1,HIGH);
	delay(1000);
	digitalWrite(1,LOW);
		}
	
}

void iniciar() {
	
	int status;
	char *codigo; 
	
	codigo = (char *) malloc(sizeof(char)*255);
	
	
	while(1){
		
		sleep(1);
		
		codigo[0] = '\0';
	
		
		status = full(codigo);
		
		switch(status){
			
			case 0:
				printf("%s\n", codigo);
				
			break;
			
			case 1:
				verificar(codigo);
				
			break;
			
		}
		
	}
	free(codigo);
	
}
