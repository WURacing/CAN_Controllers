#pragma once

#include "sam.h"

#define ENABLE_CAN0 1
#define ENABLE_CAN1 0

#define CAN0_TX_PIN PIN_PA24
#define CAN0_TX_MUX MUX_PA24G_CAN0_TX
#define CAN0_RX_PIN PIN_PA25
#define CAN0_RX_MUX MUX_PA25G_CAN0_RX

#define CAN0_RX_FIFO_0_NUM 0 /* receive FIFO, used for messages not matched by a filter into a buffer */
#define CAN0_RX_FIFO_0_OPERATION_MODE 1 /* 1: overwrite mode */
#define CAN0_RX_FIFO_0_HIGH_WATER_INT_LEVEL 0 /* 0: interrupt disabled */
#define CAN_RX_FIFO_0_DATA_SIZE CAN_RXESC_F0DS_DATA8_Val /* >8 if CAN_FD */

#define CAN0_RX_FIFO_1_NUM 0
#define CAN0_RX_FIFO_1_OPERATION_MODE 1 /* 1: overwrite mode */
#define CAN0_RX_FIFO_1_HIGH_WATER_INT_LEVEL 0 /* 0: interrupt disabled */
#define CAN_RX_FIFO_1_DATA_SIZE CAN_RXESC_F1DS_DATA8_Val /* >8 if CAN_FD */

#define CAN_RX_BUFFERS_DATA_SIZE CAN_RXESC_F1DS_DATA8_Val /* >8 if CAN_FD */
#define CAN_TX_DATA_SIZE CAN_TXESC_TBDS_DATA8_Val

enum drv_can_rx_buffer_table {	
	DRV_CAN_RX_BUFFER_CAN0_PE1,
	DRV_CAN_RX_BUFFER_CAN0_PE2,
	DRV_CAN_RX_BUFFER_CAN0_PE3,
	DRV_CAN_RX_BUFFER_CAN0_PE4,
	DRV_CAN_RX_BUFFER_CAN0_PE5,
	DRV_CAN_RX_BUFFER_CAN0_PE6,
	DRV_CAN_RX_BUFFER_CAN0_PE7,
	DRV_CAN_RX_BUFFER_CAN0_PE8,
	DRV_CAN_RX_BUFFER_CAN0_PE9,
	DRV_CAN_RX_BUFFER_CAN0_PE10,
	DRV_CAN_RX_BUFFER_CAN0_PE11,
	DRV_CAN_RX_BUFFER_CAN0_PE12,
	DRV_CAN_RX_BUFFER_CAN0_PE13,
	DRV_CAN_RX_BUFFER_CAN0_PE14,
	DRV_CAN_RX_BUFFER_CAN0_PE15,
	DRV_CAN_RX_BUFFER_CAN0_PE16,
	DRV_CAN_RX_BUFFER_CAN0_CG_Sensor,
	DRV_CAN_RX_BUFFER_CAN0_Dash_Stat,
	DRV_CAN_RX_BUFFER_CAN0_FL_Upright,
	DRV_CAN_RX_BUFFER_CAN0_FR_Upright,
	DRV_CAN_RX_BUFFER_CAN0_RL_Upright,
	DRV_CAN_RX_BUFFER_CAN0_RR_Upright,
	DRV_CAN_RX_BUFFER_CAN0_Front_Sensor,
	DRV_CAN_RX_BUFFER_CAN0_Rear_Sensor,
	DRV_CAN_RX_BUFFER_CAN0_GPS_POS,
	DRV_CAN_RX_BUFFER_CAN0_PDM_Current_A,
	DRV_CAN_RX_BUFFER_CAN0_PDM_Current_B,
	DRV_CAN_RX_BUFFER_CAN0_PDM_PWR,
	
	DRV_CAN_RX_BUFFER_COUNT
};

enum drv_can_tx_buffer_table {
	DRV_CAN_TX_BUFFER_CAN0_GPS_time,
	DRV_CAN_TX_BUFFER_CAN0_GPS_POS,
	DRV_CAN_TX_BUFFER_CAN0_Telem_Stat,
	
	DRV_CAN_TX_BUFFER_COUNT
};

#define CAN0_STANDARD_FILTERS_NUM 0
#define CAN0_EXTENDED_FILTERS_NUM DRV_CAN_RX_BUFFER_COUNT