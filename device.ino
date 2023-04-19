#include <SPI.h>
#include <EthernetENC.h>
#include <ArduinoUniqueID.h>

byte mac[] = { 0xDE, 0xAD, 0xBF, 0xEF, 0xFE, 0xED };
byte server[] = { 192, 168, 1, 179 };

EthernetClient client;

void setup() {
  // Ethernet.init(5); 

  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  pinMode(5, OUTPUT); 
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {}
  
  Serial.println("ACTS Magnetic Reader");
  Serial.print("sensor id: ");
  Serial.print((byte)UniqueID[1], HEX);
  Serial.print((byte)UniqueID[2], HEX);
  Serial.print((byte)UniqueID[3], HEX);
  Serial.println();

  mac[1] = (byte)UniqueID[1];
  mac[2] = (byte)UniqueID[2];
  mac[3] = (byte)UniqueID[3];
  mac[4] = (byte)UniqueID[4];
  mac[5] = (byte)UniqueID[5];

  Ethernet.begin(mac);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERROR: ethernet controller down");

    digitalWrite(5, 50);
    delay(1000);
    digitalWrite(5, LOW);
    
    while (true) {}
  }
  
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("ERROR: ethernet link down");

    digitalWrite(5, 150);
    delay(1000);
    digitalWrite(5, LOW);

    setup();
  }
  
  Serial.print("address: ");
  Serial.println(Ethernet.localIP());

  digitalWrite(5, 100);
  delay(100);
  digitalWrite(5, LOW);

  connect();
}

bool a = false;
bool b = false;

void loop() {
  bool aa = !digitalRead(6);
  bool bb = !digitalRead(7);

  if (a != aa) {
    Serial.println('A');
    submit(true, aa);

    a = aa;
  }

  if (b != bb) {
    Serial.println('B');
    submit(false, bb);

    b = bb;
  }
}

void connect() {
  client.stop();

  Serial.println("connecting...");

  if (client.connect(server, 141)) {
    client.print("RTPC 1E ");

    client.print((byte)UniqueID[1], HEX);
    client.print((byte)UniqueID[2], HEX);
    client.print((byte)UniqueID[3], HEX);

    client.print(" detect/magnet\n");
    client.print("s: 1\n\n");

    Serial.println("connected");
  } else {
    Serial.println("ERROR: connection failed");

    digitalWrite(5, 255);
    delay(1000);
    digitalWrite(5, LOW);

    connect();
  }
}

void submit(bool direction, bool state) {
  digitalWrite(LED_BUILTIN, HIGH);

  if (!client.connected()) {
    connect();
  }

  if (state) {
    client.print("s: t\n");
  } else {
    client.print("s: f\n");
  }

  if (direction) {
    client.print("d: p\n\n");
  } else {
    client.print("d: n\n\n");
  }

  client.flush();

  digitalWrite(LED_BUILTIN, LOW);
}