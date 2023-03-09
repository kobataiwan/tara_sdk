#ifndef _TARA_INFO_H_
#define _TARA_INFO_H_

#include <errno.h>
#include "myglobal.h"
#include "myerr.h"

#define NAME_SIZE							64
#define BRAND_NAME_SIZE				64
#define MANUFACTURER_SIZE			64
#define MODEL_NAME_SIZE				64
#define MODEL_NUMBER_SIZE			64
#define PRODUCT_NAME_SIZE			64
#define SOC_NAME_SIZE					64
#define SENSOR_NAME_SIZE			64
#define LOCATION_SIZE					64
#define SERIAL_NUMBER_SIZE		24
#define VERSION_NUMBER_SIZE		64

typedef struct {
	s8 name[NAME_SIZE+1];
	s8 location[LOCATION_SIZE+1];                     // location info
	s8 application_version[VERSION_NUMBER_SIZE+1];    // application version
	s8 fw_version[VERSION_NUMBER_SIZE+1];             // firmware version
	s8 onvif_version[VERSION_NUMBER_SIZE+1];          // onvif version
	s8 brand[BRAND_NAME_SIZE+1];                      // brand info
	s8 manufacturer[MANUFACTURER_SIZE+1];             // manufacturer name
	s8 product[PRODUCT_NAME_SIZE+1];                  // product name
	s8 model_name[MODEL_NAME_SIZE+1];                 // model name
	s8 model_number[MODEL_NUMBER_SIZE+1];             // model number
	s8 hw_version[VERSION_NUMBER_SIZE+1];             // hardware version
	s8 serial_number[SERIAL_NUMBER_SIZE+1];           // serial number
	s8 soc[SOC_NAME_SIZE+1];                          // chip name
	s8 image_sensor[SENSOR_NAME_SIZE+1];              // sensor name
} TaraInfoConfig;


#ifdef CPLUSPLUS
extern "C" {
#endif

/*
 * get current Info Configuation
 * return: 0 on success, otherwise error code returned
 * ptr: on success, current TaraInfoConfig will be stored
 */
extern s32 Tara_info_getConfig(TaraInfoConfig *pinfo);

/*
 * set brand name
 * return: 0 on success, otherwise error code returned
 * brand: on success, brand will be stored
 */
extern s32 Tara_info_setBrand(s8 *brand);

/*
 * set serial number
 * return: 0 on success, otherwise error code returned
 * num: on success, serial number will be stored
 */
extern s32 Tara_info_setSerialNumber(s8 *num);

/*
 * set name
 * return: 0 on success, otherwise error code returned
 * name: on success, name will be stored
 */
extern s32 Tara_info_setName(s8 *name);

/*
 * set location
 * return: 0 on success, otherwise error code returned
 * location: on success, location will be stored
 */
extern s32 Tara_info_setLocation(s8 *location);

//extern s32 Tara_info_getManufacturer(s8 *p);
//extern s32 Tara_info_getModel(s8 *p);
//extern s32 Tara_info_getProduct(s8 *p);
//extern s32 Tara_info_getFirmwareVersion(s8 *p);
//extern s32 Tara_info_getHardwareID(s8 *p);
//extern s32 Tara_info_getSoC(s8 *p);
//extern s32 Tara_info_getImageSensor(s8 *p);

#ifdef CPLUSPLUS
}
#endif
#endif
