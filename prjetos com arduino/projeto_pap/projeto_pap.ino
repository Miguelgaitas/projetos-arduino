#include <DHT.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Endereço MAC do Arduino
IPAddress server_addr(127, 0, 0, 1);  // Endereço IP do servidor MySQL (localhost)
char user[] = "MiguelGaitas";  // Nome de usuário do banco de dados
char password[] = "Comida24.";  // Senha do banco de dados
char db[] = "dados_dos_registros";  // Nome do banco de dados

DHT dht(2, DHT11);  // Pino do sensor DHT11
MySQL_Connection conn((Client *)&Serial);
MySQL_Cursor *cursor;

unsigned long previousMillis = 0;
unsigned long interval = 300000;  // Intervalo de uma hora em milissegundos

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // Aguarde a conexão serial
  }

  dht.begin();
  
  Serial.println("Conectando ao servidor MySQL...");
  if (conn.connect(server_addr, 3306, user, password)) {
    Serial.println("Conexão estabelecida com sucesso!");
    cursor = new MySQL_Cursor(&conn);
  } else {
    Serial.println("Falha na conexão com o servidor MySQL.");
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Verificar se a leitura do sensor foi bem-sucedida
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Falha ao ler dados do sensor DHT11.");
      delay(2000);
      return;
    }

    // Enviar os dados para o servidor MySQL
    char insertQuery[128];
    sprintf(insertQuery, "INSERT INTO weather_data (temperature, humidity) VALUES (%f, %f)", temperature, humidity);
    cursor->execute(insertQuery);

    Serial.println("Dados enviados para a base de dados.");

    delay(1000);  // Aguardar 1 segundo antes de continuar
  }
}
