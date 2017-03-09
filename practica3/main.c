#include "cmsis_os.h"
#include "lpc17xx.h"
#include "math.h"


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
	
	if(valor == 1 || valor == 2){
		return 1;
	}
	for (i = 2; i <= limite; i++){
		if (valor % i == 0){
			return 0;
		}else{
			return 1;
		}
	}
	return 0;
}

void muestraBinario(uint16_t valor){

		int estado = valor&0x01;
	

	
}



int main(){
	int i = 0;
	uint8_t primo = 0;
	uint16_t valor;
	
	configurarPuertos();
	limpiarPuertos();
	do{
		valor = leerDato();
		for ( i = valor; i <= 65535; i++){
			primo = esPrimo(valor);
			if (primo){
				muestraBinario(valor);
				
				//Check ISP
				//If ISP mostrarBin, sino mostrarBCD
				//mostrarBCD
			}
			
		}
	
	}while(1);
	
	
}

