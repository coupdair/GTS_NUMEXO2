#ifndef XMMREGS_L_H
#define XMMREGS_L_H

#define CARD_NUMBER_ADDRESS ( &cardNumber )

/* offsets defined in opb_mmregs */
#define XMMR_GT_CTRL_OFFSET                 0x00
#define XMMR_GT_DRP_ADDR_CTRL_OFFSET        0x04
#define XMMR_GT_DRP_CTRL_OFFSET             0x08
#define XMMR_MGT_REGDATA_CTRL_OFFSET        0x0C
#define XMMR_LMK_PLL_CTRL_OFFSET            0x10
#define XMMR_FINE_DELAY_CTRL_OFFSET         0x14
#define XMMR_DP_DELAY_CTRL_OFFSET           0x18
#define XMMR_DP_MUX_CTRL_OFFSET             0x1C
#define XMMR_MGT_SEL_CTRL_OFFSET            0x20 /* NOT USED BUT STILL IN MEMORY AT THIS ADDRESS */
#define XMMR_CLOCKPATH_CTRL_OFFSET          0x24
#define XMMR_TRIGGER_CTRL_OFFSET            0x28
#define XMMR_I2CGTS_OUTPUT_CTRL_OFFSET      0x2C
#define XMMR_LMK_PLL_STATUS_OFFSET          0x30
#define XMMR_TRIGGER_TEST_CTRL_OFFSET       0x34
#define XMMR_GT_STATUS_OFFSET               0x38
#define XMMR_GT_DRP_STATUS_OFFSET           0x3C
#define XMMR_GT_RXDATA_OFFSET               0x40
#define XMMR_VAL_REJ_RATE_STATUS_OFFSET     0x44
#define XMMR_CLOCKPATH_STATUS_OFFSET        0x48 /* NOT USED BUT STILL IN MEMORY AT THIS ADDRESS */
#define XMMR_TRIGGER_STATUS_OFFSET          0x4C
#define XMMR_TRIGGER_INTERNAL_STATUS_OFFSET 0x50 /* 2 registers (MSW and LSW) */
#define XMMR_GT_TXDATA_STATUS_OFFSET        0x58
#define XMMR_UPSTREAM_LEAF_STATUS_OFFSET    0x5C
#define XMMR_DOWNSTREAM_LEAF_STATUS_OFFSET  0x60
#define XMMR_CARRIER_STATUS_OFFSET          0x64
#define XMMR_REQ_BKP_RATE_STATUS_OFFSET     0x68
#define XMMR_IDLE_PERIOD_CTRL_OFFSET        0x124
#define XMMR_RATE_REQ_BKP_STATUS_OFFSET     0xF8
#define XMMR_RATE_VAL_REJ_STATUS_OFFSET     0xFC

/* NOT USED REGISTERS */
#define XMMR_DP_ULINKMUX_CTRL_OFFSET        0x00
#define XMMR_DP_SYNC_LOGIC_CTRL_OFFSET      0x00
#define XMMR_DP_TDC_CTRL_OFFSET             0x00
#define XMMR_TDC_EN_CTRL_OFFSET             0x00
#define XMMR_LED_CTRL_OFFSET                0x00
#define XMMR_TDC_DEBUG_CTRL_OFFSET          0x00
#define XMMR_I2CGTS_INPUT_CTRL_OFFSET       0x00
#define XMMR_DP_DIGITIZER_CTRL_OFFSET       0x00
#define XMMR_TRIGGER_CONF_CTRL_OFFSET       0x00
#define XMMR_DP_ACCESS_MGT_CTRL_OFFSET      0x00
#define XMMR_PERIOD_CTRL_OFFSET             0x00
#define XMMR_AURORA_CTRL_OFFSET             0x00
#define XMMR_TRIGGER_MUX_CTRL_OFFSET        0x00
#define XMMR_MGT_RRDATA_STATUS_OFFSET       0x00
#define XMMR_HARDWARE_STATUS_OFFSET         0x00
#define XMMR_I2CGTS_STATUS_OFFSET           0x00
#define XMMR_UPSTREAM_ROOT_STATUS_OFFSET    0x00
#define XMMR_DOWNSTREAM_ROOT_STATUS_OFFSET  0x00
#define XMMR_AURORA_STATUS_OFFSET           0x00
#define XMMR_IDENTIFICATION_STATUS_OFFSET   0x00
#define XMMR_REV_PROJECT_STATUS_OFFSET      0x00
#define XMMR_REV_IPLIB_STATUS_OFFSET        0x00
/*********************/

