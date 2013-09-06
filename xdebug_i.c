#include <string.h>
#include "xdebug_i.h"

#define DBG XDEBUG_I_DBG

char last_sentence_serial[MAX_CHAR_SERIAL];
char last_sentence_log[MAX_CHAR_LOG];
char global_log[MAX_CHAR_LOG];
int  choice_terminal_ethernet = DEBUG_THROUGH_TERMINAL;
char *FIRST_ARG = last_sentence_serial;

void s_strcpy_log(void)
{
  size_t n_last;
  size_t n_global;

  n_last = strlen(last_sentence_log);
  n_global = strlen(global_log);

  if (n_global + n_last + 1 >= MAX_CHAR_LOG) { /* remaining space in global_log too small */
    logAnswer();
    strncpy(global_log, last_sentence_log, MAX_CHAR_LOG);
    global_log[MAX_CHAR_LOG-1] = 0;
  }
  else if (n_last != 0) {
    strcat(global_log, last_sentence_log);
  }
}

void s_strcpy(void)
{
  printf("%s", last_sentence_serial);
}

void estrcpy(void)
{
  if (choice_terminal_ethernet == DEBUG_THROUGH_ETHERNET) {
    FIRST_ARG = last_sentence_log;
    s_strcpy_log();  
  }
  else {
    FIRST_ARG = last_sentence_serial;
    s_strcpy(); 
  }
}

int dbgTermSet(void)
{
  if (choice_terminal_ethernet == DEBUG_THROUGH_TERMINAL) {
    DBG(DBLI, "choice_terminal_ethernet already set to DEBUG_THROUGH_TERMINAL; nothing done\n");
  }
  else {
    choice_terminal_ethernet = DEBUG_THROUGH_TERMINAL;
    FIRST_ARG = last_sentence_serial;
    DBG(DBLI, "choice_terminal_ethernet has been set to DEBUG_THROUGH_TERMINAL\n");
    DBG(DBLI, "You should see this message on the terminal\n");
  }

  return XST_SUCCESS;
}

int dbgNetSet(void)
{
  if (choice_terminal_ethernet == DEBUG_THROUGH_ETHERNET) {
    DBG(DBLI, "choice_terminal_ethernet already set to DEBUG_THROUGH_ETHERNET; nothing done\n");
  }
  else {
    choice_terminal_ethernet = DEBUG_THROUGH_ETHERNET;
    FIRST_ARG = last_sentence_log;
    DBG(DBLI, "choice_terminal_ethernet has been set to DEBUG_THROUGH_ETHERNET\n");
    DBG(DBLI, "You should see this message on the computer host\n");
  }

  return XST_SUCCESS;
}

#undef DBG
