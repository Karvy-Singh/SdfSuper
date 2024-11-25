#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "wow.h"
#include "roughtui.h"

#define MAX_ITEMS 5

int window;
// Function prototypes
void init_ncursesc();
void end_ncurses();
int wrap_text(const char *text, int max_width, char ***lines);
void draw_bordered_window(WINDOW *win, int selected);
int draw_menu_item(WINDOW *win, int y, int x, MenuItem item, int selected, int max_width);
void draw_menu(WINDOW *win, MenuItem items[], int selected_item, int start_item);
void get_ascii_art_dimensions(const char *ascii_art, int *height, int *width);


void draw_menu_in_window(WINDOW* win, MenuItem* items, int max_x, int max_y, int selected_item, int start_item){
    werase(win);
    box(win, 0, 0);
    draw_menu(win, items, selected_item, start_item);
    wrefresh(win);
}

int handle_menu_input(WINDOW* win, int* selected_item, int* start_item, MenuItem* items, int max_x, int max_y){
  int ch = wgetch(win);
  switch (ch) {
        case KEY_LEFT:
          return 0; 
      case KEY_UP:
          if ((*selected_item) > 0) {
              (*selected_item)--;
              if ((*selected_item) < (*start_item)) {
                  (*start_item)--;
              }
          }
          break;
      case KEY_DOWN:
          if ((*selected_item) < MAX_ITEMS - 1) {
              (*selected_item)++;
              // Determine how many items can be displayed at once
              // Assuming each item takes up to 10 lines, adjust as needed
              int visible_items = (max_y - 4) / 10;
              if ((*selected_item) >= (*start_item) + visible_items) {
                  (*start_item)++;
              }
          }
          break;
      case 10: // Enter key
          // Handle item selection
          //werase(stdscr);
          //mvprintw(max_y / 2, (max_x - strlen("You selected: ")) / 2, "You selected: %s", items[(*selected_item)].heading);
          refresh();
          // Wait for user to press a key
          getch();
          break;
    //   case 'q':
    //   case 'Q':
    //       // Exit the program
    //       end_ncurses();
    //       return 0;
  }

  return 1;

}

int content_static(WINDOW* menu_win) {
    // Initialize ncurses
    init_ncursesc();

    // Sample menu items
   MenuItem items[MAX_ITEMS] = {
        {
            "  /\\\n"
            " /  \\\n"
            "/____\\",
            "Item 1",
            "Order_id= KHA60785GH Prod_info=mobile charger FSN:MACFGTDGXTZB qty=1 amt=405 Dispatch_Date= 1/12/24"

        },
        {
            "  ___\n"
            " / _ \\\n"
            "| (_) |\n"
            " \\___/",
            "Item 2",
            "Order_id= KHA60785GH Prod_info=mobile charger FSN:MACFGTDGXTZB qty=1 amt=405 Dispatch_Date= 1/12/24"
        },
        {
            " ____ \n"
            "| _ | \n"
            "||_|| \n"
            "|___|",
            "Item 3",
            "Order_id= KHA60785GH Prod_info=mobile charger FSN:MACFGTDGXTZB qty=1 amt=405 Dispatch_Date= 1/12/24"

        },
        {
            " _____\n"
            "|  __ \\\n"
            "| |__) |\n"
            "|_____/ ",
            "Item 4",
            "Order_id= KHA60785GH Prod_info=mobile charger FSN:MACFGTDGXTZB qty=1 amt=405 Dispatch_Date= 1/12/24"


        },
        {
             "  /\\\n"
            " //\\\\\n"
            "/____\\",
            "Item 5",
            "Order_id= KHA60785GH Prod_info=mobile charger FSN:MACFGTDGXTZB qty=1 amt=405 Dispatch_Date= 1/12/24"

        }
    };

    int selected_item = 0;
    int start_item = 0;
    int ch;
    int max_y, max_x;

    // Get screen dimensions
    getmaxyx(stdscr, max_y, max_x);

    // Create a new window for the menu
    keypad(menu_win, TRUE);

    // Main loop
    while (1) {
        draw_menu_in_window(menu_win, items, max_x, max_y, selected_item, start_item);
        if (handle_menu_input(menu_win, &selected_item, &start_item, items, max_x, max_y) == 0){
            return 0;
        }
    }

    // End ncurses mode
    end_ncurses();
    return 0;
}
// Initialize ncurses
void init_ncursesc() {
    initscr();            // Start curses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo while we do getch
    keypad(stdscr, TRUE); // Enable function keys
    curs_set(0);          // Hide the cursor
};

// End ncurses
void end_ncurses() {
    endwin(); // End curses mode
};

