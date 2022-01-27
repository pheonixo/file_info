#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <gtksourceview/gtksource.h>

#include "../lci_mp.h"
extern int lci_file_info(LCITextPort *tp, char *filename);


// extensions
//https://www.webopedia.com/reference/data-formats-and-file-extensions/
//https://en.wikipedia.org/wiki/List_of_filename_extensions

// NOTE: had to load old groovy for new gradle to work


//gcc `pkg-config --cflags gtk+-3.0` -I/usr/include/gtksourceview-3.0 -o verify lci_verify_text_mime.c lci_file_info.c `pkg-config --libs gtk+-3.0` -lgtksourceview-3.0

static
struct _odata {
  char  *filename;
  char  *viewname;
  char  *mime;
  char  *gtks;
  char  *glib;
  char  *phnx;
} outdata[256];
static int odx = 0;

void
gather_all(LCITextPort *tp, char *filename) {
    // for sort/display results
  outdata[odx].filename = strdup(tp->filename);
  outdata[odx].viewname = strrchr(outdata[odx].filename, '/') + 1;
  outdata[odx].phnx = strdup(tp->mime_type);

  char command[1024] = {"file -i '"};
  strcpy(&command[9], filename);
  size_t slen = strlen(command);
  command[slen] = '\'';
  command[(slen + 1)] = 0;
      // run shell command, getting returned stream
  FILE *fpipe = (FILE*)popen(command, "r");
  fscanf(fpipe, "%*[^:]: %[^;]", command);
  outdata[odx].mime = strdup(command);
  pclose(fpipe);

    /* attempt to use gtksourcelanguagemanager to get mime type */
  GtkSourceLanguageManager *lmgr = gtk_source_language_manager_get_default();
  GtkSourceLanguage *lang = gtk_source_language_manager_guess_language(
                                         lmgr, tp->filename, tp->mime_type);
  if (lang != NULL) {
    gchar **rstrs = gtk_source_language_get_mime_types(lang);
    if (rstrs != NULL) {
        // for sort/display results
      outdata[odx].gtks = strdup(*rstrs);
      // printf("\tgtks(%s)\n", *rstrs);
      g_strfreev(rstrs);
    }
  }

    /* attempt to use glibc to get mime type */
  GError *error;
  GFile *file = g_file_new_for_path(tp->filename);
  GFileInfo *file_info = g_file_query_info(file, "standard::*",
                                              0, NULL, &error);
    // for sort/display results (mime)
  outdata[odx].glib = strdup(g_file_info_get_content_type(file_info));
    // printf("\tglib(%s)\n", outdata[odx].glib);
    // const char *content_type = g_file_info_get_content_type(file_info);
    // char *desc = g_content_type_get_description(content_type);
    // /* you'd have to use g_loadable_icon_load to get the actual icon */
    // GIcon *icon = g_file_info_get_icon(file_info);
    // printf("(%s)\n", g_icon_to_string(icon));
    //GIcon *g_file_icon_new(GFile *file);
}

static void
compare_source_syntax(FILE *wh, char *filename, char *gtks, char *phnx) {

  GtkSourceLanguageManager *lmgr = gtk_source_language_manager_get_default();
  if ((gtks != NULL) && (*gtks != 0) && (strcmp("text/plain", gtks) != 0)) {
    GtkSourceLanguage *lang = gtk_source_language_manager_guess_language(
                                                         lmgr, filename, gtks);
    if (lang == NULL) {
      printf("missing lang (%s)\n", gtks);
      fprintf(wh, "\tgtks_source(%s)\n", "");
    } else
      fprintf(wh, "\tgtks_source(%s)\n", gtk_source_language_get_id(lang));
  } else {
    fprintf(wh, "\tgtks_source(plain)\n");
  }

  if (strcmp(phnx, "text/plain") != 0) {
    char *req_lang = strrchr(phnx, '.');
    if (req_lang == NULL) {
      req_lang = strrchr(phnx, '/');
    } else if (strcmp(".graphviz", req_lang) == 0) {
      req_lang = ".dot";
    } else if (strcmp(".cif", req_lang) == 0) {
      req_lang = ".star";
    } else if (strcmp(".csharp", req_lang) == 0) {
      req_lang = ".c-sharp";
    } else if (strcmp(".fluent", req_lang) == 0) {
      req_lang = ".ftl";
    } else if (strcmp(".javascript", req_lang) == 0) {
      req_lang = ".js";
    } else if (strcmp(".po", req_lang) == 0) {
      req_lang = ".gettext-translation";
    } else if (strcmp(".protobuf", req_lang) == 0) {
      req_lang = ".proto";
    }
    GtkSourceLanguage *lang = gtk_source_language_manager_get_language
                                                   (lmgr, (req_lang + 1));
    if (lang == NULL) {
      printf("(%s)\n", (req_lang + 1));
      fprintf(wh, "\tphnx_source(%s)\n", "plain");
    } else
      fprintf(wh, "\tphnx_source(%s)\n", gtk_source_language_get_id(lang));
  } else {
    fprintf(wh, "\tphnx_source(plain)\n");
  }
}

