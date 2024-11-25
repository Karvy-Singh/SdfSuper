#include <ncurses.h>

typedef struct {
    char *ascii_art;
    char *heading; 
    char *description;
} MenuItem;

void draw_menu_in_window(WINDOW* win, MenuItem* items, int max_x, int max_y, int selected_item, int start_item);
int handle_menu_input(WINDOW* win, int* selected_item, int* start_item, MenuItem* items, int max_x, int max_y);
int content_static(WINDOW* menu_win);
extern int window;