/*=========================================================================
| Cheetah Interface Library
|--------------------------------------------------------------------------
| Copyright (c) 2004-2006 Total Phase, Inc.
| All rights reserved.
| www.totalphase.com
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions
| are met:
|
| - Redistributions of source code must retain the above copyright
|   notice, this list of conditions and the following disclaimer.
|
| - Redistributions in binary form must reproduce the above copyright
|   notice, this list of conditions and the following disclaimer in the
|   documentation and/or other materials provided with the distribution.
|
| - Neither the name of Total Phase, Inc. nor the names of its
|   contributors may be used to endorse or promote products derived from
|   this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
| LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
| FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
| COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
| BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
| LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
| ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
| POSSIBILITY OF SUCH DAMAGE.
|--------------------------------------------------------------------------
| To access Total Phase Cheetah devices through the API:
|
| 1) Use one of the following shared objects:
|      cheetah.so       --  Linux shared object
|          or
|      cheetah.dll      --  Windows dynamic link library
|
| 2) Along with the following language module:
|      cheetah.c/h      --  C/C++ API header file and interface module
 ========================================================================*/

#ifndef __cheetah_h__
#define __cheetah_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TOTALPHASE_DATA_TYPES
#define TOTALPHASE_DATA_TYPES
typedef unsigned char       u08;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long	    u64;
typedef signed   char       s08;
typedef signed   short      s16;
typedef signed   int        s32;
typedef signed   long	    s64;
#endif

/* Set the following macro to '1' for debugging */
#define CHEETAH_DEBUG 0


/*=========================================================================
| VERSION
 ========================================================================*/
#define CHEETAH_HEADER_VERSION  0x0100   /* v1.00 */


/*=========================================================================
| STATUS CODES
 ========================================================================*/
/*
 * All API functions return an integer which is the result of the
 * transaction, or a status code if negative.  The status codes are
 * defined as follows:
 */
enum CHEETAH_STATUS {
    CHEETAH_OK                      =    0,
    CHEETAH_UNABLE_TO_LOAD_LIBRARY  =   -1,
    CHEETAH_UNABLE_TO_LOAD_DRIVER   =   -2,
    CHEETAH_UNABLE_TO_LOAD_FUNCTION =   -3,
    CHEETAH_INCOMPATIBLE_LIBRARY    =   -4,
    CHEETAH_INCOMPATIBLE_DEVICE     =   -5,
    CHEETAH_INCOMPATIBLE_DRIVER     =   -6,
    CHEETAH_COMMUNICATION_ERROR     =   -7,
    CHEETAH_UNABLE_TO_OPEN          =   -8,
    CHEETAH_UNABLE_TO_CLOSE         =   -9,
    CHEETAH_INVALID_HANDLE          =  -10,
    CHEETAH_CONFIG_ERROR            =  -11,
    CHEETAH_UNKNOWN_PROTOCOL        =  -12,
    CHEETAH_STILL_ACTIVE            =  -13,
    CHEETAH_FUNCTION_NOT_AVAILABLE  =  -14,
    CHEETAH_OS_ERROR                =  -15,
    CHEETAH_SPI_WRITE_ERROR         = -100,
    CHEETAH_SPI_BATCH_EMPTY_QUEUE   = -101,
    CHEETAH_SPI_BATCH_SHORT_BUFFER  = -102
};
#ifndef __cplusplus
typedef enum CHEETAH_STATUS CHEETAH_STATUS;
#endif


/*=========================================================================
| GENERAL TYPE DEFINITIONS
 ========================================================================*/
/*
 * Cheetah handle
 */
typedef int Cheetah;

/*
 * Cheetah version matrix.
 * 
 * This matrix describes the various version dependencies
 * of Cheetah components.  It can be used to determine
 * which component caused an incompatibility error.
 * 
 * All version numbers are of the format:
 *   (major << 8) | minor
 * 
 * ex. v1.20 would be encoded as:  0x0114
 *
 *
 * The fields in this struct are:
 *
 * Software and hardware versions.
 *    u16 software
 *    u16 hardware
 *
 * Hardware revisions that are compatible with this software version.
 * The top 16 bits gives the maximum accepted hw revision.
 * The lower 16 bits gives the minimum accepted hw revision.
 *    u32 hw_revs_for_sw
 *
 * Driver revisions that are compatible with this software version.
 * The top 16 bits gives the maximum accepted driver revision.
 * The lower 16 bits gives the minimum accepted driver revision.
 * This version checking is currently only pertinent for WIN32
 * platforms.
 *    u32 drv_revs_for_sw
 *
 * Software requires that the API interface must be >= this version.
 *    u16 api_req_by_sw
 */
