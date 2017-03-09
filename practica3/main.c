#include "cmsis_os.h"
#include "lpc17xx.h"
#include "math.h"

void retrasoMsecond(uint16_t ms){
	/*Funcion de retardo simple. Introducido un valor realiza un bucle de los mseg necesarios*/
    unsigned int i,j;

    for(i=0;i<ms;i++)
        for(j=0;j<20000;j++);
}

void configurarPuertos(){ 
	
	/*Seleccionamos todos los pines como GPIO*/
	LPC_PINCON->PINSEL0 = 0x00000000;
  LPC_PINCON->PINSEL1 = 0x00000000;
  LPC_PINCON->PINSEL2 = 0x00000000;
  LPC_PINCON->PINSEL3 = 0x00000000;
  LPC_PINCON->PINSEL4 = 0x00000000;
	
	/*Seleccionamos los modos de entrada y salida de datos*/
	LPC_GPIO0->FIODIR |= 0xFFFFFE03;
  LPC_GPIO1->FIODIR |= 0xFFFC38EC;
  LPC_GPIO2->FIODIR |= 0xFFFFFF00;
  //LPC_GPIO3->FIODIR |= 0xfbffffff;
  LPC_GPIO4->FIODIR |= 0xCFFFFFFF;
	
	/*Ponemos a 1 todos los pines que vayamos a utilizar, de esta manera nos aseguramos de que no habrá ningún problema*/
	/*TODO: ¿Esto funciona así bien o no es necesario?	
	LPC_GPIO0->FIOSET = 0xFFFFFE03;
  LPC_GPIO1->FIOSET = 0xFFFC38EC;
  LPC_GPIO2->FIOSET = 0xFFFFFF00;
  LPC_GPIO4->FIOSET = 0xCFFFFFFF;
	*/
}
	

void limpiarPuertos(){
	/*Función que pone a 0 todos los pines*/
  LPC_GPIO0->FIOCLR = 0xffffffff;
  LPC_GPIO1->FIOCLR = 0xffffffff;
  LPC_GPIO2->FIOCLR = 0xffffffff;
  LPC_GPIO3->FIOCLR = 0xffffffff;
  LPC_GPIO4->FIOCLR = 0xffffffff;
}


uint16_t leerDato(){
 /*Leemos los datos desde los pines conectados a los switches*/
	uint16_t valorInicial = 0;
	
	/*Ya que desde el switch obtenemos un valor 0 cuando lo activamos, debemos invertir el valor del pin
		Para saber si está levantado o no*/
	
	
	/*Leemos de izquierda a derecha en los pines de mayor a menor peso*/
	/*[1.23][1.22][1.21][1.20][1.19][1.18][0.30][0.29][0.27][0.28][1.30][1.31][0.23][0.24][0.25][0.26]*/
	valorInicial = !(((LPC_GPIO1->FIOPIN>>23) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>22) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>21) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>20) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>19) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>18) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>30) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>29) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>27) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>28) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>30) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>31) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>23) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>24) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>25) & 0x01));
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>26) & 0x01));
	/*Si no hemos seleccionado ningún número devolvemos el 1 para evitar problemas al dividir más adelante*/
	if (valorInicial == 0)
	{
		valorInicial = 1;
	}
	
	return valorInicial;
}

uint8_t esPrimo(uint16_t valor){
	int limite = 0;
	int i = 0;
	limite = sqrt(valor) + 1;
	/*Añadimos las excepciones de 1 y 2 que son las que pueden causar problemas*/
	if(valor == 1 || valor == 2){
		return 1;
	}
	
	/*Como no almacenamos los números debemos debemos dividir entre todos los anteriores*/
	/*Para optimizar código dividimos solo entre los impares, porque en el main hemos evitado todos los números pares*/
	for (i = 3; i <= limite; i+=2){
		if (valor % i == 0){
			return 0;
		}
	}
	return 1;
}

void muestraBinario(uint16_t numero){

		int estado;
	
	
estado = numero&0x01;
LPC_GPIO0->FIOPIN |= (estado<<0);
estado = ((numero&0x02)>>1);
LPC_GPIO1->FIOPIN |= (estado<<1);
estado = ((numero&0x04)>>2);
LPC_GPIO1->FIOPIN |= (estado<<4);
estado = ((numero&0x08)>>3);
LPC_GPIO1->FIOPIN |= (estado<<8);
estado = ((numero&0x010)>>4);
LPC_GPIO1->FIOPIN |= (estado<<9);
estado = ((numero&0x020)>>5);
LPC_GPIO1->FIOPIN |= (estado<<10);
estado = ((numero&0x040)>>6);
LPC_GPIO1->FIOPIN |= (estado<<14);
estado = ((numero&0x080)>>7);
LPC_GPIO0->FIOPIN |= (estado<<2);
estado = ((numero&0x0100)>>8);
LPC_GPIO1->FIOPIN |= (estado<<16);
estado = ((numero&0x0200)>>9);
LPC_GPIO1->FIOPIN |= (estado<<17);
estado = ((numero&0x0400)>>10);
LPC_GPIO4->FIOPIN |= (estado<<29);
estado = ((numero&0x0800)>>11);
LPC_GPIO4->FIOPIN |= (estado<<28);
estado = ((numero&0x01000)>>12);
LPC_GPIO2->FIOPIN |= (estado<<6);
estado = ((numero&0x02000)>>13);
LPC_GPIO2->FIOPIN |= (estado<<7);
estado = ((numero&0x04000)>>14);
LPC_GPIO2->FIOPIN |= (estado<<8);
estado = ((numero&0x08000)>>15);
LPC_GPIO0->FIOPIN |= (estado<<19);

	
}



int main(){
	int i = 0;
	uint8_t primo = 0;
	uint16_t valor;
	
	configurarPuertos();
	limpiarPuertos();
	do{
		valor = leerDato();
		if(valor != 2 && valor%2==0)
		{
			valor++; //Optimizamos código evitando los números pares que sabemos que no son primos, salvo el 2
		}
		for ( i = valor; i <= 65535; i+=2){
			primo = esPrimo(i);
			if (primo){
				muestraBinario(valor);
				
				//Check ISP
				//If ISP mostrarBin, sino mostrarBCD
				//mostrarBCD
			}
			
		}
	
	}while(1);
	
	
}

