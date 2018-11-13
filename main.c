#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "controle.h"


int main(void) {
	
	wiringPiSetup();
	pinMode(2,OUTPUT);
	
	digitalWrite(2,LOW);
	digitalWrite(2,HIGH);
	
	iniciar();
	
	return 0;
}

/*
 * gcc -Wall -o main main.c dao.c cartao.c controle.c -lmysqlclient -lpcsclite
 * 
 * gcc -Wall -I/usr/include/PCSC -o main main.c dao.c cartao.c controle.c -lmysqlclient -lpcsclite -lwiringPi
 * */
