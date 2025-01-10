#include <sys/types.h>
#include <dirent.h>

#ifndef ___PUT_H
#define ___PUT_H

#include "macros.h"

int  numlen     (int) WEAK;
int  spclen     (int) WEAK;
void set_format (EntryList *, Format *, Mode *) WEAK;
void setown     (Mode *, struct stat *, int *) WEAK;
void setgrp     (Mode *, struct stat *, int *) WEAK;
void setmark    (mode_t, char *, int) WEAK;
int  _should    (char, Mode *) WEAK;
void setname    (char **, struct stat *, Mode *) WEAK;
int  printname  (char *, Format *);
int  put_long   (struct dirent *, Format *, Mode *, struct stat *, char *);
int  put        (struct dirent *, Format *, Mode *);

#endif
