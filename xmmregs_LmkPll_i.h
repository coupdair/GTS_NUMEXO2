#ifndef XMMREGS_LMKPLL_I_H
#define XMMREGS_LMKPLL_I_H

#include "xmmregs_LmkPll.h"

#define LMKPLL_PLL_R    50
#define LMKPLL_PLL_N   100
#define LMKPLL_VCO_DIV   6

extern lmk_config_raw_t lmk_config_raw[];
extern lmk_config_reg_t lmk_config_reg[];
extern lmk_config_reg_t lmk_config_CLKout4;
extern lmk_config_reg_t lmk_config_reset[];

extern lmk_config_raw_t lmk_config_raw_default[];
extern lmk_config_reg_t lmk_config_reg_default[];
extern lmk_config_reg_t lmk_config_reg_change[];
extern lmk_config_reg_t lmk_config_reg_reset[];

#endif
