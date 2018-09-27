/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_TEMP_
#define H_TEMP_

#ifdef __cplusplus
extern "C" {
#endif



/* Include a vendor-specific block containing 16-bit temperature in the ad packet */
#define INCLUDE_TEMP_IN_ADVERTISEMENT   MYNEWT_VAL(INCLUDE_TEMP_IN_ADVERTISEMENT)

/* ..this includes the standard GATT temperature attribute as well as the Proxy custom one */
#define THERMOMETER_ATTRIBUTE_TEMP_CELSIUS 0x2A1f

/* Manufacturer ID for custom data contained in the ad packet */
#define PROXY_CORP_MANUF_ID             0x0304

/* the advertised temperature data is only updated every ... */
#define TEMP_IN_ADVERTISEMENT_REFRESH_INTERVAL_MS 2500  



/* These defines should be moved to MYNEWT_VAL() */
/* Maximum number of previous temperature samples available.  */
#define THERMAL_LOG_LENGTH MYNEWT_VAL(THERMAL_LOG_LENGTH)
/* Sampling rate */
#define THERMAL_LOG_SAMPLES_PER_SECOND MYNEWT_VAL(THERMAL_LOG_SAMPLES_PER_SECOND)



    
/* Init hardware and set up a logging task */
void thermal_log_init(void);
/* Returns count - may return less samples than requested if the logging task has just (re) started */
uint8_t thermal_log_read_history(uint8_t count,int16_t *dest);



#ifdef __cplusplus
}
#endif

#endif
