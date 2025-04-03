// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED) && SOC_BT_SUPPORTED

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp32-hal-bt.h"
#include "esp32-hal-log.h"

static bool _bt_initialized = false;
static bool _bt_enabled = false;

bool btInUse() {
    return _bt_initialized && _bt_enabled;
}

bool btStart() {
    esp_err_t err;

    if(_bt_enabled) {
        return true;
    }

    if(!_bt_initialized) {
        esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

        err = esp_bt_controller_init(&bt_cfg);
        if(err != ESP_OK) {
            log_e("BT controller initialize failed: %s", esp_err_to_name(err));
            return false;
        }

        _bt_initialized = true;
    }

    err = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if(err != ESP_OK) {
        log_e("BT controller enable failed: %s", esp_err_to_name(err));
        return false;
    }

    err = esp_bluedroid_init();
    if(err != ESP_OK) {
        log_e("Bluedroid initialize failed: %s", esp_err_to_name(err));
        esp_bt_controller_disable();
        return false;
    }

    err = esp_bluedroid_enable();
    if(err != ESP_OK) {
        log_e("Bluedroid enable failed: %s", esp_err_to_name(err));
        esp_bluedroid_deinit();
        esp_bt_controller_disable();
        return false;
    }

    _bt_enabled = true;
    return true;
}

bool btStop() {
    esp_err_t err;

    if(!_bt_enabled) {
        return true;
    }

    err = esp_bluedroid_disable();
    if(err != ESP_OK) {
        log_e("Bluedroid disable failed: %s", esp_err_to_name(err));
        return false;
    }

    err = esp_bluedroid_deinit();
    if(err != ESP_OK) {
        log_e("Bluedroid deinitialize failed: %s", esp_err_to_name(err));
        return false;
    }

    err = esp_bt_controller_disable();
    if(err != ESP_OK) {
        log_e("BT controller disable failed: %s", esp_err_to_name(err));
        return false;
    }

    _bt_enabled = false;
    return true;
}

#endif
