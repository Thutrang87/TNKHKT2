#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SV_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CH_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// In ra Serial khi có thiết bị ghi giá trị mới
class WriteCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) {
    Serial.print("Gia tri moi duoc ghi: ");
    Serial.println(c->getValue().c_str());
  }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("Nhom7-BLE");                 // (1) đổi theo tên nhóm
  auto* sv = BLEDevice::createServer();
  auto* se = sv->createService(SV_UUID);
  auto* ch = se->createCharacteristic(
      CH_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE  // (3) thêm WRITE
  );
  ch->setValue("MSSV: 25112113");               // (2) thông điệp riêng
  ch->setCallbacks(new WriteCallback());
  se->start();

  auto* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SV_UUID);
  adv->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE server dang chay, ten: Nhom7-BLE");
}

void loop() { delay(2000); }