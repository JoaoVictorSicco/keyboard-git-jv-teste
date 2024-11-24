#ifndef COMMUNICATION_H
#define COMMUNICATION_H

    #include "driver/gpio.h"
    #include "driver/uart.h"
    #include "esp_log.h"
    #include "tinyusb.h"
    #include "tusb_cdc_acm.h"
    #include "defines.h"
    #include "tusb_config.h"

    char msg_rx[SIZE_BUFFER];
    static const char *TAG = "[KEY]";

    extern QueueHandle_t q_send;
    extern QueueHandle_t q_data;
    extern TaskHandle_t handle_transmit_task;

    void init_communication(void);
    void transmit_msg_task (void *param);
    void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event);
    void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event);
    void init_tinyusb(void);
    void init_serial(void);

    char msg_rx[SIZE_BUFFER];

    static tusb_desc_device_t descriptor_config = {
    .bLength = sizeof(descriptor_config),
    .bDescriptorType = TUSB_DESC_DEVICE, 
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC, 
    .bDeviceSubClass = MISC_SUBCLASS_COMMON, 
    .bDeviceProtocol = MISC_PROTOCOL_IAD, 
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE, 
    .idVendor = 0x303A, //
    .idProduct = 0x2002, //
    .bcdDevice = 0x100, 
    .iManufacturer = 0x01, 
    .iProduct = 0x02, 
    .iSerialNumber = 0x03, 
    .bNumConfigurations = 0x01 
    };

    static char const *string_desc_arr[] = {
        (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
        "TinyUSB",                      // 1: Manufacturer
        "TinyUSB Device",               // 2: Product
        "123456",                       // 3: Serials
        "Example",                  // 4. MSC
    };
#endif