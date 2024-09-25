#include <Keypad.h>
#include <Adafruit_LiquidCrystal.h>
#include <Servo.h>

Servo myservo;  // Cria um objeto servo

const byte Filas = 4;
const byte Columnas = 4;

byte Pines_Filas[] = {9, 8, 7, 6};       // Pinos para as linhas
byte Pines_Columnas[] = {5, 4, 3, 2};    // Pinos para as colunas

char Teclas[Filas][Columnas] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad Teclado1 = Keypad(makeKeymap(Teclas), Pines_Filas, Pines_Columnas, Filas, Columnas);
Adafruit_LiquidCrystal lcd_1(0);

String senha = "111"; // Senha padrão
String senhaUsuario; // Armazena a senha digitada
int tentativas = 0;
const int maxTentativas = 3;

const int pirPin = 10;   // Pino do sensor PIR
const int piezoPin = 12; // Pino do buzzer
bool portaDestrancada = false;

void setup() {
  Serial.begin(9600);
  lcd_1.begin(16, 2);
  myservo.attach(13); // Conecta o servo ao pino 13
  pinMode(pirPin, INPUT); // Define o pino do sensor PIR como entrada
  pinMode(piezoPin, OUTPUT); // Define o pino do buzzer como saída
  lcd_1.print("Digite a Senha");
}

void loop() {
  char keypressed = Teclado1.getKey(); // Captura a tecla pressionada

  if (keypressed != NO_KEY) {
    Serial.println(keypressed);
    senhaUsuario += keypressed; // Adiciona tecla à senha digitada

    lcd_1.setCursor(0, 1);
    lcd_1.print("Senha: " + senhaUsuario);
    
    if (senhaUsuario.length() == 3) {
      lcd_1.setCursor(0, 1);
      if (senhaUsuario == senha) {
        lcd_1.print("Acesso Permitido");
        myservo.write(90);  // Gira o servo para destrancar
        delay(1000);        // Mantém o servo na posição por 1 segundo
        myservo.write(0);   // Retorna o servo para a posição original
        portaDestrancada = true; // Indica que a porta está destrancada
        tentativas = 0;     // Reseta as tentativas
        delay(2000);        // Espera para a próxima ação
      } else {
        lcd_1.print("Acesso Negado  ");
        digitalWrite(piezoPin, HIGH); // Ativa o buzzer
        delay(1000);                   // Som por 1 segundo
        digitalWrite(piezoPin, LOW);  // Desativa o buzzer
        tentativas++;
        delay(2000);                   // Delay para mostrar mensagem de erro
      }
      senhaUsuario = ""; // Limpa a senha digitada
      lcd_1.clear();
      lcd_1.print("Digite a Senha");

      if (tentativas >= maxTentativas) {
        lcd_1.setCursor(0, 1);
        lcd_1.print("Bloqueado!     ");
        digitalWrite(piezoPin, HIGH); // Ativa o buzzer
        delay(2000);                  // Som por 2 segundos
        digitalWrite(piezoPin, LOW);  // Desativa o buzzer
        while (true); // Bloqueia o sistema após 3 tentativas
      }
    }
  }

  // Verifica movimento após o acesso permitido
  if (portaDestrancada) {
    if (digitalRead(pirPin) == HIGH) { // Se o movimento for detectado
      lcd_1.setCursor(0, 1);
      lcd_1.print("Movimento!     ");
      digitalWrite(piezoPin, HIGH); // Ativa o buzzer como alarme
      delay(2000);                  // Alarme por 2 segundos
      digitalWrite(piezoPin, LOW);  // Desativa o buzzer
      delay(2000);                  // Aguarda antes de verificar novamente
    }
  }

}

