#include <stdio.h>
#include <math.h>
#include <sys/param.h>
#include <unistd.h>
#include <lwip/sockets.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include <esp_flash_encrypt.h>
#include "esp_netif.h"
#include <esp_task_wdt.h>
#include <esp_sntp.h>
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <string.h> 


#include "ethernet_connect.h"
#include "open62541.h"
#include "DHT22.h"
#include "model.h"
#include "Potentiometer.h"
#include "rc522.h"

#include "PWM.h"
#include "ReadInput.h"
#include "Leds.h"
#include "ADconversor.h"
#include "MCP3909.h"

#include "soc/rtc_wdt.h"







#define EXAMPLE_ESP_WIFI_SSID      "SMART SENSE"
#define EXAMPLE_ESP_WIFI_PASS      "Exsto_Labtronix"
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       5




static const char *TAG = "wifi softAP";




static int s_retry_num = 0;







#define EXAMPLE_ESP_MAXIMUM_RETRY 10

#define TAG "OPCUA_ESP32"
#define SNTP_TAG "SNTP"
#define MEMORY_TAG "MEMORY"
#define ENABLE_MDNS 1

#define MCP_CS 5
#define MCP_SCK 18
#define MCP_MISO 19
#define MCP_MOSI 23

#define MCLR 13
#define G0_gain 26
#define G1_gain 27

//MCP3909
#define QNT_AMOSTRAS        1920
int CONT_AMOSTRAS = 0;

int16_t mcp_ch0 = 0x00;
int16_t mcp_ch1 = 0x00;

unsigned int auxInt;
int habilitaMCLR = 0;

int  f_sample = 0;
int contAmostra=1;


float acc_v, acc_i, acc_p = 0;

float tensaoCH1=0;
float tensaoCH0=0;

float valorcorrenteanterior = 0;
float GANHO, G1, G0 = 1;
float VRMS, IRMS, P_ativa, P_aparente, P_reativa = 0;

int counter = 0;
//MCP3909

int habilitapins = 0;

static bool obtain_time(void);
static void initialize_sntp(void);


UA_ServerConfig *config;
static UA_Boolean sntp_initialized = false;
static UA_Boolean running = true;
static UA_Boolean isServerCreated = false;
RTC_DATA_ATTR static int boot_count = 0;
static struct tm timeinfo;
static time_t now = 0;

static UA_StatusCode




UA_ServerConfig_setUriName(UA_ServerConfig *uaServerConfig, const char *uri, const char *name)
{
    // delete pre-initialized values
    UA_String_clear(&uaServerConfig->applicationDescription.applicationUri);
    UA_LocalizedText_clear(&uaServerConfig->applicationDescription.applicationName);

    uaServerConfig->applicationDescription.applicationUri = UA_String_fromChars(uri);
    uaServerConfig->applicationDescription.applicationName.locale = UA_STRING_NULL;
    uaServerConfig->applicationDescription.applicationName.text = UA_String_fromChars(name);

    for (size_t i = 0; i < uaServerConfig->endpointsSize; i++)
    {
        UA_String_clear(&uaServerConfig->endpoints[i].server.applicationUri);
        UA_LocalizedText_clear(
            &uaServerConfig->endpoints[i].server.applicationName);

        UA_String_copy(&uaServerConfig->applicationDescription.applicationUri,
                       &uaServerConfig->endpoints[i].server.applicationUri);

        UA_LocalizedText_copy(&uaServerConfig->applicationDescription.applicationName,
                              &uaServerConfig->endpoints[i].server.applicationName);
    }

    return UA_STATUSCODE_GOOD;
}

static void opcua_task(void *arg)
{
    //BufferSize's got to be decreased due to latest refactorings in open62541 v1.2rc.
    UA_Int32 sendBufferSize = 8192;
    UA_Int32 recvBufferSize = 8192;

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    ESP_LOGI(TAG, "Fire up OPC UA Server.");
    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);
    UA_ServerConfig_setMinimalCustomBuffer(config, 4840, 0, sendBufferSize, recvBufferSize);

    const char *appUri = "open62541.esp32.server";
    UA_String hostName = UA_STRING("opcua-esp32");
