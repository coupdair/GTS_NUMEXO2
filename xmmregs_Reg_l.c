#include "xdebug_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xmmregs_Reg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int XMMRegs_Reg_Associate(XMMRegs *InstancePtr, int config)
{
  START
    REG(txmgtdata_ctrl, XMMR_MGT_REGDATA_CTRL_OFFSET, 1)
      FIELD( txcharisk_msb    )
      FIELD( txcharisk_lsb    )
      FIELD( msb              )
      FIELD( lsb              )
  NEXT
    REG(rxmgtdata_status, XMMR_GT_RXDATA_OFFSET, 1)
      FIELD( t1               )
      FIELD( t0               )
  NEXT
    REG(gt_ctrl, XMMR_GT_CTRL_OFFSET, 1)
      FIELD( trigger_rst_carrier )
      FIELD( backpressure        )
      FIELD( gtx_buf_reset       )
      FIELD( gtx_cdr_reset       )
      FIELD( gtx_pll_reset       )
      FIELD( rx_system_reset     )
      FIELD( tx_system_reset     )
      FIELD( gtx_reset           )
      FIELD( gtsStreamReady      )
  NEXT
    REG(gt_status, XMMR_GT_STATUS_OFFSET, 1)
      FIELD( rxchariscomma    )
      FIELD( rxcommadet       )
  NEXT
    REG(drp_addr_ctrl, XMMR_GT_DRP_ADDR_CTRL_OFFSET, 1)
      FIELD( drp_choice       )
      FIELD( drp_addr         )
  NEXT
    REG(drp_ctrl, XMMR_GT_DRP_CTRL_OFFSET, 1)
      FIELD( signal_detect    )
      FIELD( RST_drp_bsy      )
      FIELD( drp_we           )
      FIELD( drp_en           )
      FIELD( drp_di           )
  NEXT
    REG(drp_status, XMMR_GT_DRP_STATUS_OFFSET, 1)
      FIELD( gt_active        )
      FIELD( WARNING_choice   )
      FIELD( drp_bsy          )
      FIELD( drp_do           )
/*
  NEXT
    REG(dp_tdc_ctrl, XMMR_DP_TDC_CTRL_OFFSET, 1)
      FIELD( rst_pdet         )
      FIELD( pgen_cont        )
      FIELD( pgen_start       )
      FIELD( tdc_stop_sync_select )
      FIELD( tdc_stop_select  )
      FIELD( tdc_start_select )
*/
  NEXT
    REG(dp_delay_ctrl, XMMR_DP_DELAY_CTRL_OFFSET, 1)
      FIELD( delay            )
  NEXT
    REG(dp_mux_ctrl, XMMR_DP_MUX_CTRL_OFFSET, 1)
      FIELD( inspection_mux_sel_1   )
      FIELD( inspection_mux_sel_0   )
      FIELD( mux_clk_sync           )
      FIELD( mux_digitizer_gts_tree )
      FIELD( sfp_txmux_sync_select  )
      FIELD( use_sync               )
      FIELD( mgt_txmux_select       )
      FIELD( mgt_rxmux_select       )
  NEXT
    REG(fine_delay_ctrl, XMMR_FINE_DELAY_CTRL_OFFSET, 1)
      FIELD( TC_MD2           )
      FIELD( TC_MD1           )
      FIELD( tx_sel1          )
      FIELD( tx_sel0          )
      FIELD( rx_sel1          )
      FIELD( rx_sel0          )
      FIELD( LEN1             )
      FIELD( LEN0             )
      FIELD( delay            )
/*
  NEXT
    REG(mgt_sel_ctrl, XMMR_MGT_SEL_CTRL_OFFSET, 1)
      FIELD( ctrl_enable      )
      FIELD( rx_sel1          )
      FIELD( tx_sel1          )
      FIELD( rx_sel0          )
      FIELD( tx_sel0          )
*/
/*
  NEXT
    REG(hardware_status, XMMR_HARDWARE_STATUS_OFFSET, 1)
      FIELD( addr_ERR_status_1 )
      FIELD( addr_ERR_status_0 )
      FIELD( addr_ERR_ctrl_1   )
      FIELD( addr_ERR_ctrl_0   )
      FIELD( gts_pll_locked_filtered )
      FIELD( pll_has_unlocked  )
      FIELD( dcm_system_lock   )
      FIELD( gts_pll_locked    )
*/
  NEXT
    REG(ClockPath_ctrl, XMMR_CLOCKPATH_CTRL_OFFSET, 1)
      FIELD( rst_dcm_fb       )
      FIELD( rst_dcm_mictor   )
      FIELD( clk_select_raw   )
      FIELD( clk_select_usr   )
      FIELD( clk_select_remloc)
  NEXT
    REG(ClockPath_status, XMMR_CLOCKPATH_STATUS_OFFSET, 1)
      FIELD( rst_dcm_fb_filtered     )
      FIELD( rst_dcm_mictor_filtered )
      FIELD( rst_dcm_sys_filtered    )
      FIELD( dcm_locked_fb           )
      FIELD( dcm_locked_user         )
      FIELD( gts_pll_locked          )
      FIELD( dcm_system_locked       )
  NEXT
    REG(trigger_ctrl, XMMR_TRIGGER_CTRL_OFFSET, 1)
      FIELD( reject_window    )
      FIELD( timestamp_enable )
      FIELD( aurora_loopback  )
      FIELD( test_enable      )
      FIELD( loopback_enable  )
      FIELD( rst_core_uP      )
      FIELD( rst_dcm_uP       )
      FIELD( gts_ready        )
  NEXT
    REG(trigger_test_ctrl, XMMR_TRIGGER_TEST_CTRL_OFFSET, 1)
      FIELD( test_period_mask )
      FIELD( test_offset      )
  NEXT
    REG(i2cgts_output_ctrl, XMMR_I2CGTS_OUTPUT_CTRL_OFFSET, 1)
      FIELD( mask_crystalID      )
      FIELD( mask_L1A_gate       )
      FIELD( mask_LMK_PLL_locked )
      FIELD( crystalID           )
      FIELD( L1A_gate            )
      FIELD( LMK_PLL_locked      )
  NEXT
    REG(lmk_pll_ctrl, XMMR_LMK_PLL_CTRL_OFFSET, 1)
      FIELD( hw_ctrl          )
      FIELD( hw_init_conf     )
      FIELD( sync_pll         )
      FIELD( data             )
      FIELD( clk              )
      FIELD( le               )
      FIELD( goe              )
  NEXT
    REG(lmk_pll_status, XMMR_LMK_PLL_STATUS_OFFSET, 1)
      FIELD( init_conf        )
      FIELD( lmk_sync_pll_hw  )
      FIELD( lmk_data_hw      )
      FIELD( lmk_clk_hw       )
      FIELD( lmk_le_hw        )
      FIELD( lmk_goe_hw       )
      FIELD( dcm_ready        )
      FIELD( locked           )
  NEXT
    REG(trigger_status, XMMR_TRIGGER_STATUS_OFFSET, 1)
      FIELD( led              )
      FIELD( sw_reset_from_carrier )
      FIELD( dcm_locked_user  )
      FIELD( dcm_all_locked   )
      FIELD( rst_dcm_user     )
      FIELD( rst              )
      FIELD( trigger_type     )
  NEXT
    REG(trigger_internal_msw_status, XMMR_TRIGGER_INTERNAL_STATUS_OFFSET, 1)
      FIELD( led              )
      FIELD( L1A_gate         )
      FIELD( lt_strobe        )
  NEXT
    REG(trigger_internal_lsw_status, XMMR_TRIGGER_INTERNAL_STATUS_OFFSET + 0x4, 1)
      FIELD( tag_strobe       )
      FIELD( local_trigger    )
      FIELD( bcast_strobe     )
      FIELD( local_tag        )
      FIELD( val_rej_tag      )
      FIELD( bcast_out        )
      FIELD( rejection        )
      FIELD( validation       )
      FIELD( request          )
  NEXT
    REG(txdata_status, XMMR_GT_TXDATA_STATUS_OFFSET, 1)
  NEXT
    REG(carrier_status, XMMR_CARRIER_STATUS_OFFSET, 1)
      FIELD( LLP_Carrier_is_here   )
      FIELD( LMK_PLL_locked        )
      FIELD( sw_reset_from_carrier )
  NEXT
    REG(req_bkp_rate_status, XMMR_REQ_BKP_RATE_STATUS_OFFSET, 1)
      FIELD( bkp_rate         )
      FIELD( req_rate         )
  NEXT
    REG(val_rej_rate_status, XMMR_VAL_REJ_RATE_STATUS_OFFSET, 1)
      FIELD( rej_rate         )
      FIELD( val_rate         )
  END
}

