#include <V7RC_BT.h>

/* 修改 UUID 或者直接修改 V7RC_BT.cpp
#ifdef SERVICE_UUID   
#undef SERVICE_UUID   
#define SERVICE_UUID   "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
 */


V7RC_BT v7rc;

void setup() {
  Serial.begin(115200);
  v7rc.setupBLE("V7RC_BLE");//設定藍芽名稱
}

void loop() {

  if (v7rc.startBLE()){
    Serial.println(v7rc.readData());
    if (v7rc.setMode("SRT")){
      Serial.println("-------------------------------------------------------------------------");
      Serial.print("DATA 1：");
      Serial.println(v7rc.readSRTdata(1));
      
      Serial.print("DATA 2：");
      Serial.println(v7rc.readSRTdata(2));
      
      Serial.print("DATA 3：");
      Serial.println(v7rc.readSRTdata(3));
      
      Serial.print("DATA 4：");
      Serial.println(v7rc.readSRTdata(4));
    }
  }
  
}

/*方法使用
v7rc.startBLE() 確認藍芽是否有連線 // 回傳值:boolean
v7rc.readData()讀取資料 // 回傳值:String
v7rc.setMode("SRT") 模式是否為SRT // 回傳值:boolean
v7rc.readSRTdata(1) 讀取SRT資料(1~4) // 回傳值:int
*/