#ifdef ENABLE_MDNS
    config->mdnsEnabled = true;
    config->mdnsConfig.mdnsServerName = UA_String_fromChars(appUri);
    config->mdnsConfig.serverCapabilitiesSize = 2;
    UA_String *caps = (UA_String *)UA_Array_new(2, &UA_TYPES[UA_TYPES_STRING]);
    caps[0] = UA_String_fromChars("LDS");
    caps[1] = UA_String_fromChars("NA");
    config->mdnsConfig.serverCapabilities = caps;
    // We need to set the default IP address for mDNS since internally it's not able to detect it.
    tcpip_adapter_ip_info_t default_ip;
    
    #ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_ETH;
    #else
    tcpip_adapter_if_t tcpip_if = TCPIP_ADAPTER_IF_STA;
    #endif

    esp_err_t ret = tcpip_adapter_get_ip_info(tcpip_if, &default_ip);
    if ((ESP_OK == ret) && (default_ip.ip.addr != INADDR_ANY))
    {
        config->mdnsIpAddressListSize = 1;
        config->mdnsIpAddressList = (uint32_t *)UA_malloc(sizeof(uint32_t) * config->mdnsIpAddressListSize);
        memcpy(config->mdnsIpAddressList, &default_ip.ip.addr, sizeof(uint32_t));
    }
    else
    {
        ESP_LOGI(TAG, "Could not get default IP Address!");
    }
