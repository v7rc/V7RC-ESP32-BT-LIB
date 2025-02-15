#ifndef V7RC_BT_h
#define V7RC_BT_h


#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class V7RC_BT {

private :



public :

void setUUID(String uuid);
void setupBLE(String BLEName);

bool startBLE();
String readData();
bool setMode(String mod);
int readSRTdata(int sc);

};



#endif

