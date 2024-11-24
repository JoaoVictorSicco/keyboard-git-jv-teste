#include "communication.h"

static uint8_t rxBytes[SIZE_BUFFER + 1];
int pos = 0;
QueueHandle_t q_data;
QueueHandle_t q_send;
TaskHandle_t handle_transmit_task = NULL;

void init_communication(void)
{
    q_data = xQueueCreate (5, sizeof(msg_rx));
    q_send = xQueueCreate (5, SIZE_BUFFER);
    xTaskCreatePinnedToCore(transmit_msg_task, "transmit_msg_task", 2*4096, NULL, 10, &handle_transmit_task, 1);
}

void transmit_msg_task(void *param)
{
    char msg_tx[SIZE_BUFFER];
    while(true)
    {
        xQueueReceive(q_send,&(msg_tx), portMAX_DELAY);
        ESP_LOGI(TAG, "OUTPUT FILA: %s", msg_tx);
        if(strlen(msg_tx)>CFG_TUD_CDC_TX_BUFSIZE)
        {
            char split_buffer[CFG_TUD_CDC_TX_BUFSIZE + 1];
            int pos = 0;
            size_t size_split_buffer = sizeof(split_buffer);
            for (int i=0; i< ((int)strlen(msg_tx)/CFG_TUD_CDC_TX_BUFSIZE); i++)
            {
                strncpy(split_buffer, (msg_tx + pos), size_split_buffer);
                split_buffer[size_split_buffer] = '\0';
                tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, (uint8_t *)split_buffer, size_split_buffer);
                pos = pos + CFG_TUD_CDC_TX_BUFSIZE;
                vTaskDelay(10);
            }
            strncpy(split_buffer, (msg_tx + pos), size_split_buffer);
            // ESP_LOGI(TAG, "tamanho: %d", strlen(split_buffer));
            split_buffer[strlen(split_buffer)] = '\n';
            split_buffer[strlen(split_buffer)] = '\0';
            tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, (uint8_t *)split_buffer, size_split_buffer);

        } else {
            msg_tx[strlen(msg_tx)] = '\n';
            tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, (uint8_t *)msg_tx, CFG_TUD_CDC_TX_BUFSIZE);
        }
    }
}

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(itf, rxBytes, CFG_TUD_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK) {
        for (int i = 0; i < rx_size; i++) {
            if (rxBytes[i] == '\n') {
                msg_rx[pos] = '\0';
                pos = 0;
                ESP_LOGI(TAG, "Mensagem recebida: %s", msg_rx);
                xQueueSend(q_data, (void*)msg_rx, portMAX_DELAY);
                break;
            }

            if (pos < SIZE_BUFFER - 1) {
                msg_rx[pos] = (char)rxBytes[i];
                pos++;
            } else {
                // Truncate the message if it exceeds the buffer size
                msg_rx[SIZE_BUFFER - 1] = '\0';
                ESP_LOGW(TAG, "Message truncated: %s", msg_rx);
                pos = 0;
            }
        }
    } else {
        ESP_LOGE(TAG, "Read error");
    }

}

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rst = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed! dtr:%d, rst:%d", dtr, rst);
}

void init_tinyusb(void)
{
    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .descriptor = &descriptor_config,
        .string_descriptor = string_desc_arr,
    }; // the configuration using default values
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 64,
        .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));
    ESP_LOGI(TAG, "USB initialization DONE");
}

