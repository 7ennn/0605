// 定義連接腳位
const int trigPin = 9;     // 超音波感測器 Trig
const int echoPin = 10;    // 超音波感測器 Echo
const int relayPin = 8;    // 繼電器控制腳位
const int ledPin = 3;      // LED PWM 控制腳位

// 定義水位閾值（公分）
const int HIGH_WATER_LEVEL = 2;   // 高水位（停止加水）
const int LOW_WATER_LEVEL = 5;    // 低水位（開始加水）
const int CHECK_INTERVAL = 100;   // 檢測間隔（毫秒）

// LED 亮度參數
const int MIN_BRIGHTNESS = 0;     // 最小亮度
const int MAX_BRIGHTNESS = 255;   // 最大亮度

// 系統狀態
bool isValveOpen = false;

void setup() {
  // 初始化腳位
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // 初始狀態：關閉電磁閥
  digitalWrite(relayPin, LOW);
  
  // 初始化序列通訊
  Serial.begin(115200);
  Serial.println("\n水位控制與LED顯示系統啟動");
  Serial.println("----------------------------------------");
  Serial.println("系統設定：");
  Serial.println("高水位（停止加水）：" + String(HIGH_WATER_LEVEL) + " cm");
  Serial.println("低水位（開始加水）：" + String(LOW_WATER_LEVEL) + " cm");
  Serial.println("----------------------------------------");
}

// 測量距離的函數
int measureDistance() {
  // 發送超音波脈衝
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 接收回波並計算距離
  long duration = pulseIn(echoPin, HIGH, 23529);
  
  // 檢查是否有效讀數
  if (duration == 0) {
    Serial.println("警告：超音波感測器讀取失敗");
    return -1;
  }
  
  // 換算成公分
  return duration * 0.034 / 2;
}

// 控制電磁閥
void controlValve(bool shouldOpen) {
  if (shouldOpen != isValveOpen) {
    digitalWrite(relayPin, shouldOpen ? HIGH : LOW);
    isValveOpen = shouldOpen;
    Serial.println(shouldOpen ? "開啟電磁閥" : "關閉電磁閥");
  }
}

// 控制 LED 亮度
void updateLED(int distance) {
  // 將距離映射到亮度值
  int brightness = map(distance, LOW_WATER_LEVEL, HIGH_WATER_LEVEL, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  brightness = constrain(brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  
  // 設定 LED 亮度
  analogWrite(ledPin, brightness);
  
  Serial.print("LED亮度: ");
  Serial.println(brightness);
}

void loop() {
  // 測量當前水位
  int distance = measureDistance();
  
  // 檢查讀數是否有效
  if (distance < 0) {
    Serial.println("錯誤：無效的距離讀數，等待下次檢測");
    delay(CHECK_INTERVAL);
    return;
  }
  
  // 輸出當前狀態
  Serial.print("當前距離: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // 更新 LED 亮度
  updateLED(distance);
  
  // 根據水位控制電磁閥
  if (distance <= HIGH_WATER_LEVEL) {
    // 水位達到高水位，關閉電磁閥
    controlValve(false);
  } else if (distance >= LOW_WATER_LEVEL) {
    // 水位低於低水位，開啟電磁閥
    controlValve(true);
  }
  // 水位在中間範圍，保持當前狀態
  
  delay(CHECK_INTERVAL);
}