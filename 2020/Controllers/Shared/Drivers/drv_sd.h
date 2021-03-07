
#ifndef DRV_SD_H
#define	DRV_SD_H

#include <stdint.h>
#include <unistd.h>
#include "drv_spi.h"

//use drv_spi_transfer function to interact with SD card. It does synchronous in and out reading

//drv_spi_channel enum at: datalogger/drivers/drv_spi_private.h and .c
//add SD card channel, edit C file to set pins
//change some pins on drv_spi_private per PDM page 3
//DRV_SPI_CHANNEL_SD is value from drv_spi_channel enum for use in calls to drv_spi_transfer()

/* 
HOW THIS WORKS
    * Five FatFs functions integrate SPI implementation with the rest of the code
    * Use command indices and drv_spi_transfer() to actually accomplish moving around of data based on those FatFs functions
    * Change drv_spi_private.h and .c to create and set up channel for DRV_SPI_CHANNEL_SD
*/

#include "ff.h"
#include "diskio.h"

#define SD_SECTOR_SIZE 512
#define SD_TIMEOUT_BYTES 4000
#define SD_RECEIVE_START 0xfe
#define SD_BIGGEST_SECTOR (UINT)(256000000000/SD_SECTOR_SIZE)

/* Status of Disk Functions */
//typedef BYTE	DSTATUS;

/* Results of Disk Functions */
// typedef enum {
// 	RES_OK = 0,		/* 0: Successful */
// 	RES_ERROR,		/* 1: R/W Error */
// 	RES_WRPRT,		/* 2: Write Protected */
// 	RES_NOTRDY,		/* 3: Not Ready */
// 	RES_PARERR		/* 4: Invalid Parameter */
// } DRESULT;


// Tying command indices to numbers 
enum cmd_ind {
    CMD0 =   0,  // Software reset
    CMD1 =   1,  // Initiate initialization process
    ACMD41 = 41,  // For only SDC. Initiate initialization process
    CMD8 =   8,  // For only SDC V2. Check voltage range
    CMD9 = 9,           // Read CSD register
    //CMD10 = 10,         // Read CID register
    CMD12 =  12,         // Stop to read data
    CMD16 =  16,  // Change R/W block size
    CMD17 =  17,         // Read block
    CMD18 =  18,         // Read multiple blocks
    //CMD23 = 23,         // DON'T USE - for MMC only. Define number of blocks for next R/W command
    //ACMD23 = 23,        // For only SDC. Define number of blocks to pre-erase with next multi-block write command
    CMD24 = 24,         // Write a block
    CMD25 = 25,         // Write multiple blocks
	CMD32 = 32,		// erase start addr
	CMD33 = 33,		// erase end addr
	CMD38 = 38,		// start erase
    CMD55 = 55,         // Leading command of ACMD<n> command
    CMD58 = 58    // Read OCR
};

struct sd_reg_csd {
#if 0
	uint32_t		: 1;
	uint32_t crc	: 7;
	uint32_t		: 2;
	uint32_t file_format	: 2;
	uint32_t tmp_write_protect	: 1;
	uint32_t perm_write_protect	: 1;
	uint32_t copy				: 1;
	uint32_t file_format_grp	: 1;
	uint32_t					: 5;
	uint32_t write_bl_partial	: 1;
	uint32_t write_bl_len		: 4;
	uint32_t r2w_factor			: 3;
	uint32_t					: 2;
	uint32_t wp_grp_enable		: 1;
	uint32_t wp_grp_size		: 7;
	uint32_t sector_size		: 7;
	uint32_t erase_blk_en		: 1;
	uint32_t c_size_mult		: 3;
	uint32_t vdd_w_curr_max		: 3;
	uint32_t vdd_w_curr_min		: 3;
	uint32_t vdd_r_curr_max		: 3;
	uint32_t vdd_r_curr_min		: 3;
	uint32_t c_size				: 12;
	uint32_t					: 2;
	uint32_t dsr_imp			: 1;
	uint32_t read_blk_misalign	: 1;
	uint32_t write_blk_misalign	: 1;
	uint32_t read_bl_partial	: 1;
	uint32_t read_bl_len		: 4;
	uint32_t ccc				: 12;
	uint32_t tran_speed			: 8;
	uint32_t nsac				: 8;
	uint32_t taac				: 8;
	uint32_t					: 6;
	uint32_t csd_structure		: 2;
#else
	uint32_t csd_structure		: 2;
	uint32_t					: 6;
	uint32_t taac				: 8;
	uint32_t nsac				: 8;
	uint32_t tran_speed			: 8;
	uint32_t ccc				: 12;
	uint32_t read_bl_len		: 4;
	uint32_t read_bl_partial	: 1;
	uint32_t write_blk_misalign	: 1;
	uint32_t read_blk_misalign	: 1;
	uint32_t dsr_imp			: 1;
	uint32_t					: 6;
	uint32_t c_size				: 22;
	uint32_t					: 1;
	uint32_t erase_blk_en		: 1;
	uint32_t sector_size		: 7;
	uint32_t wp_grp_size		: 7;
	uint32_t wp_grp_enable		: 1;
	uint32_t					: 2;
	uint32_t r2w_factor			: 3;
	uint32_t write_bl_len		: 4;
	uint32_t write_bl_partial	: 1;
	uint32_t					: 5;
	uint32_t file_format_grp	: 1;
	uint32_t copy				: 1;
	uint32_t perm_write_protect	: 1;
	uint32_t tmp_write_protect	: 1;
	uint32_t file_format		: 2;
	uint32_t					: 2;
	uint32_t crc				: 7;
	uint32_t alwaysone			: 1;
#endif
};

