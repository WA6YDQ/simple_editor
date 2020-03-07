all: editor dual_editor
editor: editor.c
	gcc editor.c -lcurses -o editor
dual_editor: dual_editor.c
	gcc dual_editor.c -lcurses -o dual_editor
