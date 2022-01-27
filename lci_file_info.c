#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        // strncmp() name with wild card ender
static struct _nwlist {
  char name[32];  char mime[32];
} name_wc_list[] = {
  { ".gtkrc-", "text/x-other.gtkrc" },
  { "ChangeLog", "text/x-other.changelog" },
  { "gtkrc-", "text/x-other.gtkrc" }
};
        // strcmp() names
static struct _nlist {
  char name[32];  char mime[32];
} name_list[] = {
  { ".bash_profile", "text/x-script.sh" },
  { ".bashrc", "text/x-script.sh" },
  { ".gtkrc", "text/x-other.gtkrc" },
  { ".htaccess", "text/x-other.apache" },
  { ".profile", "text/x-script.sh" },
  { "CMakeLists.txt", "text/x-other.cmake" },
  { "Capfile", "text/x-script.ruby" },
  { "Dockerfile", "text/x-source.docker" },
  { "GNUmakefile", "text/x-source.makefile" },
  { "GNUmakefile.am", "text/x-source.automake" },
  { "Gemfile", "text/x-script.ruby" },
  { "Makefile", "text/x-source.makefile" },
  { "Makefile.am", "text/x-source.automake" },
  { "Rakefile", "text/x-script.ruby" },
  { "apache.conf", "text/x-other.apache" },
  { "configure.ac", "text/x-script.m4" },
  { "configure.in", "text/x-script.m4" },
  { "gtkrc", "text/x-other.gtkrc" },
  { "gtkrc", "text/x-other.gtkrc" },
  { "httpd.conf", "text/x-other.apache" },
  { "makefile", "text/x-source.makefile" },
  { "meson.build", "text/x-other.meson" },
  { "meson_options.txt", "text/x-other.meson" }
};
        // strcmp() extensions, begins with '*.'