/* transceiver information */
#define NO_TRANSCEIVER -1
#define TRANSCEIVER_0   0
#define TRANSCEIVER_1   1
#define TRANSCEIVER_2   2
#define TRANSCEIVER_3   3
#define MASTER_TRANSCEIVER TRANSCEIVER_0
#define NB_TRANSCEIVER  4

#define TILE_0          0
#define TILE_1          1
#define NB_TILE         2

#define REFCLK1         0
#define REFCLK2         1
#define NO_REFCLK      -1

/* RocketIO related */
#define C_NULL         -1
#define C_RESET         1
#define C_PMA_RESET     2
#define C_WAIT_LOCK     4
#define C_PCS_RESET     8
#define C_WAIT_PCS     16
#define C_ALMOST_READY 32
#define C_READY        64
#define READY          C_READY
#define NOT_READY      C_NULL
#define COMMA_DETECTED     1
#define COMMA_NOT_DETECTED 0

/* ClockPath related */
#define LOCAL_CLK      0
#define RECOVERED_CLK  1
#define MICTOR_CLK     2
#define MASTER_CLK     3
#define DCM_LOCKED     1
#define DCM_NOT_LOCKED 0

/* DataPath related */
#define DP_RXMUX_MGT 0
#define DP_RXMUX_SFP 1
#define DP_TXMUX_REG 0
#define DP_TXMUX_USR 1
#define DP_TXMUX_PLS 2
#define DP_TXMUX_TST 3
#define DP_TXMUX_ULK 4
#define DP_TXMUX_MST 5
#define DP_TXMUX_LBK 6

/* MuxInOut related */
#define PLL_LOCKED         1
#define PLL_NOT_LOCKED     0
#define WAIT_PLL_DUMMY_LOCK 50 /* max time (us) in which the oscillatory analog lock is at 1 if the pll is unlocked */
#define USE_MGT            1
#define BYPASS_MGT         0
#define FINE_DELAY_DEFAULT 0
#define MAX_DELAY          1023   /* there is only one delay line in gts_v3 */
#define MAX_LMKPLL_DELAY   0xF   /* max delay that can be programmed inside LmkPll */
#define DELAYLINE_STEP_SIZE_DEFAULT       1   /* steps of roughly 15 ps used to increment the delay in delay line*/
#define DELAYLINE_STEP_INTERVAL_DEFAULT   200 /* time between delay increments in microsec, all is spanned in ~200ms */
#define LMKPLL_STEP_SIZE_DEFAULT     1   /* steps of roughly 150 ps used to increment the delay in LmkPll*/
#define LMKPLL_STEP_INTERVAL_DEFAULT 1500 /* time between delay increments in microsec, all is spanned in ~100ms */

/* Trigger related */
#define CLK_CARRIER_READY     1
#define CLK_CARRIER_NOT_READY 0
#define TRIGGER_CORE_IS_ROOT  1
#define TRIGGER_CORE_IS_FANIN_FANOUT  2
#define TRIGGER_CORE_IS_LEAF  3
#define VME_CARRIER_IS_CARRIER   1
#define LLP_CARRIER_IS_CARRIER   2
#define AGAVA_CARRIER_IS_CARRIER 3

/* TdcDebug related */
#define DELAY_TDC_DEBUG_DEFAULT 1024

