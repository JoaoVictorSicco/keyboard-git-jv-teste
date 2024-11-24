#ifndef _DEFINES_H_
#define _DEFINES_H_

    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
    #include "sdkconfig.h"
    #include "esp_log.h"
    #include "esp_check.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"

    // hab desbilita o mux (enable) ativa em 0
    #define SELECTED 0
    #define DESELECTED 1 

    #define ON 1
    #define OFF 0

    #define FN_HIGH (1)
    #define FN_LOW  (0)

    #define ENABLE_KSI_IC3 GPIO_NUM_1

    #define ENABLE_KSO_IC4 GPIO_NUM_38 // 0 a 7
    #define ENABLE_KSO_IC5 GPIO_NUM_37 // 8 a 15
    #define ENABLE_KSO_IC6 GPIO_NUM_36 // 9 a 23

    #define FN_CTRL GPIO_NUM_5         // Tecla FN

    #define KSI_SI1 GPIO_NUM_2
    #define KSI_SI2 GPIO_NUM_3
    #define KSI_SI3 GPIO_NUM_4

    #define KSO_SO1 GPIO_NUM_35
    #define KSO_SO2 GPIO_NUM_34
    #define KSO_SO3 GPIO_NUM_33

    #define WAIT (10/portTICK_PERIOD_MS)
    #define DELAY_PRESS (20/portTICK_PERIOD_MS)
    #define DELAY_FN (40/portTICK_PERIOD_MS)

    #define SIZE_BUFFER 3500
    //#define SIZE_BUFFER_TX 1024
    
    #define TXD_PIN 17 //DAC_1
    #define RXD_PIN 18 //DAC_2

    #define ERR_JSON "invalid_json"          
    #define ERR_ACTION "action_erro"
    #define ERR_DEVICE "invalid_device"
    #define ERR_ID "invalid_id"

    #define ACT_EVENT "event"              
    #define ACT_REPLY "reply"
    #define ACT_ERRO "erro"

    #define ESP_CMD_FAIL -2                 
    #define ID_DEFAULT "ks-001"

    #define KEY_ID "ID"

    #define DEVICE "keyboard"

    #define ID_VERIFY "kb-%1d%1d%1d"

    enum action_type{
        EVENT=1,
        REPLY,
        ERRO
    };

#endif