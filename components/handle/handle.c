/* //JSON MODELO USADA COMO BASE
{
    "action": "string",
    "time_out": 0.0,
    "payload":
    {
        "dev": "string",
        "id": "string",
        "data":
        {
            "cmd":"string",
            "keyboard_matrix":
                [{"ksi":2,"kso":5, "fn":0},
                {"ksi":1,"kso":20, "fn":1}],
            "led_matrix":[{"key":"string", "pin":2, "voltage":0.0},
                        {"key":"string", "pin":3, "voltage":0.0}],
            "backlight":
            {
                "type":"string",
                "load":0.5,
                "bl_matrix": [
                    {"zone":3, "pin":0, "voltage":1.1},
                    {"zone":1, "pin":0, "voltage":1.1}
                    ]
            }
        }
    }
}

*/
#include "handle.h"
#include "ads1115.h"

#define THRESHOLD_TENSAO 1.0

TaskHandle_t handle_receive_task = NULL;

extern ads1115_mux_t A0;
extern ads1115_mux_t A1;
extern ads1115_mux_t A2;
extern ads1115_mux_t A3;
extern double valor;

void init_handle(void)
{
    xTaskCreatePinnedToCore(receive_msg_task, "receive_test_task", 2*4096, NULL, 10, &handle_receive_task, 1);
}

void receive_msg_task(void *param)
{
    char msg_receive[SIZE_BUFFER];
    while (true)
    {
        xQueueReceive(q_data, &(msg_receive), portMAX_DELAY);
        ESP_LOGI(TAG, "Msg recebida na fila: %s", msg_receive);
        // transforma a string em JSON
        cJSON *request_cmd = cJSON_Parse(msg_receive);
        if (request_cmd != NULL)
        {
            // Verifica se é um JSON válido
            cJSON *action = cJSON_GetObjectItem(request_cmd, "action");
            cJSON *payload = cJSON_GetObjectItem(request_cmd, "payload");
            if (payload == NULL || action == NULL)
            {
                send_data(ERRO, ERR_JSON, NULL, NULL);
            }
            // Verifica se é dispositivo válido
            else
            {
                payload_handle(action->valuestring, payload);
                cJSON_Delete(request_cmd);
            }
        }
        else if (request_cmd == NULL)
        {
            send_data(ERRO, ERR_JSON, NULL, NULL);
        }
    }
}

void payload_handle(char *action, cJSON *payload)
{
    cJSON *dev = cJSON_GetObjectItem(payload, "dev");
    cJSON *id_json = cJSON_GetObjectItem(payload, "id");

    int status_action = ESP_FAIL;

    if (dev == NULL)
        status_action = ESP_FAIL;

    else if (strcmp(dev->valuestring, "keyboard_scan"))
    {
        send_data(ERRO, ERR_DEVICE, NULL, NULL);
        status_action = ESP_OK;
    }
    else if (!strcmp(action, "post"))
    {
        cJSON *data = cJSON_GetObjectItem(payload, "data");
        if (data == NULL || id_json == NULL)
        {
            send_data(ERRO, ERR_JSON, NULL, NULL);
        }
        else if (!strcmp(id_json->valuestring, id))
        {
            ESP_LOGI(TAG, "Chamou o Action");
            strcpy(id_show, id_json->valuestring);
            status_action = action_post(data);
        }
        else
        {
            printf("ID incorreto\n");
            strcpy(id_show, id_json->valuestring);
            send_data(ERRO, ERR_ID, NULL, NULL);
            return;
        }
    }
    else if (!strcmp(action, "get"))
    {
        status_action = action_get();
    }
    else if (!strcmp(action, "record_id"))
    {
        status_action = action_record(id_json->valuestring);
    }
    if (status_action != ESP_OK)
    {
        if (status_action == ESP_FAIL)
        {
            send_data(ERRO, ERR_JSON, NULL, NULL);
            ESP_LOGE(TAG, "JSON INVALID.");
        }
        else
        {
            send_data(ERRO, ERR_ACTION, NULL, NULL);
            ESP_LOGE(TAG, "ACTION_ERRO");
        }
    }
}

