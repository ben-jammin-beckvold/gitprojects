// Definitions.h : header file for defined command- and response codes
// 


//////////////////////////////////////////////////////
////	Basic functions				  HEX Value	Description

#define API_CAPTURE_IMAGE				0x80	//Capture image from sensor (before enrol), returns "finger present" TRUE or FALSE.
#define API_CAPTURE_AND_ENROL_RAM		0x81	//Enrol into RAM (includes Capture Image)
#define API_CAPTURE_AND_VERIFY_RAM		0x82	//Verify against RAM (includes Capture Image)
#define API_CAPTURE_AND_VERIFY_FLASH	0x83	//Verify against FLASH slot (includes Capture Image)
#define API_CAPTURE_AND_IDENTIFY_FLASH	0x84	//Identify against all FLASH slots (includes Capture Image)
#define API_ENROL_RAM					0x85	//Enrol into RAM
#define API_VERIFY_RAM					0x86	//Verify against RAM
#define API_VERIFY_FLASH				0x87	//Verify against FLASH slot
#define API_IDENTIFY_FLASH				0x88	//Identify against all FLASH slots
#define API_CAPTURE_IMAGE_FINGERPRESENT 0x89
#define API_ENROL_FLASH					0x92
#define API_CAPTURE_AND_ENROL_FLASH     0x93

///////////////////////////////////////////////////////////////////////
//		Image transfer functions		

#define API_UPLOAD_IMAGE				0x90	//Upload image from RAM
#define API_DOWNLOAD_IMAGE				0x91	//Download image to RAM

////////////////////////////////////////////////////////////
//		Template handling		

#define API_UPLOAD_TEMPLATE				0xA0	//Upload template from RAM
#define API_DOWNLOAD_TEMPLATE			0xA1	//Download template to RAM
#define API_COPY_TEMPLATE_RAM_TO_FLASH	0xA2	//Copy template from RAM to permanent FLASH storage
#define API_UPLOAD_TEMPLATE_FROM_FLASH	0xA3	//Upload template from FLASH
#define API_DELETE_TEMPLATE_RAM			0xA4	//Erase template from RAM
#define API_DELETE_SLOT_IN_FLASH		0xA5	//Delete slot X in FLASH
#define API_DELETE_ALL_IN_FLASH			0xA6	//Delete all FLASH slots
#define API_DOWNLOAD_TEMPLATE_TO_FLASH  0xA7

//////////////////////////////////////////////////////////////
//		Algorithm settings	

#define API_SECURITY_LEVEL_RAM			0xB0	//Set security level, setting saved in RAM
#define API_SECURITY_LEVEL_STATIC		0xB1	//Set security level, setting saved in static memory

//////////////////////////////////////////////////////////////
//		Firmware commands		

#define API_FIRMWARE_VERSION			0xC0	//Upload the version string for this device
#define API_FIRMWARE_UPDATE				0xC1	//Start download of new firmware
#define API_FIRMWARE_VERSION_BOOT		0xC2	//Upload version string for boot
#define API_FIRMWARE_VERSION_FPC2000	0xC3	//Upload version string for FPC2000
#define API_FIRMWARE_INITIATE_UPGRADE   0xC4

///////////////////////////////////////////////////////////////////////////////////
//		Communication commands		

#define API_SET_BAUD_RATE_RAM			0xD0	//Set baud rate, setting saved in RAM
#define API_SET_BAUD_RATE_STATIC		0xD1	//Set baud rate, setting saved in static memory
#define API_TEST_HARDWARE				0xD2	//Test hardware components

///////////////////////////////////////////////////////////////////////////////////
//		Other commands		

#define API_CANCEL						0xE0	//Cancel ongoing command (valid for  API_CAPTURE_+_ENROL_RAM, API_CAPTURE_+_VERIFY_RAM, API_CAPTURE_+_VERIFY_AGAINST_FLASH, API_CAPTURE_+_IDENTIFY_AGAINST_FLASH)
#define API_ENTER_SLEEP_MODE			0xE1	//Enter sleep mode (wake up by activating INT-pin)
#define API_POWER_SAVE_MODE_RAM			0xE2	//Set power save mode (1=ON, 0=OFF) , setting saved in RAM
#define API_POWER_SAVE_MODE_STATIC		0xE3	//Set power save mode (1=ON, 0=OFF), setting saved in static memory
#define API_START_APPLICATION			0xE4	//Start application
#define API_GET_POWER_SAVE_MODE         0xE5    //Get currently used power mode..
#define API_RUN_SUPPLIED_PRG			0xE6    //Sends a complete program to the FPC2020 Bootcode, that will be started automatically.
#define API_INIT_SFR_REGS				0xE7	//Initilized Special Function Registers on the 8051 Processor
#define API_ADVANCED_SETTINGS			0xE8	//Controls the Low Voltage Check of the system.

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Response byte values					HEX value
#define API_FAILURE						0x00
#define API_SUCCESS						0x01
#define API_NO_FINGER_PRESENT			0x02
#define API_FINGER_PRESENT				0x03
#define API_VERIFICATION_OK				0x04
#define API_VERIFICATION_FAIL			0x05
#define API_ENROL_OK					0x06
#define API_ENROL_FAIL					0x07
#define API_HW_TEST_OK					0x08
#define API_HW_TEST_FAIL				0x09
#define API_CRC_FAIL					0x0A
#define API_PAYLOAD_TOO_LONG			0x0B
#define API_PAYLOAD_TOO_SHORT			0x0C
#define API_UNKNOWN_COMMAND				0x0D
#define API_NO_TEMPLATE_PRESENT			0x0E
#define API_IDENTIFY_OK					0x0F
#define API_IDENTIFY_FAIL				0x10
#define API_INVALID_SLOT_NR				0x11
#define API_CANCEL_SUCCESS				0x12
#define API_APPL_ALREADY_STARTED		0x13
#define API_APPL_CRC_FAIL				0x14 // CRC check on the application section has failed.
#define API_IN_BUFFER_OVERFLOW			0x15 // The pause between the header to API_DOWNLOAD_IMAGE and
#define	API_SYS_CRC_FAIL				0x16 // CRC check on the system header in FPC2020 failed
#define API_LOW_VOLTAGE					0x17 // The Power Source to FPC2020 is not delivering enough Voltage. Replace battery or other power source.