// Function to wrap text into lines of maximum width
int wrap_text(const char *text, int max_width, char ***lines) {
    int num_lines = 0;
    int capacity = 10; // Initial capacity for the lines array
    *lines = malloc(capacity * sizeof(char *));
    if (*lines == NULL) {
        return 0;
    }

    const char *start = text;
    const char *end;
    int line_len;

    while (*start) {
        // Skip leading spaces
        while (*start == ' ') start++;

        // Find how much can fit in one line
        end = start;
        line_len = 0;
        const char *last_space = NULL;
        while (*end && line_len < max_width) {
            if (*end == ' ') {
                last_space = end;
            }
            end++;
            line_len++;
        }

        if (line_len >= max_width && last_space && last_space > start) {
            // Break at the last space
            end = last_space;
        }

        // Calculate length of the line
        int len = end - start;
        char *line = malloc(len + 1);
        if (line == NULL) {
            break;
        }
        strncpy(line, start, len);
        line[len] = '\0';

        // Add the line to the array
        if (num_lines >= capacity) {
            capacity *= 2;
            char **temp = realloc(*lines, capacity * sizeof(char *));
            if (temp == NULL) {
                free(line);
                break;
            }
            *lines = temp;
        }
        (*lines)[num_lines++] = line;

        // Move to the start of the next line
        start = (*end) ? end + 1 : end;
    }

    return num_lines;
};

// Function to draw a bordered window with appropriate attributes
void draw_bordered_window(WINDOW *win, int selected) {
    // Determine attributes based on selection
    attr_t attr = selected ? A_BOLD : A_DIM;

    // Draw top border
    wattron(win, attr);
    mvwaddch(win, 0, 0, ACS_ULCORNER);
    for (int i = 1; i < getmaxx(win) - 1; i++) {
        mvwaddch(win, 0, i, ACS_HLINE);
    }
    mvwaddch(win, 0, getmaxx(win) - 1, ACS_URCORNER);

    // Draw sides
    for (int i = 1; i < getmaxy(win) - 1; i++) {
        mvwaddch(win, i, 0, ACS_VLINE);
        mvwaddch(win, i, getmaxx(win) - 1, ACS_VLINE);
    }

    // Draw bottom border
    mvwaddch(win, getmaxy(win) - 1, 0, ACS_LLCORNER);
    for (int i = 1; i < getmaxx(win) - 1; i++) {
        mvwaddch(win, getmaxy(win) - 1, i, ACS_HLINE);
    }
    mvwaddch(win, getmaxy(win) - 1, getmaxx(win) - 1, ACS_LRCORNER);
    wattroff(win, attr);
};

// Function to draw a single menu item
int draw_menu_item(WINDOW *win, int y, int x, MenuItem item, int selected, int max_width) {
    int art_height, art_width;
    get_ascii_art_dimensions(item.ascii_art, &art_height, &art_width);

    // Calculate available width and height for text
    int available_width = max_width - 2; // Subtract borders
    int text_start_x = x + art_width + 3; // Space between art and text
    int text_width = available_width - art_width - 3; // Additional space for padding

    // Wrap the description text
    char **wrapped_lines = NULL;
    int num_lines = wrap_text(item.description, text_width, &wrapped_lines);

    // Calculate item height based on content
    int description_height = num_lines;
    int content_height = (description_height > art_height) ? description_height : art_height;
    int item_height = content_height + 2; // Add top and bottom borders

    // Create a sub-window for the menu item
    WINDOW *item_win = derwin(win, item_height, max_width, y, x);
    if (item_win == NULL) {
        return 0;
    }

    // Draw border with appropriate attributes
    draw_bordered_window(item_win, selected);

    // Display ASCII art line by line
    int art_y = 1;
    char *art_copy = strdup(item.ascii_art);
    if (art_copy == NULL) {
        delwin(item_win);
        return item_height;
    }
    char *line = strtok(art_copy, "\n");
    while (line != NULL && art_y < item_height - 1) {
        mvwprintw(item_win, art_y++, 1, "%s", line);
        line = strtok(NULL, "\n");
    }
    free(art_copy);

    // Display heading in bold
    wattron(item_win, A_BOLD);
    mvwprintw(item_win, 1, art_width + 3, "%s", item.heading);
    wattroff(item_win, A_BOLD);

    // Display wrapped description
    for (int i = 0; i < num_lines && (2 + i) < item_height - 1; i++) {
        mvwprintw(item_win, 2 + i, art_width + 3, "%s", wrapped_lines[i]);
        free(wrapped_lines[i]);
    }
    free(wrapped_lines);

    // Refresh the item window to show changes
    wrefresh(item_win);

    // Clean up
    delwin(item_win);

    return item_height;
};

// Function to draw the entire menu
void draw_menu(WINDOW *win, MenuItem items[], int selected_item, int start_item) {
    int y = 1; // Starting y position in the window
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    for (int i = start_item; i < MAX_ITEMS && y < max_y - 1; i++) {
        int selected = (i == selected_item);
        // Draw each menu item and get its height
        int item_height = draw_menu_item(win, y, 1, items[i], selected, max_x - 2);
        y += item_height; // Add spacing between items
    }
};

// Function to get ASCII art dimensions
void get_ascii_art_dimensions(const char *ascii_art, int *height, int *width) {
    *height = 0;
    *width = 0;
    int current_width = 0;
    const char *ptr = ascii_art;

    while (*ptr) {
        if (*ptr == '\n') {
            (*height)++;
            if (current_width > *width) {
                *width = current_width;
            }
            current_width = 0;
        } else {
            current_width++;
        }
        ptr++;
    }
    // Account for the last line if it doesn't end with '\n'
    if (current_width > 0) {
        (*height)++;
        if (current_width > *width) {
            *width = current_width;
        }
    }
};
