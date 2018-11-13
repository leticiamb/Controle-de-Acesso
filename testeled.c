/*
 * testeled.c
 * 
 * Copyright 2018  <pi@raspberrypi>
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
 * gcc -Wall -o testeled testeled.c -lwiringPi
 * ./testeled
 * 
 */


#include <stdio.h>
#include <wiringPi.h>

int main()
{
	wiringPiSetup();
	pinMode(1,OUTPUT); //vermelho 1 verde 0
	pinMode(0,OUTPUT);
	pinMode(2,OUTPUT);
	digitalWrite(2,HIGH);

	
	digitalWrite(1,HIGH);
	delay(1000);
	digitalWrite(1,LOW);
	delay(1000);
	
	digitalWrite(0,HIGH);
	delay(1000);
	digitalWrite(0,LOW);
	delay(1000);
	
	digitalWrite(2,LOW);
	delay(1000);
	digitalWrite(2,HIGH);
	delay(1000);

	return 0;
}