union sd_csd {
	uint8_t bytes[16];
	struct {
		uint32_t csd_structure		: 2;
		uint32_t					: 6;
		uint32_t taac				: 8;
		uint32_t nsac				: 8;
		uint32_t tran_speed			: 8;
		uint32_t ccc				: 12;
		uint32_t read_bl_len		: 4;
		uint32_t read_bl_partial	: 1;
		uint32_t write_blk_misalign	: 1;
		uint32_t read_blk_misalign	: 1;
		uint32_t dsr_imp			: 1;
		uint32_t					: 2;
		uint32_t c_size				: 12;
		uint32_t vdd_r_curr_min		: 3;
		uint32_t vdd_r_curr_max		: 3;
		uint32_t vdd_w_curr_min		: 3;
		uint32_t vdd_w_curr_max		: 3;
		uint32_t c_size_mult		: 3;
		uint32_t erase_blk_en		: 1;
		uint32_t sector_size		: 7;
		uint32_t wp_grp_size		: 7;
		uint32_t wp_grp_enable		: 1;
		uint32_t					: 2;
		uint32_t r2w_factor			: 3;
		uint32_t write_bl_len		: 4;
		uint32_t write_bl_partial	: 1;
		uint32_t					: 5;
		uint32_t file_format_grp	: 1;
		uint32_t copy				: 1;
		uint32_t perm_write_protect	: 1;
		uint32_t tmp_write_protect	: 1;
		uint32_t file_format		: 2;
		uint32_t					: 2;
		uint32_t crc				: 7;
		uint32_t					: 1;
	} v1;
	struct {
		uint32_t					: 30;
		uint32_t csd_structure		: 2;
		
	} v2;
};

#if 0
		uint32_t csd_structure		: 2;
		uint32_t					: 6;
		uint32_t taac				: 8;
		uint32_t nsac				: 8;
		uint32_t tran_speed			: 8;
		uint32_t ccc				: 12;
		uint32_t read_bl_len		: 4;
		uint32_t read_bl_partial	: 1;
		uint32_t write_blk_misalign	: 1;
		uint32_t read_blk_misalign	: 1;
		uint32_t dsr_imp			: 1;
		uint32_t					: 6;
		uint32_t c_size				: 22;
		uint32_t					: 1;
		uint32_t erase_blk_en		: 1;
		uint32_t sector_size		: 7;
		uint32_t wp_grp_size		: 7;
		uint32_t wp_grp_enable		: 1;
		uint32_t					: 2;
		uint32_t r2w_factor			: 3;
		uint32_t write_bl_len		: 4;
		uint32_t write_bl_partial	: 1;
		uint32_t					: 5;
		uint32_t file_format_grp	: 1;
		uint32_t copy				: 1;
		uint32_t perm_write_protect	: 1;
		uint32_t tmp_write_protect	: 1;
		uint32_t file_format		: 2;
		uint32_t					: 2;
		uint32_t crc				: 7;
		uint32_t alwaysone			: 1;

#endif
#endif