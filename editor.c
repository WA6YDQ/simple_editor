#include <ncurses.h> // it also include stdio.h and unctl.h
#include <string.h>

char lines[1024][1024] = {0};
int linelen[1024] = {0};
int linenum = 0;
char *filename = NULL;

void initial() {
    initscr();// start curses mode
    cbreak();// opposite of nocbreak(), disable 'buffer stdin until newline entered'
    nonl();// opposite of nl(), treat enter as \n or not, default is yes
    noecho();// opposite of noecho(), display inputed characters or not, default is yes
    intrflush(stdscr, FALSE);// speed up response when character such as break inputed if TRUE
    keypad(stdscr, TRUE);// transfer special key such as arrows to KEY_STH
    refresh(); // refresh changes to monitor
}
void logs(char* msg) {
    FILE *f = fopen("log", "a+");
    fputs(msg, f);
    fclose(f);
}
void read_file() {
    FILE *f = NULL;
    if (filename) {
        f = fopen(filename, "r");
    }
    if (f == NULL) {
        logs("qq\n");
        return;
    }
    for (int i = 0; i < 1024 && fgets(lines[i], 1024, f) != NULL; i++){
        logs(lines[i]);
        linelen[i] = strlen(lines[i]);
        if (lines[i][linelen[i] - 1] == '\n') {
            lines[i][linelen[i] - 1] = 0;
            linelen[i]--;
        }
    }
    fclose(f);
}
void displaylines() {
    for (int i = 0; i < LINES - 2; i++){
        lines[i][COLS - 2] = 0;
        mvaddstr(i + 1, 1, lines[i]);
    }
}
void save_content() {
    for(int y = 1; y < LINES - 1; y++) {
        for(int x = 1; x < COLS - 1; x++) {
            lines[y - 1][x - 1] = mvinch(y, x) & A_CHARTEXT;
            if (lines[y - 1][x - 1] != ' ') {
                linenum = y - 1;
            }
        }
    }
}
void fill_back(int y, int x) {
    if (x >= linelen[y - 1])
        return;
    int i;
    for (i = x; i < linelen[y - 1] + 1; i++) {
        char ch = mvinch(y, i + 1) & A_CHARTEXT;
        mvaddch(y, i, ch);
    }
    linelen[y - 1]--;
}
void write_file() {
    FILE *f;
    if (filename) {
        f = fopen(filename, "w");
    } else {
        return;
    }
    for (int i = 0; i <= linenum && lines[i][0] != 0; i++) {
        if (linelen[i] > 0) {
            lines[i][linelen[i]] = 0;
            fputs(lines[i], f);
        }
        fputs("\n", f);
    }
    fclose(f);
}
int main(int argc, char **argv) {
    int x = 1;
    int y = 1;
    int ch;
    if (argc >= 2) {
        filename = argv[1];
    }
    read_file();
    initial();

    box(stdscr, '|', '-'); // write a box, vertical with |, horizontal with -
    attron(A_REVERSE);// decorate terminal, change terminal mode to Anti-White
    mvaddstr(0, 20, "Simple Editor");// output anti-white character at (20, 0)
    attroff(A_REVERSE);// turn off anti-white mode
    displaylines();
    move(x, y); // move curser
    do {
        ch = getch();
        switch(ch) {
            case KEY_UP:
                y--;
                break;
            case KEY_DOWN:
                y++;
                break;
            case KEY_RIGHT:
                x++;
                break;
            case KEY_LEFT:
                x--;
                break;
            case '\r': // ENTER
                y++;
                x = 1;
                break;
            case '\t': // tab
                x += 7;
                break;
            // backspace key can lead different return values depending on the platform, the terminal and the current setting
            case KEY_BACKSPACE:
            case 127:
            case '\b':
                x--;
            case KEY_DC:// DEL
                fill_back(y, x);
                break;
            case 27: // [ESC]
                // close curses mode, must called before system(), system calls
                // to return curses mode, wrefresh() doupdate()
                save_content();
                endwin();
                write_file();
                return 0;
            case KEY_HOME:
                x = 1;
                break;
            case KEY_END:
                x = linelen[y - 1];
                break;
            default:
                addch(ch); // display the character
                x++;
                if (x >= linelen[y - 1])
                    linelen[y - 1] = x;
                break;
        }
        move(y, x);
    } while(1);
}
