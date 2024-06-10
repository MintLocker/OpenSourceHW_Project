# 한성대 2024-1 오픈소스 HW 기말 프로젝트

**Smart Locker with TOTP - TOTP를 이용한 스마트 보관함** 

## OpenSourceHW

### 사용 하드웨어

- WeMos D1 R1 호환보드

- LCD - 1

- KeyPad - 1

### 보드 & 라이브러리

- ESP8266: 아두이노 IDE -> 설정 -> 추가 보드 관리자 URL ->
https://arduino.esp8266.com/stable/package_esp8266com_index.json 추가

- TOTP: https://github.com/lucadentella/TOTP-Arduino

- LiquidCrystal_I2C

- KeyPad

### 설명

보관함 OTP와 관련된 작업을 하는 코드입니다.

아두이노는 자체적으로 OTP를 생성합니다 

키패드에 OTP를 입력하여 생성한 OTP와 일치하면 잠금 해제 신호를 R3 아두이노로 전송합니다.

5회 틀린 경우 경고 신호를 R3 아두이노로 전송합니다.

---

## OpenSourceHW_1

### 사용 하드웨어

- Arduino Uno R3

- LED - 1

- Buzzer - 1

- Push Button - 1

- Servo Motor - 1

- Bluetooth - 1

- Ultrasonic Sensor - 1

### 보드 & 라이브러리

- Servo
 
- MsTimer2

### 설명

보관함 기능과 관련된 작업을 하는 코드입니다.

버튼을 누르면 잠김 상태로 전환합니다

잠김 상태에서는 서보 모터가 90도, 열림 상태에서는 0도로 설정됩니다 

잠김 상태에서 초음파 센서의 값이 일정 값 이상이면 강제로 문이 열렸다고 판단하여 경고 발생합니다
