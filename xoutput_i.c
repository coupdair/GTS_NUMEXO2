#include <string.h>
#include "xstatus.h"
#include "xoutput_i.h"

char last_sentence_output[MAX_CHAR_OUTPUT];
char global_output[MAX_CHAR_OUTPUT];
char file_name_output[MAX_CHAR_FILE_OUTPUT];

void s_strcpy_output(void)
{
  size_t n_last;
  size_t n_global;

  n_last = strlen(last_sentence_output);
  n_global = strlen(global_output);

  if (n_global + n_last + 1 >= MAX_CHAR_OUTPUT) { /* remaining space in global_output too small */
    outputAnswer();
    strncpy(global_output, last_sentence_output, MAX_CHAR_OUTPUT);
    global_output[MAX_CHAR_OUTPUT-1] = 0;
  }
  else if (n_last != 0) {
    strcat(global_output, last_sentence_output);
    outputAnswer();
  }
}

int set_output_file_name(char *file_name) 
{
  sprintf(file_name_output, "%s", file_name);
  return XST_SUCCESS;
}