struct CheetahVersion {
    u16 software;
    u16 hardware;
    u32 hw_revs_for_sw;
    u32 drv_revs_for_sw;
    u16 api_req_by_sw;
};
#ifndef __cplusplus
typedef struct CheetahVersion CheetahVersion;
#endif


/*=========================================================================
| GENERAL API
 ========================================================================*/
/*
 * Get a list of ports to which Cheetah devices are attached.
 * 
 * nelem   = maximum number of elements to return
 * devices = array into which the port numbers are returned
 * 
 * Each element of the array is written with the port number.
 * Devices that are in-use are ORed with CHEETAH_PORT_NOT_FREE
 * (0x8000).
 *
 * ex.  devices are attached to ports 0, 1, 2
 *      ports 0 and 2 are available, and port 1 is in-use.
 *      array => 0x0000, 0x8001, 0x0002
 * 
 * If the array is NULL, it is not filled with any values.
 * If there are more devices than the array size, only the
 * first nmemb port numbers will be written into the array.
 * 
 * Returns the number of devices found, regardless of the
 * array size.
 */
#define CHEETAH_PORT_NOT_FREE 0x8000
int cheetah_find_devices (
    int   nelem,
    u16 * devices
);


/*
 * Get a list of ports to which Cheetah devices are attached
 *
 * This function is the same as cheetah_find_devices() except that
 * it returns the unique IDs of each Cheetah device.  The IDs
 * are guaranteed to be non-zero if valid.
 *
 * The IDs are the unsigned integer representation of the 10-digit
 * serial numbers.
 */
int cheetah_find_devices_ext (
    int   nelem,
    u16 * devices,
    u32 * unique_ids
);


/*
 * Open the Cheetah port.
 * 
 * The port number is a zero-indexed integer.
 *
 * The port number is the same as that obtained from the
 * cheetah_find_devices() function above.
 * 
 * Returns an Cheetah handle, which is guaranteed to be
 * greater than zero if it is valid.
 * 
 * This function is recommended for use in simple applications
 * where extended information is not required.  For more complex
 * applications, the use of cheetah_open_ext() is recommended.
 */
Cheetah cheetah_open (
    int port_number
);


/*
 * Open the Cheetah port, returning extended information
 * in the supplied structure.  Behavior is otherwise identical
 * to cheetah_open() above.  If 0 is passed as the pointer to the
 * structure, this function is exactly equivalent to cheetah_open().
 * 
 * The structure is zeroed before the open is attempted.
 * It is filled with whatever information is available.
 * 
 * For example, if the hardware version is not filled, then
 * the device could not be queried for its version number.
 * 
 * This function is recommended for use in complex applications
 * where extended information is required.  For more simple
 * applications, the use of cheetah_open() is recommended.
 */
struct CheetahExt {
    CheetahVersion version;
    int            features;
};
#ifndef __cplusplus
typedef struct CheetahExt CheetahExt;
#endif

Cheetah cheetah_open_ext (
    int          port_number,
    CheetahExt * cheetah_ext
);


/*
 * Close the Cheetah port.
 */
int cheetah_close (
    Cheetah cheetah
);


/*
 * Return the unique ID for this Cheetah adapter.
 * IDs are guaranteed to be non-zero if valid.
 * The ID is the unsigned integer representation of the
 * 10-digit serial number.
 */
u32 cheetah_unique_id (
    Cheetah cheetah
);


/*
 * Return the status string for the given status code.
 * If the code is not valid or the library function cannot
 * be loaded, return a NULL string.
 */
const char * cheetah_status_string (
    int status
);


/*
 * Return the version matrix for the device attached to the
 * given handle.  If the handle is 0 or invalid, only the
 * software and required api versions are set.
 */
int cheetah_version (
    Cheetah          cheetah,
    CheetahVersion * version
);


/*
 * Sleep for the specified number of milliseconds
 * Accuracy depends on the operating system scheduler
 * Returns the number of milliseconds slept
 */
u32 cheetah_sleep_ms (
    u32 milliseconds
);


/*
 * Configure the target power pin.
 */
#define CHEETAH_TARGET_POWER_OFF 0x00
#define CHEETAH_TARGET_POWER_ON 0x01
#define CHEETAH_TARGET_POWER_QUERY 0x80
int cheetah_target_power (
    Cheetah cheetah,
    u08     power_flag
);


#define CHEETAH_HOST_IFCE_FULL_SPEED 0x00
#define CHEETAH_HOST_IFCE_HIGH_SPEED 0x01
int cheetah_host_ifce_speed (
    Cheetah cheetah
);