#endif
    UA_ServerConfig_setUriName(config, appUri, "OPC_UA_Server_ESP32");
    UA_ServerConfig_setCustomHostname(config, hostName);

    /* Add Information Model Objects Here */ 


    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________
    
    addCurrentTemperatureDataSourceVariable(server);
    addCurrentUmidadeDataSourceVariable(server);
    addcurrentstatusouput(server);
    addCurrentAI0DataSourceVariable(server);
    addCurrentAI1DataSourceVariable(server);
    

    //ANALISADOR TRIFÁSICO*/
    addCurrentTENSAODataSourceVariable(server);
    addCurrentCorrenteDataSourceVariable(server);
    addCurrentAtivaDataSourceVariable(server);
    addCurrentAparenteDataSourceVariable(server);
    addCurrentReativaDataSourceVariable(server);
    leds_init();
    
    //______________________________________________________________________________________________________________
    //______________________________________________________________________________________________________________

    ESP_LOGI(TAG, "Heap Left : %d", xPortGetFreeHeapSize());
    UA_StatusCode retval = UA_Server_run_startup(server);
    if (retval == UA_STATUSCODE_GOOD)
    {
        while (running)
        {   
            if(habilitapins <= 2){

                habilitapins++;
                //DIs
                gpio_pad_select_gpio(MCP_MISO);
                gpio_set_direction(MCP_MISO,GPIO_MODE_INPUT);
          
                //gpio_set_intr_type(MCP_MISO, GPIO_INTR_NEGEDGE);
                //gpio_intr_enable(MCP_MISO);

                
                
                //DOs
                gpio_pad_select_gpio(MCP_MOSI);
                gpio_set_direction(MCP_MOSI,GPIO_MODE_OUTPUT);

                gpio_pad_select_gpio(MCP_SCK);
                gpio_set_direction(MCP_SCK,GPIO_MODE_OUTPUT);

                gpio_pad_select_gpio(MCP_CS);
                gpio_set_direction(MCP_CS,GPIO_MODE_OUTPUT);
                
                gpio_pad_select_gpio(MCLR);
                gpio_set_direction(MCLR,GPIO_MODE_OUTPUT);

                //GAIN SETUP
                gpio_pad_select_gpio(G0_gain);
                gpio_set_direction(G0_gain,GPIO_MODE_OUTPUT);
                gpio_pad_select_gpio(G1_gain);
                gpio_set_direction(G1_gain,GPIO_MODE_OUTPUT);

                gpio_set_level(MCLR, 1);
                gpio_set_level(MCP_CS, 1);
                gpio_set_level(G1_gain, 1);
                gpio_set_level(G0_gain, 1); 

                //---------------------------------------------------------------------------------------

                //ENTRY CODE  0xAC
                gpio_set_level(MCLR, 0);
                gpio_set_level(MCP_CS, 1);
                gpio_set_level(MCP_SCK, 0);

                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1ts = (1 << MCLR);
                GPIO.out_w1ts = (1 << MCLR);
                GPIO.out_w1ts = (1 << MCLR);
                GPIO.out_w1ts = (1 << MCLR);
                GPIO.out_w1ts = (1 << MCLR);
                GPIO.out_w1ts = (1 << MCLR);

                GPIO.out_w1tc = (1 << MCP_CS);
                GPIO.out_w1tc = (1 << MCP_CS);
                GPIO.out_w1tc = (1 << MCP_CS);
                GPIO.out_w1tc = (1 << MCP_CS);
                GPIO.out_w1tc = (1 << MCP_CS);
                GPIO.out_w1tc = (1 << MCP_CS);
                GPIO.out_w1tc = (1 << MCP_CS);
                GPIO.out_w1tc = (1 << MCP_CS);


                //POST HPF1 DUAL CHANNEL OUTPUT  (0xAC)
                //1

                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1ts = (1 << MCP_MOSI); 
                GPIO.out_w1ts = (1 << MCP_SCK); 

                        
                //2
                GPIO.out_w1tc = (1 << MCP_SCK);
                GPIO.out_w1tc = (1 << MCP_MOSI);  
                GPIO.out_w1ts = (1 << MCP_SCK); 


                //3
                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1ts = (1 << MCP_MOSI);
                GPIO.out_w1ts = (1 << MCP_SCK);


                //4
                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1tc = (1 << MCP_MOSI); 
                GPIO.out_w1ts = (1 << MCP_SCK); 

                //5
                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1ts = (1 << MCP_MOSI);  
                GPIO.out_w1ts = (1 << MCP_SCK); 


                //6
                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1ts = (1 << MCP_MOSI); 
                GPIO.out_w1ts = (1 << MCP_SCK); 


                //7
                GPIO.out_w1tc = (1 << MCP_SCK); 
                GPIO.out_w1tc = (1 << MCP_MOSI); 
                GPIO.out_w1ts = (1 << MCP_SCK); 


                //8
                GPIO.out_w1tc = (1 << MCP_SCK);  
                GPIO.out_w1tc = (1 << MCP_MOSI); 
                GPIO.out_w1ts = (1 << MCP_SCK);  

                GPIO.out_w1ts = (1 << MCP_CS); 
                GPIO.out_w1ts = (1 << MCP_CS); 

                gpio_set_level(MCP_CS,0);

                gpio_set_level(MCP_SCK,0);
                gpio_set_level(MCP_SCK,0);
        
                
                vTaskDelay(5/portTICK_RATE_MS);

                
            }
            if(habilitapins>2){
                    f_sample = 0;
                    contAmostra=1;

                    mcp_ch1 = 0x00;
                    mcp_ch0 = 0x00;
            
                    tensaoCH0 = 0;
                    tensaoCH1 = 0;

                    while(contAmostra<=3)
                    {       
                        GPIO.out_w1tc = (1 << MCP_CS); 

                        if(gpio_get_level(MCP_MISO)==1)
                        {
                                f_sample = 1;
                                mcp_ch0=0;
                                mcp_ch1=0;
                        }

                        if((gpio_get_level(MCP_MISO)==0) && (f_sample==1))
                        {
                                for(int j=0; j<32; j++)
                                {
                                    
                                    GPIO.out_w1ts = (1 << MCP_SCK); 
                                    GPIO.out_w1tc = (1 << MCP_SCK); 

                                    if(j<15)
                                    {
                                            mcp_ch1 = mcp_ch1 | gpio_get_level(MCP_MISO); // talvez seja interessante | 0
                                            mcp_ch1 = mcp_ch1 << 1;
                                    }
                                    else if(j<16)
                                    {
                                            mcp_ch1 = mcp_ch1 | gpio_get_level(MCP_MISO);
                                    }
                                    else if(j>=16 && j<31)
                                    {

                                            mcp_ch0 = mcp_ch0 | gpio_get_level(MCP_MISO); // talvez seja interessante | 0
                                            mcp_ch0 = mcp_ch0 << 1;
                                    }
                                    else if(j<32)
                                    {
                                            mcp_ch0 = mcp_ch0 | gpio_get_level(MCP_MISO);
                                    }
                                        
                                }  
                        contAmostra++;
                        f_sample=0;
                        }
                    }

                    if(mcp_ch1>=32768)
                    {
                        mcp_ch1 = - 65536 + mcp_ch1;
                    }
                    if(mcp_ch0>=32768){
                        mcp_ch0 =  - 65536 + mcp_ch0;
                    }
                    
                    
                    tensaoCH1=0;
                    tensaoCH0=0;
                    tensaoCH1=mcp_ch1;
                    tensaoCH0=mcp_ch0;

                   
           

                    if(CONT_AMOSTRAS < QNT_AMOSTRAS)
                    {
                        acc_v = acc_v + powf(tensaoCH1, 2);
                        acc_i = acc_i + powf(tensaoCH0, 2);
                        acc_p = acc_p + tensaoCH1*tensaoCH0;
                        CONT_AMOSTRAS++;
                    }

                    if(CONT_AMOSTRAS >= QNT_AMOSTRAS)
                    {
                        VRMS = acc_v/QNT_AMOSTRAS;
                        VRMS = sqrtf(VRMS)/75.2;
                        IRMS = acc_i/QNT_AMOSTRAS;
                        IRMS = sqrtf(IRMS)/22.3;   //m[A]

                        P_aparente = (VRMS * IRMS)/1000;

                        P_ativa = acc_p /QNT_AMOSTRAS;
                        P_ativa = sqrtf(P_ativa)/250;

                        
                        P_reativa = sqrtf(powf(P_aparente,2) - powf(P_ativa,2));

                        Analisador(VRMS, IRMS, P_ativa, P_aparente, P_reativa);

                        acc_v = 0;
                        acc_i = 0;
                        acc_p = 0;
                        CONT_AMOSTRAS = 0;

                    }

                    GPIO.out_w1ts = (1 << MCP_CS); 
            }
                    


                    
            
            UA_Server_run_iterate(server, false);
            ESP_ERROR_CHECK(esp_task_wdt_reset());
            taskYIELD();
        }
        UA_Server_run_shutdown(server);
    }
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
}

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(SNTP_TAG, "Notification of a time synchronization event");
}

