// this is parameter passed between routines to gather data
// extra values are from my routines, intended to be altered
// by user for thier needs

typedef struct _LciTextPort {
    /* these need for me at this point in time */
  void  *text_window;  // GtkWidget scrolled window
  void  *textview;     // GtkWidget
  void  *textbuffer;   // GtkSourceBuffer
    /* these are the actual needs for mime parser */
  char  *filename;     // inode name
  char  *d_name;       // pointer to start of displayed name
  char  *mime_type;    // classified file type
  char  *cs_encoding;  // codeset on openning
    /* this needed for me, used with my LCITextPort(s) */
  int   edx;           // multiple ports, this port index value
} LCITextPort;

