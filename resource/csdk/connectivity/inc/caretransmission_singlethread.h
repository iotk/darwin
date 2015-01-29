/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
/**
 * @file caretransmission_singlethread.h
 * @brief
 */
#ifndef __CA_RETRANSMISSION_SINGLETHREAD_H_
#define __CA_RETRANSMISSION_SINGLETHREAD_H_

#include <stdint.h>

#include "uarraylist.h"
#include "cacommon.h"

/** CA_ETHERNET, CA_WIFI, CA_LE **/
#define DEFAULT_RETRANSMISSION_TYPE     ((1<<0)|(1<<1)|(1<<3))

/** default retransmission trying count is 4. **/
#define DEFAULT_RETRANSMISSION_COUNT    4

/** check period is 1 sec. **/
#define RETRANSMISSION_CHECK_PERIOD     1000000

/** retransmission data send method type**/
typedef CAResult_t (*CADataSendMethod_t)(const CARemoteEndpoint_t *endpoint, void *pdu,
        uint32_t size);

/** retransmission timeout callback type**/
typedef void (*CATimeoutCallback_t)(const CARemoteEndpoint_t *endpoint, void *pdu,
                                    uint32_t size);

typedef struct
{
    /** retransmission support connectivity type **/
    CAConnectivityType_t supportType;
    /** retransmission trying count **/
    uint8_t tryingCount;

} CARetransmissionConfig_t;

typedef struct
{
    /** send method for retransmission data **/
    CADataSendMethod_t dataSendMethod;
    /** callback function for retransmit timeout **/
    CATimeoutCallback_t timeoutCallback;
    /** retransmission configure data **/
    CARetransmissionConfig_t config;
    /** Variable to inform the thread to stop **/
    CABool_t isStop;
    /** array list on which the thread is operating. **/
    u_arraylist_t *dataList;
} CARetransmission_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Initializes the retransmission context
 * @param   context [IN]    context for retransmission
 * @param   handle  [IN]    thread pool handle
 * @param   retransmissionSendMethod    [IN]    function to be called for retransmission
 * @param   timeoutCallback [IN]    callback for retransmit timeout
 * @param   config  [IN]    configuration for retransmission.
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionInitialize(CARetransmission_t *context,
                                      CADataSendMethod_t retransmissionSendMethod,
                                      CATimeoutCallback_t timeoutCallback,
                                      CARetransmissionConfig_t *config);

/**
 * @brief   Pass the sent pdu data. if retransmission process need, internal thread will wake up and
 *             process the retransmission data.
 * @param   context     [IN]    context for retransmission
 * @param   endpoint    [IN]    endpoint information
 * @param   pdu         [IN]    sent pdu binary data
 * @param   size        [IN]    sent pdu binary data size
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionSentData(CARetransmission_t *context,
                                    const CARemoteEndpoint_t *endpoint,
                                    const void *pdu, uint32_t size);

/**
 * @brief   Paas the received pdu data. if received pdu is ACK data for the retransmission CON data,
 *             the specified CON data will remove on retransmission list.
 * @param   context     [IN]    context for retransmission
 * @param   endpoint    [IN]    endpoint information
 * @param   pdu         [IN]    received pdu binary data
 * @param   size        [IN]    received pdu binary data size
 * @param   void        [OUT]   pdu data of the request for reset and ack
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionReceivedData(CARetransmission_t *context,
                                        const CARemoteEndpoint_t *endpoint, const void *pdu,
                                        uint32_t size, void **retransmissionPdu);

/**
 * @brief   Stopping the retransmission context
 * @param   context [IN]    context for retransmission
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionStop(CARetransmission_t *context);

/**
 * @brief   Terminating the retransmission context
 * @param   context [IN]    context for retransmission
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionDestroy(CARetransmission_t *context);

/**
 * @brief   Retransmitting the request/response for CONFIRMABLE type
 */
void CACheckRetransmissionList();

/**
 * @brief   Invoke Retranmission according to TimedAction Response
 * @param   context [IN]    context for retransmission
 */
void CARetransmissionBaseRoutine(void *threadValue);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // __CA_RETRANSMISSION_SINGLETHREAD_H_
