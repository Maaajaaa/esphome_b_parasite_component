#include "b_parasite_dev.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace b_parasite_dev {

static const char *const TAG = "b_parasite";

void BParasite::dump_config() {
  ESP_LOGCONFIG(TAG, "b_parasite");
  LOG_SENSOR("  ", "Battery Voltage", this->battery_voltage_);
  LOG_SENSOR("  ", "Temperature", this->temperature_);
  LOG_SENSOR("  ", "Humidity", this->humidity_);
  LOG_SENSOR("  ", "Soil Moisture", this->soil_moisture_);
  LOG_SENSOR("  ", "Illuminance", this->illuminance_);
}

bool BParasite::parse_device(const esp32_ble_tracker::ESPBTDevice &device) {
  if (device.address_uint64() != address_) {
    ESP_LOGVV(TAG, "parse_device(): unknown MAC address.");
    return false;
  }
  ESP_LOGD(TAG, "parse_device(): MAC address %s found.", device.address_str().c_str());
  const auto &service_datas = device.get_service_datas();
  if (service_datas.size() != 1) {
    ESP_LOGE(TAG, "Unexpected service_datas size (%d)", service_datas.size());
    return false;
  }
  const auto &service_data = service_datas[0];

  ESP_LOGD(TAG, "Service data:");
  for (const uint8_t byte : service_data.data) {
    ESP_LOGD(TAG, "0x%02x", byte);
  }

  const auto &data = service_data.data;
  ESP_LOGD(TAG, "byte 0? %02x", data[0]);

  if(data[0] != 0x40){
    ESP_LOGE(TAG, "check that the BTHomeData version is indeed supportet");
  }

  float temp_celsius, humidity_percent, battery_voltage, moisture_percent;

  if(data[1] == 0x02){
    uint16_t temp_millicelsius = data[3] << 8 | data[2];
    temp_celsius = temp_millicelsius / 100.0f;
  }else{    
    ESP_LOGE(TAG, "varying (but valid as per standard) data format of temperature mismatch not implemented yet");
  }

 
  if(data[4] == 0x03){
    // Relative air humidity as 2 bytes unsigned int
    uint16_t humidity = data[6] << 8 | data[5];
    humidity_percent = (0.01f * humidity);
  }else{    
    ESP_LOGE(TAG, "varying (but valid as per standarasd) data format of humiditz mismatch not implemented yet");
  }

  //bit 7 is 0x05 illuminance, followed by 3 bytes of illuminance, factor is 0.01 lux

  // Ambient light in lux.
  float illuminance = (data[8] << 16 | data[9] << 8 | data[10]) * 0.01f;

  if(data[11] == 0x0c){
     // Battery voltage in millivolts.
    uint16_t battery_millivolt = data[13] << 8 | data[12];
    battery_voltage = battery_millivolt / 1000.0f;
  }else{    
    ESP_LOGE(TAG, "varying (but valid as per standarasd) data format of voltage mismatch not implemented yet");
  }

  if(data[14] == 0x0c){
     // Relative soil moisture in 0.01 %
    uint16_t soil_moisture = data[16] << 8 | data[15];
    moisture_percent = (0.01 * soil_moisture);
  }else{    
    ESP_LOGE(TAG, "varying (but valid as per standarasd) data format of moisture mismatch not implemented yet");
  }

  //no idea what thats suppoed to be
  /*const uint8_t protocol_version = data[0] >> 4;
  if (protocol_version != 1 && protocol_version != 2) {
    ESP_LOGE(TAG, "Unsupported protocol version for some reason not: %u", protocol_version);
    //return false;
  }*/

  // Some b-parasite versions have an (optional) illuminance sensor.#
  //Maja: but this isn't how you check for it
  //bool has_illuminance = data[0] & 0x1;

  // Counter for deduplicating messages.
  // Maja: don't get the idea there
  uint8_t counter = data[1] & 0x0f;
  if (last_processed_counter_ == counter) {
    ESP_LOGE(TAG, "Skipping already processed counter (%u)", counter);
    //return false;
  }




  if (battery_voltage_ != nullptr) {
    battery_voltage_->publish_state(battery_voltage);
  }
  if (temperature_ != nullptr) {
    temperature_->publish_state(temp_celsius);
  }
  if (humidity_ != nullptr) {
    humidity_->publish_state(humidity_percent);
  }
  if (soil_moisture_ != nullptr) {
    soil_moisture_->publish_state(moisture_percent);
  }
  if (illuminance_ != nullptr) {
    //if (has_illuminance) {
      illuminance_->publish_state(illuminance);
    //} else {
      ESP_LOGE(TAG, "No lux information is present in the BLE packet");
    //}
  }

  last_processed_counter_ = counter;
  return true;
}

}  // namespace b_parasite_dev
}  // namespace esphome

#endif  // USE_ESP32