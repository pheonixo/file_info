#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

// gcc -o parser create_mime_parser.c

/*
dependant on files in:
/usr/share/gtksourceview-3.0/language-specs
creates C file "lci_mime_parser.c"
lci_mime_parser.c becomes non-dependant on language-specs
but returned mime_type can be used to access gtksourceview files
lci_mime_parser.c has:
extern int lci_verify_text_mime(LCITextPort *tp);
which fills LCITextPort->mime_type, depending on
whether supplied LCITextPort->mime_type is a text file,
a strdup() mime name. User responsible for free(ing)
when done. lci_mime_parser.c free(s) previous strdup()
name if LCITextPort->mime_type != NULL.

intended use:
  get standard mime type, I use linux 'file' utility
  run return through lci_verify_text_mime().
*/

static int
compare(const void *a0, const void *b0) {
  return strcmp((const char *)a0, (const char *)b0);
}

int
main(void) {

  struct {
    char filename[32];
    char name[4][32];
    char ext[24][16];
    char type[16];
  } lang[512];
  memset(lang, 0, sizeof(lang));
  int ldx = 0;
  char dirname[128] = { "/usr/share/gtksourceview-3.0/language-specs/" };
  DIR *dirp = opendir(dirname);
  struct dirent *ent;
    /* walk each entry of root directory */
  while ((ent = readdir(dirp)) != NULL) {
      /* skip system files */
    if (ent->d_name[0] == '.')  continue;
    if (strcmp(ent->d_name, "def.lang") == 0)  continue;
    if (strcmp(ent->d_name, "dokuwiki.lang") == 0)  continue;
    if (strcmp(ent->d_name, "gtk-doc.lang") == 0)  continue;
    if (strcmp(ent->d_name, "haddock.lang") == 0)  continue;
      // same as idl
    if (strcmp(ent->d_name, "idl-exelis.lang") == 0)  continue;
    if (strcmp(ent->d_name, "javascript-expressions.lang") == 0)  continue;
    if (strcmp(ent->d_name, "javascript-functions-classes.lang") == 0)  continue;
    if (strcmp(ent->d_name, "javascript-literals.lang") == 0)  continue;
    if (strcmp(ent->d_name, "javascript-modules.lang") == 0)  continue;
    if (strcmp(ent->d_name, "javascript-statements.lang") == 0)  continue;
    if (strcmp(ent->d_name, "javascript-values.lang") == 0)  continue;
    if (strcmp(ent->d_name, "jsdoc.lang") == 0)  continue;
    if (strcmp(ent->d_name, "language.dtd") == 0)  continue;
    if (strcmp(ent->d_name, "language.rng") == 0)  continue;
    if (strcmp(ent->d_name, "language2.rng") == 0)  continue;
    if (strcmp(ent->d_name, "mediawiki.lang") == 0)  continue;
      // can not support, conflicts with spice language
      // and LTSpice, which based on, doesn't use extensions listed
    if (strcmp(ent->d_name, "spice.lang") == 0)  continue;
      // not lang, internal lib api
    if (strcmp(ent->d_name, "tera.lang") == 0)  continue;
    if (strcmp(ent->d_name, "testv1.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-js-statements.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-js-modules.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-js-literals.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-js-functions-classes.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-js-expressions.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-type-literals.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-type-generics.lang") == 0)  continue;
    if (strcmp(ent->d_name, "typescript-type-expressions.lang") == 0)  continue;
    strcpy(lang[ldx++].filename, ent->d_name);
  }
  closedir(dirp);
//printf("closedir ldx = %d\n", ldx);
    /* with sorted text file types, grab identified file
     * extension, and genre type */
  qsort(lang, ldx, sizeof(lang[0]), compare);
  size_t dlen = strlen(dirname);
  for (int idx = 0; idx < ldx; idx++) {
    strcpy(&dirname[dlen], lang[idx].filename);
    FILE *rh = fopen(dirname, "r");
//printf("fopen (%s)\n", dirname);
    do {
      char scanline[256], keyword[32];
      fscanf(rh, "%s", scanline);
      if (strcmp(scanline, "<language") == 0) {
        fscanf(rh, "%*s%*[^\"]\"%*[^\"]\"%s", scanline);
        if (strncmp(scanline, "_section=\"", 10) != 0) {
          fscanf(rh, "%s", scanline);
          if (strncmp(scanline, "_section=\"", 10) != 0)
            printf("ERROR on _section, lang(%s)\n", dirname);
        }
        sscanf(scanline, "_section=\"%[^\"]", keyword);
        if (strncmp(keyword, "Scientific", 10) == 0)
          strcpy(lang[idx].type, "science");
        else if (strncmp(keyword, "Source", 6) == 0) {
          if (strcmp(lang[idx].filename, "idl.lang") == 0)
            strcpy(lang[idx].type, "science");
          else if (strcmp(lang[idx].filename, "meson.lang") == 0)
            strcpy(lang[idx].type, "other");
          else if (strcmp(lang[idx].filename, "sparql.lang") == 0)
            strcpy(lang[idx].type, "other");
          else
            strcpy(lang[idx].type, "source");
        } else if (strncmp(keyword, "Script", 6) == 0)
          strcpy(lang[idx].type, "script");
        else if (strncmp(keyword, "Markup", 6) == 0)
          strcpy(lang[idx].type, "markup");
        else if (strncmp(keyword, "Other", 5) == 0)
          strcpy(lang[idx].type, "other");
      }
        /* filename extension, with overrides */
      if (strcmp(scanline, "<property") == 0) {
        fscanf(rh, " %[^>]>", scanline);
        if (strncmp(scanline, "name=\"globs\"", 11) == 0) {
            // error in julia.lang
          fscanf(rh, " %[^<]", scanline);
          int read, tr = 0, edx = 0, ndx = 0;
            /* forced add in */
          if (strcmp(lang[idx].filename, "asm.lang") == 0) {
            strcpy(lang[idx].ext[edx], "s");
            edx++;
            strcpy(lang[idx].ext[edx], "S");
            edx++;
            strcpy(lang[idx].ext[edx], "inc");
            edx++;
          }
          if (strcmp(lang[idx].filename, "bluespec.lang") == 0) {
            strcpy(lang[idx].ext[edx], "bs");
            edx++;
          }
          if (strcmp(lang[idx].filename, "c.lang") == 0) {
            strcpy(lang[idx].ext[edx], "pch");
            edx++;
          }
          if (strcmp(lang[idx].filename, "commonlisp.lang") == 0) {
            strcpy(lang[idx].ext[edx], "lsp");
            edx++;
          }
          if (strcmp(lang[idx].filename, "css.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "css");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "docbook.lang") == 0) {
            strcpy(lang[idx].ext[edx], "dbk");
            edx++;
          }
          if (strcmp(lang[idx].filename, "erb-html.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "html.erb");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "ftl.lang") == 0) {
            strcpy(lang[idx].filename, "fluent");
          }
          if (strcmp(lang[idx].filename, "forth.lang") == 0) {
            strcpy(lang[idx].ext[edx], "fth");
            edx++;
            strcpy(lang[idx].ext[edx], "fr");
            edx++;
            strcpy(lang[idx].ext[edx], "frt");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "fsharp.lang") == 0) {
            strcpy(lang[idx].ext[edx], "fsi");
            edx++;
          }
          if (strcmp(lang[idx].filename, "gap.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "g");
            edx++;
            strcpy(lang[idx].ext[edx], "gi");
            edx++;
            strcpy(lang[idx].ext[edx], "gap");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "glsl.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "glsl");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "gradle.lang") == 0) {
              // override input data
            strcpy(lang[idx].type, "other");
          }
          if (strcmp(lang[idx].filename, "gtkrc.lang") == 0) {
            strcpy(lang[idx].ext[edx], "gtkrc");
            edx++;
          }
          if (strcmp(lang[idx].filename, "haml.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "haml");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "idl.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "idl");
            edx++;
            strcpy(lang[idx].ext[edx], "pro");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "matlab.lang") == 0) {
              // override input data
            strcpy(lang[idx].type, "vnd");
            strcpy(lang[idx].ext[edx], "mat");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "maxima.lang") == 0) {
            strcpy(lang[idx].ext[edx], "mc");
            edx++;
          }
          if (strcmp(lang[idx].filename, "objc.lang") == 0) {
            strcpy(lang[idx].ext[edx], "mm");
            edx++;
          }
          if (strcmp(lang[idx].filename, "octave.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "m");
            edx++;
            strcpy(lang[idx].ext[edx], "oct");
            edx++;
            strcpy(lang[idx].ext[edx], "mex");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "python3.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "py3");
            edx++;
            strcpy(lang[idx].ext[edx], "pyi");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "R.lang") == 0) {
            strcpy(lang[idx].filename, "r");
            strcpy(lang[idx].ext[edx], "rda");
            edx++;
            strcpy(lang[idx].ext[edx], "rdata");
            edx++;
            strcpy(lang[idx].ext[edx], "rds");
            edx++;
          }
          if (strcmp(lang[idx].filename, "rhtml.lang") == 0) {
            strcpy(lang[idx].ext[edx], "rhtml");
            edx++;
            strcpy(lang[idx].ext[edx], "eco");
            edx++;
            strcpy(lang[idx].ext[edx], "jeco");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "ruby_on_rails.lang") == 0) {
              // override input data
            strcpy(lang[idx].filename, "rubyonrails");
            strcpy(lang[idx].ext[edx], "ru");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "scheme.lang") == 0) {
            strcpy(lang[idx].ext[edx], "ss");
            edx++;
          }
          if (strcmp(lang[idx].filename, "star.lang") == 0) {
            strcpy(lang[idx].filename, "cif");
          }
          if (strcmp(lang[idx].filename, "tcl.lang") == 0) {
            strcpy(lang[idx].ext[edx], "tm");
            edx++;
          }
          if (strcmp(lang[idx].filename, "xml.lang") == 0) {
              // override input data
            strcpy(lang[idx].ext[edx], "abw");
            edx++;
            strcpy(lang[idx].ext[edx], "fo");
            edx++;
            strcpy(lang[idx].ext[edx], "glade");
            edx++;
            strcpy(lang[idx].ext[edx], "jnlp");
            edx++;
            strcpy(lang[idx].ext[edx], "kino");
            edx++;
            strcpy(lang[idx].ext[edx], "mml");
            edx++;
            strcpy(lang[idx].ext[edx], "plist");
            edx++;
            strcpy(lang[idx].ext[edx], "rdf");
            edx++;
            strcpy(lang[idx].ext[edx], "rss");
            edx++;
            strcpy(lang[idx].ext[edx], "siv");
            edx++;
            strcpy(lang[idx].ext[edx], "smil");
            edx++;
            strcpy(lang[idx].ext[edx], "smi");
            edx++;
            strcpy(lang[idx].ext[edx], "svg");
            edx++;
            strcpy(lang[idx].ext[edx], "wml");
            edx++;
            strcpy(lang[idx].ext[edx], "xbel");
            edx++;
            strcpy(lang[idx].ext[edx], "xhtml");
            edx++;
            strcpy(lang[idx].ext[edx], "xib");
            edx++;
            strcpy(lang[idx].ext[edx], "xmi");
            edx++;
            strcpy(lang[idx].ext[edx], "xml");
            edx++;
            strcpy(lang[idx].ext[edx], "xspf");
            edx++;
            strcpy(lang[idx].ext[edx], "xul");
            edx++;
            strcpy(lang[idx].ext[edx], "xslfo");
            edx++;
            strcpy(lang[idx].ext[edx], "zabw");
            edx++;
            goto override;
          }
          if (strcmp(lang[idx].filename, "vhdl.lang") == 0) {
            strcpy(lang[idx].ext[edx], "vhdl");
            edx++;
          }
            /* filenames and wildcards */
          do {
            read = 0;
            if (sscanf(&scanline[tr], "%[^;];%n", keyword, &read) == 0)  break;
            if (read == 0) {
              if (scanline[tr] != 0) {
                sscanf(&scanline[tr], "%s%n", keyword, &read);
              } else
                break;
            }
            tr += read;
            if (keyword[0] == '*') {
              if (keyword[1] == '.') {
                strcpy(lang[idx].ext[edx], &keyword[2]);
                edx++;
              } else {
                if (strcmp(keyword, "*bashrc") == 0) {
                  strcpy(lang[idx].name[ndx], ".bashrc");
                  ndx++;
                } else {
                  strcpy(lang[idx].name[ndx], &keyword[1]);
                  ndx++;
                }
              }
            } else if (keyword[0] == '[') {
              lang[idx].name[ndx][0] = keyword[1];
              lang[idx].name[(ndx + 1)][0] = keyword[2];
              strcpy(&lang[idx].name[ndx][1], &keyword[4]);
              strcpy(&lang[idx].name[(ndx + 1)][1], &keyword[4]);
              ndx += 2;
            } else {
              strcpy(lang[idx].name[ndx], keyword);
              ndx++;
            }
          } while (1);
  override:
          fclose(rh);
          break;
        }
      }
      fscanf(rh, "%*[^\n]%c", scanline);
    } while (1);
  }
    /* with all parsed files, sort into 3 types:
     * named, wildcard named, extension */
  struct {
    char name[32];
    char filename[32];
    char type[16];
  } lang_names[64];
  int lndx = 0;
  struct {
    char name[32];
    char filename[32];
    char type[16];
  } lang_names_n[32];
  int lnndx = 0;
  struct {
    char ext[16];
    char filename[32];
    char type[16];
  } extension[1024];
  int edx = 0;
  memset(lang_names, 0, sizeof(lang_names));
  memset(lang_names_n, 0, sizeof(lang_names_n));
  memset(extension, 0, sizeof(extension));
  for (int idx = 0; idx < ldx; idx++) {
    for (int ndx = 0; lang[idx].ext[ndx][0] != 0; ndx++) {
      strcpy(extension[edx].ext, lang[idx].ext[ndx]);
      strcpy(extension[edx].filename, lang[idx].filename);
      strcpy(extension[edx].type, lang[idx].type);
      edx++;
    }
    for (int ndx = 0; lang[idx].name[ndx][0] != 0; ndx++) {
      char *aptr = strchr(lang[idx].name[ndx], '*');
      if (aptr == NULL) {
        strcpy(lang_names[lndx].name, lang[idx].name[ndx]);
        strcpy(lang_names[lndx].filename, lang[idx].filename);
        strcpy(lang_names[lndx].type, lang[idx].type);
        lndx++;
      } else {
        *aptr = 0;
        strcpy(lang_names_n[lnndx].name, lang[idx].name[ndx]);
        strcpy(lang_names_n[lnndx].filename, lang[idx].filename);
        strcpy(lang_names_n[lnndx].type, lang[idx].type);
        lnndx++;
      }
    }
  }
  qsort(lang_names, lndx, sizeof(lang_names[0]), compare);
  qsort(lang_names_n, lnndx, sizeof(lang_names_n[0]), compare);
  qsort(extension, edx, sizeof(extension[0]), compare);

    /* create C file, ability to identify text files by mime types */
  FILE *rh = fopen("lci_mp.h", "r");
  if (rh == NULL) {
    puts("can't open lci_mp.h");
    return 0;
  }
  fseek(rh, 0 , SEEK_END);
  long filesize = ftell(rh);
  fseek(rh, 0 , SEEK_SET);
  char *buffer = malloc(filesize);
  fread(buffer, filesize, 1, rh);
  fclose(rh);

  FILE *wh = fopen("lci_file_info.c", "w");

  fprintf(wh, "#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n\n");
  fwrite(buffer, filesize, 1, wh);
  free(buffer);

  fprintf(wh, "        // strncmp() name with wild card ender\n");
  fprintf(wh, "static struct _nwlist {\n  char name[32];  char mime[32];\n}"
       " name_wc_list[] = {\n");
  for (int idx = 0; idx < lnndx; idx++) {
    char mime_name[32];
    sscanf(lang_names_n[idx].filename, "%[^.]", mime_name);
    if (idx != (lnndx - 1))
      fprintf(wh, "  { \"%s\", \"text/x-%s.%s\" },\n",
                      lang_names_n[idx].name, lang_names_n[idx].type, mime_name);
    else
      fprintf(wh, "  { \"%s\", \"text/x-%s.%s\" }\n};\n",
                      lang_names_n[idx].name, lang_names_n[idx].type, mime_name);
  }

  fprintf(wh, "        // strcmp() names\n");
  fprintf(wh, "static struct _nlist {\n  char name[32];  char mime[32];\n}"
       " name_list[] = {\n");
  for (int idx = 0; idx < lndx; idx++) {
    char mime_name[32];
    sscanf(lang_names[idx].filename, "%[^.]", mime_name);
    if (idx != (lndx - 1))
      fprintf(wh, "  { \"%s\", \"text/x-%s.%s\" },\n",
                      lang_names[idx].name, lang_names[idx].type, mime_name);
    else
      fprintf(wh, "  { \"%s\", \"text/x-%s.%s\" }\n};\n",
                      lang_names[idx].name, lang_names[idx].type, mime_name);
  }

  fprintf(wh, "        // strcmp() extensions, begins with '*.'\n");
  fprintf(wh, "static struct _elist {\n  char ext[32];  char mime[32];\n}"
       " extension_list[] = {\n");
  for (int idx = 0; idx < edx; idx++) {
    char mime_name[32];
    sscanf(extension[idx].filename, "%[^.]", mime_name);
    if (idx != (edx - 1))
      fprintf(wh, "  { \"%s\", \"text/x-%s.%s\" },\n",
                      extension[idx].ext, extension[idx].type, mime_name);
    else
      fprintf(wh, "  { \"%s\", \"text/x-%s.%s\" }\n};\n",
                      extension[idx].ext, extension[idx].type, mime_name);
  }

  fprintf(wh, "\nstatic char *\n"
         "__m_differienate(char *filename) {\n\n"
         "  char scanstr[1024];\n"
         "  char *r_objc = \"text/x-source.objc\";\n"
         "  char *r_octave = \"text/x-science.octave\";\n"
         "  //char *r_matlab = \"text/x-vnd.matlab\";\n"
         "#ifdef __APPLE__\n"
         "  char *r_str = r_objc;\n"
         "#else\n"
         "  char *r_str = r_octave;\n"
         "#endif\n\n"
         "  FILE *rh = fopen(filename, \"r\");\n"
         "  do {\n"
         "    int count = fscanf(rh, \"%%s%%*[^\\n]\\n\", scanstr);\n"
         "    if (count == EOF) break;\n"
         "    if (count == 0) break;\n"
         "    if (scanstr[0] == '/') {\n"
         "      if ((scanstr[1] == '/') || (scanstr[1] == '*')) {\n"
         "        r_str = r_objc;  break;  }\n"
         "      continue;\n    }\n"
         "    if (scanstr[0] == '%%') {\n"
         "      r_str = r_octave;  break;  }\n"
         "    if (scanstr[0] == '#') {\n"
         "      if (scanstr[1] == 0) {\n"
         "        r_str = r_octave;  break;  }\n"
         "      if (strncmp(\"#import\", scanstr, 7)) {\n"
         "        r_str = r_objc;  break;  }\n"
         "      if (strncmp(\"#include\", scanstr, 8)) {\n"
         "        r_str = r_objc;  break;  }\n"
         "      continue;\n    }\n"
         "    if (strcmp(\"function\", scanstr)) {\n"
         "      r_str = r_octave;  break;  }\n"
         "  } while (1);\n"
         "  fclose(rh);\n"
         "  return r_str;\n}\n");

  fprintf(wh, "\nstatic int\n"
         "compare(const void *a0, const void *b0) {\n"
         "  return strcmp((const char *)a0, (const char *)b0);\n"
         "}\n");

  fprintf(wh, "\nstatic int\nlci_verify_text_mime(LCITextPort *tp) {\n\n");
  fprintf(wh, "  void *bsptr;\n"
              "  char strptr[32] = { \"text/plain\" };\n"
              "  char *rstrptr = strptr;\n");
  fprintf(wh, "  if (strncmp(tp->mime_type, \"text\", 4) != 0)  return 1;\n");

  fprintf(wh, "  if (strcmp(tp->mime_type, \"text/x-shellscript\") == 0) {\n"
              "    rstrptr = \"text/x-script.sh\";\n"
              "    goto ptr_exit;\n"
              "  }\n");

  fprintf(wh, "    // allow override of plain with iana types, but check if gets overriden\n"
              "  if    (strcmp(tp->mime_type, \"text/calandar\") == 0) {\n"
              "      rstrptr = \"text/calendar\";\n"
              "  } else if (strcmp(tp->mime_type, \"text/html\") == 0) {\n"
              "    rstrptr = \"text/html\";\n"
              "  } else if (strcmp(tp->mime_type, \"text/rtf\") == 0) {\n"
              "      rstrptr = \"text/rtf\";\n"
              "  } else if (strcmp(tp->mime_type, \"text/troff\") == 0) {\n"
              "      rstrptr = \"text/troff\";\n"
              "  } else if (strcmp(tp->mime_type, \"text/xml\") == 0) {\n"
              "      rstrptr = \"text/x-markup.xml\";\n"
              "  }\n");

  fprintf(wh, "    // set a default found by 'file', match up to phnx if can\n"
              "  if ((tp->mime_type[5] == 'x') && (tp->mime_type[6] == '-')) {\n"
              "    bsptr = bsearch(&(tp->mime_type[7]), extension_list,\n"
              "                    sizeof(extension_list)/sizeof(extension_list[0]),\n"
              "                    sizeof(extension_list[0]), compare);\n"
              "    if (bsptr != NULL)\n"
              "      rstrptr = ((struct _elist*)bsptr)->mime;\n"
              "    else {\n"
              "      if (strcmp(&tp->mime_type[7], \"msdos-batch\") == 0) {\n"
              "        rstrptr = \"text/x-script.dosbatch\";\n"
              "        goto ptr_exit;\n"
              "      }\n"
              "      sprintf(&strptr[5], \"x-other.%%s\", &tp->mime_type[7]);\n"
              "    }\n"
              "  }\n");
  fprintf(wh, "        // strncmp()\n");
  for (int idx = 0; idx < lnndx; idx++)
    fprintf(wh, "  if (strncmp(tp->d_name, name_wc_list[%d].name, %ld) == 0) {\n"
           "    rstrptr = name_wc_list[%d].mime;\n"
           "    goto ptr_exit;\n  }\n",
           idx, strlen(lang_names_n[idx].name), idx);

