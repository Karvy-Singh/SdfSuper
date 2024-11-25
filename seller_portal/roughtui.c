#include <curses.h>
#include <stdio.h>
#include <menu.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "portal.h"
#include "wow.h"
#include "roughtui.h"
#include "product_new.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4

char *choices[] = {
    "ORDERS", "INVENTORY", "SALES"
};

void run_tui() {
    int rows, cols;

    // Initialize ncurses mode
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh(); // Refresh the screen to initialize it properly

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
    attron(A_BOLD);
    mvwprintw(nav_win, navbar_h / 2, 3, "JAINWIN COMMERCIAL ENTERPRISES");
    refresh();

    ITEM **my_items;
    int c;
    MENU *my_menu;
    int n_choices, i;

    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    
    // Populate the menu items
    for (i = 0; i < n_choices; ++i) {
        my_items[i] = new_item(choices[i], "");
    }
    my_items[n_choices] = (ITEM *)NULL;

    my_menu = new_menu((ITEM **)my_items);
    set_menu_win(my_menu, sidebar_win);
    set_menu_sub(my_menu, derwin(sidebar_win, 6, 36, 1, 1));

    // Draw boxes around each window
    box(nav_win, 0, 0);
    box(sidebar_win, 0, 0);
    box(content_win, 0, 0);

    post_menu(my_menu);

    // Refresh each window to display it
    refresh();
    wrefresh(nav_win);
    wrefresh(sidebar_win);
    wrefresh(content_win);

    while ((c = getch()) != KEY_F(1)) {
        switch (c) {
            case KEY_DOWN:
                if (window == 0 || window==2) { // Placeholder: Define or remove "window" variable
                    menu_driver(my_menu, REQ_DOWN_ITEM);
                    break;
                }
            case KEY_UP:
                if (window == 0 || window==2) { // Placeholder: Define or remove "window" variable
                    menu_driver(my_menu, REQ_UP_ITEM);
                    break;
                }
            case KEY_RIGHT:
            case 10:
                if (strcmp(item_name(current_item(my_menu)), "ORDERS") == 0) {
                    move(navbar_h + 1, side_c + 1);
                    if (content_static(content_win) == 0) { // Placeholder: Define "content_static"
                        window = 0; // Placeholder: Define or remove "window"
                    }
                }
                if (strcmp(item_name(current_item(my_menu)), "SALES") == 0) {
                    wclear(content_win);
                    box(content_win, 0, 0);
                    wrefresh(content_win);
                    move(navbar_h + 1, side_c + 1);
                    printw("February| ");
                    for (int i = 0; i < 30; i++) {
                        addch(' ' | A_REVERSE);
                    }
                    move(navbar_h + 3, side_c + 1);
                    printw("March|    ");
                    for (int i = 0; i < 20; i++) {
                        addch(' ' | A_REVERSE);
                    }
                    move(navbar_h + 5, side_c + 1);
                    printw("April|    ");
                    for (int i = 0; i < 50; i++) {
                        addch(' ' | A_REVERSE);
                    }
                    move(navbar_h + 7, side_c + 1);
                    printw("May|      ");
                    for (int i = 0; i < 40; i++) {
                        addch(' ' | A_REVERSE);
                    }
                    move(navbar_h + 9, side_c + 1);
                    printw("June|     ");
                    for (int i = 0; i < 70; i++) {
                        addch(' ' | A_REVERSE);
                    }
                    refresh();
                    break;
                }
                if (strcmp(item_name(current_item(my_menu)), "INVENTORY") == 0){
                    int disp_ret = 1;
                    while (disp_ret == 1){
                      disp_ret = display_products_from_file("Products.csv", content_win);
                      if(disp_ret == 2){
                          window=2;
                      }
                    }
                }
        }
        wrefresh(sidebar_win);
        refresh();
    }

    // Free resources
    for (i = 0; i < n_choices; i++) {
        free_item(my_items[i]);
    }
    free_menu(my_menu);
    endwin();
}
