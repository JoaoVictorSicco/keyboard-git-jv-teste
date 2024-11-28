#include "keyboard.h"

static const char* KSI_TAG = "[KSI]";
static const char* KSO_TAG = "[KSO]";

esp_err_t init_keyboard(void){

    gpio_set_level(ENABLE_KSI_IC3,DESELECTED);
    gpio_set_level(ENABLE_KSO_IC4,DESELECTED);
    gpio_set_level(ENABLE_KSO_IC5,DESELECTED);
    gpio_set_level(ENABLE_KSO_IC6,DESELECTED);
    fn_status_level(FN_LOW);
    vTaskDelay(10);

    gpio_set_level(KSI_SI1,OFF);
    gpio_set_level(KSI_SI2,OFF);
    gpio_set_level(KSI_SI3,OFF);
    gpio_set_level(KSO_SO1,OFF);
    gpio_set_level(KSO_SO2,OFF);
    gpio_set_level(KSO_SO3,OFF);
    vTaskDelay(10);

    gpio_set_direction(ENABLE_KSI_IC3,GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_KSO_IC4,GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_KSO_IC5,GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_KSO_IC6,GPIO_MODE_OUTPUT);

    gpio_set_direction(FN_CTRL,GPIO_MODE_OUTPUT);       // Declara FN_CTRL como saida
    vTaskDelay(10);

    gpio_set_direction(KSI_SI1,GPIO_MODE_OUTPUT);
    gpio_set_direction(KSI_SI2,GPIO_MODE_OUTPUT);
    gpio_set_direction(KSI_SI3,GPIO_MODE_OUTPUT);
    gpio_set_direction(KSO_SO1,GPIO_MODE_OUTPUT);
    gpio_set_direction(KSO_SO2,GPIO_MODE_OUTPUT);
    gpio_set_direction(KSO_SO3,GPIO_MODE_OUTPUT);
    vTaskDelay(10);

    gpio_set_level(ENABLE_KSI_IC3,DESELECTED);
    gpio_set_level(ENABLE_KSO_IC4,DESELECTED);
    gpio_set_level(ENABLE_KSO_IC5,DESELECTED);
    gpio_set_level(ENABLE_KSO_IC6,DESELECTED);
    fn_status_level(FN_LOW);
    vTaskDelay(10);

    gpio_set_level(KSI_SI1,OFF);
    gpio_set_level(KSI_SI2,OFF);
    gpio_set_level(KSI_SI3,OFF);
    gpio_set_level(KSO_SO1,OFF);
    gpio_set_level(KSO_SO2,OFF);
    gpio_set_level(KSO_SO3,OFF);
    vTaskDelay(10);
    return ESP_OK;
}


esp_err_t disable_all_ksi(void){
    vTaskDelay(DELAY_PRESS); 
    gpio_set_level(ENABLE_KSI_IC3, DESELECTED);
    // vTaskDelay(1);

    gpio_set_level(KSI_SI1, OFF);
    gpio_set_level(KSI_SI2, OFF);
    gpio_set_level(KSI_SI3, OFF);
    // vTaskDelay(WAIT);
    
    return ESP_OK;
}

esp_err_t disable_all_kso(uint8_t kso){
    vTaskDelay(DELAY_PRESS); 

    gpio_set_level(ENABLE_KSO_IC4, DESELECTED);
    gpio_set_level(ENABLE_KSO_IC5, DESELECTED);
    gpio_set_level(ENABLE_KSO_IC6, DESELECTED);

    gpio_set_level(KSO_SO1, OFF);
    gpio_set_level(KSO_SO2, OFF);
    gpio_set_level(KSO_SO3, OFF);
    // vTaskDelay(WAIT);
    
    return ESP_OK;
}


esp_err_t kso_select_ic(uint8_t kso, uint8_t mode)
{
    if(kso <= 7)
        gpio_set_level(ENABLE_KSO_IC4, mode);
    else if (kso >= 8 && kso <= 15)
        gpio_set_level(ENABLE_KSO_IC5, mode);
    else if (kso >= 16 && kso <= 23)
        gpio_set_level(ENABLE_KSO_IC6, mode);
    else
        return ESP_FAIL;
        
    return ESP_OK;
}

