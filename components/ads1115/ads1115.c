
#include "ads1115.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "esp_log.h"

/* Escolhe o pino A0, A1, S2 ou A3*/
ads1115_mux_t A0 = ADS1115_MUX_0_GND;
ads1115_mux_t A1 = ADS1115_MUX_1_GND;
ads1115_mux_t A2 = ADS1115_MUX_2_GND;
ads1115_mux_t A3 = ADS1115_MUX_3_GND;

double valor;
double tensao_backlight;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
  const bool ret = 1; // dummy value to pass to queue
  QueueHandle_t gpio_evt_queue = (QueueHandle_t) arg; // find which queue to write
  // xQueueHandle gpio_evt_queue = (xQueueHandle) arg; // find which queue to write
  xQueueSendFromISR(gpio_evt_queue, &ret, NULL);
}

static esp_err_t ads1115_write_register(ads1115_t* ads, ads1115_register_addresses_t reg, uint16_t data) {
  i2c_cmd_handle_t cmd;
  esp_err_t ret;
  uint8_t out[2];

  out[0] = data >> 8; // get 8 greater bits
  out[1] = data & 0xFF; // get 8 lower bits
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd); // generate a start command
  i2c_master_write_byte(cmd,(ads->address<<1) | I2C_MASTER_WRITE,1); // specify address and write command
  i2c_master_write_byte(cmd,reg,1); // specify register
  i2c_master_write(cmd,out,2,1); // write it
  i2c_master_stop(cmd); // generate a stop command
  ret = i2c_master_cmd_begin(ads->i2c_port, cmd, ads->max_ticks); // send the i2c command
  i2c_cmd_link_delete(cmd);
  ads->last_reg = reg; // change the internally saved register
  return ret;
}

static esp_err_t ads1115_read_register(ads1115_t* ads, ads1115_register_addresses_t reg, uint8_t* data, uint8_t len) {
  i2c_cmd_handle_t cmd;
  esp_err_t ret;

  if(ads->last_reg != reg) { // if we're not on the correct register, change it
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,(ads->address<<1) | I2C_MASTER_WRITE,1);
    i2c_master_write_byte(cmd,reg,1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(ads->i2c_port, cmd, ads->max_ticks);
    i2c_cmd_link_delete(cmd);
    ads->last_reg = reg;
  }
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd); // generate start command
  i2c_master_write_byte(cmd,(ads->address<<1) | I2C_MASTER_READ,1); // specify address and read command
  i2c_master_read(cmd, data, len, 0); // read all wanted data
  i2c_master_stop(cmd); // generate stop command
  ret = i2c_master_cmd_begin(ads->i2c_port, cmd, ads->max_ticks); // send the i2c command
  i2c_cmd_link_delete(cmd);
  return ret;
}

ads1115_t ads1115_config(i2c_port_t i2c_port, uint8_t address) {
  ads1115_t ads; // setup configuration with default values
  ads.config.bit.OS = 1; // always start conversion
  ads.config.bit.MUX = ADS1115_MUX_0_GND;
  ads.config.bit.PGA = ADS1115_FSR_4_096;
  ads.config.bit.MODE = ADS1115_MODE_SINGLE;
  ads.config.bit.DR = ADS1115_SPS_64;
  ads.config.bit.COMP_MODE = 0;
  ads.config.bit.COMP_POL = 0;
  ads.config.bit.COMP_LAT = 0;
  ads.config.bit.COMP_QUE = 0b11;

  ads.i2c_port = i2c_port; // save i2c port
  ads.address = address; // save i2c address
  ads.rdy_pin.in_use = 0; // state that rdy_pin not used
  ads.last_reg = ADS1115_MAX_REGISTER_ADDR; // say that we accessed invalid register last
  ads.changed = 1; // say we changed the configuration
  ads.max_ticks = 100/portTICK_PERIOD_MS;
  return ads; // return the completed configuration
}

void ads1115_set_mux(ads1115_t* ads, ads1115_mux_t mux) {
  ads->config.bit.MUX = mux;
  ads->changed = 1;
}

void ads1115_set_rdy_pin(ads1115_t* ads, gpio_num_t gpio) {
  const static char* TAG = "ads1115_set_rdy_pin";
  gpio_config_t io_conf;
  esp_err_t err;

  io_conf.intr_type = GPIO_INTR_NEGEDGE; // positive to negative (pulled down)
  io_conf.pin_bit_mask = 1<<gpio;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = 1;
  io_conf.pull_down_en = 0;
  gpio_config(&io_conf); // set gpio configuration

  ads->rdy_pin.gpio_evt_queue = xQueueCreate(1, sizeof(bool));
  gpio_install_isr_service(0);

  ads->rdy_pin.in_use = 1;
  ads->rdy_pin.pin = gpio;
  ads->config.bit.COMP_QUE = 0b00; // assert after one conversion
  ads->changed = 1;

  err = ads1115_write_register(ads, ADS1115_LO_THRESH_REGISTER_ADDR,0); // set lo threshold to minimum
  if(err) ESP_LOGE(TAG,"could not set low threshold: %s",esp_err_to_name(err));
  err = ads1115_write_register(ads, ADS1115_HI_THRESH_REGISTER_ADDR,0xFFFF); // set hi threshold to maximum
  if(err) ESP_LOGE(TAG,"could not set high threshold: %s",esp_err_to_name(err));
}

