#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/spi_master.h"

#define RC522_DEFAULT_MISO_2                 (25)
#define RC522_DEFAULT_MOSI_2                (23)
#define RC522_DEFAULT_SCK_2                  (19)
#define RC522_DEFAULT_SDA_2                  (22)
#define RC522_DEFAULT_SPI_HOST_2             (SPI3_HOST)
#define RC522_DEFAULT_SCAN_INTERVAL_MS_2     (125)
#define RC522_DEFAULT_TACK_STACK_SIZE_2      (2048)
#define RC522_DEFAULT_TACK_STACK_PRIORITY_2  (4)

typedef void(*rc522_tag_callback_t_2)(uint8_t*);

typedef struct {
    int miso_io_2;                    /*<! MFRC522 MISO gpio (Default: 25) */
    int mosi_io_2;                    /*<! MFRC522 MOSI gpio (Default: 23) */
    int sck_io_2;                     /*<! MFRC522 SCK gpio  (Default: 19) */
    int sda_io_2;                     /*<! MFRC522 SDA gpio  (Default: 22) */
    spi_host_device_t spi_host_id_2;  /*<! Default VSPI_HOST (SPI3) */
    rc522_tag_callback_t_2 callback_2;  /*<! Scanned tags handler */
    uint16_t scan_interval_ms_2;      /*<! How fast will ESP32 scan for nearby tags, in miliseconds. Default: 125ms */
    size_t task_stack_size_2;         /*<! Stack size of rc522 task (Default: 4 * 1024) */
    uint8_t task_priority_2;          /*<! Priority of rc522 task (Default: 4) */
} rc522_config_t_2;

typedef rc522_config_t_2 rc522_start_args_t_2;


/**
 * @brief Initialize RC522 module.
 *        To start scanning tags - call rc522_resume or rc522_start2_2 function.
 * @param config Configuration
 * @return ESP_OK on success
 */
esp_err_t rc522_init_2(rc522_config_t_2* config);

/**
 * @brief Convert serial number (array of 5 bytes) to uint64_t number
 * @param sn Serial number
 * @return Serial number in number representation. If fail, 0 will be retured
 */
uint64_t rc522_sn_to_u64_2(uint8_t* sn);

/**
 * @brief Check if RC522 is inited
 * @return true if RC522 is inited
 */
bool rc522_is_inited_2();

/**
 * @brief This function will call rc522_init function and immediately start to scan tags by calling rc522_resume function.
 *        NOTE: This function will be refactored in future to just start scanning without
 *        initialization (same as rc522_resume). For initialization rc522_init will be required to call before this function.
 * @param start_args Configuration
 * @return ESP_OK on success
 */
esp_err_t rc522_start_2(rc522_start_args_t_2 start_args);

/**
 * @brief Start to scan tags. If already started, ESP_OK will just be returned.
 *        NOTE: This function is implemented because in time of implementation rc522_start function is intented for
 *        initialization and scanning in once. In future, when rc522_start gonna be refactored to just start to scan tags
 *        without initialization, this function will be just alias of rc522_start.
 * @return ESP_OK on success
 */
esp_err_t rc522_start2_2();

/**
 * @brief Start to scan tags. If already started, ESP_OK will just be returned.
 * @return ESP_OK on success
 */
#define rc522_resume_2() rc522_start2_2()

/**
 * @brief Pause scan tags. If already paused, ESP_OK will just be returned.
 * @return ESP_OK on success
 */
esp_err_t rc522_pause_2();

/**
 * @brief Destroy RC522 and free all resources
 */
void rc522_destroy_2();

//#ifdef __cplusplus
//}
//#endif