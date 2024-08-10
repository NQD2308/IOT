int cambien = D5;  // khai báo chân digital 10 cho cảm biến

int Led = D7;  //kháo báo chân digital 8 cho đèn LED

void setup() {
  Serial.begin(9600);
  pinMode(Led, OUTPUT);  //pinMode xuất tín hiệu đầu ra cho led

  pinMode(cambien, INPUT);  //pinMode nhận tín hiệu đầu vào cho cảm biê
}

void loop() {

  int value = digitalRead(cambien);  //lưu giá trị cảm biến vào biến value
  if (value == 1) {
    digitalWrite(Led, HIGH);  //xuất giá trị ra đèn LED
  } else {
    digitalWrite(Led, LOW);
  }

  Serial.println(value);

  delay(500);
}