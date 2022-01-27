int
lci_file_info(LCITextPort *tp, char *filename) {

  char charset[32];
  char command[1024] = {"file -i '"};
// TODO: fix realpath over mount points
  if (realpath(filename, &command[9]) == NULL) {
printf("realpath failure on %s\n", filename);
    return -1;
  }
  tp->filename = strdup(&command[9]);
  tp->d_name = strrchr(tp->filename, '/') + 1;

  size_t slen = strlen(command);
  command[slen] = '\'';
  command[(slen + 1)] = 0;
    // run shell command, getting returned stream
  FILE *fpipe = (FILE*)popen(command, "r");
  *charset = 0;
  fscanf(fpipe, "%*[^:]: %[^;]; charset=%s", command, charset);
  if (strncmp(command, "cannot", 6) == 0) {
printf("%s\n", command);
    pclose(fpipe);
    free(tp->filename);
    tp->filename = NULL;
    tp->d_name = NULL;
    return -1;
  }
  pclose(fpipe);

  tp->mime_type = strdup(command);
    // file supplies encoding all lower case, differs from iana
  char *cptr = charset;
  char ch0 = *charset;
  if ((ch0 != 'w') && (ch0 != 'b') && (ch0 != 's')) {
    if (strcmp("unknown-8bit", cptr) == 0) {
      strcpy(charset, "windows-1251");
    } else
      while (*cptr != 0) {
        if (*cptr > 0x060) *cptr &= ~0x020;  cptr++;
      }
  } else {
    if (ch0 == 'b') {
      if (strcmp(cptr, "binary") == 0) {
        char bom[4];
        FILE *rh = fopen(tp->filename, "r");
        fread(bom, 4, 1, rh);
        fclose(rh);
        unsigned cp = *(unsigned*)bom;
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
          /* rlwm, rrwm, or; rotl and rotr and or; */
        cp = (((cp >> 8) | (cp << 24)) & 0xFF00FF00)
           | (((cp << 8) | (cp >> 24)) & 0x00FF00FF);
#endif
        if (cp == 0xFFFE0000) {
          tp->cs_encoding = strdup("UTF-32LE");
          return 0;
        }
        if (cp == 0x0000FEFF) {
          tp->cs_encoding = strdup("UTF-32BE");
          return 0;
        }
        tp->cs_encoding = strdup("BINARY");
        return 1;
      }
      *cptr &= ~0x020;
      if (cptr[4] != 0) {
        cptr = &cptr[4];
        while (*cptr != 0) {
          if (*cptr > 0x060) *cptr &= ~0x020;  cptr++;
        }
      }
    } else {
      strcpy(charset, "Shift_JIS");
    }
  }
  tp->cs_encoding = strdup(charset);
    // may replace tp->mime_type
  return lci_verify_text_mime(tp);
}