int XMMRegs_Reg_Write(XMMRegs *InstancePtr)
{
  int config = XREG_WRITE_ALL;

  START
    REG(txmgtdata_ctrl, XMMR_MGT_REGDATA_CTRL_OFFSET, 1)
      FIELD( txcharisk_msb    )
      FIELD( txcharisk_lsb    )
      FIELD( msb              )
      FIELD( lsb              )
  NEXT
    REG(gt_ctrl, XMMR_GT_CTRL_OFFSET, 1)
      FIELD( trigger_rst_carrier )
      FIELD( backpressure        )
      FIELD( gtx_buf_reset       )
      FIELD( gtx_cdr_reset       )
      FIELD( gtx_pll_reset       )
      FIELD( rx_system_reset     )
      FIELD( tx_system_reset     )
      FIELD( gtx_reset           )
      FIELD( gtsStreamReady      )
  NEXT
    REG(drp_addr_ctrl, XMMR_GT_DRP_ADDR_CTRL_OFFSET, 1)
      FIELD( drp_choice       )
      FIELD( drp_addr         )
  NEXT
    REG(drp_ctrl, XMMR_GT_DRP_CTRL_OFFSET, 1)
      FIELD( signal_detect    )
      FIELD( RST_drp_bsy      )
      FIELD( drp_we           )
      FIELD( drp_en           )
      FIELD( drp_di           )
  NEXT
    REG(dp_delay_ctrl, XMMR_DP_DELAY_CTRL_OFFSET, 1)
      FIELD( delay            )
  NEXT
    REG(dp_mux_ctrl, XMMR_DP_MUX_CTRL_OFFSET, 1)
      FIELD( inspection_mux_sel_1   )
      FIELD( inspection_mux_sel_0   )
      FIELD( mux_clk_sync           )
      FIELD( mux_digitizer_gts_tree )
      FIELD( sfp_txmux_sync_select  )
      FIELD( use_sync               )
      FIELD( mgt_txmux_select       )
      FIELD( mgt_rxmux_select       )
  NEXT
    REG(fine_delay_ctrl, XMMR_FINE_DELAY_CTRL_OFFSET, 1)
      FIELD( TC_MD2           )
      FIELD( TC_MD1           )
      FIELD( tx_sel1          )
      FIELD( tx_sel0          )
      FIELD( rx_sel1          )
      FIELD( rx_sel0          )
      FIELD( LEN1             )
      FIELD( LEN0             )
      FIELD( delay            )
  NEXT
    REG(ClockPath_ctrl, XMMR_CLOCKPATH_CTRL_OFFSET, 1)
      FIELD( rst_dcm_fb       )
      FIELD( rst_dcm_mictor   )
      FIELD( clk_select_raw   )
      FIELD( clk_select_usr   )
      FIELD( clk_select_remloc)
  NEXT
    REG(trigger_ctrl, XMMR_TRIGGER_CTRL_OFFSET, 1)
      FIELD( reject_window    )
      FIELD( timestamp_enable )
      FIELD( aurora_loopback  )
      FIELD( test_enable      )
      FIELD( loopback_enable  )
      FIELD( rst_core_uP      )
      FIELD( rst_dcm_uP       )
      FIELD( gts_ready        )
  NEXT
    REG(trigger_test_ctrl, XMMR_TRIGGER_TEST_CTRL_OFFSET, 1)
      FIELD( test_period_mask )
      FIELD( test_offset      )
  NEXT
    REG(i2cgts_output_ctrl, XMMR_I2CGTS_OUTPUT_CTRL_OFFSET, 1)
      FIELD( mask_crystalID      )
      FIELD( mask_L1A_gate       )
      FIELD( mask_LMK_PLL_locked )
      FIELD( crystalID           )
      FIELD( L1A_gate            )
      FIELD( LMK_PLL_locked      )
  NEXT
    REG(lmk_pll_ctrl, XMMR_LMK_PLL_CTRL_OFFSET, 1)
      FIELD( hw_ctrl          )
      FIELD( hw_init_conf     )
      FIELD( sync_pll         )
      FIELD( data             )
      FIELD( clk              )
      FIELD( le               )
      FIELD( goe              )
  END
}
