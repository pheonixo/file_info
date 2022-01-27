verify: lci_verify_text_mime.c ../lci_file_info.c
	gcc `pkg-config --cflags gtk+-3.0` -I/usr/include/gtksourceview-3.0 -o verify lci_verify_text_mime.c ../lci_file_info.c `pkg-config --libs gtk+-3.0` -lgtksourceview-3.0

tall: run_all_test_files.c ../lci_file_info.c libpfile.a
	gcc `pkg-config --cflags gtk+-3.0` -I/usr/include/gtksourceview-3.0 -o tall run_all_test_files.c ../lci_file_info.c `pkg-config --libs gtk+-3.0` -lgtksourceview-3.0 -L. -lpfile

