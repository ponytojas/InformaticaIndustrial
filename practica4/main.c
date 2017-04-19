    /***************************************/
   /*   Practica 4 - Interrupciones       */
  /*   Hector Gonzalez Antón             */
 /*   Daniel Villalobos del Baño        */
/***************************************/

//#include "cmsis_os.h"
#include "lpc17xx.h"
#include "math.h"
#define TRUE 1
#define FALSE 0
#define NUM_MAX 65535

/*******************Variables**************************/

uint16_t numeroCalculo = 0;
uint16_t numeroFinal = 65535;
uint16_t primerValor;
uint16_t bit = 16;
uint16_t numeroMaximo = 65535;
uint16_t valorKey2;

uint32_t contador25;		//contador 25 necesita ser de 32 y no de 16 bits, porque podrían darse situaciones donde supere los 16 bits
uint32_t valorBCD;

uint8_t subidaBajada = FALSE; // variable 0 subida variable 1 bajada
uint8_t primo = FALSE;
uint8_t isp = FALSE;		//Variable que controla BCD o BIN
uint8_t auxLed2 = FALSE;	//Variable que controla si debe alternarse el led2
uint8_t finLeds = FALSE;	//Variable auxiliar que termina el proceso de los leds
uint8_t auxDisplay = FALSE;
uint8_t auxSeg;
uint8_t finDisplay = FALSE;
uint8_t tiempoDisplay = 0;
uint8_t apagarDisplay = FALSE;
/****************************************************/

/*******************Funciones*************************/

//configurar Pines Entrada/Salida
void configurarPuertos(){ 
	
	/*Seleccionamos todos los pines como GPIO*/
	LPC_PINCON->PINSEL0 = 0x00000000;
  LPC_PINCON->PINSEL1 = 0x00000000;
  LPC_PINCON->PINSEL2 = 0x00000000;
  LPC_PINCON->PINSEL3 = 0x00000000;
  LPC_PINCON->PINSEL4 = 0x00000000;
	
	/*Seleccionamos los modos de entrada y salida de datos*/
	LPC_GPIO0->FIODIR |= 0x000001FC;
	LPC_GPIO1->FIODIR |= 0x0003C713;
	LPC_GPIO2->FIODIR |= 0x000000FF;
	LPC_GPIO3->FIODIR |= 0xffffffff;
	LPC_GPIO4->FIODIR |= 0x30000000;
}

//configurar interrupciones
void configIRQ(){
	/*El LPC1768 nos permite configurar las interrupciones directamente como flancos de subida o bajada ('R'aising y 'F'alling) directamente sin utilizar el extpolar
		Ni ninguna otra función*/
	NVIC_EnableIRQ(EINT3_IRQn);
	LPC_GPIOINT->IO2IntEnF |= (1 << 10); //Falling, Flanco Bajada
	LPC_GPIOINT->IO2IntEnF |= (1 << 11); //Falling, Flanco Bajada
	LPC_GPIOINT->IO2IntEnR |= (1 << 12); //Raising, Flanco Subida
	LPC_GPIOINT->IO2IntEnF |= (1 << 12); //Falling, Flanco Bajada
	SysTick_Config(SystemCoreClock / 1000); //Esta forma de configurar el systick, nos evita utilizar el SysTick->LOAD y demás funciones
	//Configura automaticamente utilizando el reloj interno de la placa para que funcione cada 1mseg la llamada
}

//limpiarPines
void limpiarPuertos(){
	/*Función que pone a 0 todos los pines*/
  LPC_GPIO0->FIOCLR = 0xffffffff;
  LPC_GPIO1->FIOCLR = 0xffffffff;
  LPC_GPIO2->FIOCLR = 0xffffffff;
  LPC_GPIO3->FIOCLR = 0xffffffff;
  LPC_GPIO4->FIOCLR = 0xffffffff;
	LPC_GPIO3->FIOSET = 0x04000000;
	LPC_GPIO3->FIOSET = 0x02000000; 
	LPC_GPIO2 -> FIOSET = ~(0xFFFFFF00); //Apagamos el display
}