static void initialize_sntp(void)
{
    ESP_LOGI(SNTP_TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
    sntp_initialized = true;
}

static bool obtain_time(void)
{
    initialize_sntp();
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    memset(&timeinfo, 0, sizeof(struct tm));
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry <= retry_count)
    {
        ESP_LOGI(SNTP_TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(esp_task_wdt_reset());
    }
    time(&now);
    localtime_r(&now, &timeinfo);
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));
    return timeinfo.tm_year > (2016 - 1900);
}


static void opc_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    if (sntp_initialized != true)
    {
        if (timeinfo.tm_year < (2016 - 1900))
        {
            ESP_LOGI(SNTP_TAG, "Time is not set yet. Settting up network connection and getting time over NTP.");
            if (!obtain_time())
            {
                ESP_LOGE(SNTP_TAG, "Could not get time from NTP. Using default timestamp.");
            }
            time(&now);
        }
        localtime_r(&now, &timeinfo);
        ESP_LOGI(SNTP_TAG, "Current time: %d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }

    if (!isServerCreated)
    {
        xTaskCreatePinnedToCore(opcua_task, "opcua_task", 23000, NULL, 8, NULL, 0);
        ESP_LOGI(MEMORY_TAG, "Heap size after OPC UA Task : %d", esp_get_free_heap_size());
        isServerCreated = true;
        
    }
}


static void disconnect_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{

}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);

                   
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}



void wifi_init_softap(void)
{
    static char vSSID[32] ="Exsto Labtronix Restritawwq";
    static char vPW[16] ="3xst0w!f!";
    //s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

//-------------------------------------------------------------------
    ESP_ERROR_CHECK(esp_task_wdt_init(10, true));
    ESP_ERROR_CHECK(esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(0)));
    ESP_ERROR_CHECK(esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(1)));

//---------------------------------------------------------------------

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


//opc_event_handler
     

    #ifdef CONFIG_EXAMPLE_CONNECT_WIFI
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&opc_event_handler,NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&opc_event_handler,NULL));
    //ESP_ERROR_CHECK(example_connect());
    #endif

    /*#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&wifi_event_handler,NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_event_handler,NULL));
    //ESP_ERROR_CHECK(example_connect());
    #endif*/
    

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }


    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);

    
}
    

void app_main(void)
{   
    ++boot_count;
    //Workaround for CVE-2019-15894
    //spi_flash_init();
    if (esp_flash_encryption_enabled())
    {
        esp_flash_write_protect_crypt_cnt();
    }

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    
    //nome_rede = "Exsto Labtronix Restrita";
    //senha_rede = "3xst0w!f!";
    wifi_init_softap();

}

