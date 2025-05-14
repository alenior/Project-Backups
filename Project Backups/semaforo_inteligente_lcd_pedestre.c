#include <16F874.h>   // PIC utilizado
#define BUZZER PIN_C4
#define VERDE_PEDESTRE PIN_D3

#fuses XT, NOWDT, NOPUT
#use delay(clock=1000000)

#include <LCD.c>

int fluxo_a = 0;
int fluxo_b = 0;
int verdeA = 0;
int verdeB = 0;
int contador_verde_a = 0;
int contador_verde_b = 0;
int tempo_fluxo_a_ativo = 0;
int tempo_fluxo_b_ativo = 0;
int pedestre_chama = 0;

// Atualiza o LCD com tempo e fluxo
void atualiza_lcd_temporizado() {

   if (pedestre_chama == 0) {

      lcd_gotoxy(1, 1);

      if (verdeA)
         printf(lcd_putc, "%02u|A:%2u  ", contador_verde_a, fluxo_a);
      else
         printf(lcd_putc, "--|A:%2u  ", fluxo_a);

      if (verdeB)
         printf(lcd_putc, "%02u|B:%2u", contador_verde_b, fluxo_b);
      else
         printf(lcd_putc, "--|B:%2u", fluxo_b);
   }
   else {
      lcd_gotoxy(1, 1);

      if (verdeA)
         printf(lcd_putc, "%02u|A:%2u**", contador_verde_a, fluxo_a);
      else
         printf(lcd_putc, "--|A:%2u**", fluxo_a);

      if (verdeB)
         printf(lcd_putc, "%02u|B:%2u", contador_verde_b, fluxo_b);
      else
         printf(lcd_putc, "--|B:%2u", fluxo_b);
   }
}

// Interrupção para sensores RB4 e RB5
#INT_RB
void trata_rb() {
   if (input(PIN_B4) == 0 && verdeA && tempo_fluxo_a_ativo) {
      fluxo_a++;
      atualiza_lcd_temporizado();
   }
   if (input(PIN_B5) == 0 && verdeB && tempo_fluxo_b_ativo) {
      fluxo_b++;
      atualiza_lcd_temporizado();
   }
   if (input(PIN_B6) == 0)
      if (pedestre_chama == 0) {
         pedestre_chama = 1;
         output_low(BUZZER);   // Ativa o buzzer puxando para GND
         delay_ms(100);
         output_high(BUZZER);  // Desativa o buzzer
         delay_ms(250);
         output_low(BUZZER);   // Ativa o buzzer puxando para GND
         delay_ms(100);
         output_high(BUZZER);  // Desativa o buzzer
      }
      else {
         pedestre_chama = 0;
         output_low(BUZZER);   // Ativa o buzzer puxando para GND
         delay_ms(100);
         output_high(BUZZER);  // Desativa o buzzer
         delay_ms(250);
         output_low(BUZZER);   // Ativa o buzzer puxando para GND
         delay_ms(100);
         output_high(BUZZER);  // Desativa o buzzer
      }
}

// Compara os fluxos e retorna o tempo total em segundos (30, 35 ou 40)
int compara(int fluxoA, int fluxoB) {
   int relacao;
   if (fluxoB == 0)
      fluxoB = 1;
   relacao = (fluxoA * 10) / fluxoB;
   if (relacao <= 13) return 30;   // 15 + 15
   else if (relacao <= 16) return 35; // 15 + 20
   else return 40; // 15 + 25
}

