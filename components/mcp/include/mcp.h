#ifndef __MCP_H__
#define __MCP_H__

    #include <stdio.h>
    #include <string.h>
    #include "driver/gpio.h"
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    esp_err_t init_mcp4011(void); 
    esp_err_t up_down_digipot(bool up_down, uint8_t amount);

    #define DIGIPOT_CS GPIO_NUM_12
    #define DIGIPOT_UD GPIO_NUM_13
    #define BL_SCR_CTRL GPIO_NUM_14

    //USADO NO MCP QUE FUNCIONA COM LOGICA INVERSA, O MCP ACEITA OS COMANDOS APÓS BAIXAR O CS
    #define HIGH 0
    #define LOW 1

    #define UP 0
    #define DOWN 1

    #define R11 2.7
    #define RAB 10000
    #define MAX 63

#endif