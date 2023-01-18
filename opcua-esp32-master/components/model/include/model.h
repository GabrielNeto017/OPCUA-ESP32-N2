#include "open62541.h"



//________________________________________________________________ Temperatura ______________________________________________________________________

/* Temperature */
UA_StatusCode
readCurrentTemperature(UA_Server *server,
                       const UA_NodeId *sessionId, void *sessionContext,
                       const UA_NodeId *nodeId, void *nodeContext,
                       UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                       UA_DataValue *dataValue);

void addCurrentTemperatureDataSourceVariable(UA_Server *server);

//________________________________________________________________ Umidade ______________________________________________________________________
/* Umidade*/
UA_StatusCode
readCurrentUmidade(UA_Server *server,
                   const UA_NodeId *sessionId, void *sessionContext,
                   const UA_NodeId *nodeId, void *nodeContext,
                   UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
                   UA_DataValue *dataValue);

void addCurrentUmidadeDataSourceVariable(UA_Server *server);




//________________________________________________________ P W M / INPUTS (DIs)____________________________________________________________________
UA_StatusCode
PWMout(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void addcurrentstatusouput(UA_Server *server);


//___________________________________________________________ A I 0______________________________________________________________________________
UA_StatusCode
AI0OUTPUT(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void addCurrentAI0DataSourceVariable(UA_Server *server);

//___________________________________________________________ A I 1______________________________________________________________________________
UA_StatusCode
AI1OUTPUT(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void addCurrentAI1DataSourceVariable(UA_Server *server);

//_________________________________________________________ LEITURA TENSAO____________________________________________________________________________
UA_StatusCode
readCurrentTensao(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void  addCurrentTENSAODataSourceVariable(UA_Server *server);

//_________________________________________________________ LEITURA CORRENTE__________________________________________________________________________
UA_StatusCode
readCurrentCorrente(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void addCurrentCorrenteDataSourceVariable(UA_Server *server);

//_________________________________________________________ LEITURA POT ATIVA___________________________________________________________________________
UA_StatusCode
readCurrentAtiva(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void  addCurrentAtivaDataSourceVariable(UA_Server *server);

//_________________________________________________________ LEITURA POT APARENTE__________________________________________________________________________
UA_StatusCode
readCurrentAparente(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void addCurrentAparenteDataSourceVariable(UA_Server *server);

//_________________________________________________________ LEITURA POT REATIVA_________________________________________________________________________
UA_StatusCode
readCurrentReativa(UA_Server *server,
        const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *nodeId, void *nodeContext,
        UA_Boolean sourceTimeStamp, const UA_NumericRange *range,
        UA_DataValue *dataValue);

void addCurrentReativaDataSourceVariable(UA_Server *server);