uint16_t leerDato(){
 /*Leemos los datos desde los pines conectados a los switches*/
	uint16_t valorInicial = 0;
	/*Ya que desde el switch obtenemos un valor 0 cuando lo activamos, debemos invertir el valor del pin para saber si está levantado o no*/
	/*Leemos de izquierda a derecha en los pines de mayor a menor peso*/
	/*[1.23][1.22][1.21][1.20][1.19][1.18][0.30][0.29][0.27][0.28][1.30][1.31][0.23][0.24][0.25][0.26]*/
	valorInicial = !(((LPC_GPIO1->FIOPIN>>23) & 0x01));	//15
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>22) & 0x01));	//14
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>21) & 0x01));	//13
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>19) & 0x01));	//12
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>20) & 0x01));	//11
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>18) & 0x01));	//10
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>30) & 0x01));	//9
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>29) & 0x01));	//8
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>27) & 0x01));	//7
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>28) & 0x01));	//6
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>30) & 0x01));	//5
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO1->FIOPIN>>31) & 0x01));	//4
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>23) & 0x01));	//3
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>24) & 0x01));	//2
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>25) & 0x01));	//1
	valorInicial = (valorInicial<<1) + !(((LPC_GPIO0->FIOPIN>>26) & 0x01));	//0
	/*Si no hemos seleccionado ningún número devolvemos el 1 para evitar problemas al dividir más adelante*/
	if (valorInicial == 0){
		valorInicial = 1;
	} else if(valorInicial != 2 && valorInicial%2==0){
			valorInicial++; //Optimizamos código evitando los números pares que sabemos que no son primos, salvo el 2
		}
	return valorInicial;
}

uint8_t esPrimo(uint16_t valor){
	int limite = 0;
	int i = 0;
	/*Añadimos las excepciones de 1 y 2 que son las que pueden causar problemas*/
	if(valor == 1 || valor == 2){
		return 1;
	}
	limite = sqrt(valor) + 1;
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
	/*Aunque el manual del LPC1768 aconseja utilizar FIOPIN solo como RO, en este caso al no ser pines continuos del mismo puerto*/
	/*Ej: P4.0 a P4.18, nos facilita y optimiza mejor el código utilizar FIOPIN como RW*/
		int estado = 1; //Iniciamos estado a un valor para evitar elementos basura
	/*Vamos pasando por todos los números HEX del valor y encendiendo mediante OR cada uno de los leds que corresponda*/
	estado = numero&0x01;
	LPC_GPIO0->FIOCLR |= 0x1FC;
  LPC_GPIO1->FIOCLR |= 0x3C713;
  LPC_GPIO4->FIOCLR3 |= 0x30;
	LPC_GPIO0->FIOPIN |= (estado<<3);
	estado = ((numero&0x02)>>1);
	LPC_GPIO0->FIOPIN |= (estado<<2);
	estado = ((numero&0x04)>>2);
	LPC_GPIO1->FIOPIN |= (estado<<0);
	estado = ((numero&0x08)>>3);
	LPC_GPIO1->FIOPIN |= (estado<<1);
	estado = ((numero&0x010)>>4);
	LPC_GPIO1->FIOPIN |= (estado<<4);
	estado = ((numero&0x020)>>5);
	LPC_GPIO1->FIOPIN |= (estado<<8);
	estado = ((numero&0x040)>>6);
	LPC_GPIO1->FIOPIN |= (estado<<9);
	estado = ((numero&0x080)>>7);
	LPC_GPIO1->FIOPIN |= (estado<<10);
	estado = ((numero&0x0100)>>8);
	LPC_GPIO1->FIOPIN |= (estado<<14);
	estado = ((numero&0x0200)>>9);
	LPC_GPIO1->FIOPIN |= (estado<<15);
	estado = ((numero&0x0400)>>10);
	LPC_GPIO1->FIOPIN |= (estado<<16);
	estado = ((numero&0x0800)>>11);
	LPC_GPIO4->FIOPIN |= (estado<<29);
	estado = ((numero&0x01000)>>12);
	LPC_GPIO4->FIOPIN |= (estado<<28);
	estado = ((numero&0x02000)>>13);
	LPC_GPIO0->FIOPIN |= (estado<<4);
	estado = ((numero&0x04000)>>14);
	LPC_GPIO0->FIOPIN |= (estado<<5);
	estado = ((numero&0x08000)>>15);
	LPC_GPIO0->FIOPIN |= (estado<<6);
	estado = ((numero&0x010000)>>16);
	LPC_GPIO0->FIOPIN |= (estado<<7);
	estado = ((numero&0x020000)>>17);
	LPC_GPIO0->FIOPIN |= (estado<<8);
}


