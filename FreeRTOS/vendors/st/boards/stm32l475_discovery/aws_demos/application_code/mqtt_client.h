/*******************************************************************************
 *
 ******************************************************************************/

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * MqttAWSPortInit
 *
 * Should be called during initialization.
 *
 * NOTE: Define as (0) if nothing should to be done.
 ******************************************************************************/
uint32_t ulMqttAWSPortInit(void);

/*******************************************************************************
 * MqttAWSPortSend
 *
 * Sends data to the cloud.
 *
 * @param pvData The data.
 * @param ulSize The data size.
 * @param name The type of slice to be sent.
 * @param pulBytesSent The number of bytes that were sent.
 ******************************************************************************/
uint32_t ulMqttAWSPortSend(const void* pvData, uint32_t ulSize, const char* name, uint32_t* pulBytesSent);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_CLIENT_H */