static struct _elist {
  char ext[32];  char mime[32];
} extension_list[] = {
  { "4th", "text/x-source.ansforth94" },
  { "C", "text/x-source.cpp" },
  { "Cakefile", "text/x-script.coffee" },
  { "DEM", "text/x-science.maxima" },
  { "F", "text/x-source.fortran" },
  { "F90", "text/x-source.fortran" },
  { "MAC", "text/x-science.maxima" },
  { "R", "text/x-science.r" },
  { "Rhistory", "text/x-science.r" },
  { "Rnw", "text/x-markup.sweave" },
  { "Rout", "text/x-science.r" },
  { "Rout.fail", "text/x-science.r" },
  { "Rout.save", "text/x-science.r" },
  { "Rt", "text/x-science.r" },
  { "S", "text/x-source.asm" },
  { "Snw", "text/x-markup.sweave" },
  { "WXM", "text/x-science.maxima" },
  { "abnf", "text/x-source.abnf" },
  { "abw", "text/x-markup.xml" },
  { "adb", "text/x-source.ada" },
  { "adoc", "text/x-markup.asciidoc" },
  { "ads", "text/x-source.ada" },
  { "al", "text/x-script.perl" },
  { "as", "text/x-source.actionscript" },
  { "asciidoc", "text/x-markup.asciidoc" },
  { "asd", "text/x-source.commonlisp" },
  { "asm", "text/x-source.asm" },
  { "asp", "text/x-source.asp" },
  { "awk", "text/x-script.awk" },
  { "bat", "text/x-script.dosbatch" },
  { "bbl", "text/x-markup.latex" },
  { "bib", "text/x-markup.bibtex" },
  { "boo", "text/x-source.boo" },
  { "bs", "text/x-source.bluespec" },
  { "bsv", "text/x-source.bluespec" },
  { "c", "text/x-source.c" },
  { "c++", "text/x-source.cpp" },
  { "cbd", "text/x-source.cobol" },
  { "cbl", "text/x-source.cobol" },
  { "cc", "text/x-source.cpp" },
  { "cdb", "text/x-source.cobol" },
  { "cdc", "text/x-source.cobol" },
  { "cfc", "text/x-markup.cfml" },
  { "cfm", "text/x-markup.cfml" },
  { "cfml", "text/x-markup.cfml" },
  { "cg", "text/x-source.cg" },
  { "cif", "text/x-science.cif" },
  { "cl", "text/x-source.opencl" },
  { "cls", "text/x-markup.latex" },
  { "cmake", "text/x-other.cmake" },
  { "cmake.in", "text/x-other.cmake" },
  { "cmd", "text/x-script.dosbatch" },
  { "cob", "text/x-source.cobol" },
  { "coffee", "text/x-script.coffee" },
  { "cpp", "text/x-source.cpp" },
  { "cs", "text/x-source.csharp" },
  { "css", "text/x-other.css" },
  { "csv", "text/x-other.csv" },
  { "ctest", "text/x-other.cmake" },
  { "ctest.in", "text/x-other.cmake" },
  { "cu", "text/x-source.cuda" },
  { "cuh", "text/x-source.cuda" },
  { "cxx", "text/x-source.cpp" },
  { "d", "text/x-source.d" },
  { "dart", "text/x-source.dart" },
  { "dbk", "text/x-markup.docbook" },
  { "dem", "text/x-science.maxima" },
  { "desktop", "text/x-other.desktop" },
  { "diff", "text/x-other.diff" },
  { "docbook", "text/x-markup.docbook" },
  { "dot", "text/x-other.dot" },
  { "dpatch", "text/x-other.dpatch" },
  { "dtd", "text/x-markup.dtd" },
  { "dtl", "text/x-markup.dtl" },
  { "dtx", "text/x-markup.latex" },
  { "e", "text/x-source.eiffel" },
  { "eco", "text/x-markup.rhtml" },
  { "eif", "text/x-source.eiffel" },
  { "erb", "text/x-markup.erb" },
  { "erl", "text/x-source.erlang" },
  { "ex", "text/x-source.elixir" },
  { "exs", "text/x-source.elixir" },
  { "f", "text/x-source.fortran" },
  { "f90", "text/x-source.fortran" },
  { "f95", "text/x-source.fortran" },
  { "fcl", "text/x-science.fcl" },
  { "feature", "text/x-other.cucumber" },
  { "fish", "text/x-script.fish" },
  { "flex", "text/x-source.lex" },
  { "fo", "text/x-markup.xml" },
  { "for", "text/x-source.fortran" },
  { "forth", "text/x-source.ansforth94" },
  { "fr", "text/x-source.forth" },
  { "frt", "text/x-source.forth" },
  { "fs", "text/x-source.fsharp" },
  { "fsi", "text/x-source.fsharp" },
  { "fth", "text/x-source.forth" },
  { "ftl", "text/x-other.fluent" },
  { "g", "text/x-science.gap" },
  { "gap", "text/x-science.gap" },
  { "gd", "text/x-script.gdscript" },
  { "gdb", "text/x-other.gdb-log" },
  { "gemspec", "text/x-script.ruby" },
  { "geojson", "text/x-other.json" },
  { "gi", "text/x-science.gap" },
  { "glade", "text/x-markup.xml" },
  { "glsl", "text/x-source.glsl" },
  { "go", "text/x-source.go" },
  { "gradle", "text/x-other.gradle" },
  { "groovy", "text/x-source.groovy" },
  { "gs", "text/x-source.genie" },
  { "gsp", "text/x-markup.gsp" },
  { "gtkrc", "text/x-other.gtkrc" },
  { "gv", "text/x-other.dot" },
  { "h", "text/x-source.chdr" },
  { "h++", "text/x-source.cpphdr" },
  { "haml", "text/x-markup.haml" },
  { "hcl", "text/x-other.terraform" },
  { "hh", "text/x-source.cpphdr" },
  { "hp", "text/x-source.cpphdr" },
  { "hpp", "text/x-source.cpphdr" },
  { "hrl", "text/x-source.erlang" },
  { "hs", "text/x-source.haskell" },
  { "htm", "text/x-markup.html" },
  { "html", "text/x-markup.html" },
  { "html.erb", "text/x-markup.erb-html" },
  { "hx", "text/x-source.haxe" },
  { "idl", "text/x-science.idl" },
  { "ijm", "text/x-science.imagej" },
  { "ijs", "text/x-source.j" },
  { "impl", "text/x-source.opal" },
  { "inc", "text/x-source.asm" },
  { "ini", "text/x-other.ini" },
  { "ins", "text/x-markup.latex" },
  { "j", "text/x-source.objj" },
  { "jade", "text/x-markup.jade" },
  { "java", "text/x-source.java" },
  { "jeco", "text/x-markup.rhtml" },
  { "jl", "text/x-science.julia" },
  { "jnlp", "text/x-markup.xml" },
  { "js", "text/x-script.javascript" },
  { "js.erb", "text/x-markup.erb-js" },
  { "json", "text/x-other.json" },
  { "jsx", "text/x-script.jsx" },
  { "kdelnk", "text/x-other.desktop" },
  { "kino", "text/x-markup.xml" },
  { "kt", "text/x-source.kotlin" },
  { "kts", "text/x-source.kotlin" },
  { "l", "text/x-source.lex" },
  { "la", "text/x-other.libtool" },
  { "lai", "text/x-other.libtool" },
  { "lean", "text/x-source.lean" },
  { "less", "text/x-other.less" },
  { "lex", "text/x-source.lex" },
  { "lgt", "text/x-source.logtalk" },
  { "lhs", "text/x-source.haskell-literate" },
  { "lisp", "text/x-source.commonlisp" },
  { "ll", "text/x-source.llvm" },
  { "lo", "text/x-other.libtool" },
  { "lock", "text/x-other.toml" },
  { "logcat", "text/x-other.logcat" },
  { "lsp", "text/x-source.commonlisp" },
  { "ltx", "text/x-markup.latex" },
  { "lua", "text/x-script.lua" },
  { "m", "text/x-source.objc" },
  { "m", "text/x-science.octave" },
  { "m4", "text/x-script.m4" },
  { "mac", "text/x-science.maxima" },
  { "mak", "text/x-source.makefile" },
  { "make", "text/x-source.makefile" },
  { "mako", "text/x-script.mako" },
  { "markdown", "text/x-markup.markdown" },
  { "mat", "text/x-vnd.matlab" },
  { "mc", "text/x-science.maxima" },
  { "md", "text/x-markup.markdown" },
  { "mex", "text/x-science.octave" },
  { "mif", "text/x-science.cif" },
  { "mjs", "text/x-script.javascript" },
  { "mk", "text/x-source.makefile" },
  { "mkd", "text/x-markup.markdown" },
  { "ml", "text/x-source.ocaml" },
  { "mli", "text/x-source.ocaml" },
  { "mll", "text/x-source.ocaml" },
  { "mly", "text/x-source.ocaml" },
  { "mm", "text/x-source.objc" },
  { "mml", "text/x-markup.xml" },
  { "mo", "text/x-science.modelica" },
  { "mop", "text/x-science.modelica" },
  { "mxml", "text/x-markup.mxml" },
  { "n", "text/x-source.nemerle" },
  { "nrx", "text/x-source.netrexx" },
  { "nsh", "text/x-other.nsis" },
  { "nsi", "text/x-other.nsis" },
  { "ocl", "text/x-other.ocl" },
  { "oct", "text/x-science.octave" },
  { "ooc", "text/x-source.ooc" },
  { "p", "text/x-source.pascal" },
  { "page", "text/x-markup.mallard" },
  { "pas", "text/x-source.pascal" },
  { "patch", "text/x-other.diff" },
  { "pc", "text/x-other.pkgconfig" },
  { "pch", "text/x-source.c" },
  { "perl", "text/x-script.perl" },
  { "php", "text/x-script.php" },
  { "php3", "text/x-script.php" },
  { "php4", "text/x-script.php" },
  { "phtml", "text/x-script.php" },
  { "pig", "text/x-source.pig" },
  { "pl", "text/x-script.perl" },
  { "plist", "text/x-markup.xml" },
  { "pm", "text/x-script.perl" },
  { "po", "text/x-other.po" },
  { "pot", "text/x-other.po" },
  { "pp", "text/x-other.puppet" },
  { "prg", "text/x-source.bennugd" },
  { "pro", "text/x-science.idl" },
  { "prolog", "text/x-source.prolog" },
  { "proto", "text/x-other.protobuf" },
  { "ps1", "text/x-source.powershell" },
  { "psd1", "text/x-source.powershell" },
  { "psm1", "text/x-source.powershell" },
  { "pug", "text/x-markup.jade" },
  { "py", "text/x-script.python" },
  { "py3", "text/x-script.python3" },
  { "pyi", "text/x-script.python3" },
  { "r", "text/x-science.r" },
  { "rake", "text/x-script.ruby" },
  { "rb", "text/x-script.ruby" },
  { "rda", "text/x-science.r" },
  { "rdata", "text/x-science.r" },
  { "rdf", "text/x-markup.xml" },
  { "rds", "text/x-science.r" },
  { "rej", "text/x-other.diff" },
  { "rhtml", "text/x-markup.rhtml" },
  { "rnw", "text/x-markup.sweave" },
  { "rq", "text/x-other.sparql" },
  { "rs", "text/x-source.rust" },
  { "rss", "text/x-markup.xml" },
  { "rst", "text/x-markup.rst" },
  { "ru", "text/x-script.rubyonrails" },
  { "s", "text/x-source.asm" },
  { "sass", "text/x-other.sass" },
  { "scala", "text/x-source.scala" },
  { "sce", "text/x-science.scilab" },
  { "sci", "text/x-science.scilab" },
  { "scm", "text/x-source.scheme" },
  { "scss", "text/x-other.scss" },
  { "sh", "text/x-script.sh" },
  { "sig", "text/x-source.sml" },
  { "sign", "text/x-source.opal" },
  { "siv", "text/x-markup.xml" },
  { "slim", "text/x-markup.slim" },
  { "smi", "text/x-markup.xml" },
  { "smil", "text/x-markup.xml" },
  { "sml", "text/x-source.sml" },
  { "snw", "text/x-markup.sweave" },
  { "sol", "text/x-source.solidity" },
  { "solidity", "text/x-source.solidity" },
  { "spec", "text/x-other.rpmspec" },
  { "sql", "text/x-source.sql" },
  { "ss", "text/x-source.scheme" },
  { "str", "text/x-science.cif" },
  { "sty", "text/x-markup.latex" },
  { "sv", "text/x-source.systemverilog" },
  { "svg", "text/x-markup.xml" },
  { "svh", "text/x-source.systemverilog" },
  { "swift", "text/x-source.swift" },
  { "sys", "text/x-script.dosbatch" },
  { "t", "text/x-script.perl" },
  { "t2t", "text/x-markup.t2t" },
  { "tcl", "text/x-script.tcl" },
  { "tex", "text/x-markup.latex" },
  { "texi", "text/x-markup.texinfo" },
  { "texinfo", "text/x-markup.texinfo" },
  { "tf", "text/x-other.terraform" },
  { "tfvars", "text/x-other.terraform" },
  { "thrift", "text/x-source.thrift" },
  { "tk", "text/x-script.tcl" },
  { "tm", "text/x-script.tcl" },
  { "tml", "text/x-other.toml" },
  { "toml", "text/x-other.toml" },
  { "topojson", "text/x-other.json" },
  { "tpp", "text/x-source.cpp" },
  { "ts", "text/x-script.typescript" },
  { "tsx", "text/x-script.typescript-jsx" },
  { "v", "text/x-source.verilog" },
  { "vala", "text/x-source.vala" },
  { "vapi", "text/x-source.vala" },
  { "vb", "text/x-source.vbnet" },
  { "vhd", "text/x-source.vhdl" },
  { "vhdl", "text/x-source.vhdl" },
  { "wml", "text/x-markup.xml" },
  { "wxm", "text/x-science.maxima" },
  { "xbel", "text/x-markup.xml" },
  { "xhtml", "text/x-markup.xml" },
  { "xib", "text/x-markup.xml" },
  { "xmi", "text/x-markup.xml" },
  { "xml", "text/x-markup.xml" },
  { "xsl", "text/x-markup.xslt" },
  { "xslfo", "text/x-markup.xml" },
  { "xslt", "text/x-markup.xslt" },
  { "xspf", "text/x-markup.xml" },
  { "xul", "text/x-markup.xml" },
  { "y", "text/x-other.yacc" },
  { "yacc", "text/x-other.yacc" },
  { "yaml", "text/x-other.yaml" },
  { "yar", "text/x-other.yara" },
  { "yara", "text/x-other.yara" },
  { "yml", "text/x-other.yaml" },
  { "zabw", "text/x-markup.xml" }
};