int action_get(void)
{
    read_flash(KEY_ID, id);
    send_data(REPLY, NULL, NULL, NULL);
    return ESP_OK;
}

int action_record(char *id_json)
{
    int d1, d2, d3;
    if (id_json == NULL)
        return ESP_FAIL;

    // VERIFICAÇÃO SE O ID TEM 6 CARACTERES, É DIFERENTE DO ANTERIOR E ESTÁ NO PADRAO "kb-" + "000" 3 inteiros aleatorios
    if (strlen(id_json) == 6 && strcmp(id_json, id) &&
        sscanf(id_json, ID_VERIFY, &d1, &d2, &d3) == 3)
    {
        ESP_LOGI(TAG, "NOVO ID ESCRITO");
        write_flash(KEY_ID, id_json);
        strcpy(id_show, id_json);
    }
    else
    {
        ESP_LOGE(TAG, "ID FORA DO PADRÃO");
        strcpy(id_show, id_json);
        send_data(ERRO, ERR_ID, NULL, NULL);
        return ESP_FAIL;
    }
    send_data(REPLY, NULL, NULL, NULL);
    return ESP_OK;
}

int action_post(cJSON *data)
{
    char *teste = cJSON_PrintUnformatted(data);
    printf("%s\n", teste);
    // recebe o tipo de comando
    cJSON *cmd_json = cJSON_GetObjectItem(data, "cmd");
    // valida o comando
    if (cmd_json == NULL)
        return ESP_FAIL;

    const char *cmd = cmd_json->valuestring;
    // comando para chavear um periférico
    if (!strcmp(cmd, "all_keys"))
    {
        cJSON *keyboard_test = cJSON_GetObjectItem(data, "keyboard_test");
        cJSON *kb_matrix = cJSON_GetObjectItem(keyboard_test, "keyboard_matrix");
        cJSON *delay_press = cJSON_GetObjectItem(keyboard_test, "delay_press");
        press_keys(kb_matrix, delay_press); // Se eu não quiser mandar nada eu coloco NULL do segundo parametro
        
        ESP_LOGI(TAG, "ALL KEYS EXECUTADO");
        send_data(REPLY, NULL, NULL, NULL);
        return ESP_OK;
    }
    else if (!strcmp(cmd, "single_key"))
    {
        cJSON *kb_matrix = cJSON_GetObjectItem(data, "keyboard_matrix");

        if (kb_matrix == NULL)
            return ESP_FAIL;

        cJSON *ksi_js = cJSON_GetObjectItem(kb_matrix, "ksi");
        cJSON *kso_js = cJSON_GetObjectItem(kb_matrix, "kso");
        cJSON *fn_js = cJSON_GetObjectItem(kb_matrix, "fn");

        if (ksi_js == NULL || kso_js == NULL || fn_js == NULL)
            return ESP_FAIL;

        ESP_LOGI(TAG, "SINGLE KEY EXECUTADO");
        press_single_fn_mode(ksi_js->valueint, kso_js->valueint, fn_js->valueint);
    }
    else if (!strcmp(cmd, "key_led"))
    {
        
        cJSON *keyboard_test = cJSON_GetObjectItem(data, "keyboard_test");
        if (keyboard_test == NULL)
        return ESP_FAIL;

        // Obtém o delay de pressionamento configurável
        cJSON *delay_press = cJSON_GetObjectItem(keyboard_test, "delay_press");
        if(delay_press == NULL)
            return ESP_FAIL;

        cJSON *kb_matrix = cJSON_GetObjectItem(keyboard_test, "keyboard_matrix");
        if(kb_matrix == NULL)
            return ESP_FAIL;

        // cJSON *kb_matrix = cJSON_GetObjectItem(data, "keyboard_matrix");
        // press_keys(kb_matrix);

        cJSON *led_matrix = cJSON_GetObjectItem(data, "led_matrix");
        if (led_matrix == NULL)
            return ESP_FAIL;

        int n_size = cJSON_GetArraySize(led_matrix);
        if (n_size == 0)
            return ESP_FAIL;

        // VERIFICA TODOS OS VALORES DENTRO DO VETOR, SE ALGUM DELES ESTIVER ERRADO, NAO EXECUTA NENHUMA TECLA
        for (int i = 0; i < n_size; i++)
        {
            cJSON *pin = cJSON_GetObjectItem(cJSON_GetArrayItem(led_matrix, i), "pin");
            cJSON *voltage = cJSON_GetObjectItem(cJSON_GetArrayItem(led_matrix, i), "voltage");

            if (pin == NULL || voltage == NULL)
                return ESP_FAIL;
        }

        // O inicio tem que ser feito aqui.

        // PASSA NOVAMENTE PELO ARRAY EXECUTANDO AS TECLAS CASO TODAS ESTEJAM CORRETAS NO JSON
        ads1115_t my_ads = ads1115_config(I2C_MASTER_NUM, 0x48); // Inicialize o ADS1115

        // Verificação inicial da tensão do capslock
        for(int i=0; i < n_size; i++)
        {
            cJSON *pin = cJSON_GetObjectItem(cJSON_GetArrayItem(led_matrix, i), "pin");
            cJSON *voltage = cJSON_GetObjectItem(cJSON_GetArrayItem(led_matrix, i), "voltage");

            if(pin->valueint == 1) 
            { 
                // Adicionar múltiplas leituras para maior confiabilidade
                double initial_voltage = 0;
                const int NUM_READINGS = 5;
                
                for(int j = 0; j < NUM_READINGS; j++) {
                    ads1115_set_mux(&my_ads, A1);
                    initial_voltage += 2*ads1115_get_voltage(&my_ads);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                initial_voltage /= NUM_READINGS;
                
                printf("Tensao inicial do Capslock: %.2f V\n", initial_voltage);
                if(initial_voltage > THRESHOLD_TENSAO) 
                {
                    printf("Tensao acima do Threshold, vou desligar\n");
                    press_keys(kb_matrix, delay_press);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    
                    // Verificar se realmente desligou
                    double check_voltage = 0;
                    for(int j = 0; j < NUM_READINGS; j++) {
                        ads1115_set_mux(&my_ads, A1);
                        check_voltage += 2*ads1115_get_voltage(&my_ads);
                        vTaskDelay(pdMS_TO_TICKS(100));
                    }
                    check_voltage /= NUM_READINGS;
                    
                    if(check_voltage > THRESHOLD_TENSAO) {
                        printf("Falha ao desligar Capslock. Tensao: %.2f V\n", check_voltage);
                        // Tentar desligar novamente ou retornar erro
                    }
                }
            }

            // Aqui é para o F4
            else if(pin->valueint == 2) 
            { 
                // Adicionar múltiplas leituras para maior confiabilidade
                double initial_voltage = 0;
                const int NUM_READINGS = 5;
                
                for(int j = 0; j < NUM_READINGS; j++) {
                    ads1115_set_mux(&my_ads, A2);
                    initial_voltage += 2*ads1115_get_voltage(&my_ads);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                initial_voltage /= NUM_READINGS;
                
                printf("Tensao inicial do F4: %.2f V\n", initial_voltage);
                if(initial_voltage > THRESHOLD_TENSAO) 
                {
                    printf("Tensao F4 acima do Threshold, vou desligar\n");
                    press_keys(kb_matrix, delay_press);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    
                    // Verificar se realmente desligou
                    double check_voltage = 0;
                    for(int j = 0; j < NUM_READINGS; j++) {
                        ads1115_set_mux(&my_ads, A2);
                        check_voltage += 2*ads1115_get_voltage(&my_ads);
                        vTaskDelay(pdMS_TO_TICKS(100));
                    }
                    check_voltage /= NUM_READINGS;
                    
                    if(check_voltage > THRESHOLD_TENSAO) {
                        printf("Falha ao desligar F4. Tensao: %.2f V\n", check_voltage);
                        // Tentar desligar novamente ou retornar erro
                    }
                }
            }
        }

        // Executa a sequencia normal
        printf("Iniciando a rotina normal, vou pressionar a tecla\n");
        //TODO: Testar todos os press_keys(colocando NULL no segundo parametro)
        press_keys(kb_matrix, delay_press); // Point 2
        vTaskDelay(pdMS_TO_TICKS(100));

        // Leitura das tensões
        for (int i = 0; i < n_size; i++)
        {
            cJSON *pin = cJSON_GetObjectItem(cJSON_GetArrayItem(led_matrix, i), "pin");
            cJSON *voltage = cJSON_GetObjectItem(cJSON_GetArrayItem(led_matrix, i), "voltage");

            if(pin->valueint == 1)
                ads1115_set_mux(&my_ads, A1); // Seleciona A0, a1, A2 ou A3 em relação ao GND
            if(pin->valueint == 2)
                ads1115_set_mux(&my_ads, A2); // Seleciona A0, a1, A2 ou A3 em relação ao GND
            if(pin->valueint == 3)
                ads1115_set_mux(&my_ads, A3); // Seleciona A0, a1, A2 ou A3 em relação ao GND

            voltage->valuedouble = 2*ads1115_get_voltage(&my_ads);
        }

        // vTaskDelay(pdMS_TO_TICKS(100)); 
        send_data(REPLY, NULL, NULL, data);
        printf("Agora vou desativar a tecla\n");
        printf("Antes de desativar o capslock\n");
        press_keys(kb_matrix, delay_press);  // Desativar o capslock no final // Point 3
        printf("Desativou o capslock\n");
        return ESP_OK;
       
    }
    else if (!strcmp(cmd, "backlight"))
    {
        cJSON *backlight = cJSON_GetObjectItem(data, "backlight");
        if (backlight == NULL)
            return ESP_FAIL;
        cJSON *load_current = cJSON_GetObjectItem(backlight, "load_current");
        cJSON *vcc = cJSON_GetObjectItem(backlight, "vcc");
        cJSON *threshold_min_json = cJSON_GetObjectItem(backlight, "threshold_min"); // TODO: Verificar threshold


        float Refetivo = vcc->valuedouble / (load_current->valuedouble + 0.00001);
        float Rcarga = Refetivo * RAB / (RAB - Refetivo);
        int N = 63 - (R11 * 63) / Rcarga;
        up_down_digipot(UP, MAX);
        vTaskDelay(pdMS_TO_TICKS(100));
        up_down_digipot(DOWN, MAX-N);
        printf("valor N: %d, valor para descer %d\n",N, MAX-N);


        double voltage = 0;
        cJSON *ksi_js = cJSON_GetObjectItem(backlight, "ksi");
        cJSON *kso_js = cJSON_GetObjectItem(backlight, "kso");
        cJSON *fn_js = cJSON_GetObjectItem(backlight, "fn");
        cJSON *voltages_array = cJSON_GetObjectItem(backlight, "voltages");

        ads1115_t my_ads = ads1115_config(I2C_MASTER_NUM, 0x48); // Inicialize o ADS1115
        float threshold_min = threshold_min_json->valuedouble; // TODO: Verificar threshold
        for(int i=0; i<5; i++){
            press_single_fn_mode(ksi_js->valueint, kso_js->valueint, fn_js->valueint);
            vTaskDelay(pdMS_TO_TICKS(1000));
            voltage = 2*ads1115_get_voltage(&my_ads);
            if(voltage < threshold_min) // TODO: Verificar threshold
                break;
        }
        for(int i=0; i<5; i++){
            cJSON *voltage_json = cJSON_CreateNumber(voltage);
            cJSON_AddItemToArray(voltages_array, voltage_json);
            press_single_fn_mode(ksi_js->valueint, kso_js->valueint, fn_js->valueint);
            vTaskDelay(pdMS_TO_TICKS(2000));
            voltage = 2*ads1115_get_voltage(&my_ads);
            if(voltage < threshold_min) // TODO: Verificar threshold
                break;
        }

        // vTaskDelay(50); // TODO Verificar a necessidade desse delay e se dá pra diminuir
        send_data(REPLY, NULL, NULL, data);
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "COMANDO INVALIDO");
        return ESP_FAIL;
    }
    send_data(REPLY, NULL, NULL, data);
    return ESP_OK;
}

esp_err_t send_data(enum action_type action, char *erro, char *event, cJSON *data)
{
    // Cria o JSON data em caso de erro e evento
    if (action == ERRO || action == EVENT)
    {
        cJSON *cmd = cJSON_CreateObject();
        switch ((int)action)
        {
        case ERRO:
            cJSON_AddStringToObject(cmd, "cmd", erro);
            transmit_payload(ACT_ERRO, cmd);
            break;
        case EVENT:
            cJSON_AddStringToObject(cmd, "cmd", event);
            transmit_payload(ACT_EVENT, cmd);
        }
        cJSON_Delete(cmd);
    }
    else if (action == REPLY)
    {
        // Em caso de resposta reaproveita o JSON data da action
        transmit_payload(ACT_REPLY, data);
    }
    else
    {
        ESP_LOGE(TAG, "ACTION INVALIDA");
    }
    return ESP_OK;
}

esp_err_t transmit_payload(char *action, cJSON *data)
{
    // Criando o header default do JSON: action, payload, dev, id
    cJSON *response_cmd = cJSON_CreateObject();
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(response_cmd, "action", action);
    cJSON_AddItemToObject(response_cmd, "payload", payload);
    cJSON_AddStringToObject(payload, "dev", DEVICE);
    cJSON_AddStringToObject(payload, "id", id);
    char *string_data;

    // copia do JSON data
    if(data != NULL){
        string_data = cJSON_PrintUnformatted(data);
        cJSON *data_reply = cJSON_Parse(string_data);

        // aloca o JSON data no payload
        cJSON_AddItemToObject(payload, "data", data_reply);
    }
    // transforma para string
    char *s_reply = cJSON_PrintUnformatted(response_cmd);
    ESP_LOGI(TAG, "OUTPUT: %s", s_reply);

    // envia a string para fila
    xQueueSend(q_send, (void *)s_reply, portMAX_DELAY);

    // desaloca memoria
    free(s_reply);
    if(data != NULL)
        free(string_data);

    cJSON_Delete(response_cmd);
    return ESP_OK;
}

cJSON* press_keys(cJSON *kb_matrix, cJSON *delay_press)
{
        if (kb_matrix == NULL)
            return NULL;

        int n_size = cJSON_GetArraySize(kb_matrix);
        if (n_size == 0)
            return NULL;

        // VERIFICA TODOS OS VALORES DENTRO DO VETOR, SE ALGUM DELES ESTIVER ERRADO, NAO EXECUTA NENHUMA TECLA
        for (int i = 0; i < n_size; i++)
        {
            cJSON *ksi_js = cJSON_GetObjectItem(cJSON_GetArrayItem(kb_matrix, i), "ksi");
            cJSON *kso_js = cJSON_GetObjectItem(cJSON_GetArrayItem(kb_matrix, i), "kso");
            cJSON *fn_js = cJSON_GetObjectItem(cJSON_GetArrayItem(kb_matrix, i), "fn");

            if (ksi_js == NULL || kso_js == NULL || fn_js == NULL)
                return NULL;
        }
        // PASSA NOVAMENTE PELO ARRAY EXECUTANDO AS TECLAS CASO TODAS ESTEJAM CORRETAS NO JSON
        for (int i = 0; i < n_size; i++)
        {
            cJSON *ksi_js = cJSON_GetObjectItem(cJSON_GetArrayItem(kb_matrix, i), "ksi");
            cJSON *kso_js = cJSON_GetObjectItem(cJSON_GetArrayItem(kb_matrix, i), "kso");
            cJSON *fn_js = cJSON_GetObjectItem(cJSON_GetArrayItem(kb_matrix, i), "fn");

            printf("ksi %d, kso %d, fn %d\n", ksi_js->valueint, kso_js->valueint, fn_js->valueint);
            press_single_fn_mode(ksi_js->valueint, kso_js->valueint, fn_js->valueint);

            
            //TODO: Colocar um if aqui. Se delay_press for NULL não executa esse delay
            if(delay_press != NULL)
            {
                vTaskDelay(pdMS_TO_TICKS(delay_press->valueint));
            }
            //TODO: Tentar colocar um delay aqui configurável pelo JSON
        }
        return kb_matrix;
}