//ao selecionar uma porta com o si1 si2 e si3, teoricamente vai ficar 1 no binario,
//entao o case 0 é só ligar e desligar o ksi
esp_err_t ksi_select(uint8_t ksi)
{
    vTaskDelay(WAIT);
    // SI3 é o mais significativo
    switch (ksi)
    {
    case 0: //KSI0
        gpio_set_level(KSI_SI1,OFF);
        gpio_set_level(KSI_SI2,OFF);
        gpio_set_level(KSI_SI3,OFF);
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        ESP_LOGI(KSI_TAG,"CASE 0");
        break;
    case 1: //KSI1 
        gpio_set_level(KSI_SI1,ON);
        gpio_set_level(KSI_SI2,OFF);
        gpio_set_level(KSI_SI3,OFF);
        ESP_LOGI(KSI_TAG,"CASE 1");
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        break;
    case 2: //KSI2
        gpio_set_level(KSI_SI1,OFF);
        gpio_set_level(KSI_SI2,ON);
        gpio_set_level(KSI_SI3,OFF);
        ESP_LOGI(KSI_TAG,"CASE 2");
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        break;
    case 3:
        gpio_set_level(KSI_SI1,ON);
        gpio_set_level(KSI_SI2,ON);
        gpio_set_level(KSI_SI3,OFF);
        ESP_LOGI(KSI_TAG,"CASE 3");
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        break;

    case 4: //(100)
        gpio_set_level(KSI_SI1,OFF);
        gpio_set_level(KSI_SI2,OFF);
        gpio_set_level(KSI_SI3,ON);
        ESP_LOGI(KSI_TAG,"CASE 4");
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        break;

    case 5: //KSI5
        gpio_set_level(KSI_SI1,ON);
        gpio_set_level(KSI_SI2,OFF);
        gpio_set_level(KSI_SI3,ON);
        ESP_LOGI(KSI_TAG,"CASE 5");
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        break;
    case 6: //KSI6
        gpio_set_level(KSI_SI1,OFF);
        gpio_set_level(KSI_SI2,ON);
        gpio_set_level(KSI_SI3,ON);
        ESP_LOGI(KSI_TAG,"CASE 6");
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        break;
    case 7: //KSI7
        gpio_set_level(KSI_SI1, ON);
        gpio_set_level(KSI_SI2, ON);
        gpio_set_level(KSI_SI3, ON);
        ESP_LOGI(KSI_TAG,"CASE 7");
        gpio_set_level(ENABLE_KSI_IC3, SELECTED);
        break;

    default:
        ESP_LOGI(KSI_TAG,"ERROR CASE DEFAULT");
        gpio_set_level(ENABLE_KSI_IC3, DESELECTED);
        return ESP_FAIL;        
    }
    return ESP_OK;
}

esp_err_t kso_select(uint8_t kso)
{
    vTaskDelay(WAIT);
    // KSI3 é o mais significativo 

    switch (kso%8)
    {
    case 0: //KSO0
        gpio_set_level(KSO_SO1,OFF);
        gpio_set_level(KSO_SO2,OFF);
        gpio_set_level(KSO_SO3,OFF);
        ESP_LOGI(KSO_TAG,"CASE 0");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;
    case 1: //KSO1
        gpio_set_level(KSO_SO1,ON);
        gpio_set_level(KSO_SO2,OFF);
        gpio_set_level(KSO_SO3,OFF);
        ESP_LOGI(KSO_TAG,"CASE 1");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;
    case 2: //KSO2
        gpio_set_level(KSO_SO1,OFF);
        gpio_set_level(KSO_SO2,ON);
        gpio_set_level(KSO_SO3,OFF);
        ESP_LOGI(KSO_TAG,"CASE 2");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;
    case 3: //KSO3    
        gpio_set_level(KSO_SO1,ON);
        gpio_set_level(KSO_SO2,ON);
        gpio_set_level(KSO_SO3,OFF);
        ESP_LOGI(KSO_TAG,"CASE 3");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;
    case 4: //KSO4
        gpio_set_level(KSO_SO1,OFF);
        gpio_set_level(KSO_SO2,OFF);
        gpio_set_level(KSO_SO3,ON);
        ESP_LOGI(KSO_TAG,"CASE 4");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;
    case 5: //KSO5
        gpio_set_level(KSO_SO1,ON);
        gpio_set_level(KSO_SO2,OFF);
        gpio_set_level(KSO_SO3,ON);
        ESP_LOGI(KSO_TAG,"CASE 5");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;
    case 6: //KSO6
        gpio_set_level(KSO_SO1,OFF);
        gpio_set_level(KSO_SO2,ON);
        gpio_set_level(KSO_SO3,ON);
        ESP_LOGI(KSO_TAG,"CASE 6");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;
    case 7: //KSO7
        gpio_set_level(KSO_SO1, ON);
        gpio_set_level(KSO_SO2, ON);
        gpio_set_level(KSO_SO3, ON);
        ESP_LOGI(KSO_TAG,"CASE 7");
        kso_select_ic(kso, SELECTED);
        disable_all_kso(kso);
        break;

    default:
        ESP_LOGI(KSO_TAG,"ERROR CASE DEFAULT");
        kso_select_ic(kso, DESELECTED);
        return ESP_FAIL;        
    }

    return ESP_OK;
}

