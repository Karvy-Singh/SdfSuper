#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include "product.h"
#include "display.h"
#include "cart.h"
#include "portal.h"

// Define screen types using integer constants
#define SCREEN_NONE 0
#define SCREEN_WELCOME 1
#define SCREEN_BROWSE_PRODUCTS 2
#define SCREEN_MY_CART 3
#define SCREEN_MANAGE_ACCOUNT 4

int start_customer_portal() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);  // Hide the cursor
    keypad(stdscr, TRUE);

    // Ensure getch() is blocking
    timeout(-1);

    // Start color if possible
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);  // For focused window border
        init_pair(2, COLOR_BLACK, COLOR_WHITE); // For menu selection
    }

    // Get terminal size
    int height, width;
    getmaxyx(stdscr, height, width);

    // Calculate dimensions
    int top_height = height * 0.15;
    int side_width = width * 0.3;
    int content_height = height - top_height;
    int content_width = width - side_width;

    // Create windows
    WINDOW *top_win = newwin(top_height, width, 0, 0);
    WINDOW *sidebar_win = newwin(content_height, side_width, top_height, 0);
    WINDOW *content_win = newwin(content_height, content_width, top_height, side_width);

    // Enable keypad for windows
    keypad(sidebar_win, TRUE);
    keypad(content_win, TRUE);

    // Box the windows
    box(top_win, 0, 0);
    box(sidebar_win, 0, 0);
    box(content_win, 0, 0);

    // Print application name in top window
    char *app_name = "E-commerce Portal";
    mvwprintw(top_win, top_height / 2, (width - strlen(app_name)) / 2, "%s", app_name);
    wrefresh(top_win);

    // Sidebar menu options
    ITEM *items[5];
    items[0] = new_item("Browse Products", "");
    items[1] = new_item("My Cart", "");
    items[2] = new_item("Manage Account", "");
    items[3] = new_item("Logout", "");
    items[4] = NULL;

    MENU *menu = new_menu(items);
    set_menu_win(menu, sidebar_win);

    // Get the size of the sidebar window
    int sidebar_height, sidebar_width;
    getmaxyx(sidebar_win, sidebar_height, sidebar_width);

    // Create a subwindow for the menu inside the sidebar window
    set_menu_sub(menu, derwin(sidebar_win, sidebar_height - 2, sidebar_width - 2, 1, 1));
    set_menu_format(menu, sidebar_height - 2, 1);
    set_menu_mark(menu, " > ");

    // Set menu options for coloring
    if (has_colors()) {
        set_menu_fore(menu, COLOR_PAIR(2) | A_REVERSE);
        set_menu_back(menu, COLOR_PAIR(0));
    }

    // Post the menu
    post_menu(menu);
    wrefresh(sidebar_win);
    wrefresh(menu_sub(menu));

    // Initial focus on sidebar
    int focus = 0;

    // Highlight focused window border
    if (has_colors()) {
        wattron(sidebar_win, COLOR_PAIR(1));
        box(sidebar_win, 0, 0);
        wattroff(sidebar_win, COLOR_PAIR(1));
    } else {
        box(sidebar_win, 0, 0);
    }

    wrefresh(sidebar_win);
    wrefresh(menu_sub(menu));

    // Display initial content in content window (e.g., welcome message)
    int current_screen = SCREEN_WELCOME;
    wclear(content_win);
    box(content_win, 0, 0);
    mvwprintw(content_win, content_height / 2, (content_width - 21) / 2, "Welcome to our Store!");
    wrefresh(content_win);

    Product **products = NULL;
    int n_products = 0;
    int selected_product = 0;
    int first_visible_product = 0;

    int running = 1;
    int c;
    while (running) {
        c = wgetch((focus == 0) ? sidebar_win : content_win);

        if (c == ERR) {
            // No input received, continue the loop
            continue;
        }

        if (c == 'q') {
            running = 0;
            break;
        }

        if (focus == 0) {
            // Focus is on sidebar
            switch (c) {
                case KEY_DOWN:
                    menu_driver(menu, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(menu, REQ_UP_ITEM);
                    break;
                case 10: { // Enter key
                    ITEM *cur_item = current_item(menu);
                    const char *name = item_name(cur_item);

                    if (strcmp(name, "Browse Products") == 0) {
                        // Load products
                        if (products != NULL) {
                            free_products2(products);
                        }
                        products = load_products2();
                        // Count products
                        n_products = 0;
                        while (products[n_products] != NULL) n_products++;
                        selected_product = 0;
                        first_visible_product = 0;
                        current_screen = SCREEN_BROWSE_PRODUCTS;
                        // Display products
                        wclear(content_win);
                        box(content_win, 0, 0);
                        display_products(content_win, products, selected_product, &first_visible_product);
                        wrefresh(content_win);
                    } else if (strcmp(name, "My Cart") == 0) {
                        current_screen = SCREEN_MY_CART;
                        // Display the cart
                        wclear(content_win);
                        box(content_win, 0, 0);
                        display_cart(content_win);
                        wrefresh(content_win);
                    } else if (strcmp(name, "Manage Account") == 0) {
                        current_screen = SCREEN_MANAGE_ACCOUNT;
                        // Placeholder for manage account functionality
                        wclear(content_win);
                        box(content_win, 0, 0);
                        mvwprintw(content_win, 1, 2, "Manage Account - Feature Coming Soon!");
                        wrefresh(content_win);
                    } else if (strcmp(name, "Logout") == 0) {
                        running = 0;
                        break;
                    }
                    break;
                }
                case KEY_RIGHT:
                    // Switch focus to content window
                    focus = 1;
                    // Update window borders
                    // Unhighlight sidebar
                    box(sidebar_win, 0, 0);
                    wrefresh(sidebar_win);
                    // Highlight content window
                    if (has_colors()) {
                        wattron(content_win, COLOR_PAIR(1));
                        box(content_win, 0, 0);
                        wattroff(content_win, COLOR_PAIR(1));
                    } else {
                        box(content_win, 0, 0);
                    }
                    wrefresh(content_win);
                    break;
                default:
                    break;
            }
            // Refresh the menu and sidebar window
            wrefresh(sidebar_win);
            wrefresh(menu_sub(menu));
        } else if (focus == 1) {
            // Focus is on content window
            switch (c) {
                case KEY_LEFT:
                    // Switch focus to sidebar
                    focus = 0;
                    // Update window borders
                    // Unhighlight content window
                    box(content_win, 0, 0);
                    wrefresh(content_win);
                    // Highlight sidebar
                    if (has_colors()) {
                        wattron(sidebar_win, COLOR_PAIR(1));
                        box(sidebar_win, 0, 0);
                        wattroff(sidebar_win, COLOR_PAIR(1));
                    } else {
                        box(sidebar_win, 0, 0);
                    }
                    wrefresh(sidebar_win);
                    wrefresh(menu_sub(menu));
                    break;
                case KEY_DOWN:
                    if (current_screen == SCREEN_BROWSE_PRODUCTS && products != NULL) {
                        if (selected_product < n_products - 1) {
                            selected_product++;
                            wclear(content_win);
                            box(content_win, 0, 0);
                            display_products(content_win, products, selected_product, &first_visible_product);
                            wrefresh(content_win);
                        }
                    }
                    break;
                case KEY_UP:
                    if (current_screen == SCREEN_BROWSE_PRODUCTS && products != NULL) {
                        if (selected_product > 0) {
                            selected_product--;
                            wclear(content_win);
                            box(content_win, 0, 0);
                            display_products(content_win, products, selected_product, &first_visible_product);
                            wrefresh(content_win);
                        }
                    }
                    break;
                case 10: // Enter key
                    if (current_screen == SCREEN_BROWSE_PRODUCTS && products != NULL) {
                        add_to_cart(products[selected_product]);
                    }
                    break;
                default:
                    break;
            }
            wrefresh(content_win);
        }
    }
    // Cleanup
    if (products != NULL) {
        free_products2(products);
    }

    free_cart();
    unpost_menu(menu);
    free_menu(menu);
    for (int i = 0; i < 4; ++i)
        free_item(items[i]);

    delwin(top_win);
    delwin(sidebar_win);
    delwin(content_win);

    endwin();
    return 0;
}
