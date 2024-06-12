#include <Servo.h> //서보 모터 헤더
#include <MsTimer2.h> //타이머 인터럽트 헤더
#include <SoftwareSerial.h> //블루투스 통신용 헤더

/*===Packet==========================================*/
#define ACK 0xE0
#define NAK 0xF0
#define OP 0x01
#define CL 0x02 //해당 코드에서는 사용되지 않으나 기능 확장용으로 추가
#define AL 0x10

/*===Bluetooth=======================================*/
const char tx = 5;
const char rx = 6;
SoftwareSerial BTSerial(tx, rx);

/*===LED, Buzzer=====================================*/
const char rLedPin = 10;
const char gLedPin = 9;
const char buzPin = 8;
bool state = LOW;
bool isAlert = false;

/*===Ultrasonic======================================*/
const char trigPin = 13;
const char echoPin = 12; 
long duration, cm;

/*===Servo Motor=====================================*/
Servo servo;
const char servoPin = 4;

/*===Button==========================================*/
const char btnPin = 3;

/*===etc=============================================*/
bool locked = true;

/*===function========================================*/
//경고 작동 함수
void alert() {
  state = !state;
  digitalWrite(rLedPin, state);
  digitalWrite(buzPin, state);
}

//버튼 인터럽트 함수
void closeLocker()  
{
  digitalWrite(rLedPin, HIGH);
  locked = true;
  servo.write(90);
  Serial.write(CL);
}

//초음파 센서 값 cm로 변환 함수
long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

/*===main============================================*/
void setup() {

  //serial init
  Serial.begin(115200);

  //bluetooth init
  BTSerial.begin(9600);

  //servo init
  servo.attach(servoPin);

  //timer init
  MsTimer2::set(1000, alert);

  //led, buzzer init
  pinMode(gLedPin, OUTPUT);  
  pinMode(rLedPin, OUTPUT);
  pinMode(buzPin, OUTPUT);

  //ultrasonic init
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  
  //button init
  pinMode(btnPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(btnPin), closeLocker, RISING);
}

void loop() {
  //led
  digitalWrite(gLedPin, locked ? LOW : HIGH);

  //ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);

  Serial.println(cm);
  
  if(locked && cm > 20 && !isAlert && locked){ //잠금 상태에서 초음파 센서 값 일정 이상이면 경고
    isAlert = true;
    MsTimer2::start();
  }
  
  //bluetooth
  if(BTSerial.available()){
    char recieve = BTSerial.read();
    if(recieve == 'D'){
      MsTimer2::stop();
      state = LOW;
      digitalWrite(rLedPin, state);
      digitalWrite(buzPin, state);
    }
  }
  delay(100);
}

/*===Serial Event====================================*/
void serialEvent() {
  byte packet[4];
  char packetIndex = 0;
  bool packetStarted = false;
  while (Serial.available()) {
    char inputByte = Serial.read();

    if (!packetStarted) {
      if (inputByte == 0x47) { // 시작 바이트 확인

        packetStarted = true;
        packetIndex = 0;
        packet[packetIndex++] = inputByte;
      }
    } else {
      packet[packetIndex++] = inputByte;

      if (packetIndex == 4) { // 패킷 전체가 올바른 경우 데이터 처리
        if (packet[3] == 0x0A) { // 종료 바이트 확인
          if (packet[1] | packet[2] == 0xFF) { // 체크섬 확인
            Serial.println("received!");
            if (packet[1] == OP) { //OPEN
              locked = false;
              servo.write(0);
              Serial.write(ACK);
            } else if(packet[1] == CL) { //CLOSE
              locked = true;
              servo.write(90);
              Serial.write(ACK);
            } else if (packet[1] == AL) { //ALERT
              locked = true;
              digitalWrite(gLedPin, LOW);
              Serial.write(ACK);
              MsTimer2::start();
            }
          } else {
            Serial.write(NAK); // 체크섬 오류
          }
        } else {
          Serial.write(NAK); // 종료 바이트 오류
        }
        packetStarted = false;
        packetIndex = 0;
      }
    }
  }
}