esp_err_t press_single(uint8_t ksi, uint8_t kso)
{
    int status_ksi = 1;
    int status_kso = 1;

    if(ksi <= 7){
        status_ksi = ksi_select(ksi);
        if (status_ksi != ESP_OK){
            ESP_LOGE(KSI_TAG,"ERROR KSI");
            return ESP_FAIL;
        }
    }

    if(kso <= 23){
        status_kso = kso_select(kso);
        if (status_kso != ESP_OK){
            ESP_LOGE(KSO_TAG,"ERROR KSO");
            return ESP_FAIL;
        }
    }
    // vTaskDelay(DELAY_PRESS); // Tempo em que a tecla tá pressionada
    disable_all_ksi();
    disable_all_kso(kso);
    
    if(status_ksi != 0 || status_kso != 0){
        ESP_LOGE("[KSI/KSO]","ERROR KSI/KSO");
        return ESP_FAIL;
    }

    return ESP_OK;
}

// Função para habilitar ou desabilitar a tecla FN (HIGH ou LOW)
esp_err_t fn_status_level(uint8_t mode)
{
    gpio_set_level(FN_CTRL, mode);
    vTaskDelay(WAIT);
    return ESP_OK;
}

esp_err_t press_single_fn_mode(uint8_t ksi, uint8_t kso, bool status_fn)
{
    esp_err_t check_keys;
    if(status_fn){
        vTaskDelay(DELAY_FN);
        fn_status_level(FN_HIGH);
        vTaskDelay(DELAY_FN);
    }

    check_keys = press_single(ksi, kso);
    fn_status_level(FN_LOW);

    if(check_keys != ESP_OK){
        ESP_LOGE("[KSI/KSO]","ERROR KSI/KSO");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t press_all(uint8_t *keyboard_matrix, bool status_fn) 
{
    uint16_t len_matrix = strlen((char*)keyboard_matrix);
    char str_ksi[3] = {0};
    char str_kso[3] = {0};
    char temp[3] = {0};

    for (int x = 0; x < len_matrix; ) {
        memset(str_ksi, 0, sizeof(str_ksi));
        memset(str_kso, 0, sizeof(str_kso));
        memset(temp, 0, sizeof(temp));

        for (;;) {
            if (keyboard_matrix[x] == ';') {
                x++;
                break;
            }
            snprintf(temp, sizeof(temp), "%c", keyboard_matrix[x]);
            strncat(str_ksi, temp, sizeof(str_ksi) - strlen(str_ksi) - 1);
            x++;
        }

        for (;;) {
            if (keyboard_matrix[x] == ';') {
                x++;
                break;
            }
            snprintf(temp, sizeof(temp), "%c", keyboard_matrix[x]);
            strncat(str_kso, temp, sizeof(str_kso) - strlen(str_kso) - 1);
            x++;
        }
        int ksi = atoi(str_ksi);
        int kso = atoi(str_kso); 
        press_single_fn_mode(ksi, kso, status_fn);
        // printf("%s %s\n", str_ksi, str_kso);
    }
    return ESP_OK;
}