/*=========================================================================
| SPI API
 ========================================================================*/
/*
 * Set the SPI bit rate in kilohertz.
 */
int cheetah_spi_bitrate (
    Cheetah cheetah,
    int     bitrate_khz
);


/*
 * These configuration parameters specify how to clock the
 * bits that are sent and received on the Cheetah SPI
 * interface.
 * 
 * The polarity option specifies which transition
 * constitutes the leading edge and which transition is the
 * falling edge.  For example, CHEETAH_SPI_POL_RISING_FALLING
 * would configure the SPI to idle the SCK clock line low.
 * The clock would then transition low-to-high on the
 * leading edge and high-to-low on the trailing edge.
 * 
 * The phase option determines whether to sample or setup on
 * the leading edge.  For example, CHEETAH_SPI_PHASE_SAMPLE_SETUP
 * would configure the SPI to sample on the leading edge and
 * setup on the trailing edge.
 * 
 * The bitorder option is used to indicate whether LSB or
 * MSB is shifted first.
 *
 * The SS polarity option is to indicate the polarity of the
 * slave select pin (active high or active low).  Each of
 * the lower three bits of ss_polarity corresponds to each
 * of the SS lines.  Set the bit value for a given SS line
 * to 0 for active low or 1 for active high.
 */
enum CHEETAH_SPI_POLARITY {
    CHEETAH_SPI_POL_RISING_FALLING = 0,
    CHEETAH_SPI_POL_FALLING_RISING = 1
};
#ifndef __cplusplus
typedef enum CHEETAH_SPI_POLARITY CHEETAH_SPI_POLARITY;
#endif

enum CHEETAH_SPI_PHASE {
    CHEETAH_SPI_PHASE_SAMPLE_SETUP = 0,
    CHEETAH_SPI_PHASE_SETUP_SAMPLE = 1
};
#ifndef __cplusplus
typedef enum CHEETAH_SPI_PHASE CHEETAH_SPI_PHASE;
#endif

enum CHEETAH_SPI_BITORDER {
    CHEETAH_SPI_BITORDER_MSB = 0,
    CHEETAH_SPI_BITORDER_LSB = 1
};
#ifndef __cplusplus
typedef enum CHEETAH_SPI_BITORDER CHEETAH_SPI_BITORDER;
#endif

/*
 * Configure the SPI master interface
 */
int cheetah_spi_configure (
    Cheetah              cheetah,
    CHEETAH_SPI_POLARITY polarity,
    CHEETAH_SPI_PHASE    phase,
    CHEETAH_SPI_BITORDER bitorder,
    u08                  ss_polarity
);


/*
 * Clear the batch queue
 */
int cheetah_spi_queue_clear (
    Cheetah cheetah
);


/*
 * Enable / disable the master outputs
 */
int cheetah_spi_queue_oe (
    Cheetah cheetah,
    u08     oe
);


/*
 * Queue a delay in clock cycles
 * The return value is the actual number of cycles queued.
 */
int cheetah_spi_queue_delay_cycles (
    Cheetah cheetah,
    int     cycles
);


/*
 * Queue a delay in nanoseconds
 * The return value is the approximate number of nanoseconds queued.
 */
int cheetah_spi_queue_delay_ns (
    Cheetah cheetah,
    int     nanoseconds
);


/*
 * Assert the slave select lines.  Each of the lower three
 * bits of ss_polarity corresponds to each of the SS lines.
 * Set the bit value for a given SS line to 1 to assert the
 * line or 0 to deassert the line.  The polarity is determined
 * by cheetah_spi_configure() above.
 */
int cheetah_spi_queue_ss (
    Cheetah cheetah,
    u08     active
);


/*
 * Repeatedly send a single byte
 */
int cheetah_spi_queue_byte (
    Cheetah cheetah,
    int     count,
    u08     data
);


/*
 * Send a byte array.  Repeated bytes are automatically
 * optimized into a repeated byte sequence.
 */
int cheetah_spi_queue_array (
    Cheetah     cheetah,
    int         num_bytes,
    const u08 * data_out
);


/*
 * Calculate the expected length of the returned data
 */
int cheetah_spi_batch_length (
    Cheetah cheetah
);


/*
 * Perform the SPI shift operation.
 *
 * After the operation completes, the batch queue is untouched.
 * Therefore, this function may be called repeatedly in rapid
 * succession.
 */
int cheetah_spi_batch_shift (
    Cheetah cheetah,
    int     num_bytes,
    u08 *   data_in
);




#ifdef __cplusplus
}
#endif

#endif  /* __cheetah_h__ */