void muestraBCD(uint32_t numero){
	/*Realizamos el encendido de los LEDs igual que hemos realizado en Binario una vez el número lo hemos convertido a BCD*/
	int estado = 0;
	LPC_GPIO0->FIOCLR |= 0x1FC;
  LPC_GPIO1->FIOCLR |= 0x3C713;
  LPC_GPIO4->FIOCLR3 |= 0x30;
	estado = ((numero&0x010000000)>>28);
	LPC_GPIO0->FIOPIN |= (estado<<3);
	estado = ((numero&0x020000000)>>29);
	LPC_GPIO0->FIOPIN |= (estado<<2);
	estado = ((numero&0x040000000)>>30);
	LPC_GPIO1->FIOPIN |= (estado<<0);
	estado = ((numero&0x080000000)>>31);
	LPC_GPIO1->FIOPIN |= (estado<<1);
	estado = ((numero&0x01000000)>>24);
	LPC_GPIO1->FIOPIN |= (estado<<4);
	estado = ((numero&0x02000000)>>25);
	LPC_GPIO1->FIOPIN |= (estado<<8);
	estado = ((numero&0x04000000)>>26);
	LPC_GPIO1->FIOPIN |= (estado<<9);
	estado = ((numero&0x08000000)>>27);
	LPC_GPIO1->FIOPIN |= (estado<<10);
	estado = ((numero&0x0100000)>>20);
	LPC_GPIO1->FIOPIN |= (estado<<14);
	estado = ((numero&0x0200000)>>21);
	LPC_GPIO1->FIOPIN |= (estado<<15);
	estado = ((numero&0x0400000)>>22);
	LPC_GPIO1->FIOPIN |= (estado<<16);
	estado = ((numero&0x0800000)>>23);
	LPC_GPIO4->FIOPIN |= (estado<<29);
	estado = ((numero&0x010000)>>16);
	LPC_GPIO4->FIOPIN |= (estado<<28);
	estado = ((numero&0x020000)>>17);
	LPC_GPIO0->FIOPIN |= (estado<<4);
	estado = ((numero&0x040000)>>18);
	LPC_GPIO0->FIOPIN |= (estado<<5);
	estado = ((numero&0x080000)>>19);
	LPC_GPIO0->FIOPIN |= (estado<<6);
	estado = ((numero&0x01000)>>12);
	LPC_GPIO0->FIOPIN |= (estado<<7);
	estado = ((numero&0x02000)>>13);
	LPC_GPIO0->FIOPIN |= (estado<<8);
}

uint32_t valorBinToBCD(uint16_t valorBin){
	/*Dividimos en secciones de 4 números y dividimos entre 10 para generar el número en BCD*/
	uint32_t resultado = 0;
  int i = 0;
  for(i=1;i<=8;i++){
    resultado<<=4;
    resultado|=(valorBin%10);
    valorBin/=10;
  }
  return resultado;
}

void led2(){
	/*Comprobamos si el pin tiene un 1 (led encendido) o 0 (led apagado) y realizamos la tarea inversa*/
	if((LPC_GPIO3->FIOPIN>>25 & 0x01)){
		 LPC_GPIO3->FIOCLR = 0x02000000; 
	}else{
		LPC_GPIO3->FIOSET = 0x02000000;  
	}
}

