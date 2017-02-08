#include <lpc17xx.h>
#include <math.h>					
#define NUM_MAX 65535				

int primo[65535];						
int limite = 0;							
uint16_t numeroEvaluado = 3;			
int pos = 0;	


uint8_t esprimo(uint16_t numero){ 
	int i; 
limite = sqrt(numeroEvaluado)+1;		
			for(i=1; i<=pos; i++){									
				if (primo[i] > limite)								
				return 1;
				if(numeroEvaluado%primo[i] == 0){			
					return 0;										
				}				
		}
			return 1;
}

int main (){

	uint8_t ifprimo=1;
	primo[0]=2;
	while (numeroEvaluado < NUM_MAX)
	{
		ifprimo = esprimo(numeroEvaluado);
		if(ifprimo == 1){
			pos++;														
				primo[pos] = numeroEvaluado;	
		}
		ifprimo=1;
		numeroEvaluado +=2;
	
	}
}
