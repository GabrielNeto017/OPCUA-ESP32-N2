#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "rc522_2.h"

static const char* TAG_2 = "ESP-RC522_2";

struct rc522_2 {
    bool running;
    rc522_config_t_2* config;
    spi_device_handle_t spi_2;
    TaskHandle_t task_handle;
    bool scan_started;
    bool tag_was_present_last_time;
};

typedef struct rc522_2* rc522_handle_t_2;

static rc522_handle_t_2 hndl = NULL;

#define rc522_fw_version() rc522_read(0x37)

bool rc522_is_inited_2() {
    return hndl != NULL;
}

static esp_err_t rc522_spi_init() {

     
    if(! hndl || ! hndl->config) {
        ESP_LOGE(TAG_2, "Fail to init SPI. Invalid handle");
        return ESP_ERR_INVALID_STATE;
    }

    if(hndl->spi_2) {
        ESP_LOGW(TAG_2, "SPI already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    spi_bus_config_t buscfg = {
        .miso_io_num = hndl->config->miso_io_2,
        .mosi_io_num = hndl->config->mosi_io_2,
        .sclk_io_num = hndl->config->sck_io_2,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 5000000,
        .mode = 0,
        .spics_io_num = hndl->config->sda_io_2,
        .queue_size = 7,
        .flags = SPI_DEVICE_HALFDUPLEX
    };

    esp_err_t err = spi_bus_initialize(hndl->config->spi_host_id_2, &buscfg, 0);
    

   if((err != ESP_OK) && err!=259) {
        return err;
   }

     err = spi_bus_add_device(hndl->config->spi_host_id_2, &devcfg, &hndl->spi_2);

    if(err != ESP_OK) {
        //spi_bus_free(hndl->config->spi_host_id_2);
        hndl->spi_2 = NULL;
        printf("entrei nesse cara aqui\n");
    }

    return err;
}

static esp_err_t rc522_write_n(uint8_t addr, uint8_t n, uint8_t *data) {
    uint8_t* buffer = (uint8_t*) malloc(n + 1);
    buffer[0] = (addr << 1) & 0x7E;

    for (uint8_t i = 1; i <= n; i++) {
        buffer[i] = data[i-1];
    }

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 8 * (n + 1);
    t.tx_buffer = buffer;

    esp_err_t ret = spi_device_transmit(hndl->spi_2, &t);

    free(buffer);

    return ret;
}

static esp_err_t rc522_write(uint8_t addr, uint8_t val) {
    return rc522_write_n(addr, 1, &val);
}

static uint8_t* rc522_read_n(uint8_t addr, uint8_t n) {
    if (n <= 0) {
        return NULL;
    }

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    uint8_t* buffer = (uint8_t*) malloc(n);
    
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;
    t.tx_data[0] = ((addr << 1) & 0x7E) | 0x80;
    t.rxlength = 8 * n;
    t.rx_buffer = buffer;

    esp_err_t ret = spi_device_transmit(hndl->spi_2, &t);
    assert(ret == ESP_OK);

    return buffer;
}

static uint8_t rc522_read(uint8_t addr) {
    uint8_t* buffer = rc522_read_n(addr, 1);
    uint8_t res = buffer[0];
    free(buffer);

    return res;
}

static esp_err_t rc522_set_bitmask(uint8_t addr, uint8_t mask) {
    return rc522_write(addr, rc522_read(addr) | mask);
}

static esp_err_t rc522_clear_bitmask(uint8_t addr, uint8_t mask) {
    return rc522_write(addr, rc522_read(addr) & ~mask);
}

static esp_err_t rc522_antenna_on() {
    esp_err_t ret;

    if(~ (rc522_read(0x14) & 0x03)) {
        ret = rc522_set_bitmask(0x14, 0x03);

        if(ret != ESP_OK) {
            return ret;
        }
    }

    return rc522_write(0x26, 0x60); // 43dB gain
}

static void rc522_task(void* arg);

esp_err_t rc522_init_2(rc522_config_t_2* config) {
    if(! config) {
        return ESP_ERR_INVALID_ARG;
    }

    if(hndl) {
        ESP_LOGW(TAG_2, "Already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if(! (hndl = calloc(1, sizeof(struct rc522_2)))) {
        return ESP_ERR_NO_MEM;
    }

    if(! (hndl->config = calloc(1, sizeof(rc522_config_t_2)))) {
        rc522_destroy_2();
        return ESP_ERR_NO_MEM;
    }

    // copy config considering defaults
    hndl->config->callback_2         = config->callback_2;
    hndl->config->miso_io_2          = config->miso_io_2 == 0 ? RC522_DEFAULT_MISO_2 : config->miso_io_2;
    hndl->config->mosi_io_2          = config->mosi_io_2 == 0 ? RC522_DEFAULT_MOSI_2 : config->mosi_io_2;
    hndl->config->sck_io_2           = config->sck_io_2 == 0 ? RC522_DEFAULT_SCK_2 : config->sck_io_2;
    hndl->config->sda_io_2           = config->sda_io_2 == 0 ? RC522_DEFAULT_SDA_2 : config->sda_io_2;
    hndl->config->spi_host_id_2      = config->spi_host_id_2 == 0 ? RC522_DEFAULT_SPI_HOST_2 : config->spi_host_id_2;
    hndl->config->scan_interval_ms_2 = config->scan_interval_ms_2 < 50 ? RC522_DEFAULT_SCAN_INTERVAL_MS_2 : config->scan_interval_ms_2;
    hndl->config->task_stack_size_2  = config->task_stack_size_2 == 0 ? RC522_DEFAULT_TACK_STACK_SIZE_2 : config->task_stack_size_2;
    hndl->config->task_priority_2    = config->task_priority_2 == 0 ? RC522_DEFAULT_TACK_STACK_PRIORITY_2 : config->task_priority_2;

    esp_err_t err = rc522_spi_init();


    if(err != ESP_OK) {
        rc522_destroy_2();
        return err;
    }
    
    // ---------- RW test ------------
   // const uint8_t test_addr = 0x24, test_val = 0x25;
   // for(uint8_t i = test_val; i < test_val + 2; i++) {
   //     if((err = rc522_write(test_addr, i)) != ESP_OK || rc522_read(test_addr) != i) {
   //         ESP_LOGE(TAG_2, "RW test fail");
   //         rc522_destroy_2();
   //         return err;
   //     }
   // }
    // ------- End of RW test --------

    rc522_write(0x01, 0x0F);
    rc522_write(0x2A, 0x8D);
    rc522_write(0x2B, 0x3E);
    rc522_write(0x2D, 0x1E);
    rc522_write(0x2C, 0x00);
    rc522_write(0x15, 0x40);
    rc522_write(0x11, 0x3D);

    rc522_antenna_on();

    hndl->running = true;
    if (xTaskCreatePinnedToCore(rc522_task, "rc522_task", hndl->config->task_stack_size_2, NULL, hndl->config->task_priority_2, &hndl->task_handle,1) != pdTRUE) {
        ESP_LOGE(TAG_2, "Fail to create rc522 task");
        rc522_destroy_2();
        return err;
    }

    if(err != ESP_OK) {
        ESP_LOGE(TAG_2, "Fail to create timer");
        rc522_destroy_2();
        return err;
    }

    ESP_LOGI(TAG_2, "Initialized (firmware: 0x%x)", rc522_fw_version());
    return ESP_OK;
}

uint64_t rc522_sn_to_u64_2(uint8_t* sn) {
    if(!sn) {
        return 0;
    }

    uint64_t result = 0;
    for(int i = 4; i >= 0; i--) {
        result |= ((uint64_t) sn[i] << (i * 8));
    }

    return result;
}

/* Returns pointer to dynamically allocated array of two element */
static uint8_t* rc522_calculate_crc(uint8_t *data, uint8_t n) {
    rc522_clear_bitmask(0x05, 0x04);
    rc522_set_bitmask(0x0A, 0x80);

    rc522_write_n(0x09, n, data);

    rc522_write(0x01, 0x03);

    uint8_t i = 255;
    uint8_t nn = 0;

    for(;;) {
        nn = rc522_read(0x05);
        i--;

        if(! (i != 0 && ! (nn & 0x04))) {
            break;
        }
    }

    uint8_t* res = (uint8_t*) malloc(2); 
    
    res[0] = rc522_read(0x22);
    res[1] = rc522_read(0x21);

    return res;
}

static uint8_t* rc522_card_write(uint8_t cmd, uint8_t *data, uint8_t n, uint8_t* res_n) {
    uint8_t *result = NULL;
    uint8_t irq = 0x00;
    uint8_t irq_wait = 0x00;
    uint8_t last_bits = 0;
    uint8_t nn = 0;
    
    if(cmd == 0x0E) {
        irq = 0x12;
        irq_wait = 0x10;
    }
    else if(cmd == 0x0C) {
        irq = 0x77;
        irq_wait = 0x30;
    }

    rc522_write(0x02, irq | 0x80);
    rc522_clear_bitmask(0x04, 0x80);
    rc522_set_bitmask(0x0A, 0x80);
    rc522_write(0x01, 0x00);

    rc522_write_n(0x09, n, data);

    rc522_write(0x01, cmd);

    if(cmd == 0x0C) {
        rc522_set_bitmask(0x0D, 0x80);
    }

    uint16_t i = 1000;

    for(;;) {
        nn = rc522_read(0x04);
        i--;

        if(! (i != 0 && (((nn & 0x01) == 0) && ((nn & irq_wait) == 0)))) {
            break;
        }
    }

    rc522_clear_bitmask(0x0D, 0x80);

    if(i != 0) {
        if((rc522_read(0x06) & 0x1B) == 0x00) {
            if(cmd == 0x0C) {
                nn = rc522_read(0x0A);
                last_bits = rc522_read(0x0C) & 0x07;

                if (last_bits != 0) {
                    *res_n = (nn - 1) + last_bits;
                } else {
                    *res_n = nn;
                }

                result = (uint8_t*) malloc(*res_n);

                for(i = 0; i < *res_n; i++) {
                    result[i] = rc522_read(0x09);
                }
            }
        }
    }

    return result;
}

static uint8_t* rc522_request(uint8_t* res_n) {
    uint8_t* result = NULL;
    rc522_write(0x0D, 0x07);

    uint8_t req_mode = 0x26;
    result = rc522_card_write(0x0C, &req_mode, 1, res_n);

    if(*res_n * 8 != 0x10) {
        free(result);
        return NULL;
    }

    return result;
}

static uint8_t* rc522_anticoll() {
    uint8_t res_n;

    rc522_write(0x0D, 0x00);
    uint8_t* result = rc522_card_write(0x0C, (uint8_t[]) { 0x93, 0x20 }, 2, &res_n);

    if(result && res_n != 5) { // all cards/tags serial numbers is 5 bytes long (?)
        free(result);
        return NULL;
    }

    return result;
}

static uint8_t* rc522_get_tag() {
    uint8_t* result = NULL;
    uint8_t* res_data = NULL;
    uint8_t res_data_n;

    res_data = rc522_request(&res_data_n);

    if(res_data != NULL) {
        free(res_data);

        result = rc522_anticoll();

        if(result != NULL) {
            uint8_t buf[] = { 0x50, 0x00, 0x00, 0x00 };
            uint8_t* crc = rc522_calculate_crc(buf, 2);

            buf[2] = crc[0];
            buf[3] = crc[1];

            free(crc);

            res_data = rc522_card_write(0x0C, buf, 4, &res_data_n);
            free(res_data);

            rc522_clear_bitmask(0x08, 0x08);


            return result;
        }
    }
    
    return NULL;
}

esp_err_t rc522_start_2(rc522_start_args_t_2 start_args) {
    esp_err_t err = rc522_init_2(&start_args);
    return err != ESP_OK ? err : rc522_start2_2();
}

esp_err_t rc522_start2_2() {
    if(! hndl) { return ESP_ERR_INVALID_STATE; }

    hndl->scan_started = true;

    return ESP_OK;
}

esp_err_t rc522_pause_2() {
    if(! hndl) { return ESP_ERR_INVALID_STATE; }

    if(! hndl->scan_started) {
        return ESP_OK;
    }

    hndl->scan_started = false;

    return ESP_OK;
}

void rc522_destroy_2() {
    if(! hndl) { return; }

    rc522_pause_2(); // stop timer
    hndl->running = false; // task will delete itself

    if(hndl->spi_2) {
        spi_bus_remove_device(hndl->spi_2);
        //spi_bus_free(hndl->config->spi_host_id_2);
        hndl->spi_2 = NULL;
    }

    free(hndl->config);
    hndl->config = NULL;

    free(hndl);
    hndl = NULL;
}

static void rc522_task(void* arg) {
    while(hndl->running) {
        if(!hndl->scan_started) {
            
            vTaskDelay(100 / portTICK_PERIOD_MS);
            
            continue;
        }

        uint8_t* serial_no = rc522_get_tag();

        if(serial_no && ! hndl->tag_was_present_last_time) {
            rc522_tag_callback_t_2 cb = hndl->config->callback_2;
            if(cb) { cb(serial_no); }
        }
        
        if((hndl->tag_was_present_last_time = (serial_no != NULL))) {
            free(serial_no);
            serial_no = NULL;
        }

        int delay_interval_ms = hndl->config->scan_interval_ms_2;

        if(hndl->tag_was_present_last_time) {
            delay_interval_ms *= 2; // extra scan-bursting prevention
        }

        vTaskDelay(delay_interval_ms / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}