/* Reg related */
#define XREG_SETUP      1
#define XREG_UPDATE_ALL 2
#define XREG_WRITE_ALL  3
#define XREG_STOP       4

/* struct and union definition */

typedef struct
{
  unsigned int                  : 14; /* 31->18 */
  unsigned int txcharisk_msb    : 1; /* 17 */
  unsigned int txcharisk_lsb    : 1; /* 16 */
  unsigned int msb              : 8; /* 15->8 */
  unsigned int lsb              : 8; /* 7->0 */
} XMMRegs_txmgtdata_ctrl;

typedef struct
{
  unsigned int t1               : 16; /* 31->16*/
  unsigned int t0               : 16; /* 15->0 */
} XMMRegs_rxmgtdata_status;

typedef struct
{
  unsigned int t1               : 16; /* 31->16*/
  unsigned int t0               : 16; /* 15->0 */
} XMMRegs_rrdata_status;

typedef struct
{
  unsigned int trigger_rst_carrier : 1; /*31*/
  unsigned int backpressure     : 1; /*30*/
  unsigned int                  : 10;
  unsigned int txchardispval    : 2; /*19->18*/
  unsigned int txchardispmode   : 2; /*17->16*/
  unsigned int                  : 2;
  unsigned int gtx_buf_reset    : 1; /*13*/
  unsigned int gtx_cdr_reset    : 1; /*12*/
  unsigned int gtx_pll_reset    : 1; /*11*/
  unsigned int rx_system_reset  : 1; /*10*/
  unsigned int tx_system_reset  : 1; /*9*/
  unsigned int gtx_reset        : 1; /*8*/
  unsigned int                  : 3;
  unsigned int rx_pcs_reset     : 1; /*4*/
  unsigned int tx_pcs_reset     : 1; /*3*/
  unsigned int gtsStreamReady   : 1; /*2*/
  unsigned int                  : 2;
} XMMRegs_gt_ctrl;

typedef struct
{
  unsigned int rx_init_status   : 8; /*31->24*/
  unsigned int tx_init_status   : 8; /*23->16*/
  unsigned int                  : 2;
  unsigned int rxchariscomma    : 2; /*13->12*/
  unsigned int rxrealign        : 1; /*11*/
  unsigned int rxcommadet       : 1; /*10*/
  unsigned int rxbuferr         : 1; /*9*/
  unsigned int txbuferr         : 1; /*8*/
  unsigned int                  : 4;
  unsigned int rx_locks         : 1; /*3*/
  unsigned int tx_locks         : 1; /*2*/
  unsigned int rx_system_ready  : 1; /*1*/
  unsigned int tx_system_ready  : 1; /*0*/
} XMMRegs_gt_status;

typedef struct
{
  unsigned int                  : 20;
  unsigned int drp_choice       : 4; /*11->8*/
  unsigned int drp_addr         : 8; /*7->0*/
} XMMRegs_drp_addr_ctrl;

typedef struct
{
  unsigned int                  : 9;
  unsigned int signal_detect    : 4; /*22->19*/
  unsigned int RST_drp_bsy      : 1; /*18*/
  unsigned int drp_we           : 1; /*17*/
  unsigned int drp_en           : 1; /*16*/
  unsigned int drp_di           : 16; /*15->0*/
} XMMRegs_drp_ctrl;

typedef struct
{
  unsigned int                  : 12;
  unsigned int gt_active        : 1; /*19*/
  unsigned int WARNING_choice   : 2; /*18->17*/
  unsigned int drp_bsy          : 1; /*16*/
  unsigned int drp_do           : 16; /*15->0*/
} XMMRegs_drp_status;

typedef struct
{
  unsigned int                      : 23;
  unsigned int rst_pdet             : 1; /*8*/
  unsigned int pgen_cont            : 1; /*7*/
  unsigned int pgen_start           : 1; /*6*/
  unsigned int tdc_stop_sync_select : 2; /*5->4*/
  unsigned int tdc_stop_select      : 2; /*3->2*/
  unsigned int tdc_start_select     : 2; /*1->0*/
} XMMRegs_dp_tdc_ctrl;

