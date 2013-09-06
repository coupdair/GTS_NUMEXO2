#include "xmmregs_LmkPll.h"
#include "xmmregs_LmkPll_i.h"

/* lmk3000 default raw GTS configuration */

lmk_config_raw_t lmk_config_raw_default[]=
{
  {0, 0x8000010},
  {0, 0x0001010},
  {1, 0x0000010},
  {2, 0x0000010},
  {3, 0x0007010},
  {4, 0x0007010},
  {5, 0x0000010},
  {6, 0x0000010},
  {7, 0x0000010},
  {13,0x0299000},
  {14,0x0850060},
  {15,0xd8000C0}
};

/* should be equal to the vector with .address=4 of lmk_config_reg_default[] 
Otherwise it is overwritten in XMMRegs_LmkPll_Setup */

lmk_config_reg_t lmk_config_CLKout4 =
{ .r1r7 =   /* PLL_OUT1 for FB_CLK -> clock of the gts tree */
  { 
    .reserved1  = 0, 
    .CLKout_MUX = 3,  /* divided and delayed */
    .CLKout_EN  = 1,  /* enabled        */
    .CLKout_DIV = 1,  /* divided by 2   */ 
    .CLKout_DLY = 0,
    .address    = 4
  }
};

/* lmk3000 default reg GTS configuration */
/* i=100 o=0*200++3*100+4*100 */

lmk_config_reg_t lmk_config_reg_default[] =
{ 
  { .r0 = { 
    .reset      = 1,
    .reserved1  = 0, 
    .CLKout_MUX = 0,
    .CLKout_EN  = 0,  
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,
    .address    = 0,
    }
  },

  /* FPGA_CLK_CLEAN -> ref clock of MGT */
  { .r0 = { 
    .reset      = 0, 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed */
    .CLKout_EN  = 1,  /* enabled  */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,
    .address    = 0
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 1
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 2
    }
  },

  /* PLL_OUT0 for ADC_CLK -> clock to align digitizers*/
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 3,  /* divided and delayed */
    .CLKout_EN  = 1,  /* enabled        */
    .CLKout_DIV = 1,  /* divided by 2   */ 
    .CLKout_DLY = 0,
    .address    = 3
    }
  },

  /* PLL_OUT1 for FB_CLK -> clock of the gts tree */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 3,  /* divided and delayed */
    .CLKout_EN  = 1,  /* enabled        */
    .CLKout_DIV = 1,  /* divided by 2   */ 
    .CLKout_DLY = 0,
    .address    = 4
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 5
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 6
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 7
    }
  },

  { .r13 = { 
    .reserved1    = 10,     /* constant value      */ 
    .OSCin_FREQ   = 100,    /* 100 MHz             */
    .VCO_R4_LF    = 0,      /* < 200 ohm           */
    .VCO_R3_LF    = 0,      /* < 600 ohm           */ 
    .VCO_C3_C4_LF = 0,      /* 0 10 pf             */
    .address      = 13
    }
  },

  { .r14 = { 
    .reserved1        = 0,  /* constant value      */ 
    .EN_Fout          = 0,  /* disabled            */
    .EN_CLKout_Global = 1,  /* enabled             */
    .POWERDOWN        = 0,  /* ON                  */ 
    .reserved2        = 0,  /* constant value      */
    .PLL_MUX          = 5,  /* Anl. Lock Det. H    */
    .PLL_R            = LMKPLL_PLL_R,  /* 4095             */
    .reserved3        = 0,  /* constant value      */
    .address          = 14  
    }
  },

  { .r15 = { 
    .PLL_CP_GAIN      = 3,   /* 3.2mA 32x          */ 
    .VCO_DIV          = LMKPLL_VCO_DIV,   /* 6                  */
    .PLL_N            = LMKPLL_PLL_N,  /* 8190             */
    .reserved1        = 0,   /* constant value     */ 
    .address          = 15  
    }
  }
};


/* i=100 o=0*200++3*100+4*100 */

lmk_config_reg_t lmk_config_reg_change[] =
{ 
  { .r0 = { 
    .reset      = 1,
    .reserved1  = 0, 
    .CLKout_MUX = 0,
    .CLKout_EN  = 0,  
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,
    .address    = 0
    }
  },

  /* FPGA_CLK_CLEAN -> ref clock of MGT */
  { .r0 = { 
    .reset      = 0, 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed */
    .CLKout_EN  = 1,  /* enabled  */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,
    .address    = 0
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 1
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 2
    }
  },

  /* PLL_OUT0 for ADC_CLK -> clock to align digitizers*/
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 3,  /* divided and delayed */
    .CLKout_EN  = 1,  /* enabled        */
    .CLKout_DIV = 1,  /* divided by 2   */ 
    .CLKout_DLY = 0,
    .address    = 3
    }
  },

  /* PLL_OUT1 for FB_CLK -> clock of the gts tree */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 3,  /* divided and delayed */
    .CLKout_EN  = 1,  /* enabled        */
    .CLKout_DIV = 1,  /* divided by 2   */ 
    .CLKout_DLY = 0,
    .address    = 4
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 5
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 6
    }
  },

  /* not used */
  { .r1r7 = { 
    .reserved1  = 0, 
    .CLKout_MUX = 0,  /* bypassed   */
    .CLKout_EN  = 0,  /* disabled   */
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,  
    .address    = 7
    }
  },

  { .r13 = { 
    .reserved1    = 10,     /* constant value      */ 
    .OSCin_FREQ   = 100,    /* 100 MHz             */
    .VCO_R4_LF    = 0,      /* < 200 ohm           */
    .VCO_R3_LF    = 0,      /* < 600 ohm           */ 
    .VCO_C3_C4_LF = 0,      /* 0 10 pf             */
    .address      = 13
    }
  },

  { .r14 = { 
    .reserved1        = 0,  /* constant value      */ 
    .EN_Fout          = 0,  /* disabled            */
    .EN_CLKout_Global = 1,  /* enabled             */
    .POWERDOWN        = 0,  /* ON                  */ 
    .reserved2        = 0,  /* constant value      */
    .PLL_MUX          = 5,  /* Anl. Lock Det. H    */
    .PLL_R            = 4095, /*                     */
    .reserved3        = 0,  /* constant value      */
    .address          = 14  
    }
  },

  { .r15 = { 
    .PLL_CP_GAIN      = 3,   /* 3.2mA 32x          */ 
    .VCO_DIV          = 6,   /* 6                  */
    .PLL_N            = 8190,  /*                    */
    .reserved1        = 0,   /* constant value     */ 
    .address          = 15  
    }
  }
};


lmk_config_reg_t lmk_config_reg_reset[] ={ 
  { .r0 = { 
    .reset      = 1,
    .reserved1  = 0, 
    .CLKout_MUX = 0,
    .CLKout_EN  = 0,  
    .CLKout_DIV = 1,
    .CLKout_DLY = 0,
    .address    = 0
    }
  }
};