static char *
__m_differienate(char *filename) {

  char scanstr[1024];
  char *r_objc = "text/x-source.objc";
  char *r_octave = "text/x-science.octave";
  //char *r_matlab = "text/x-vnd.matlab";
#ifdef __APPLE__
  char *r_str = r_objc;
#else
  char *r_str = r_octave;
#endif

  FILE *rh = fopen(filename, "r");
  do {
    int count = fscanf(rh, "%s%*[^\n]\n", scanstr);
    if (count == EOF) break;
    if (count == 0) break;
    if (scanstr[0] == '/') {
      if ((scanstr[1] == '/') || (scanstr[1] == '*')) {
        r_str = r_objc;  break;  }
      continue;
    }
    if (scanstr[0] == '%') {
      r_str = r_octave;  break;  }
    if (scanstr[0] == '#') {
      if (scanstr[1] == 0) {
        r_str = r_octave;  break;  }
      if (strncmp("#import", scanstr, 7)) {
        r_str = r_objc;  break;  }
      if (strncmp("#include", scanstr, 8)) {
        r_str = r_objc;  break;  }
      continue;
    }
    if (strcmp("function", scanstr)) {
      r_str = r_octave;  break;  }
  } while (1);
  fclose(rh);
  return r_str;
}

static int
compare(const void *a0, const void *b0) {
  return strcmp((const char *)a0, (const char *)b0);
}

