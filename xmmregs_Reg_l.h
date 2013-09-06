#ifndef XMMREGS_REG_L_H
#define XMMREGS_REG_L_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Reg related */
#define MAX_NAME       32
#define MAX_NUM_FIELD  32
//#define MAX_NUM_REG   (XPAR_OPB_MMREGS_0_HIGH_ADDRESS-XPAR_OPB_MMREGS_0_BASE_ADDRESS)/4
#define MAX_NUM_REG   128

#define START\
  XMMRegs_Reg_t *reg;\
  XMMRegs_Bitfield_t *field;\
  unsigned int pos;\
  int i, j, size, offset, n_field;\
  char *str;\
  \
  str = calloc(sizeof(char), MAX_NAME);\
  if ( (config == XREG_SETUP) && (InstancePtr->reg == NULL) ) {\
    InstancePtr->reg = (XMMRegs_Reg_t *) calloc( sizeof(XMMRegs_Reg_t), MAX_NUM_REG );\
    for (i = 0; i < MAX_NUM_REG; i++) {\
      InstancePtr->reg[i].name = (char *) calloc( sizeof(char), MAX_NAME );\
      InstancePtr->reg[i].bitfield = (XMMRegs_Bitfield_t *) calloc( sizeof(XMMRegs_Bitfield_t), MAX_NUM_FIELD );\
      for (j = 0; j < MAX_NUM_FIELD; j++) {\
        InstancePtr->reg[i].bitfield[j].name = (char *) calloc( sizeof(char), MAX_NAME );\
      }\
    }\
  }\
  else if ( (config == XREG_STOP) && (InstancePtr->reg != NULL) ) {\
    for (i = 0; i < MAX_NUM_REG; i++) {\
      for (j = 0; j < MAX_NUM_FIELD; j++) {\
        free(InstancePtr->reg[i].bitfield[j].name);\
        InstancePtr->reg[i].bitfield[j].name = NULL;\
      }\
      free(InstancePtr->reg[i].name);\
      free(InstancePtr->reg[i].bitfield);\
      InstancePtr->reg[i].name = NULL;\
      InstancePtr->reg[i].bitfield = NULL;\
    }\
    free(InstancePtr->reg);\
    InstancePtr->reg = NULL;\
  }\
  {
  
#define REG(arg_reg, arg_offset, arg_size)\
  XMMRegs_ ## arg_reg *t, *s;\
  \
  if ( (config == XREG_UPDATE_ALL) || (config == XREG_WRITE_ALL) || (config == XREG_SETUP) ) {\
    size = arg_size;\
    offset = arg_offset;\
    \
    if ( config == XREG_SETUP ) {\
      for (i = 0; i < arg_size; i++) {\
        pos = (arg_offset)/4 + i;\
        reg = &(InstancePtr->reg)[pos];\
        if (arg_size <= 1 )\
          strcpy(reg->name, #arg_reg);\
        else {\
          sprintf(str, "%s.[%d]", #arg_reg, i);\
          strcpy(reg->name, str);\
        }\
        reg->offset = (arg_offset) + 0x4 * i;\
        t = (XMMRegs_ ## arg_reg *) (InstancePtr->BaseAddress + reg->offset);\
        reg->val = (unsigned int *) t;\
      }\
    }\
    t = (XMMRegs_ ## arg_reg *) (InstancePtr->BaseAddress + arg_offset);\
    s = t;\
    n_field = 0;\
  }
  
#define FIELD(arg_field)\
  if ( (config == XREG_UPDATE_ALL) || (config == XREG_WRITE_ALL) || (config == XREG_SETUP) ) {\
    for (i = 0; i < size; i++) {\
      pos = offset/4 + i;\
      reg = &InstancePtr->reg[pos];\
      field = &reg->bitfield[n_field];\
      s = t + i;\
      switch (config) {\
        case XREG_SETUP :\
          strcpy(field->name, #arg_field);\
          field->val = s->arg_field;\
          break;\
        case XREG_UPDATE_ALL :\
          field->val = s->arg_field;\
          break;\
        case XREG_WRITE_ALL :\
          s->arg_field = field->val;\
          break;\
        default :\
          break;\
      }\
    }\
    n_field++;\
  }
  
#define NEXT }{

#define END } free(str); return XST_SUCCESS;

#endif
