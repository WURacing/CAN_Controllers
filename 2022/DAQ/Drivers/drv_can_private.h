#pragma once

#include "sam.h"

#include "vehicle.h"
#include "pe3.h"

#define ENABLE_CAN0 1
#define ENABLE_CAN1 1

#define CAN0_TX_PIN PIN_PA24
#define CAN0_TX_MUX MUX_PA24G_CAN0_TX
#define CAN0_RX_PIN PIN_PA25
#define CAN0_RX_MUX MUX_PA25G_CAN0_RX

#define CAN1_TX_PIN PIN_PB10
#define CAN1_TX_MUX MUX_PB10G_CAN1_TX
#define CAN1_RX_PIN PIN_PB11
#define CAN1_RX_MUX MUX_PB11G_CAN1_RX

#define CAN0_RX_FIFO_0_NUM 0 /* receive FIFO, used for messages not matched by a filter into a buffer */
#define CAN0_RX_FIFO_0_OPERATION_MODE 1 /* 1: overwrite mode */
#define CAN0_RX_FIFO_0_HIGH_WATER_INT_LEVEL 0 /* 0: interrupt disabled */
#define CAN1_RX_FIFO_0_NUM 0 /* receive FIFO, used for messages not matched by a filter into a buffer */
#define CAN1_RX_FIFO_0_OPERATION_MODE 1 /* 1: overwrite mode */
#define CAN1_RX_FIFO_0_HIGH_WATER_INT_LEVEL 0 /* 0: interrupt disabled */
#define CAN_RX_FIFO_0_DATA_SIZE CAN_RXESC_F0DS_DATA8_Val /* >8 if CAN_FD */

#define CAN0_RX_FIFO_1_NUM 0
#define CAN0_RX_FIFO_1_OPERATION_MODE 1 /* 1: overwrite mode */
#define CAN0_RX_FIFO_1_HIGH_WATER_INT_LEVEL 0 /* 0: interrupt disabled */
#define CAN1_RX_FIFO_1_NUM 0
#define CAN1_RX_FIFO_1_OPERATION_MODE 1 /* 1: overwrite mode */
#define CAN1_RX_FIFO_1_HIGH_WATER_INT_LEVEL 0 /* 0: interrupt disabled */
#define CAN_RX_FIFO_1_DATA_SIZE CAN_RXESC_F1DS_DATA8_Val /* >8 if CAN_FD */

#define CAN_RX_BUFFERS_DATA_SIZE CAN_RXESC_F1DS_DATA8_Val /* >8 if CAN_FD */
#define CAN_TX_DATA_SIZE CAN_TXESC_TBDS_DATA8_Val

// No CAN messages to receive for now
enum drv_can_rx_buffer_table {		
	DRV_CAN_RX_BUFFER_PE3_PE01,
//	DRV_CAN_RX_BUFFER_PE3_PE02,
//	DRV_CAN_RX_BUFFER_PE3_PE03,
//	DRV_CAN_RX_BUFFER_PE3_PE04,
//	DRV_CAN_RX_BUFFER_PE3_PE05,
//	DRV_CAN_RX_BUFFER_PE3_PE06,
//	DRV_CAN_RX_BUFFER_PE3_PE07,
//	DRV_CAN_RX_BUFFER_PE3_PE08,
//	DRV_CAN_RX_BUFFER_PE3_PE09,
//	DRV_CAN_RX_BUFFER_PE3_PE10,
//	DRV_CAN_RX_BUFFER_PE3_PE11,
//	DRV_CAN_RX_BUFFER_PE3_PE12,
//	DRV_CAN_RX_BUFFER_PE3_PE13,
//	DRV_CAN_RX_BUFFER_PE3_PE14,
//	DRV_CAN_RX_BUFFER_PE3_PE15,
//	DRV_CAN_RX_BUFFER_PE3_PE16,
	DRV_CAN_RX_BUFFER_VEHICLE_SB_FRONT1_SIGNALS1,
	DRV_CAN_RX_BUFFER_COUNT
};

// Send two messages with sensor signal information
enum drv_can_tx_buffer_table {
	DRV_CAN_TX_BUFFER_COUNT
};

#define CAN0_STANDARD_FILTERS_NUM 1
#define CAN0_EXTENDED_FILTERS_NUM 1

#define CAN1_STANDARD_FILTERS_NUM 0
#define CAN1_EXTENDED_FILTERS_NUM 1

#define VEHICLE_CAN	CAN0
#define PE3_CAN		CAN1 

#define CAN0_STBY_PIN PIN_PA18
#define CAN1_STBY_PIN PIN_PA19