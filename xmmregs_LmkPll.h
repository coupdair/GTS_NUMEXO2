#ifndef XMMREGS_LMKPLL_H
#define XMMREGS_LMKPLL_H

#define XMMRegs_LmkPll_HW_H(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->hw_ctrl = 1)
#define XMMRegs_LmkPll_HW_L(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->hw_ctrl = 0)
#define XMMRegs_LmkPll_LE(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->le)
#define XMMRegs_LmkPll_LE_L(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->le = 0)
#define XMMRegs_LmkPll_LE_H(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->le = 1)
#define XMMRegs_LmkPll_DATA(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->data)
#define XMMRegs_LmkPll_DATA_L(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->data = 0)
#define XMMRegs_LmkPll_DATA_H(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->data = 1)
#define XMMRegs_LmkPll_CLK(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->clk)
#define XMMRegs_LmkPll_CLK_L(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->clk = 0)
#define XMMRegs_LmkPll_CLK_H(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->clk = 1)
#define XMMRegs_LmkPll_GOE(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->goe)
#define XMMRegs_LmkPll_GOE_L(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->goe = 0)
#define XMMRegs_LmkPll_GOE_H(ba) (((XMMRegs_lmk_pll_ctrl *)(ba + XMMR_LMK_PLL_CTRL_OFFSET))->goe = 1)
#define XMMRegs_LmkPll_LOCKED(ba) (((XMMRegs_lmk_pll_status *)(ba + XMMR_LMK_PLL_STATUS_OFFSET))->locked)

#define LMKCFG_SIZE(i) (sizeof(i)/sizeof(lmk_config_raw_t))

typedef struct
{
  int address;
  unsigned int data;
} lmk_config_raw_t;

typedef struct
{
  unsigned int reset        : 1;  
  unsigned int reserved1    : 12;  /* set to 0 */
  unsigned int CLKout_MUX   : 2;
  unsigned int CLKout_EN    : 1;
  unsigned int CLKout_DIV   : 8;
  unsigned int CLKout_DLY   : 4;
  unsigned int address      : 4;  /* set to register number */
} lmk_r0_t;

typedef struct
{
  unsigned int reserved1    : 13;  /* set to 0 */
  unsigned int CLKout_MUX   : 2;
  unsigned int CLKout_EN    : 1;
  unsigned int CLKout_DIV   : 8;
  unsigned int CLKout_DLY   : 4;
  unsigned int address      : 4;  /* set to register number */
} lmk_r1_r7_t;

typedef struct
{
  unsigned int reserved1        : 10; /* set to 0x00a*/
  unsigned int OSCin_FREQ       : 8;
  unsigned int VCO_R4_LF        : 3;
  unsigned int VCO_R3_LF        : 3;
  unsigned int VCO_C3_C4_LF     : 4;
  unsigned int address          : 4; /* set to 13 */
} lmk_r13_t;

typedef struct
{
  unsigned int reserved1        : 3;  /* set to 0 */
  unsigned int EN_Fout          : 1;
  unsigned int EN_CLKout_Global : 1;
  unsigned int POWERDOWN        : 1;
  unsigned int reserved2        : 2;  /* set to 0 */
  unsigned int PLL_MUX          : 4;
  unsigned int PLL_R            : 12;
  unsigned int reserved3        : 4;  /* set to 0 */
  unsigned int address          : 4;  /* seto to 14 */
} lmk_r14_t;

typedef struct
{
 unsigned int PLL_CP_GAIN       : 2;
 unsigned int VCO_DIV           : 4;
 unsigned int PLL_N             : 18;
 unsigned int reserved1         : 4;  /*set to 0 */
 unsigned int address           : 4;  /* set to 15 */
} lmk_r15_t;

typedef union
{
  unsigned int binary;
  lmk_r0_t     r0;
  lmk_r1_r7_t  r1r7;
  lmk_r13_t    r13;
  lmk_r14_t    r14;
  lmk_r15_t    r15;
} lmk_config_reg_t;

#endif
