#include "mcp.h"

esp_err_t init_mcp4011(void){

    gpio_set_direction(DIGIPOT_CS, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIGIPOT_UD, GPIO_MODE_OUTPUT);
    gpio_set_direction(BL_SCR_CTRL, GPIO_MODE_OUTPUT);

    gpio_set_level(DIGIPOT_CS, HIGH);
    gpio_set_level(DIGIPOT_UD, HIGH);
    gpio_set_level(BL_SCR_CTRL, LOW);
    return ESP_OK;  
}


/*O protocolo U/D simples usa o estado do pino U/D
na borda descendente do pino CS para determinar se
o modo Incremento ou Decremento é desejado. Bordas
subidas subsequentes do pino U/D movem o limpador.
O valor do limpador não irá estourar ou transbordar.*/

esp_err_t up_down_digipot(bool up_down, uint8_t amount){

    gpio_set_level(DIGIPOT_UD, up_down);
    ets_delay_us(10); //10 microssegundo, é necessario pelo menos 0,5.
    gpio_set_level(DIGIPOT_CS, LOW);

    for(int i=0; i < amount; i++){
        gpio_set_level(DIGIPOT_UD, !up_down);
        ets_delay_us(10); //10 microssegundo, é necessario pelo menos 0,5.
        gpio_set_level(DIGIPOT_UD, up_down);
        ets_delay_us(10); //10 microssegundo, é necessario pelo menos 0,5.
    }
    gpio_set_level(DIGIPOT_CS, HIGH);
    ets_delay_us(10); //10 microssegundo, é necessario pelo menos 0,5.
    gpio_set_level(DIGIPOT_UD, HIGH);
    return ESP_OK;
}

//Raw = Rab.(63-n)/63
//Rcarga = R11 . Rab / Raw => R11 . 63/(63-n)  

//N = 63 - Rcarga / (R11 . 63)                    //Dps arredondar o valor.

//Refetiva = 1/((1/Rcarga)+(1/Rab)) => Rcarga . Rab / (Rcarga + Rab)

//Rcarga = Refetivo . Rab / (Rab - Refetivo)

//Corrente = Vbacklight / Refetiva 
//Refetivo = Vbacklight / Corrente