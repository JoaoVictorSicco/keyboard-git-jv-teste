#include "communication.h"
#include "handle.h"
#include "keyboard.h"
#include "ads1115.h"
#include "mcp.h"

#define BL_LOAD_CTRL GPIO_NUM_14

void app_main(void)
{
    while(!init_flash());
    init_communication();
    init_handle();
    init_tinyusb();
    init_keyboard();
    keyled_main();
    //MCP precisa ser iniciado e colocado na POSIÇÃO MAXIMA
    init_mcp4011();
    up_down_digipot(UP, 64);

    gpio_set_direction(BL_LOAD_CTRL, GPIO_MODE_OUTPUT);
    vTaskDelay((pdMS_TO_TICKS(100)));
    gpio_set_level(BL_LOAD_CTRL,1);
    // vTaskDelay((pdMS_TO_TICKS(100)));
    // gpio_set_level(BL_LOAD_CTRL,0);
    // press_single(0,0);
    // for(int i = 0; i<=7; i++)
    // {
    //     for(int j = 0; j<=16; j++)
    //     {
    //         // press_single_fn_mode(i,j,0);
    //         press_single(i,j);
    //         printf("ksi: %d / kso: %d\n", i, j);
    //         vTaskDelay(pdMS_TO_TICKS(500));
    //     }

    // }
}