static
int osort(const void *a0, const void *b0) {

  char *lang0 = ((struct _odata*)a0)->phnx;
  char *lang1 = ((struct _odata*)b0)->phnx;
  if ((lang0 == NULL) || (lang1 == NULL)) {
    puts("error");
    return 0;
  }
  lang0 = strrchr(lang0, '.');
  lang1 = strrchr(lang1, '.');
  if (lang0 == NULL) {
    if (lang1 == NULL)  return 0;
    return (strcmp("", lang1));
  }
  if (lang1 == NULL) {
    return (strcmp(lang0, ""));
  }
  return (strcmp((lang0 + 1), (lang1 + 1)));
}

int
main(void) {

  int status;
  LCITextPort tp = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0 };
  memset(outdata, 0, sizeof(outdata));
  char dirname[128] = { "../test_files/" };
  size_t dlen = strlen(dirname);
  DIR *dirp = opendir(dirname);
  struct dirent *ent;
    /* walk each entry of root directory */
  while ((ent = readdir(dirp)) != NULL) {
      /* skip system files */
    if (ent->d_name[0] == '.') {
      if ((ent->d_name[1] == 0) || (ent->d_name[1] == '.'))
        continue;
    }
    strcpy(&dirname[dlen], ent->d_name);
/* set all LCITextPort entries, store in outdata */
/* file data to retrieve listed in tp->filename */
    tp.filename = dirname;
    tp.d_name = &dirname[dlen];
    //lci_open_file(&tp);
/* file data to retrieve (filename), return in tp */
    if ((status = lci_file_info(&tp, dirname)) != 0) {
      if (status == -1) {
        printf("open error on (%s)\n", dirname);
        continue;
      }
      printf("not a text file (%s) (%s)\n", tp.filename, tp.mime_type);
      free(tp.filename);
      free(tp.mime_type);
      free(tp.cs_encoding);
      continue;
    }
      // insert lci into comparison list, gather others
    gather_all(&tp, dirname);
/* normally do something with LCITextPort before free(ing) */
    free(tp.filename);
    free(tp.mime_type);
    free(tp.cs_encoding);
    odx++;
  }
  closedir(dirp);

  printf("%d %ld %ld\n", odx, sizeof(outdata[0]), sizeof(outdata));
  qsort(outdata, odx, sizeof(outdata[0]), osort);

  FILE *wh = fopen("./verify_out.txt", "w");
  for (int idx = 0; idx < odx; idx++) {
    fprintf(wh, "opening:\n\tfilename(%s)\n\tviewname(%s)\n\tmime(%s)\n"
                "\tgtks(%s)\n\tglib(%s)\n\tphnx(%s)\n",
                outdata[idx].filename, outdata[idx].viewname, outdata[idx].mime,
                ((outdata[idx].gtks != NULL) ? outdata[idx].gtks : ""),
                outdata[idx].glib, outdata[idx].phnx);
      /* added data test after attached to program */
    compare_source_syntax(wh, outdata[idx].filename,
                          outdata[idx].gtks, outdata[idx].phnx);
      /* clean up */
    free(outdata[idx].filename);
    free(outdata[idx].mime);
    if (outdata[idx].gtks != NULL)
      free(outdata[idx].gtks);
    free(outdata[idx].glib);
    free(outdata[idx].phnx);
  }

  fclose(wh);

  //wh = fopen("/home/steven/Pictures/text128_icons/text-x-c.svg", "r");
  // GFile *gh = g_file_new_for_path("/home/steven/Pictures/text128_icons/text-x-c.svg");
  // GIcon *icon = g_file_icon_new(gh);
  // printf("(%s)\n", g_icon_to_string(icon));
  // g_object_unref(gh);
  //fclose(wh);

  return 0;
}

