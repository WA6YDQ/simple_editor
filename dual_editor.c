#include<ncurses.h> // it also include stdio.h and unctl.h
void initial() {
    initscr();// start curses mode
    cbreak();// opposite of nocbreak(), disable 'buffer stdin until newline entered'
    nonl();// opposite of nl(), treat enter as \n or not, default is yes
    noecho();// opposite of noecho(), display inputed characters or not, default is yes
    intrflush(stdscr, FALSE);// speed up response when character such as break inputed if TRUE
    keypad(stdscr, TRUE);// transfer special key such as arrows to KEY_STH
    refresh(); // refresh changes to monitor
}
int main() {
    WINDOW *win[2], *curwin, *helpwin;
    int nowwin;
    int x, y;
    int ch;
    initial();
    // size of windows
    win[0] = newwin(LINES / 2 - 1, COLS - 1, 0, 0);
    win[1] = newwin(LINES / 2 - 1, COLS - 1, LINES / 2, 0);
    helpwin = newwin(3, 30, 2, COLS / 2 - 15);
    box(helpwin, '|', '-');
    mvwaddstr(helpwin, 0, 10, "HELP");
    mvwaddstr(helpwin, 1, 4, "Hit any key to continue..");

    for (int i = 0; i < COLS - 1; i++) {
        mvaddch(LINES / 2 - 1, i, '-'); // write border line
    }
    nowwin = 0;
    curwin = win[nowwin];
    getyx(curwin, y, x);
    move(0, 0);
    refresh();
    refresh();
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
                x = 0;
                break;
            case '\t': // tab, pop up help window
                touchwin(helpwin);
                wrefresh(helpwin); // call help window
                getch();
                // rewrite 1, 2 window
                touchwin(win[1 - nowwin]);
                wrefresh(win[1 - nowwin]);
                touchwin(curwin);
                wrefresh(curwin);
                break;
            // backspace key can lead different return values depending on the platform, the terminal and the current setting
            case KEY_BACKSPACE:
            case 127:
            case '\b':
                x--;
                wmove(curwin, y, x);
            case KEY_DC:// DEL
                waddch(curwin, ' '); // delete a character
                break;
            case 27: // [ESC]
                nowwin = 1 - nowwin;
                curwin = win[nowwin];
                getyx(curwin, y, x);
                break;
            case 'q':
                endwin();
                return 0;
                break;
            default:
                waddch(curwin, ch); // display the character
                x++;
                break;
        }
        wmove(curwin, y, x);
        wrefresh(curwin);
    } while(1);
}
