#include <ncurses.h>

int main() {
    int rows, cols;

    // Initialize ncurses mode
    initscr();
    refresh();  // Refresh the screen to initialize it properly

    // Get the terminal size
    getmaxyx(stdscr, rows, cols);

    WINDOW *nav_win;
    WINDOW *sidebar_win;
    WINDOW *content_win;

    int navbar_h = rows * 15 / 100;
    int side_c = cols * 30 / 100;

    // Create windows with specific dimensions
    nav_win = newwin(navbar_h, cols, 0, 0);
    sidebar_win = newwin(rows - navbar_h, side_c, navbar_h, 0);
    content_win = newwin(rows - navbar_h, cols - side_c, navbar_h, side_c);

    // Draw boxes around each window
    box(nav_win, 0, 0);
    box(sidebar_win, 0, 0);
    box(content_win, 0, 0);

    // Refresh each window to display it
    wrefresh(nav_win);
    wrefresh(sidebar_win);
    wrefresh(content_win);

    // Wait for user input to end
    getch();
    endwin();

    return 0;
}
