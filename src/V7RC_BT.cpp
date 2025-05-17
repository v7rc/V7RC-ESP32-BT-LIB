// V7RC BLE 藍芽控制 鴻兔科技 (u2025.05.16)

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp_gap_ble_api.h>
#include <V7RC_BT.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *pCharacteristicTx;
BLECharacteristic *pCharacteristicRx;
bool btConnected = false;
bool btReceiveDone = false;
char btRxLoadChar[100] = {0};
String sendTemp = "";

unsigned long lastReceiveTime = 0;
const unsigned long TIMEOUT_INTERVAL = 5000; // 5秒超時
bool isTimeout = false;

int V7RC_A[4] = {3, 7, 11, 15};
int V7RC_B[4] = {7, 11, 15, 19};
byte numberBase = 10;

void clearBuffer() {
  memset(btRxLoadChar, 0, sizeof(btRxLoadChar));
  btReceiveDone = false;
}

class BTServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      btConnected = true;
      lastReceiveTime = millis();
      isTimeout = false;
    };

    void onDisconnect(BLEServer* pServer) {
      btConnected = false;
      clearBuffer(); // 安全起見，清空資料

      Serial.println("Disconnected, restarting advertising...");
      pServer->getAdvertising()->start(); // <<<< 這行很重要！
    }
};

class BLECallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      unsigned long now = millis();

      if (now - lastReceiveTime > TIMEOUT_INTERVAL) {
        clearBuffer();
      }

      lastReceiveTime = now;
      isTimeout = false;

      btReceiveDone = false;
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0 && rxValue.length() < sizeof(btRxLoadChar)) {
        clearBuffer();
        memcpy(btRxLoadChar, rxValue.c_str(), rxValue.length());

        for (int i = 0; i < strlen(btRxLoadChar); i++) {
          if (btRxLoadChar[i] == '\r' || btRxLoadChar[i] == '\n') {
            btRxLoadChar[i] = 0;
            break;
          }
        }

        btReceiveDone = true;
      } else {
        clearBuffer();
      }
    }
};

void V7RC_BT::setupBLE(String BLEName) {
  const char *ble_name = BLEName.c_str();
  BLEDevice::init(ble_name);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BTServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristicTx = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristicTx->addDescriptor(new BLE2902());

  pCharacteristicRx = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE);
  pCharacteristicRx->setCallbacks(new BLECallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();
}

void V7RC_BT::loopBLE() {
  if (btConnected) {
    if (millis() - lastReceiveTime > TIMEOUT_INTERVAL && !isTimeout) {
      isTimeout = true;
      clearBuffer();
      Serial.println("BLE Timeout, buffer cleared.");
    }
  }
}

String V7RC_BT::readdata(){
  return String(btRxLoadChar);
}

bool V7RC_BT::connect() {
  return btConnected && btReceiveDone && strlen(btRxLoadChar) > 0 && !isTimeout;
}

bool V7RC_BT::setMode(String mod) {
  return String(btRxLoadChar).startsWith(mod);
}

int V7RC_BT::readSRTdata(int sc) {
  if (strlen(btRxLoadChar) >= V7RC_B[sc - 1]) {
    return strtol(String(btRxLoadChar).substring(V7RC_A[sc - 1], V7RC_B[sc - 1]).c_str(), 0, numberBase);
  } else {
    return 0;
  }
}
