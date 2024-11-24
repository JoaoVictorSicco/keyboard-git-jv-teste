#ifndef FLASH_H_
#define FLASH_H_

    #include "defines.h"
    #include "nvs.h"
    #include "nvs_flash.h"

    char id[50];
    char id_show[50];

    uint8_t init_flash();
    esp_err_t write_flash(const char* key, char* data);
    uint8_t read_flash(const char* key, char* value);

#endif