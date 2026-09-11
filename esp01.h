#ifndef _ESP01_H_
#define _ESP01_H_

/* Function declarations for ESP01 WiFi module */

/* Check AT command communication */
int esp01_connectAP_AT(void);

/* Disable command echo */
int esp01_connectAP_ATE0(void);

/* Configure TCP single connection mode */
int esp01_connectAP_TCP_MODE(void);

/* Disconnect from access point */
int esp01_connectAP_QUIT_AP(void);

/* Connect ESP01 to WiFi access point */
int esp01_connectAP_JOIN_AP(void);

/* Send data to ThingSpeak cloud */
int esp01_sendToThingspeak(int ,int );

/* Upload sensor data */
void update_data(int ,int num);

/* Initialize ESP01 module */
void init_esp01(void);

#endif
