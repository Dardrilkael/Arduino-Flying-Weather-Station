
void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println("Inicando");
}

void loop(){

  if(Serial.available()){
    char incomingByte = Serial.read();
    if (incomingByte =='R' || incomingByte == 'r'){
      Serial.println("\nReiniciando 🔄\n");
      delay(1000);
      esp_restart();
    }
  }
  

  static int counter = 0;
  Serial.printf("Looping (%i)\n",counter++);
  delay(1500);
}