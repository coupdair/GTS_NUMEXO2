#ifndef XDIAGNOSE_L_H
#define XDIAGNOSE_L_H

#define DGN_OK " "
#define DGN_ERR "*"
#define DGN_WARN "!"
#define DGN_DONTCARE "-"

#define DIAGN_ONE(name, val, expected, explane) {\
  printf("%-17s -> %u | %s | %s\n", name, val, (val == expected) ? DGN_OK : DGN_ERR, (val != expected) ? explane : ""); \
}

#define DIAGN_WARN(name, val, expected, explane) {\
  printf("%-17s -> %u | %s | %s\n", name, val, (val == expected) ? DGN_OK : DGN_WARN, (val != expected) ? explane : ""); \
}

#define DIAGN_DONT_CARE(name, val, expected, explane) {\
  printf("%-17s -> %u | %s | %s\n", name, val, DGN_DONT_CARE, explane); \
}

#endif

