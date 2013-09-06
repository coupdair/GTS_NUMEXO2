#include "xdebug_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xmmregs_Reg.h"
#include <string.h>

#define DBG XMMR_REG_DBG

#define YES 1
#define NO  0

/*************************************************
*  static functions
**************************************************/

/* a ctrl register has the form toto_ctrl */
int name_is_the_one_of_a_ctrl_register(char *name)
{
  char * last_underscore = strrchr(name, '_');

  if (last_underscore != NULL) {
    if ( strcmp( last_underscore, "_ctrl" ) == 0 ) {
      return YES;
    }
    else {
      DBG(DBLI, "%s is NOT a control register\n", name);
      return NO;
    }
  }
  else {
    DBG(DBLI, "%s is NOT a control register\n", name);
    return NO;
  }
}

/*************************************************
*  driver functions
**************************************************/

int XMMRegs_Reg_ReadAll(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_Reg_t      *reg;
  XMMRegs_Bitfield_t *field;
  int i;

  reg = &InstancePtr->reg[0];
  for (i = 0; i < MAX_NUM_REG; i++) {
    if (strcmp(reg[i].name,"") != 0) {
      printf("%-15s : 0x%08X (offset = 0x%08X)\n", reg[i].name, *reg[i].val, reg[i].offset);
      for (field = &reg[i].bitfield[0]; strcmp(field->name,"") != 0; field++) {
        printf("  %-18s : %u\n", field->name, field->val);
      }
    }     
  }

  return status;
}

int XMMRegs_Reg_ReadReg(XMMRegs *InstancePtr, char *reg_name)
{
  int status = XST_SUCCESS;
  XMMRegs_Reg_t      *reg;
  XMMRegs_Bitfield_t *field;
  int i;

  reg = &InstancePtr->reg[0];
  for (i = 0; i < MAX_NUM_REG; i++) {
    if ( strncmp(reg[i].name,reg_name,strlen(reg_name)) == 0) {
      printf("%-15s : 0x%08X (offset = 0x%08X)\n", reg[i].name, *(reg[i].val), reg[i].offset);
      for (field = &reg[i].bitfield[0]; strcmp(field->name,"") != 0; field++) {
        printf("  %-18s : %u\n", field->name, field->val);
      }
    }
  }

  return status;
}

int XMMRegs_Reg_ReadField(XMMRegs *InstancePtr, char *reg_name, char *field_name)
{
  int status = XST_SUCCESS;
  XMMRegs_Reg_t      *reg;
  XMMRegs_Bitfield_t *field;
  int i,j;

  reg = &InstancePtr->reg[0];
  for (i = 0; i < MAX_NUM_REG; i++) {
    if ( strncmp(reg[i].name,reg_name,strlen(reg_name) ) == 0) {
      for (j = 0; j < MAX_NUM_FIELD; j++) {
        field = &reg[i].bitfield[j]; 
	if (strncmp(field->name, field_name,strlen(field_name)) == 0) {
          printf("%-15s : %-18s : %u\n", reg[i].name, field->name, field->val);
	}
      }
    }     
  }

  return status;
}

int XMMRegs_Reg_WriteField(XMMRegs *InstancePtr, char *reg_name, char *field_name, unsigned int val_field)
{
  int status = XST_SUCCESS;
  XMMRegs_Reg_t      *reg;
  XMMRegs_Bitfield_t *field;
  int i,j;

  reg = &InstancePtr->reg[0];
  for (i = 0; i < MAX_NUM_REG; i++) {
    if ( strncmp(reg[i].name,reg_name,strlen(reg_name) ) == 0) {
      if ( name_is_the_one_of_a_ctrl_register(reg[i].name) == YES ) { /* the register is a ctrl register */
        for (j = 0; j < MAX_NUM_FIELD; j++) {
          field = &reg[i].bitfield[j];
          if (strncmp(field->name, field_name,strlen(field_name)) == 0) {
            field->val = val_field;
            printf("%-15s : %-18s : %u -> written\n", reg[i].name, field->name, field->val);
          }
        }
      }
    }     
  }

  return status;
}

/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

int XMMRegs_Reg_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_Reg_Associate(InstancePtr, XREG_SETUP);

  return status;
}

int XMMRegs_Reg_Start(XMMRegs *InstancePtr)
{
  return XMMRegs_Reg_Associate(InstancePtr, XREG_UPDATE_ALL);
}

int XMMRegs_Reg_Stop(XMMRegs *InstancePtr)
{
  return XMMRegs_Reg_Associate(InstancePtr, XREG_STOP);
}

int XMMRegs_Reg_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_Reg_Setup(InstancePtr);
  status |= XMMRegs_Reg_Start(InstancePtr);

  return status;
}

int XMMRegs_Reg_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_Reg_Stop(InstancePtr);
  status |= XMMRegs_Reg_Init(InstancePtr);

  return status;
}

#undef YES
#undef NO
#undef DBG
