#include "flash.h"

uint8_t init_flash()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    if(!read_flash(KEY_ID, id))
    {
        ESP_LOGI("FLASH", "VOU ESCREVER O ID DEFAULT %s", id);
        write_flash(KEY_ID, ID_DEFAULT);
    }else{
        ESP_LOGI("FLASH","ID LIDO %s", id);
    }
    return 1;
}

esp_err_t write_flash(const char* key, char* data)
{
    nvs_handle my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK)
    {
        nvs_set_str(my_handle, key, data);
        nvs_commit(my_handle);
        nvs_close(my_handle);

        strcpy(id, data);
        return ESP_OK;
    }
    return ESP_FAIL;
}

uint8_t read_flash(const char* key, char* value)
{
    ESP_LOGD("FLASH", "DENTRO READ FLASH");
    nvs_handle my_handle;
    ESP_LOGD("FLASH", "DENTRO READ FLASH, ANTES DO NVS_OPEN");
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    ESP_LOGD("FLASH", "ERR nvs_open: %d", err);

    if (err == ESP_OK)
    {
        char values[1024];
        size_t tam = 1024;
        err = nvs_get_str(my_handle, key, values, &tam);
        ESP_LOGD("FLASH", "ERR nvs_get_str: %d", err);
        switch (err)
        {
        case ESP_OK:
            strcpy(value, values);
            nvs_close(my_handle);
            return 1;
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            sprintf(value, "CHAVE NÃO INICIALIZADA.");
            nvs_close(my_handle);
            return 0;
            break;
        }
        nvs_close(my_handle);
    }
    return 0;
}