typedef struct
{
  unsigned int                      : 24;
  unsigned int delay                : 8; /*7->0*/
} XMMRegs_dp_delay_ctrl;

typedef struct
{
  unsigned int inspection_mux_sel_1  : 4; /*31->28*/
  unsigned int inspection_mux_sel_0  : 4; /*27->24*/
  unsigned int                       : 14; /*23->10*/
  unsigned int mux_clk_sync          : 1; /*9*/
  unsigned int mux_digitizer_gts_tree: 1; /*8*/
  unsigned int sfp_txmux_sync_select : 1; /*7*/
  unsigned int use_sync              : 1; /*6*/
  unsigned int mgt_txmux_select      : 4; /*5->2*/
  unsigned int mgt_rxmux_select      : 2; /*1->0*/
} XMMRegs_dp_mux_ctrl;

typedef struct
{
  unsigned int                     : 30;
  unsigned int ulinkmux            : 2; /*1->0*/
} XMMRegs_dp_ulinkmux_ctrl;

typedef struct
{
  unsigned int                     : 21;
  unsigned int mgt_txmux_sync      : 4; /*10->7*/
  unsigned int sfp_txmux_sync      : 4; /*6->3*/
  unsigned int sfp_sync            : 2; /*2->1*/
  unsigned int use_sync            : 1; /*0*/
} XMMRegs_dp_sync_logic_ctrl;

typedef struct
{
  unsigned int ctrl_enable         : 1; /*31*/
  unsigned int                     : 12;
  unsigned int one_sweep           : 1; /*18*/
  unsigned int delay               : 18; /* 17 -> 0 */  
} XMMRegs_tdc_debug_ctrl;

typedef struct
{
  unsigned int                     : 29; 
  unsigned int en_stop2            : 1; /*2*/
  unsigned int en_stop1            : 1; /*1*/
  unsigned int en_start            : 1; /*0*/ 
} XMMRegs_tdc_en_ctrl;

typedef struct
{
  unsigned int                     : 9;
  unsigned int TC_MD2              : 1; /*22*/
  unsigned int TC_MD1              : 1; /*21*/
  unsigned int                     : 1;
  unsigned int tx_sel1             : 1; /*19*/
  unsigned int tx_sel0             : 1; /*18*/
  unsigned int rx_sel1             : 1; /*17*/
  unsigned int rx_sel0             : 1; /*16*/
  unsigned int                     : 3;
  unsigned int LEN1                : 1; /*12*/
  unsigned int LEN0                : 1; /*11*/
  unsigned int delay               : 11; /*10->0*/
} XMMRegs_fine_delay_ctrl;

typedef struct
{
  unsigned int ctrl_enable         : 1; /*31*/
  unsigned int                     : 15; 
  unsigned int rx_sel1             : 4; /*15->12*/
  unsigned int tx_sel1             : 4; /*11->8*/
  unsigned int rx_sel0             : 4; /*7->4*/
  unsigned int tx_sel0             : 4; /*3->0*/
} XMMRegs_mgt_sel_ctrl;

typedef struct
{
  unsigned int mux_led             : 2; /*31->30*/
  unsigned int                     : 23; 
  unsigned int led1_b              : 1; /*6*/
  unsigned int led1_g              : 1; /*5*/
  unsigned int led1_r              : 1; /*4*/
  unsigned int led0_b              : 1; /*3*/
  unsigned int led0_g              : 1; /*2*/
  unsigned int led0_r              : 1; /*1*/
  unsigned int led2                : 1; /*0*/
} XMMRegs_led_ctrl;

