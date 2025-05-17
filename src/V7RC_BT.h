// V7RC BLE 藍芽控制 鴻兔科技 (u2025.05.16)
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
void loopBLE();
bool connect();
String readdata();
bool setMode(String mod);
int readSRTdata(int sc);

};



#endif

