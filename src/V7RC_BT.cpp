/*V7RC BLE控制庫 (v250211)
*操作問題請回報(hr_user@trgreat.com)
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <V7RC_BT.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *pCharacteristic;
bool btConnected = false;
bool btReceiveDone=false;
String btRxLoad="";
String sendTemp="";


int V7RC_A[4] ={3,7,11,15};
int V7RC_B[4] ={7,11,15,19};
byte numberBase=10;

class BTServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      btConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      btConnected = false;
    }
    bool ReadBTConnect(){
      return btConnected;  
    }
};

class BLECallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      btReceiveDone=false;
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        btRxLoad="";
        for (int i = 0; i < rxValue.length(); i++){
          btRxLoad +=(char)rxValue[i];
        }
        btRxLoad.replace("\r","");
        btRxLoad.replace("\n","");
        btReceiveDone=true;
      }
    }
};

void V7RC_BT::setupBLE(String BLEName){
  const char *ble_name=BLEName.c_str();
  BLEDevice::init(ble_name);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BTServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic= pService->createCharacteristic(CHARACTERISTIC_UUID_TX,BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX,BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new BLECallbacks());
  pService->start();
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->setScanResponse(true);
  pServer->getAdvertising()->setMinPreferred(0x06);
  pServer->getAdvertising()->setMinPreferred(0x12);
  pServer->getAdvertising()->start();
}


String V7RC_BT::readData(){
 return btRxLoad;
}

bool V7RC_BT::startBLE(){
  return btConnected && btReceiveDone && btRxLoad.length()>0;
}

bool V7RC_BT::setMode(String mod){
  return btRxLoad.startsWith(mod);
}

int V7RC_BT::readSRTdata(int sc){
  return strtol(btRxLoad.substring(V7RC_A[sc-1],V7RC_B[sc-1]).c_str(),0,numberBase);
}