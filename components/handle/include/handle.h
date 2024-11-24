#ifndef HANDLE_H
#define HANDLE_H

    #include "cJSON.h"
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "communication.h"
    #include "keyboard.h"
    #include "flash.h"
    #include "defines.h"
    #include "mcp.h"

    extern TaskHandle_t handle_receive_task;
    //extern QueueHandle_t q_data;

    void init_handle(void);

    /**
    * @brief Task para monitorar nova mensagem
    */
    void receive_msg_task (void *param);

    /**
    * @brief Realizar alguma ação com base no comando enviado pela serial
    * @param data JSON com a informação da ação
    */
    int action_post(cJSON *data);

    /**
    * @brief Criar/Formatar JSON de resposta
    * @param action tipo de comando: resposta, evento, erro
    * @param erro tipo de erro: erro na mensagem ou serial
    * @param event tipo de evento
    * @param data JSON em caso de action reply
    */
    esp_err_t send_data(enum action_type action, char* erro, char* event, cJSON *data);
       
       
    /**
    * @brief Transmitir JSON
    * @param data JSON com a informação da ação
    */
    esp_err_t transmit_payload(char *action, cJSON* data);

    /**
    * @brief Escrever no JSON da FLASH
    * @param id_name Chave principal do JSON
    * @param value_name valor que ficará na Chave do JSON
    */
    esp_err_t write_flash_json(char *id_name, char* value_name);
    uint8_t init_flash();
    void WriteFlash(char key[], char data[]);
    uint8_t ReadFlash(char key[], char value[]);
    void payload_handle(char* action, cJSON *payload);
    int action_get(void);
    int action_record(char *id_json);
    cJSON* press_keys(cJSON *data);


#endif