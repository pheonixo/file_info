//open all in test_files outside LCode, dont load into app/viewport, just a buffer
#include "../lci.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <gtksourceview/gtksource.h>

//gcc `pkg-config --cflags gtk+-3.0` -I/usr/include/gtksourceview-3.0 -o tall run_all_test_files.c lci_file_info.c `pkg-config --libs gtk+-3.0` -lgtksourceview-3.0 -L.. -lpfile

static char iPath[256] = { "/home/steven/Pictures/text128_icons/text-x-" };
static int  iAppend = 43;  // strlen(iPath)
static int odx = 0;

/* needed to set text, must be utf-8, in libpfile.a due to conflicts with gtk */
extern char *phnx_buffered_file(LCITextPort *);

/* sets the basics for LCITextPort
 * returns -1 on bad file name, cant get info
 * returns 1 on non-text file, tp basics filled
 * returns 0 on text mime, tp basics filled
 */

extern int lci_file_info(LCITextPort *tp, char *filename);

LCITextPort *
lci_add_view(LCITextPort *tp) {

    /* Test for handling of text mime type done on open
     * Determine buffer set up (plain vs known syntax)
     * Attach buffer to containers
     * Attach container to session
     */
  if (strcmp(tp->mime_type, "text/plain") != 0) {
    char *req_lang = strrchr(tp->mime_type, '.');
    if (req_lang == NULL) {
      req_lang = strrchr(tp->mime_type, '/');
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

    GtkSourceLanguageManager *lmgr = gtk_source_language_manager_get_default();
    GtkSourceLanguage *lang = gtk_source_language_manager_get_language
                               (lmgr, (req_lang + 1));
    if (lang != NULL)
      tp->textbuffer = gtk_source_buffer_new_with_language(lang);
    else {
      if ((strcmp((req_lang + 1), "troff") != 0) &&
          (strcmp((req_lang + 1), "rtf") != 0))
printf("no lang for (%s) (%s) (%s)\n", tp->filename, (req_lang + 1), tp->cs_encoding);
      tp->textbuffer = gtk_source_buffer_new(NULL);
    }
  } else {
    tp->textbuffer = gtk_source_buffer_new(NULL);
  }
    /* filename NULL on new file with "untitled" */
    /* probable add on create type of file on new, with name */
  if (tp->filename != NULL) {
      /* must isolate, conflicting types for locales */
    char *buffer = phnx_buffered_file(tp);
    if (buffer == NULL) {
//TODO: run msg error dialog
puts("ERROR: file didn't open");
      goto add_exit;
    }
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(tp->textbuffer), buffer, -1);
    free(buffer);
  }
    // icon check
    char *req_lang = strrchr(tp->mime_type, '.');
    if (req_lang == NULL)
      req_lang = strrchr(tp->mime_type, '/');
    if (req_lang != NULL) {
      strcpy(&iPath[iAppend], (req_lang + 1));
      strcat(iPath, ".svg");
      struct stat buf;
      if (stat(iPath, &buf) != 0)
        if (strcmp((req_lang + 1), "rtf") != 0)
printf("missing icon (%s) for (%s)\n", iPath, tp->filename);
    } else {
printf("NULL icon () for (%s) file: (%s)\n", tp->mime_type, tp->filename);
    }
    // create new view
  return tp;

    // failure on open
add_exit:
//TODO: run msg error dialog
puts("failed to open");
  free(tp->filename);
  free(tp->mime_type);
  free(tp->cs_encoding);
  return NULL;
}

/* this runs all files in test files, also checks matching icon */
#if 1
int
main(void) {

  int status;
  char dirname[128] = { "/home/steven/Development/Projects/LCode/mime_parser/test_files/" };
  size_t dlen = strlen(dirname);
  DIR *dirp = opendir(dirname);
  struct dirent *ent;
    /* walk each entry of root directory */
  while ((ent = readdir(dirp)) != NULL) {
      /* skip system files */
      /* skip system files */
    if (ent->d_name[0] == '.') {
      if ((ent->d_name[1] == 0) || (ent->d_name[1] == '.'))
        continue;
    }
    strcpy(&dirname[dlen], ent->d_name);
    LCITextPort tp = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0 };
/* set all LCITextPort entries, store in outdata */
/* file data to retrieve listed in tp->filename */
    tp.filename = dirname;
    tp.d_name = &dirname[dlen];
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
/* normally do something with LCITextPort before free(ing) */
    if (lci_add_view(&tp) != NULL) {
      free(tp.filename);
      free(tp.mime_type);
      free(tp.cs_encoding);
      g_object_unref(G_OBJECT(tp.textbuffer));
    }
    odx++;
  }
  closedir(dirp);
printf("files checked: %d\n", odx);
  return 0;
}
#else
/* intent to run on files starting from a directory,
 * recursive directory entry (find dir files) */

char dirname[2048];

int
walk_directory(char *filename, size_t dlen) {

  DIR *dirp = opendir(filename);
  if (dirp == NULL)  return 0;
  struct dirent *ent;
    /* walk each entry of root directory */
  while ((ent = readdir(dirp)) != NULL) {
      /* skip system files */
    if (ent->d_name[0] == '.') {
      if ((ent->d_name[1] == 0) || (ent->d_name[1] == '.'))
        continue;
    }
    strcpy(&dirname[dlen], ent->d_name);
    struct stat buf;
    stat(dirname, &buf);
    if (S_ISDIR(buf.st_mode)) {
      size_t nlen = strlen(dirname);
      dirname[nlen] = '/';
      dirname[(++nlen)] = 0;
      walk_directory(dirname, nlen);
      continue;
    }
    int status;
    LCITextPort tp = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0 };
    tp.filename = dirname;
    tp.d_name = &dirname[dlen];
    if ((status = lci_file_info(&tp, dirname)) != 0) {
      // handle errors
      if (status == -1)  continue;
      free(tp.filename);
      free(tp.mime_type);
      free(tp.cs_encoding);
      continue;
    }
      // error == NULL, info freed in lci_add_view()
    if (lci_add_view(&tp) != NULL) {
        // no err, just free up, goto new test
      free(tp.filename);
      free(tp.mime_type);
      free(tp.cs_encoding);
      g_object_unref(G_OBJECT(tp.textbuffer));
    }
      // check off that reviewed a file
    odx++;
  }
  closedir(dirp);
  return 0;
}

int
main(void) {

// used to read mac
//  realpath("/mnt/steven", dirname);
// on linux, my home
  realpath("/home/steven", dirname);
  size_t dlen = strlen(dirname);
  dirname[dlen] = '/';
  dirname[(++dlen)] = 0;
  walk_directory(dirname, dlen);
printf("files checked: %d\n", odx);
  return 0;
}
#endif