void pedestre_passando() {
   const char animacoes[6][6] = {
      " >---",
      " ->--",
      " -->-",
      " --->",
      " !!!!",
      " XXXX"
   };

   int tempo_pedestre;
   output_c(0b01010001); // Vermelho A (RC0), Vermelho B (RC6), Vermelho Pedestre (RC3)
   output_high(VERDE_PEDESTRE);

   lcd_putc('\f');

   for (tempo_pedestre = 10; tempo_pedestre >= 0; tempo_pedestre--) {
      lcd_gotoxy(1, 1);
      printf(lcd_putc, "FECHA EM %2u ----", tempo_pedestre);
      lcd_gotoxy(12, 1);

      // Animação conforme tempo
      if (tempo_pedestre >= 7 && tempo_pedestre <= 10) {
         lcd_putc(animacoes[(10 - tempo_pedestre) % 4]);  // >--- até --->
         output_low(BUZZER); delay_ms(500);
         output_high(BUZZER); delay_ms(500);
      }

      else if (tempo_pedestre >= 3 && tempo_pedestre <= 6) {
         lcd_putc(animacoes[(10 - tempo_pedestre) % 4]);  // >--- até ---> novamente
         output_low(BUZZER); delay_ms(500);
         output_high(BUZZER); delay_ms(500);
      }

      else if (tempo_pedestre == 2 || tempo_pedestre == 1) {
         lcd_putc(animacoes[4]);  // " !!!!"
         output_low(BUZZER); delay_ms(100);
         output_high(BUZZER); delay_ms(200);
         output_low(BUZZER); delay_ms(300);
         output_high(BUZZER); delay_ms(400);
      }

      else {
         lcd_putc(animacoes[5]);  // " XXXX"
         output_low(BUZZER); delay_ms(1000);
         output_high(BUZZER);
         break;
      }
   }

   output_c(0b01011001); // Vermelho A (RC0), Vermelho B (RC6), Vermelho Pedestre (RC3)
   output_low(VERDE_PEDESTRE);
}

void main() {
   int i, tempo_total;
   output_low(VERDE_PEDESTRE);

   lcd_init();
   lcd_putc("\fInicializando...");
   delay_ms(500);
   lcd_putc('\f');
   atualiza_lcd_temporizado();

   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);

   while (TRUE) {

      pedestre_chama = 0;

      // -------- SEMÁFORO A: Verde A + Vermelho B
      output_c(0b01011100); // Verde A (RC2), Vermelho B (RC6), Vermelho Pedestre (RC3)
      verdeA = 1;
      verdeB = 0;
      contador_verde_a = 0;

      output_low(BUZZER);   // Ativa o buzzer puxando para GND
      delay_ms(100);
      output_high(BUZZER);  // Desativa o buzzer

      tempo_fluxo_a_ativo = 1;  // ativa contagem de fluxo A

      // Espera 15s com fluxo ativo
      for (i = 0; i < 15; i++) {
         delay_ms(1000);
         contador_verde_a++;
         atualiza_lcd_temporizado();
      }

      tempo_fluxo_a_ativo = 0;  // desativa contagem de fluxo A

      // Compara e define tempo total
      tempo_total = compara(fluxo_a, fluxo_b);

      // Continua no verde para completar o tempo total
      for (i = 15; i < tempo_total; i++) {
         delay_ms(1000);
         contador_verde_a++;
         atualiza_lcd_temporizado();
      }

      verdeA = 0;

      // Amarelo A + Vermelho B
      output_c(0b01011010); // Amarelo A (RC1), Vermelho B (RC6), Vermelho Pedestre (RC3)
      delay_ms(3000);

      fluxo_b = 0;

      if (pedestre_chama == 1)
         pedestre_passando();

      pedestre_chama = 0;

      // -------- SEMÁFORO B: Verde B + Vermelho A
      output_c(0b00111001); // Verde B (RC5), Vermelho A (RC0), Vermelho Pedestre (RC3)
      verdeB = 1;
      contador_verde_b = 0;

      output_low(BUZZER);   // Ativa o buzzer puxando para GND
      delay_ms(100);
      output_high(BUZZER);  // Desativa o buzzer

      tempo_fluxo_b_ativo = 1;  // ativa contagem de fluxo B

      for (i = 0; i < 15; i++) {
         delay_ms(1000);
         contador_verde_b++;
         atualiza_lcd_temporizado();
      }

      tempo_fluxo_b_ativo = 0;  // desativa contagem de fluxo B

      tempo_total = compara(fluxo_b, fluxo_a);

      for (i = 15; i < tempo_total; i++) {
         delay_ms(1000);
         contador_verde_b++;
         atualiza_lcd_temporizado();
      }

      verdeB = 0;

      // Amarelo B + Vermelho A
      output_c(0b10011001); // Amarelo B (RC7), Vermelho A (RC0), Vermelho Pedestre (RC3)
      delay_ms(3000);

      fluxo_a = 0;

      if (pedestre_chama == 1)
         pedestre_passando();
   }
}