void led3(uint8_t bit, uint16_t numero){
	uint16_t aux = 0x01;
	uint16_t i;
	LPC_GPIO3->FIOSET = 0x04000000; //Apagamos el led siempre para que haya constancia de que variamos
	/*Con una variable de la forma 0x01 en 16 bit, desplazamos el 1 hasta el bit que toque comprobar*/
	for (i = 0; i <= bit-1; i++){
		aux = aux << 1;
	}
	/*Una vez hemos desplazado hasta el bit correspondiente, comprobamos si le multiplicación lógcia (AND &)
	con el número es diferente de 0, en cuyo caso debemos encender el led*/
	aux &= numero;
	if (aux != 0){
		LPC_GPIO3->FIOCLR = 0x04000000;
	}
}

void leds(){
	while(!finLeds){
	if (auxLed2){
		//se ejecuta Led2()
		led2();
		//reducimos el auxiliar de Bit
		bit--;
		//se ejecuta Led3()
		led3(bit,numeroCalculo);
		}
	auxLed2 = FALSE;
	finLeds=TRUE;
	}
	finLeds= FALSE;
}

void display (uint8_t digito){
	
	contador25 = 0; //Ponemos el contador del systick a 0 para controlar el tiempo que encendemos y apagamos cada bit para que sea visible el cambio entre dígito y dígito
	while (contador25<= 300);
	
	LPC_GPIO2 -> FIOSET = ~(0xFFFFFF00);
	contador25 = 0;
	
	while (contador25<= 250);
	
	switch (digito){ //Un switch tipo en función del valor que le pasemos
		/*Al ser de ánodo común es necesario mandar una señal de 0 por los pines que correspondan*/
		case 0:
			/* 0 // A + B + C + D + E + F */
		LPC_GPIO2->FIOCLR = 0x0000003F;
		break;
		
		case 1:
			/* 1 // B + C */
		LPC_GPIO2->FIOCLR = 0x00000006;
		break;
		
		case 2:
			/* 2 // A + B + D + E + G */
		LPC_GPIO2->FIOCLR = 0x0000005B;
		break;
		
		case 3:
			/* 3 // A + B + C + D + G */
		LPC_GPIO2->FIOCLR = 0x0000004F;
		break;
		
		case 4:
			/* 4 // B + C + F + G */
		LPC_GPIO2->FIOCLR = 0x00000066;
		break;
		
		case 5: 
			/* 5 // A + C + D + F + G */
		LPC_GPIO2->FIOCLR = 0x0000006D;
		break;
		
		case 6:
			/* 6 // A + C + D + E + F + G */
		LPC_GPIO2->FIOCLR = 0x0000007D;
		break;
		
		case 7:
			/* 7 // A + B + C */
		LPC_GPIO2->FIOCLR = 0x00000007;
		break;
		
		case 8: 
		/* 8 // A + B + C + D + E + F*/
		LPC_GPIO2->FIOCLR = 0x0000007F;
		break;
		
		case 9:
			/* 9 // A + B + C + D + F + G*/
		LPC_GPIO2->FIOCLR = 0x0000006F;
		break;
	}
	contador25 = FALSE;
}


void displayDigito(uint16_t numero, uint8_t digito){
	uint8_t aux;
	int i;
	/*Dividimos el número en dígitos hasta llegar al dígito que queremos mostrar*/
	for (i = 0; i <= digito-1; i++){
		aux = numero %10;
		numero = numero /10;
	} 
	display(aux);
	
}