typedef struct
{
  unsigned int                     : 12;
  unsigned int addr_ERR_status_1   : 1; /*19*/
  unsigned int addr_ERR_status_0   : 1; /*18*/
  unsigned int addr_ERR_ctrl_1     : 1; /*17*/
  unsigned int addr_ERR_ctrl_0     : 1; /*16*/
  unsigned int                     : 9; /*15->7*/
  unsigned int gts_pll_locked_filtered : 1; /*6*/
  unsigned int pll_has_unlocked    : 1; /*5*/ 
  unsigned int                     : 3; /*4->2*/
  unsigned int dcm_system_lock     : 1; /*1*/
  unsigned int gts_pll_locked      : 1; /*0*/
} XMMRegs_hardware_status;

typedef struct
{
  unsigned int                     : 27;
  unsigned int rst_dcm_fb          : 1; /*4*/
  unsigned int rst_dcm_mictor      : 1; /*3*/
  unsigned int clk_select_raw      : 1; /*2*/
  unsigned int clk_select_usr      : 1; /*1*/
  unsigned int clk_select_remloc   : 1; /*0*/
} XMMRegs_ClockPath_ctrl;

typedef struct
{
  unsigned int                         : 23; 
  unsigned int rst_dcm_fb_filtered     : 1; /*8*/
  unsigned int rst_dcm_mictor_filtered : 1; /*7*/
  unsigned int                         : 1; /*6*/
  unsigned int rst_dcm_sys_filtered    : 1; /*5*/
  unsigned int dcm_locked_fb           : 1; /*4*/
  unsigned int dcm_locked_user         : 1; /*3*/
  unsigned int gts_pll_locked          : 1; /*2*/
  unsigned int                         : 1; /*1*/
  unsigned int dcm_system_locked       : 1; /*0*/
} XMMRegs_ClockPath_status;

typedef struct
{
  unsigned int reject_window       : 16; /*31->16*/
  unsigned int                     : 9;
  unsigned int timestamp_enable    : 1; /*6*/
  unsigned int aurora_loopback     : 1; /*5*/
  unsigned int test_enable         : 1; /*4*/
  unsigned int loopback_enable     : 1; /*3*/
  unsigned int rst_core_uP         : 1; /*2*/
  unsigned int rst_dcm_uP          : 1; /*1*/
  unsigned int gts_ready           : 1; /*0*/
} XMMRegs_trigger_ctrl;

typedef struct
{
  unsigned int test_period_mask    : 16; /*31->16*/
  unsigned int test_offset         : 16; /*15->0*/
} XMMRegs_trigger_test_ctrl;

typedef struct
{
  unsigned int                     : 23;
  unsigned int mux_bcast_debug     : 1; /*8*/
  unsigned int mux_trigger_type    : 8; /*7->0*/
} XMMRegs_trigger_mux_ctrl;

typedef struct
{
  unsigned int mask_gpio_uP        : 16; /*31->16*/
  unsigned int gpio_uP             : 16; /*15->0*/
} XMMRegs_i2cgts_input_ctrl;

typedef struct
{
  unsigned int mask_crystalID      : 8; /*31->24*/
  unsigned int                     : 1;
  unsigned int mask_L1A_gate       : 1; /*22*/
  unsigned int                     : 4;
  unsigned int mask_LMK_PLL_locked : 1; /*17*/
  unsigned int                     : 1;
  unsigned int crystalID           : 8; /*15->8*/
  unsigned int                     : 1;
  unsigned int L1A_gate            : 1; /*6*/
  unsigned int                     : 4;
  unsigned int LMK_PLL_locked      : 1; /*1*/
  unsigned int                     : 1;
} XMMRegs_i2cgts_output_ctrl;

typedef struct
{
  unsigned int                     : 24;
  unsigned int mux_clk_sync        : 4;
  unsigned int mux                 : 4;
} XMMRegs_dp_digitizer_ctrl;

typedef struct
{
  unsigned int trg_protection_time : 16; /*31->16*/
  unsigned int led_base            : 16; /*15->0*/
} XMMRegs_trigger_conf_ctrl;

