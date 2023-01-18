#include "open62541.h"
#include "model.h"

#include "driver/gpio.h"
#include "Potentiometer.h"
#include "dht11.h"
#include "rc522.h"
#include "rc522_2.h"
#include <stdio.h>
#include "string.h"

#include "PWM.h"
#include "ReadInput.h"
#include "Leds.h"
#include "MCP3909.h"

#include "ADconversor.c"
#include "PWM.c"
#include "MCP3909.c"

#include "Leds.c"





//int ESTADO2 = 10;

char RFID1[20];
char RFID2[20];



//bool handler = false;
bool stateFlag1 = false;
bool stateFlag2 = false;
int PPM=0;

#define MEMORY_TAG "MEMORY"

//________________________________________________________________OPC DHT11  ______________________________________________________________________

UA_StatusCode
readCurrentTemperature(UA_Server *server,
                       const UA_NodeId *sessionId, void *sessionContext,
                       const UA_NodeId *nodeId, void *nodeContext,
                       UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                       UA_DataValue *dataValue)
{
    UA_Float temperature = DHT11_read().temperature;
    //printf(" o status do dht11 é %i: \n", DHT11_read().status);
    UA_Variant_setScalarCopy(&dataValue->value, &temperature,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    //ESP_LOGI(MEMORY_TAG, "apos da temperatura : %d", esp_get_free_heap_size());
    return UA_STATUSCODE_GOOD;
}

void addCurrentTemperatureDataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "temperature");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "temperature");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Ambient Temperature");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    DHT11_init(GPIO_NUM_25);
    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentTemperature;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}

/* Umidade DHT11*/

UA_StatusCode
readCurrentUmidade(UA_Server *server,
                   const UA_NodeId *sessionId, void *sessionContext,
                   const UA_NodeId *nodeId, void *nodeContext,
                   UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                   UA_DataValue *dataValue)
{
    UA_Float umidade = DHT11_read().humidity;
    UA_Variant_setScalarCopy(&dataValue->value, &umidade,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    return UA_STATUSCODE_GOOD;
}

void addCurrentUmidadeDataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "umidade");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "umidade");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Ambient Umidade");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentUmidade;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}


//__________________________________________________________   P W M    &    DI s __________________________________________________________________________


UA_StatusCode
PWMout(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue)

{
    //DIs
    ReadInput_init();
    ReadInput_Main();

    //DOs
    PWM_init();
    statusled();

    UA_Float Estado = ESTADO;


    UA_Variant_setScalarCopy(&dataValue->value, &Estado,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    
    return UA_STATUSCODE_GOOD;
}

void addcurrentstatusouput(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "ESTADO");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "ESTADO");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Fita de Led");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    //DHT11_init(GPIO_NUM_32);
    UA_DataSource timeDataSource;
    timeDataSource.read = PWMout;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}


//__________________________________________________________   AI0  __________________________________________________________________________


UA_StatusCode
AI0OUTPUT(UA_Server *server,
                       const UA_NodeId *sessionId, void *sessionContext,
                       const UA_NodeId *nodeId, void *nodeContext,
                       UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                       UA_DataValue *dataValue)
{
    requisicao();
    leituraAI0();
    UA_Float AI0 = val;

    UA_Variant_setScalarCopy(&dataValue->value, &AI0,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    //ESP_LOGI(MEMORY_TAG, "apos da temperatura : %d", esp_get_free_heap_size());
    return UA_STATUSCODE_GOOD;
}

void addCurrentAI0DataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "AI0");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "AI0");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Leitura Analógica 0");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = AI0OUTPUT;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}


//__________________________________________________________   AI1  __________________________________________________________________________


UA_StatusCode
AI1OUTPUT(UA_Server *server,
                       const UA_NodeId *sessionId, void *sessionContext,
                       const UA_NodeId *nodeId, void *nodeContext,
                       UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                       UA_DataValue *dataValue)
{
    //requisicao();
    errorled_off();
    leituraAI1();
    UA_Float AI1 = val2;
    UA_Variant_setScalarCopy(&dataValue->value, &AI1,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;

    return UA_STATUSCODE_GOOD;
}

void addCurrentAI1DataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "AI1");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "AI1");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Leitura Analógica 0");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = AI1OUTPUT;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}