void llamadaFunciones(){
	configurarPuertos();
	configIRQ();
	limpiarPuertos();
	primerValor = leerDato();
	numeroCalculo = primerValor;
	while(numeroCalculo != numeroFinal){
		//primo o no
		primo = esPrimo(numeroCalculo);
		if(primo){
		
			
			switch (isp){
				case 0:
					//BCD
				valorBCD = valorBinToBCD(numeroCalculo);
				muestraBCD(valorBCD);
				break;
				
				case 1: 
					//binario
				muestraBinario(numeroCalculo);
				break;
				
			}
			contador25=0;
			auxSeg = 6;
					
			while (bit!=0){
				leds();
			}
				contador25=FALSE;
				bit = 16;
		
		while(!finDisplay)
				{
					if (auxSeg == 1)
					{
						finDisplay = TRUE;
					}
					if(tiempoDisplay && auxSeg != 0){
						auxSeg--;
						displayDigito(numeroCalculo, auxSeg);
						tiempoDisplay = FALSE;
					}
				}
				finDisplay = FALSE;
				contador25 = FALSE;
		if (numeroCalculo == 1 || numeroCalculo == 2){
		numeroCalculo--;
		}
	}
		switch (subidaBajada){
			case 0:
				/*Para evitar problemas al llegar a los números máximos y minimos que aceptamos (65535 y 0)
				Forzamos el reinicio del programa en estos casos, evitando así que se bloquee o ciagamos en una excepción fatal*/
			if(numeroCalculo + 2 >= NUM_MAX){
				llamadaFunciones();
				break;
			}
				numeroCalculo += 2;
				break;
			
			case 1:
				if (numeroCalculo == 3){ 
				numeroCalculo--; //Ya que restamos de 2 en 2 para evitar los pares, nos encontramos que con el 3 nos saltamos el 2 que si es cierto que es par, es primo, así lo evitamos
					break;
				}
				if (numeroCalculo - 2 <= 0){
					llamadaFunciones();
					break;
				}
				numeroCalculo -= 2;
				break;
		}
		
	}
}
/****************************************************/



/*******************Excepciones***********************/

void SysTick_Handler(){
	/*Aunque utilizamos el systick para más operaciones, las principales son las que encienden los leds y el display, en el resto de operaciones nos apoyamos en el contador*/
	contador25++;
		
	if ((contador25-1)%125 == 0) {auxLed2 = TRUE;} 	//En el caso de que hayan pasado 125ms led2 se convierte en TRUE
	
	if ((contador25-1)%400 == 0) {tiempoDisplay = TRUE;}	//En el caso de que hayan pasado 400ms se ejecutarDisplay se convierte en TRUE

}


void EINT3_IRQHandler (void)
{
	if (LPC_GPIOINT->IO2IntStatF & (1 << 10)){
	//Flanco de bajada en el 2.10
  LPC_GPIOINT->IO2IntClr |= (1 << 10); //Limpiamos el estado de la interrupción
			if(!isp){isp = TRUE;} //Alternamos el estado ISP que muestra en BIN o BCD el numero primo
      else{isp = FALSE;}
  return;
  }
	
	if (LPC_GPIOINT->IO2IntStatF & (1 << 11)) {
  //Flanco de bajada en el 2.11
  LPC_GPIOINT->IO2IntClr |= (1 << 11); //Limpiamos el estado de la interrupción
		if(!subidaBajada){ //Cambiamos el orden ascendente o descendente de cálculo de primos
		subidaBajada = TRUE;
		numeroFinal = primerValor;
		}else{
		subidaBajada = FALSE;
			numeroFinal = numeroMaximo;
		}
  return;
  }
		if (LPC_GPIOINT->IO2IntStatF & (1 << 12)) {
			 //Flanco de bajada en el 2.12
  LPC_GPIOINT->IO2IntClr |= (1 << 12); //Limpiamos el estado de la interrupción
  valorKey2 = contador25;
	numeroCalculo = leerDato();
		
  return;
  }
	if (LPC_GPIOINT->IO2IntStatR & (1 << 12)) {
		 //Flanco de subida en el 2.11
  LPC_GPIOINT->IO2IntClr |= (1 << 12);//Limpiamos el estado de la interrupción
		numeroFinal= leerDato();
  return;
  }
}


/****************************************************/


int main(){

	while(1){
		llamadaFunciones();
	}
}

