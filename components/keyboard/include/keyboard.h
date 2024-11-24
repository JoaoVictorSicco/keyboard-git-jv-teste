#ifndef _KB_H_
#define _KB_H_

    #include "driver/gpio.h"
    #include "defines.h"

    esp_err_t press_all(uint8_t *keyboard_matrix, bool status_fn);
    esp_err_t press_single_fn_mode(uint8_t ksi, uint8_t kso, bool status_fn);
    esp_err_t fn_status_level(uint8_t mode);
    esp_err_t press_single(uint8_t ksi, uint8_t kso);
    esp_err_t kso_select(uint8_t kso);
    esp_err_t ksi_select(uint8_t ksi);
    esp_err_t kso_select_ic(uint8_t kso, uint8_t mode);
    esp_err_t disable_all_kso(uint8_t kso);
    esp_err_t disable_all_ksi(void);
    esp_err_t init_keyboard(void);
    

#endif