typedef struct
{
  unsigned int                     : 28;
  unsigned int mux                 : 4;
} XMMRegs_dp_access_mgt_ctrl;

typedef struct
{
  unsigned int period_mask         : 32;
} XMMRegs_period_ctrl;

typedef struct
{
  unsigned int hw_ctrl             : 1; /*31*/
  unsigned int hw_init_conf        : 1; /*30*/
  unsigned int                     : 25; /*29->5*/
  unsigned int sync_pll            : 1; /*4*/
  unsigned int data                : 1; /*3*/
  unsigned int clk                 : 1; /*2*/
  unsigned int le                  : 1; /*1*/
  unsigned int goe                 : 1; /*0*/
} XMMRegs_lmk_pll_ctrl;

typedef struct
{
  unsigned int                     : 18; /*31->14*/
  unsigned int init_conf           : 1; /*13*/
  unsigned int lmk_sync_pll_hw     : 1; /*12*/
  unsigned int lmk_data_hw         : 1; /*11*/
  unsigned int lmk_clk_hw          : 1; /*10*/
  unsigned int lmk_le_hw           : 1; /*9*/
  unsigned int lmk_goe_hw          : 1; /*8*/
  unsigned int                     : 6; /* 7 downto 2 */
  unsigned int dcm_ready           : 1; /*1*/
  unsigned int locked              : 1; /*0*/
} XMMRegs_lmk_pll_status;

typedef struct
{
  unsigned int                     : 9;
  unsigned int led                 : 7; /*22->16*/
  unsigned int                     : 3;
  unsigned int sw_reset_from_carrier : 1; /*12*/
  unsigned int dcm_locked_user     : 1; /*11*/
  unsigned int dcm_all_locked      : 1; /*10*/
  unsigned int rst_dcm_user        : 1; /*9*/
  unsigned int rst                 : 1; /*8*/
  unsigned int trigger_type        : 8; /*7->0*/
} XMMRegs_trigger_status;

typedef struct
{
  unsigned int gpio_o_msw          : 16; /*31->16*/
  unsigned int gpio_i              : 16; /*15->0*/
} XMMRegs_i2cgts_status;

typedef struct
{
  unsigned int                     : 32; 
} XMMRegs_txdata_status;

typedef struct
{
  unsigned int                     : 23;
  unsigned int led                 : 7; /*40->34*/
  unsigned int L1A_gate            : 1; /*33*/
  unsigned int lt_strobe           : 1; /*32*/
} XMMRegs_trigger_internal_msw_status; /* most significant word */

typedef struct
{
  unsigned int tag_strobe          : 1; /*31*/
  unsigned int local_trigger       : 2; /*30->29*/
  unsigned int bcast_strobe        : 1; /*28*/
  unsigned int local_tag           : 8; /*27->20*/
  unsigned int val_rej_tag         : 8; /*19->12*/
  unsigned int bcast_out           : 8; /*11->4*/
  unsigned int rejection           : 1; /*3*/
  unsigned int validation          : 1; /*2*/
  unsigned int request             : 2; /*1->0*/
} XMMRegs_trigger_internal_lsw_status; /* least significant word */

typedef struct
{
  unsigned int                     : 32;
} XMMRegs_upstream_root_msw_status; /* most significant word */

typedef struct
{
  unsigned int                     : 7; /*31->25*/
  unsigned int idle                : 1; /*24*/
  unsigned int crystal_id          : 8; /*23->16*/
  unsigned int timestamp           : 16; /*15->0*/
} XMMRegs_upstream_root_lsw_status; /* least significant word */

typedef struct
{
  unsigned int                     : 8;
  unsigned int event_number        : 24; /*23->0*/
} XMMRegs_downstream_root_msw_status; /* most significant word */

typedef struct
{
  unsigned int                     : 6; /*31->26*/
  unsigned int reject              : 1; /*25*/
  unsigned int validate            : 1; /*24*/
  unsigned int crystal_id          : 8; /*23->16*/
  unsigned int timestamp           : 16; /*15->0*/
} XMMRegs_downstream_root_lsw_status; /* least significant word */

