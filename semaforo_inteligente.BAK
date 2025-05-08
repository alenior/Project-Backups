/*#include <16F874.h>   //Biblioteca do PIC utilizado

#use delay(clock=1000000)
#fuses XT, NOWDT, NOPUT

void main() {

  while(TRUE){
     output_d(0b00100001);
     delay_ms(15000);
     output_d(0b00010001);
     delay_ms(2000);
     output_d(0b00001100);
     delay_ms(15000);
     output_d(0b00001010);
     delay_ms(2000);
     }
}*/
#include <16F874.h>
#use delay(clock=4000000) // 4 MHz
#fuses XT,NOWDT,NOPUT,NOLVP
#use standard_io(B)
#use standard_io(D)

int fluxoA = 0, fluxoB = 0;
int tempoVerdeA = 30, tempoVerdeB = 30;

void liga_semaforo_A(int tempo) {
    output_high(PIN_B0);  // Verde A
    output_low(PIN_B1);   // Amarelo A
    output_high(PIN_B5);  // Vermelho B

    fluxoA = 0;
    for(int i = 0; i < 15; i++) {
        if(input(PIN_D0)) fluxoA++;
        delay_ms(1000);
    }

    // Verifica fluxo anterior da B para ajustar o tempo de A
    if(fluxoA <= 1.3 * fluxoB) tempoVerdeA = 30;
    else if(fluxoA <= 1.6 * fluxoB) tempoVerdeA = 35;
    else tempoVerdeA = 40;

    fluxoB = 0; // Zera fluxo anterior da via B

    // Continua verde o tempo restante
    for(int i = 15; i < tempoVerdeA; i++) {
        if(input(PIN_D0)) fluxoA++;
        delay_ms(1000);
    }

    output_low(PIN_B0);  // Apaga verde
    output_high(PIN_B1); // Amarelo A
    delay_ms(3000);
    output_low(PIN_B1);
}

void liga_semaforo_B(int tempo) {
    output_high(PIN_B3);  // Verde B
    output_low(PIN_B4);   // Amarelo B
    output_high(PIN_B2);  // Vermelho A

    fluxoB = 0;
    for(int i = 0; i < 15; i++) {
        if(input(PIN_D1)) fluxoB++;
        delay_ms(1000);
    }

    if(fluxoB <= 1.3 * fluxoA) tempoVerdeB = 30;
    else if(fluxoB <= 1.6 * fluxoA) tempoVerdeB = 35;
    else tempoVerdeB = 40;

    fluxoA = 0;

    for(int i = 15; i < tempoVerdeB; i++) {
        if(input(PIN_D1)) fluxoB++;
        delay_ms(1000);
    }

    output_low(PIN_B3);  // Apaga verde
    output_high(PIN_B4); // Amarelo B
    delay_ms(3000);
    output_low(PIN_B4);
}

void main() {
    while(TRUE) {
        // Semáforo A
        output_low(PIN_B2); // Apaga vermelho A
        liga_semaforo_A(tempoVerdeA);
        output_high(PIN_B2); // Liga vermelho A

        // Semáforo B
        output_low(PIN_B5); // Apaga vermelho B
        liga_semaforo_B(tempoVerdeB);
        output_high(PIN_B5); // Liga vermelho B
    }
}