static int
lci_verify_text_mime(LCITextPort *tp) {

  void *bsptr;
  char strptr[32] = { "text/plain" };
  char *rstrptr = strptr;
  if (strncmp(tp->mime_type, "text", 4) != 0)  return 1;
  if (strcmp(tp->mime_type, "text/x-shellscript") == 0) {
    rstrptr = "text/x-script.sh";
    goto ptr_exit;
  }
    // allow override of plain with iana types, but check if gets overriden
  if    (strcmp(tp->mime_type, "text/calandar") == 0) {
      rstrptr = "text/calendar";
  } else if (strcmp(tp->mime_type, "text/html") == 0) {
    rstrptr = "text/html";
  } else if (strcmp(tp->mime_type, "text/rtf") == 0) {
      rstrptr = "text/rtf";
  } else if (strcmp(tp->mime_type, "text/troff") == 0) {
      rstrptr = "text/troff";
  } else if (strcmp(tp->mime_type, "text/xml") == 0) {
      rstrptr = "text/x-markup.xml";
  }
    // set a default found by 'file', match up to phnx if can
  if ((tp->mime_type[5] == 'x') && (tp->mime_type[6] == '-')) {
    bsptr = bsearch(&(tp->mime_type[7]), extension_list,
                    sizeof(extension_list)/sizeof(extension_list[0]),
                    sizeof(extension_list[0]), compare);
    if (bsptr != NULL)
      rstrptr = ((struct _elist*)bsptr)->mime;
    else {
      if (strcmp(&tp->mime_type[7], "msdos-batch") == 0) {
        rstrptr = "text/x-script.dosbatch";
        goto ptr_exit;
      }
      sprintf(&strptr[5], "x-other.%s", &tp->mime_type[7]);
    }
  }
        // strncmp()
  if (strncmp(tp->d_name, name_wc_list[0].name, 7) == 0) {
    rstrptr = name_wc_list[0].mime;
    goto ptr_exit;
  }
  if (strncmp(tp->d_name, name_wc_list[1].name, 9) == 0) {
    rstrptr = name_wc_list[1].mime;
    goto ptr_exit;
  }
  if (strncmp(tp->d_name, name_wc_list[2].name, 6) == 0) {
    rstrptr = name_wc_list[2].mime;
    goto ptr_exit;
  }
  bsptr = bsearch(tp->d_name, name_list,
                  sizeof(name_list)/sizeof(name_list[0]),
                  sizeof(name_list[0]), compare);
  if (bsptr != NULL) {
    rstrptr = ((struct _nlist*)bsptr)->mime;
    goto ptr_exit;
  }
  char *extptr = strrchr(tp->d_name, '.');
  if (extptr != NULL) {
    extptr++;
        // strcmp() extensions, iana defined
    if (strcmp(extptr, "css") == 0) {
      rstrptr = "text/css";
      goto ptr_exit;
    }
    if (strcmp(extptr, "csv") == 0) {
      rstrptr = "text/csv";
      goto ptr_exit;
    }
    if (strcmp(extptr, "dot") == 0) {
      rstrptr = "text/vnd.graphviz";
      goto ptr_exit;
    }
    if (strcmp(extptr, "html") == 0) {
      rstrptr = "text/html";
      goto ptr_exit;
    }
    if (strcmp(extptr, "m") == 0) {
      rstrptr = __m_differienate(tp->filename);
      goto ptr_exit;
    }
    if (strcmp(extptr, "gv") == 0) {
      rstrptr = "text/vnd.graphviz";
      goto ptr_exit;
    }
    if (strcmp(extptr, "md") == 0) {
      rstrptr = "text/markdown";
      goto ptr_exit;
    }
        // strcmp() extensions
    bsptr = bsearch(extptr, extension_list,
                    sizeof(extension_list)/sizeof(extension_list[0]),
                    sizeof(extension_list[0]), compare);
    if (bsptr != NULL)
      rstrptr = ((struct _elist*)bsptr)->mime;
  }
ptr_exit:
  if (tp->mime_type != NULL)
    free(tp->mime_type);
  tp->mime_type = strdup(rstrptr);
  return 0;
}

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

