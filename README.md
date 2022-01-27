# file_info
mime type info retrival from a file, with text mime icons  
c function to gather resolved filename, mime type, and file encoding.  
REQUIRES  
'file' version >= 5.32, linux utility program, only tested on 5.32  
  
FUNCTION API  
int  lci_file_info(LCITextPort \*tp, char \*filename);  
INPUT  
filename: the file to retrieve data on  
tp: a pointer to structure containing members:  
  char \*filename;  
  char \*d_name;  
  char \*mime_type;  
  char \*cs_encoding;  
RETURN  
Returns -1 on file open/existance errors, no tp data returned.  
Returns 1 on a non-text file, tp contains the 4 requested data.  
Returns 0 text file, no error, tp contains the 4 requested data.  

Intended to be modified by user for personal use.  
Sequence for use:  
make parser  
./parser  
This will create 'lci_file_info.c'.  
'make parser' pulls in all files in language-specs directory, and create_mime_parser.c.  
'./parser' merges lci_mp.h, info.c, and the data obtained from the
make parser stage, to create 'lci_file_info.c'.  
  
mime type for text files returns mimes classified as text/x-??????.???? or text/?????.  
text/???? are iana mime defined, where text/x-??????.???? are extensions with first set of '?'s states a genre, and
second set of '?'s the actual language classication closely matching a syntax parser to use.  
  
language-specs directory contains gtksourceview languages. It contains those from the 3.0 dev package  
and patched gtksourceview languages from 5.3.2 to work with 3.0.  
  
mime-icons directory contains icons, currently 128, 16px sizes, to cover every text mime type,  
except rtf, lci_file_info() can return.
  
The example files are intended to show use, integration. They require Gtk and gtksourceview 3.0.
Since was designed on Ubuntu 18, I believe its Gtk 3.22.
