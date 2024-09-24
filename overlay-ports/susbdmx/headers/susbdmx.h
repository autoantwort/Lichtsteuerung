/*
 * SUSBDMX.H -- include file for Peperoni-Lights SUSBDMX.DLL
 *   to communicate with Soundlight USBDMX-ONE usb to dmx512 interface.
 *
 * Copyright (C) 2009 by Dr. Jan Menzel
 * All rights reserved 
 */

#ifndef SUSBDMX_H
#define SUSBDMX_H

#ifdef __cplusplus 
extern "C" {
#endif

// define the dll space this file is used in
#ifdef SUSBDMX_DLL_EXPORT
#define SUSBDMX_TYPE	__declspec(dllexport) __stdcall
#else
#define SUSBDMX_TYPE	__declspec(dllimport) __stdcall
#endif

/*
 * DLL version
 */
static const USHORT SUSBDMX_DLL_VERSION = 0x0406;

/*
 * MACRO to verify dll version
 */
#define SUSBDMX_DLL_VERSION_CHECK() (susbdmx_version() >= SUSBDMX_DLL_VERSION)

/* **************************************
 * functions defined in the susbdmx.dll *
 ****************************************/

/*
 *   susbdmx_version(): returns the version number (16bit, 4 digits BCD)
 * Current version is SUSBDMX_DLL_VERSION. Use the Macro 
 * SUSBDMX_DLL_VERSION_CHECK() compare dll's and header files version.
 */
USHORT	SUSBDMX_TYPE	susbdmx_version();

/*
 *   susbdmx_open(): open device number <device>, where 0 is the first
 * and unlimit devices are supported. The function returnes FALSE
 * if <device> is not supported. Use returned handle h to access the 
 * device later on. One device can be opened an unlimited number of times.
 */
BOOL	SUSBDMX_TYPE	susbdmx_open(USHORT device, PHANDLE h);

/*
 *   susbdmx_close(): close the device identified by the given handle.
 */
BOOL	SUSBDMX_TYPE	susbdmx_close(HANDLE h);

/*
 *   susbdmx_device_id(): read the device id of the device
 */
BOOL	SUSBDMX_TYPE	susbdmx_device_id(HANDLE h, PUSHORT pid);

/*
 *   susbdmx_is_XXX(): identify the device identified by the given handle.
 * Each function returns TRUE if the device matches.
 */
BOOL	SUSBDMX_TYPE	susbdmx_is_usbdmxone(HANDLE h);
BOOL	SUSBDMX_TYPE	susbdmx_is_usbdmxtwo(HANDLE h);

/*
 *   susbdmx_product_get(): read the product string from the device.
 * size specifies the maximum size of the buffer pointed to by <string> 
 * (unit bytes).
 */
BOOL	SUSBDMX_TYPE	susbdmx_product_get(HANDLE h, PWCHAR string, USHORT size);

/*
 *   susbdmx_serial_get(): read the serial number from the device.
 * size specifies the maximum size of the buffer pointed to by <string> 
 * (unit bytes).
 */
BOOL	SUSBDMX_TYPE	susbdmx_serial_get(HANDLE h, PWCHAR string, USHORT size);

/*
 *   susbdmx_device_version(): Read the the device version of a device.
 * the device version is one of the values within the USBs configuration
 * descriptor (BcdDevice). pversion is only valid if the function returns
 * TRUE.
 */
BOOL	SUSBDMX_TYPE	susbdmx_device_version(HANDLE h, PUSHORT pversion);

/*
 *   susbdmx_tx(): transmitt a frame using the new protocol on bulk endpoints
 *
 * INPUTs:	h			- handle to the device, returned by usbdmx_open()
 *			universe	- addressed universe
 *			slots		- number of bytes to be transmitted, as well as sizeof(buffer)
 *						  for DMX512: buffer[0] == startcode, slots <= 513
 *			buffer		- data to be transmitted,  !!! sizeof(buffer) >= slots !!!
 *			config		- configuration of the transmitter, see below for possible values
 *			time		- time value in s, depending on config, either timeout or delay
 *			time_break	- break time in s (can be zero, to not transmitt a break)
 *			time_mab	- Mark-after-Break time (can be zero)
 * OUTPUTs:	ptimestamp	- timestamp of this frame in ms, does overrun
 *			pstatus		- status of this transmission, see below for possible values
 */
BOOL	SUSBDMX_TYPE	susbdmx_tx(	IN HANDLE h, IN UCHAR universe, IN USHORT slots, 
									IN PUCHAR buffer, IN UCHAR config, IN FLOAT time, 
									IN FLOAT time_break, IN FLOAT time_mab, 
									OUT PUSHORT ptimestamp, OUT PUCHAR pstatus);
/*
 * values of config (to be ored together)
 */
#define SUSBDMX_BULK_CONFIG_DELAY	(0x01)	// delay frame by time
#define SUSBDMX_BULK_CONFIG_BLOCK	(0x02)	// block while frame is not transmitting (timeout given by time)
#define SUSBDMX_BULK_CONFIG_RX		(0x04)	// switch to RX after having transmitted this frame
#define SUSBDMX_BULK_CONFIG_NORETX	(0x08)	// do not retransmit this frame
#define SUSBDMX_BULK_CONFIG_TXIRQ	(0x40)	// send data using transmitter IRQ instead of timer

/*
 *   susbdmx_rx(): receive a frame using the new protocol on bulk endpoints
 *
 * INPUTs:	h			- handle to the device, returned by usbdmx_open()
 *			universe	- addressed universe
 *			slots_set	- number of bytes to receive, as well as sizeof(buffer)
 *						  for DMX512: buffer[0] == startcode, slots_set <= 513
 *			buffer		- data to be transmitted,  !!! sizeof(buffer) >= slots !!!
 *			timeout		- timeout for receiving the total frame in s,
 *			timeout_rx	- timeout between two slots used to detect premature end of frames
 * OUTPUTs:	pslots_get	- number of slots actually received, *pslots_get <= slots_set
 *          ptimestamp	- timestamp of this frame in ms, does overrun
 *			pstatus		- status of the reception, see below for possible values
 */
BOOL	SUSBDMX_TYPE	susbdmx_rx(	IN HANDLE h, IN UCHAR universe, IN USHORT slots_set, 
									IN PUCHAR buffer, IN FLOAT timeout, IN FLOAT timeout_rx,
									OUT PUSHORT pslots_get, OUT PUSHORT ptimestamp, OUT PUCHAR pstatus);

/*
 * values of *pstatus
 */
#define SUSBDMX_BULK_STATUS_OK				(0x00)
#define SUSBDMX_BULK_STATUS_TIMEOUT			(0x01)	// request timed out	
#define SUSBDMX_BULK_STATUS_TX_START_FAILED	(0x02)	// delayed start failed
#define SUSBDMX_BULK_STATUS_UNIVERSE_WRONG	(0x03)	// wrong universe addressed\tabularnewline
#define SUSBDMX_BULK_STATUS_RX_RDM_LENGTH	(0x08)	// rdm length decoded
#define SUSBDMX_BULK_STATUS_RX_OLD_FRAME	(0x10)	// old frame not read
#define SUSBDMX_BULK_STATUS_RX_TIMEOUT		(0x20)	// receiver finished with timeout (ored with others)
#define SUSBDMX_BULK_STATUS_RX_NO_BREAK		(0x40)	// frame without break received (ored with others)
#define SUSBDMX_BULK_STATUS_RX_FRAMEERROR	(0x80)	// frame finished with frame error (ored with others)

/*
 * macro to check, it the return status is ok
 */
#define SUSBDMX_BULK_STATUS_IS_OK(s) (s == SUSBDMX_BULK_STATUS_OK)

/*
 *   susbdmx_id_led_XXX(): get/set the "id-led", the way the TX-led is handled:
 * special value: see below
 * other:         the blue led blinks the given number of times and then pauses
 */
BOOL	SUSBDMX_TYPE	susbdmx_id_led_set(HANDLE h, UCHAR id);
BOOL	SUSBDMX_TYPE	susbdmx_id_led_get(HANDLE h, PUCHAR id);

/*
 * special values of id
 */
#define SUSBDMX_ID_LED_USB		(0xff)	// display the USB status: blink with 2Hz on USB transactions
#define SUSBDMX_ID_LED_USB_RX	(0xfe)	// display USB and receiver status. the LED blinks red if not valid dmx signal in received

/******************************************************************************
 * High-Level functions facilitating the above
 */

/*
 * SUSBDMX_DMXTX - send dmx data
 *
 * INPUTs:	h			- handle to the device, returned by usbdmx_open()
 *			universe	- addressed universe
 *			slots		- number of bytes to transmitt without startcode, as well as sizeof(buffer)
 *			buffer		- data to be transmitted without startcode,  !!! sizeof(buffer) >= slots !!!
 * RETURN:	== 0		- data transmitted
 *			!= 0		- data not transmitted
 */
int SUSBDMX_TYPE susbdmx_dmxtx(HANDLE h, UCHAR universe, USHORT slots, const void *buffer);

/*
 * SUSBDMX_DMXRX - receive dmx data with startcode 0
 *
 * INPUTs:	h			- handle to the device, returned by usbdmx_open()
 *			universe	- addressed universe
 *			buffersize	- size of buffer
 *			buffer		- received data without startcode
 * RETURN:	== 0		- no data received, or data with non-DMX startcode
 *			>  0		- amount of received data
 *			<  0		- reception error
 */
int SUSBDMX_TYPE susbdmx_dmxrx(HANDLE h, UCHAR universe, USHORT buffersize, void *buffer);
// return values for dmxrx and tx
enum {
	SUSBDMX_DMX_INPUT			= -1,	// input parameter wrong
	SUSBDMX_DMX_TX				= -10,	// transmission failed
	SUSBDMX_DMX_UNIVERSE		= -11,	// universe number wrong
	SUSBDMX_DMX_RX				= -20,	// reception failed
	SUSBDMX_DMX_RX_NOBREAK		= -23,	// reception failed with no break
};


/*
 * SUSBDMX_RDM - exchange rdm data
 *
 * INPUTs:	h				- handle to the device, returned by usbdmx_open()
 *			universe		- addressed universe
 *			txlen			- number of bytes to transmitt
 *			txbuffer		- data to be transmitted,  !!! sizeof(buffer) >= txlen !!!
 *							  START code, Sub START Code and Message Length are inserted befor,
 *							  Checksum is appended behind the data.
 *          rxsize          - size of receive buffer (set to 0 if not data is to be received)
 *          rxbuffer        - receive buffer
 *                            Checksum is validated and stipped
 *                            START code, Sub START code and length are validated and stipped
 * RETURN:	> 0				- length of received data without START codes, length and Checksum
 *                            (can be larger than rxsize if rxsize was to small)
 *			< 0				- handling/reception error
 */
int SUSBDMX_TYPE susbdmx_rdm(HANDLE h, UCHAR universe, UCHAR txlen, const void *txbuffer, UCHAR rxsize, void *rxbuffer);

/*
 * SUSBDMX_RDMDISCOVERY - exchange rdm discovery data (handle special response without break)
 *
 * INPUTs:	h				- handle to the device, returned by usbdmx_open()
 *			universe		- addressed universe
 *			txlen			- number of bytes to transmitt
 *			txbuffer		- data to be transmitted,  !!! sizeof(buffer) >= txlen !!!
 *							  START code, Sub START Code and Message Length are inserted befor,
 *							  Checksum is appended behind the data.
 *          rxsize          - size of receive buffer
 *          rxbuffer        - receive buffer, contains received UIDs
 * RETURN:	> 0				- length of UIDs (can be larger than rxsize if rxsize was to small to fit all uids)
 *			< 0				- handling/reception error
 */
int SUSBDMX_TYPE susbdmx_rdmdiscovery(HANDLE h, UCHAR universe, UCHAR txlen, const void *txbuffer, UCHAR rxsize, void *rxbuffer);

// return values for rdm() and rdmdiscovery():
enum {
	SUSBDMX_RDM_INPUT			= -1,	// input parameter wrong
	SUSBDMX_RDM_TX				= -10,	// transmission failed
	SUSBDMX_RDM_UNIVERSE		= -11,	// universe number wrong
	SUSBDMX_RDM_RX				= -20,	// reception failed
	SUSBDMX_RDM_RX_TIMEOUT		= -21,	// reception failed with timed out
	SUSBDMX_RDM_RX_FRAMEERROR	= -22,	// reception failed with frame error
	SUSBDMX_RDM_RX_NOBREAK		= -23,	// reception failed with no break
	SUSBDMX_RDM_RX_LENGTH		= -24,	// reception failed with length error (received less data than expected)
	SUSBDMX_RDM_RX_STARTCODE	= -25,	// wrong startcode received
	SUSBDMX_RDM_RX_SUBSTARTCODE	= -26,	// wrong sub-startcode received
	SUSBDMX_RDM_RX_CHECKSUM		= -27,	// reception failed with checksum error
	SUSBDMX_RDM_COLLISION		= -30	// collision detected
};

#ifdef __cplusplus 
}
#endif

#endif // SUSBDMX_H