fprintf(wh, 
"  bsptr = bsearch(tp->d_name, name_list,\n"
"                  sizeof(name_list)/sizeof(name_list[0]),\n"
"                  sizeof(name_list[0]), compare);\n"
"  if (bsptr != NULL) {\n"
"    rstrptr = ((struct _nlist*)bsptr)->mime;\n"
"    goto ptr_exit;\n"
"  }\n"
"  char *extptr = strrchr(tp->d_name, '.');\n"
"  if (extptr != NULL) {\n"
"    extptr++;\n"
"        // strcmp() extensions, iana defined\n"
"    if (strcmp(extptr, \"css\") == 0) {\n"
"      rstrptr = \"text/css\";\n"
"      goto ptr_exit;\n    }\n"
"    if (strcmp(extptr, \"csv\") == 0) {\n"
"      rstrptr = \"text/csv\";\n"
"      goto ptr_exit;\n    }\n"
"    if (strcmp(extptr, \"dot\") == 0) {\n"
"      rstrptr = \"text/vnd.graphviz\";\n"
"      goto ptr_exit;\n"
"    }\n"
"    if (strcmp(extptr, \"html\") == 0) {\n"
"      rstrptr = \"text/html\";\n"
"      goto ptr_exit;\n    }\n"
"    if (strcmp(extptr, \"m\") == 0) {\n"
"      rstrptr = __m_differienate(tp->filename);\n"
"      goto ptr_exit;\n    }\n"
"    if (strcmp(extptr, \"gv\") == 0) {\n"
"      rstrptr = \"text/vnd.graphviz\";\n"
"      goto ptr_exit;\n    }\n"
"    if (strcmp(extptr, \"md\") == 0) {\n"
"      rstrptr = \"text/markdown\";\n"
"      goto ptr_exit;\n    }\n"
"        // strcmp() extensions\n"
"    bsptr = bsearch(extptr, extension_list,\n"
"                    sizeof(extension_list)/sizeof(extension_list[0]),\n"
"                    sizeof(extension_list[0]), compare);\n"
"    if (bsptr != NULL)\n"
"      rstrptr = ((struct _elist*)bsptr)->mime;\n"
"  }\n"
"ptr_exit:\n"
"  if (tp->mime_type != NULL)\n"
"    free(tp->mime_type);\n"
"  tp->mime_type = strdup(rstrptr);\n"
"  return 0;\n");
fprintf(wh, "}\n\n");

  rh = fopen("info.c", "r");
  if (rh == NULL) {
    puts("can't open info.c");
  	fclose(wh);
    return 0;
  }
  fseek(rh, 0 , SEEK_END);
  filesize = ftell(rh);
  fseek(rh, 0 , SEEK_SET);
  buffer = malloc(filesize);
  fread(buffer, filesize, 1, rh);
  fclose(rh);
  fwrite(buffer, filesize, 1, wh);
  free(buffer);

  fclose(wh);

  return 0;
}