void ads1115_set_pga(ads1115_t* ads, ads1115_fsr_t fsr) {
  ads->config.bit.PGA = fsr;
  ads->changed = 1;
}

void ads1115_set_mode(ads1115_t* ads, ads1115_mode_t mode) {
  ads->config.bit.MODE = mode;
  ads->changed = 1;
}

void ads1115_set_sps(ads1115_t* ads, ads1115_sps_t sps) {
  ads->config.bit.DR = sps;
  ads->changed = 1;
}

void ads1115_set_max_ticks(ads1115_t* ads, TickType_t max_ticks) {
  ads->max_ticks = max_ticks;
}

int16_t ads1115_get_raw(ads1115_t* ads) {
  const static char* TAG = "ads1115_get_raw";
  const static uint16_t sps[] = {8,16,32,64,128,250,475,860};
  const static uint8_t len = 2;
  uint8_t data[2];
  esp_err_t err;
  bool tmp; // temporary bool for reading from queue

  if(ads->rdy_pin.in_use) {
    gpio_isr_handler_add(ads->rdy_pin.pin, gpio_isr_handler, (void*)ads->rdy_pin.gpio_evt_queue);
    xQueueReset(ads->rdy_pin.gpio_evt_queue);
  }
  // see if we need to send configuration data
  if((ads->config.bit.MODE==ADS1115_MODE_SINGLE) || (ads->changed)) { // if it's single-ended or a setting changed
    err = ads1115_write_register(ads, ADS1115_CONFIG_REGISTER_ADDR, ads->config.reg);
    if(err) {
      ESP_LOGE(TAG,"could not write to device: %s",esp_err_to_name(err));
      if(ads->rdy_pin.in_use) {
        gpio_isr_handler_remove(ads->rdy_pin.pin);
        xQueueReset(ads->rdy_pin.gpio_evt_queue);
      }
      return 0;
    }
    ads->changed = 0; // say that the data is unchanged now
  }

  if(ads->rdy_pin.in_use) {
    xQueueReceive(ads->rdy_pin.gpio_evt_queue, &tmp, portMAX_DELAY);
    gpio_isr_handler_remove(ads->rdy_pin.pin);
  }
  else {
    // wait for 1 ms longer than the sampling rate, plus a little bit for rounding
    vTaskDelay((((1000/sps[ads->config.bit.DR]) + 1) / portTICK_PERIOD_MS)+1);
  }

  err = ads1115_read_register(ads, ADS1115_CONVERSION_REGISTER_ADDR, data, len);
  if(err) {
    ESP_LOGE(TAG,"could not read from device: %s",esp_err_to_name(err));
    return 0;
  }
  return ((uint16_t)data[0] << 8) | (uint16_t)data[1];
}

double ads1115_get_voltage(ads1115_t* ads) {
  const double fsr[] = {6.144, 4.096, 2.048, 1.024, 0.512, 0.256};
  const int16_t bits = (1L<<15)-1;
  int16_t raw;

  raw = ads1115_get_raw(ads);
  return (double)raw * fsr[ads->config.bit.PGA] / (double)bits;
}

esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);

    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void read_voltage_task(void *pvParameter) {

    ads1115_t my_ads = ads1115_config(I2C_MASTER_NUM, 0x48); // Inicialize o ADS1115
    // ads1115_set_mux(&my_ads, A3); // Seleciona A0, a1, A2 ou A3 em relação ao GND
    ads1115_set_mux(&my_ads, A0);


    while (1) {
        // valor = 2*ads1115_get_voltage(&my_ads);
        //printf("Valor capslock: %.2lf V\n", valor);
        tensao_backlight = 2*ads1115_get_voltage((&my_ads));
        // printf("Valor capslock: %.2lf V\n", valor);
        printf("Valor backlight: %.2lf V\n", tensao_backlight);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo
    }
}

void keyled_main()
{
    esp_err_t err = i2c_master_init();
    if (err != ESP_OK) {
        printf("I2C init failed: %s\n", esp_err_to_name(err));
        return;
    }
    xTaskCreate(read_voltage_task, "read_voltage_task", 4096, NULL, 25, NULL);
}




