#ifndef XOUTPUT_I_H
#define XOUTPUT_I_H

#include <stdio.h>

#define eprintf_output sprintf
#define estrcpy_output s_strcpy_output
#define FIRST_ARG_OUTPUT last_sentence_output

#define MAX_CHAR_OUTPUT 996
#define MAX_CHAR_FILE_OUTPUT 128

extern int outputAnswer(void);

extern void s_strcpy_output(void);
extern char last_sentence_output[MAX_CHAR_OUTPUT];
extern char global_output[MAX_CHAR_OUTPUT];
extern char file_name_output[MAX_CHAR_FILE_OUTPUT];

#endif