typedef struct
{
  unsigned int bkp_rate            : 16; /*31->16*/
  unsigned int req_rate            : 16; /*15->0*/
} XMMRegs_req_bkp_rate_status; 

typedef struct
{
  unsigned int rej_rate            : 16; /*31->16*/
  unsigned int val_rate            : 16; /*15->0*/
} XMMRegs_val_rej_rate_status; 

typedef struct
{
  unsigned int                     : 14; /*31->18*/
  unsigned int idle_period_en      : 1; /*17*/
  unsigned int idle_period         : 17; /*16->0*/
} XMMRegs_idle_period_ctrl; 

typedef struct
{
  unsigned int                       : 29;
  unsigned int LLP_Carrier_is_here   : 1; /*2*/
  unsigned int LMK_PLL_locked        : 1; /*1*/
  unsigned int sw_reset_from_carrier : 1; /*0*/
} XMMRegs_carrier_status; 

typedef struct
{
  unsigned int                       : 8;
  unsigned int trigger_type          : 8; /*23 downto 16*/
  unsigned int carrier_type          : 8; /*15 downto 0*/
  unsigned int gts_version           : 8; /*7->0*/
} XMMRegs_identification_status; 

typedef struct
{
  unsigned int                       : 16;
  unsigned int project               : 16; /*15 downto 0*/
} XMMRegs_rev_project_status; 

typedef struct
{
  unsigned int SharedLib             : 16; /*31 downto 16*/
  unsigned int GtsIPLib              : 16; /*15 downto 0*/
} XMMRegs_rev_IPLib_status; 

typedef union
{
  unsigned int               uint;
  XMMRegs_txmgtdata_ctrl     txmgtdata;
  XMMRegs_gt_ctrl            gt;
  XMMRegs_drp_addr_ctrl      drp_addr;
  XMMRegs_drp_ctrl           drp;
  XMMRegs_dp_tdc_ctrl        dp_tdc;
  XMMRegs_dp_delay_ctrl      dp_delay;
  XMMRegs_dp_mux_ctrl        dp_mux;
  XMMRegs_dp_ulinkmux_ctrl   dp_ulinkmux;
  XMMRegs_dp_sync_logic_ctrl dp_sync;
  XMMRegs_tdc_en_ctrl        tdc_en;
  XMMRegs_fine_delay_ctrl    fine_delay;
  XMMRegs_mgt_sel_ctrl       mgt_sel;
  XMMRegs_led_ctrl           led;
  XMMRegs_tdc_debug_ctrl     tdc_debug;
  XMMRegs_ClockPath_ctrl     ClockPath;
} XMMRegs_ctrl;

typedef union
{
  unsigned int               uint;
  XMMRegs_txmgtdata_ctrl     txmgtdata;
  XMMRegs_dp_tdc_ctrl        tdc;
  XMMRegs_dp_delay_ctrl      delay;
  XMMRegs_dp_mux_ctrl        mux;
  XMMRegs_dp_ulinkmux_ctrl   ulinkmux;
  XMMRegs_dp_sync_logic_ctrl sync;
} XMMRegs_DataPath_ctrl;

typedef union
{
  unsigned int               uint;
  XMMRegs_gt_status          gt;
  XMMRegs_drp_status         drp;
  unsigned int               rxdata;
  unsigned int               mgt_rrdata;
  XMMRegs_hardware_status    hardware;
  XMMRegs_ClockPath_status   ClockPath;
} XMMRegs_status;

typedef struct
{
  unsigned int bkp_rate            : 16; /*31->16*/
  unsigned int req_rate            : 16; /*15->0*/
} XMMRegs_rate_req_bkp_status; 

typedef struct
{
  unsigned int rej_rate            : 16; /*31->16*/
  unsigned int val_rate            : 16; /*15->0*/
} XMMRegs_rate_val_rej_status; 

#endif