//__________________________________________________________   ANALISADOR TRIFÁSICO  __________________________________________________________________________


/* TENSAO MCP3909 */
UA_StatusCode
readCurrentTensao(UA_Server *server,
                       const UA_NodeId *sessionId, void *sessionContext,
                       const UA_NodeId *nodeId, void *nodeContext,
                       UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                       UA_DataValue *dataValue)
{
    UA_Double tensao = Vglobal;

    UA_Variant_setScalarCopy(&dataValue->value, &tensao,
                             &UA_TYPES[UA_TYPES_DOUBLE]);
    dataValue->hasValue = true;
    //ESP_LOGI(MEMORY_TAG, "apos da temperatura : %d", esp_get_free_heap_size());
    return UA_STATUSCODE_GOOD;
}

void addCurrentTENSAODataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "tensao");
    attr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "tensao");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Tensao Lida");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentTensao;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}

/* CORRENTE MCP3909 */

UA_StatusCode
readCurrentCorrente(UA_Server *server,
                   const UA_NodeId *sessionId, void *sessionContext,
                   const UA_NodeId *nodeId, void *nodeContext,
                   UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                   UA_DataValue *dataValue)
{
    //MCP3909_init();
    mcpInitSPI();
    EXECUTE_SPI();

    UA_Float corrente = Iglobal;
    UA_Variant_setScalarCopy(&dataValue->value, &corrente,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    return UA_STATUSCODE_GOOD;
}

void addCurrentCorrenteDataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "corrente");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "corrente");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Corrente Lida");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentCorrente;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}


/* POT ATIVA  */

UA_StatusCode
readCurrentAtiva(UA_Server *server,
                   const UA_NodeId *sessionId, void *sessionContext,
                   const UA_NodeId *nodeId, void *nodeContext,
                   UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                   UA_DataValue *dataValue)
{
    //MCP3909_init();
    mcpInitSPI();
    EXECUTE_SPI();

    UA_Float corrente = Pglobal;
    UA_Variant_setScalarCopy(&dataValue->value, &corrente,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    return UA_STATUSCODE_GOOD;
}

void addCurrentAtivaDataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Pativa");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "Pativa");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Potencia Lida");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentAtiva;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}


/* POT APARENTE  */

UA_StatusCode
readCurrentAparente(UA_Server *server,
                   const UA_NodeId *sessionId, void *sessionContext,
                   const UA_NodeId *nodeId, void *nodeContext,
                   UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                   UA_DataValue *dataValue)
{
    UA_Float corrente = Sglobal;
    UA_Variant_setScalarCopy(&dataValue->value, &corrente,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    return UA_STATUSCODE_GOOD;
}

void addCurrentAparenteDataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Paparente");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "Paparente");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Potencia Lida");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentAparente;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}

/* POT REATIVA  */

UA_StatusCode
readCurrentReativa(UA_Server *server,
                   const UA_NodeId *sessionId, void *sessionContext,
                   const UA_NodeId *nodeId, void *nodeContext,
                   UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                   UA_DataValue *dataValue)
{
    UA_Float corrente = Rglobal;
    UA_Variant_setScalarCopy(&dataValue->value, &corrente,
                             &UA_TYPES[UA_TYPES_FLOAT]);
    dataValue->hasValue = true;
    return UA_STATUSCODE_GOOD;
}

void addCurrentReativaDataSourceVariable(UA_Server *server)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Preativa");
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ;

    UA_NodeId currentNodeId = UA_NODEID_STRING(1, "Preativa");
    UA_QualifiedName currentName = UA_QUALIFIEDNAME(1, "Potencia Lida");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_NodeId variableTypeNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE);
    UA_DataSource timeDataSource;
    timeDataSource.read = readCurrentReativa;
    UA_Server_addDataSourceVariableNode(server, currentNodeId, parentNodeId,
                                        parentReferenceNodeId, currentName,
                                        variableTypeNodeId, attr,
                                        timeDataSource, NULL, NULL);
}

