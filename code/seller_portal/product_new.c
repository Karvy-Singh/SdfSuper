#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <menu.h>
#include "product_new.h"

#define MAX_LINE_LENGTH 1024
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// Function prototypes
static int display_products(Product ***products_ptr, WINDOW *win);

Product **load_products(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open product CSV file");
        return NULL;
    }

    Product **products = NULL;
    size_t count = 0;
    size_t capacity = 10;
    products = malloc(capacity * sizeof(Product *));
    if (!products) {
        fclose(file);
        perror("Memory allocation error");
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (strlen(line) <= 1) continue;

        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Parse CSV line
        char *token;
        int id = 0;
        char *name = NULL;
        char *desc = NULL;
        int qty = 0;
        double price = 0.0;
        char *cat = NULL;
        double stars = 0.0;

        // Tokenize the line
        token = strtok(line, ",");
        if (token) {
            id = atoi(token);
        }

        token = strtok(NULL, ",");
        if (token) {
            name = strdup(token);
        }

        token = strtok(NULL, ",");
        if (token) {
            desc = strdup(token);
        }

        token = strtok(NULL, ",");
        if (token) {
            qty = atoi(token);
        }

        token = strtok(NULL, ",");
        if (token) {
            price = atof(token);
        }

        token = strtok(NULL, ",");
        if (token) {
            cat = strdup(token);
        }

        token = strtok(NULL, ",");
        if (token) {
            stars = atof(token);
        }

        // Create a new Product
        Product *product = malloc(sizeof(Product));
        if (!product) {
            perror("Memory allocation error");
            free(name);
            free(desc);
            free(cat);
            continue;
        }
        product->id = id;
        product->name = name ? name : strdup("Unnamed Product");
        product->desc = desc ? desc : strdup("No description");
        product->qty = qty;
        product->price = price;
        product->cat = cat ? cat : strdup("Uncategorized");
        product->stars = stars;

        // Add the product to the array
        if (count >= capacity) {
            capacity *= 2;
            Product **temp = realloc(products, capacity * sizeof(Product *));
            if (!temp) {
                perror("Memory allocation error");
                free_products(products);
                fclose(file);
                return NULL;
            }
            products = temp;
        }
        products[count++] = product;
    }

    fclose(file);

    // Null-terminate the array
    products = realloc(products, (count + 1) * sizeof(Product *));
    if (!products) {
        perror("Memory allocation error");
        return NULL;
    }
    products[count] = NULL;

    return products;
}

void free_products(Product **products) {
    if (!products) return;
    for (size_t i = 0; products[i] != NULL; ++i) {
        free(products[i]->name);
        free(products[i]->desc);
        free(products[i]->cat);
        free(products[i]);
    }
    free(products);
}

int save_products_to_csv(const char *filename, Product **products) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Could not open file for saving");
        return -1;
    }

    for (int i = 0; products[i] != NULL; ++i) {
        fprintf(file, "%d,%s,%s,%d,%.2f,%s,%.1f\n",
                products[i]->id,
                products[i]->name,
                products[i]->desc,
                products[i]->qty,
                products[i]->price,
                products[i]->cat,
                products[i]->stars);
    }

    fclose(file);
    return 0; // Success
}

int delete_product(Product ***products_ptr, int id) {
    Product **products = *products_ptr;
    int i = 0;

    // Find the product by ID
    while (products[i] != NULL) {
        if (products[i]->id == id) {
            // Free the product's memory
            free(products[i]->name);
            free(products[i]->desc);
            free(products[i]->cat);
            free(products[i]);

            // Shift remaining products in the array
            int j = i;
            while (products[j + 1] != NULL) {
                products[j] = products[j + 1];
                j++;
            }
            products[j] = NULL;

            // Reallocate the array
            *products_ptr = realloc(products, (j + 1) * sizeof(Product *));
            if (!*products_ptr) {
                perror("Memory allocation error");
                return -1;
            }
            return 0; // Success
        }
        i++;
    }
    return -1; // Product not found

}

int add_new_product(Product ***products_ptr, WINDOW *win) {
    Product **products = *products_ptr;
    int n_products = 0;
    while (products[n_products] != NULL) n_products++;

    // Clear the window for input
    werase(win);
    box(win, 0, 0);
    wrefresh(win);

    echo();
    curs_set(1);
    char input_str[256];

    // ID
    mvwprintw(win, 2, 2, "Enter Product ID: ");
    wclrtoeol(win);
    wrefresh(win);
    wgetnstr(win, input_str, sizeof(input_str));
    int new_id = atoi(input_str);

    // Check for duplicate IDs
    for (int i = 0; i < n_products; i++) {
        if (products[i]->id == new_id) {
            mvwprintw(win, 3, 2, "ID already exists! Press any key to return.");
            wclrtoeol(win);
            wrefresh(win);
            noecho();
            curs_set(0);
            wgetch(win);
            return 1;
        }
    }

    // Name
    mvwprintw(win, 3, 2, "Enter Product Name: ");
    wclrtoeol(win);
    wrefresh(win);
    wgetnstr(win, input_str, sizeof(input_str));
    char *new_name = strdup(input_str);

    // Description
    mvwprintw(win, 4, 2, "Enter Product Description: ");
    wclrtoeol(win);
    wrefresh(win);
    wgetnstr(win, input_str, sizeof(input_str));
    char *new_desc = strdup(input_str);

    // Quantity
    mvwprintw(win, 5, 2, "Enter Quantity: ");
    wclrtoeol(win);
    wrefresh(win);
    wgetnstr(win, input_str, sizeof(input_str));
    int new_qty = atoi(input_str);

    // Price
    mvwprintw(win, 6, 2, "Enter Price: ");
    wclrtoeol(win);
    wrefresh(win);
    wgetnstr(win, input_str, sizeof(input_str));
    double new_price = atof(input_str);

    // Category
    mvwprintw(win, 7, 2, "Enter Category: ");
    wclrtoeol(win);
    wrefresh(win);
    wgetnstr(win, input_str, sizeof(input_str));
    char *new_cat = strdup(input_str);

    // Stars
    mvwprintw(win, 8, 2, "Enter Stars: ");
    wclrtoeol(win);
    wrefresh(win);
    wgetnstr(win, input_str, sizeof(input_str));
    double new_stars = atof(input_str);

    noecho();
    curs_set(0);

    // Create new product
    Product *new_product = malloc(sizeof(Product));
    if (!new_product) {
        mvwprintw(win, 9, 2, "Memory allocation error! Press any key to return.");
        wclrtoeol(win);
        wrefresh(win);
        wgetch(win);
        return 1;
    }
    new_product->id = new_id;
    new_product->name = new_name;
    new_product->desc = new_desc;
    new_product->qty = new_qty;
    new_product->price = new_price;
    new_product->cat = new_cat;
    new_product->stars = new_stars;

    // Append to products array
    Product **new_products = realloc(products, (n_products + 2) * sizeof(Product *));
    if (!new_products) {
        mvwprintw(win, 9, 2, "Memory allocation error! Press any key to return.");
        wclrtoeol(win);
        wrefresh(win);
        wgetch(win);
        free(new_product->name);
        free(new_product->desc);
        free(new_product->cat);
        free(new_product);
        return 1;
    }
    new_products[n_products] = new_product;
    new_products[n_products + 1] = NULL; // Null-terminate the array
    *products_ptr = new_products; // Update the pointer to reflect the reallocation

    // Save to CSV
    save_products_to_csv("Products.csv", new_products);

    // Confirmation
    mvwprintw(win, 9, 2, "Product added successfully! Press any key to return.");
    wclrtoeol(win);
    wrefresh(win);
    wgetch(win);

    return 0;
}


int display_products_from_file(const char *filename, WINDOW *win) {
    // Load products from CSV
    Product **products = load_products(filename);
    if (!products) {
        // Handle error
        fprintf(stderr, "Failed to load products from %s\n", filename);
        return 0;
    }

    // Initialize ncurses (should be done in main if already initialized)
    // initscr();
    // cbreak();
    // noecho();
    // curs_set(0);  // Hide the cursor

    // Start color if possible
    if (has_colors()) {
        start_color();
        use_default_colors();
    }

    // Display products with navigation
    int result = display_products(&products, win);

    // Cleanup
    free_products(products);
    endwin();

    return result;
}

static int display_products(Product ***products_ptr, WINDOW *win) {
    Product **products = *products_ptr;
    int height, width;
    getmaxyx(win, height, width);

    int n_products = 0;
    while (products[n_products] != NULL) n_products++;
    int n_menu_items = n_products + 1;

    int selected_product = 0;
    int first_visible_product = 0;

    int ch;
    keypad(win, TRUE);

    while (1) {
        werase(win);
        box(win, 0, 0);

        int y = 1;
        for (int menu_index = first_visible_product; menu_index < n_menu_items && y < height - 1; ++menu_index) {
            if (selected_product == menu_index) {
                wattron(win, A_REVERSE);
            }

            if (menu_index == 0) {
                mvwprintw(win, y++, 1, "ADD New Product");
            } else {
                int product_index = menu_index - 1; // Adjust index
                mvwprintw(win, y++, 1, "%d: %s", products[product_index]->id, products[product_index]->name);
            }

            if (selected_product == menu_index) {
                wattroff(win, A_REVERSE);
            }
        }   
    //     int y = 1;

    //     if (selected_product == 0) {
    //         wattron(win, A_REVERSE);
    //     }
    //     mvwprintw(win, y++, 1, "ADD New Product");
    //     if (selected_product == 0) {
    //         wattroff(win, A_REVERSE);
    //     }

    //    for (int i = first_visible_product; i < n_menu_items && y < height - 1; ++i) {
    //         int product_index = i + 1; // Adjust index because of "ADD" option at index 0
    //         if (product_index == selected_product) {
    //             wattron(win, A_REVERSE);
    //         }
    //         mvwprintw(win, y++, 1, "%d: %s", products[i]->id, products[i]->name);
    //         if (product_index == selected_product) {
    //             wattroff(win, A_REVERSE);
    //         }
    //     }
        wrefresh(win);

        ch = wgetch(win);
        if (ch == KEY_UP) {
            if (selected_product > 0) {
                selected_product--;
                if (selected_product < first_visible_product) {
                    first_visible_product--;
                }
            }
        } else if (ch == KEY_LEFT) {
            return 2;
        } else if (ch == KEY_DOWN) {
            if (selected_product < n_menu_items - 1) {
                selected_product++;
                if (selected_product - first_visible_product >= height - 2) {
                    first_visible_product++;
                }
            }
        } else if (ch == 10) { // Enter key
            // Show product details
            if (selected_product == 0) {
                // "ADD" option selected
                int res = add_new_product(products_ptr, win);

                // After adding, refresh the products array and counts
                products = *products_ptr;
                n_products = 0;
                while (products[n_products] != NULL) n_products++;
                n_menu_items = n_products + 1;

                // Reload the window
                return 1;
               }
        
            
            werase(win);
            box(win, 0, 0);
            Product *p = products[selected_product-1];
            mvwprintw(win, 1, 1, "Product Details:");
            mvwprintw(win, 3, 1, "ID: %d", p->id);
            mvwprintw(win, 4, 1, "Name: %s", p->name);
            mvwprintw(win, 5, 1, "Description: %s", p->desc);
            mvwprintw(win, 6, 1, "Quantity: %d", p->qty);
            mvwprintw(win, 7, 1, "Price: $%.2f", p->price);
            mvwprintw(win, 8, 1, "Category: %s", p->cat);
            mvwprintw(win, 9, 1, "Stars: %.1f", p->stars);

            // Handling the secondary menu
            ITEM **my_items = NULL;
            MENU *my_menu = NULL;
            int n_choices2 = 3;

            const char *choices2[] = {
                "UPDATE",
                "DELETE",
                "Exit Submenu",
            };

            // Initialize the secondary menu
            my_items = (ITEM **)calloc(n_choices2 + 1, sizeof(ITEM *));
            if (!my_items) {
                mvwprintw(win, height - 2, 1, "Memory allocation error!");
                wrefresh(win);
                wgetch(win);
                break;
            }
            for (int i = 0; i < n_choices2; ++i) {
                my_items[i] = new_item(choices2[i], "");
                if (!my_items[i]) {
                    mvwprintw(win, height - 2, 1, "Memory allocation error!");
                    wrefresh(win);
                    wgetch(win);
                    break;
                }
            }
            my_items[n_choices2] = NULL;

            my_menu = new_menu((ITEM **)my_items);
            if (!my_menu) {
                mvwprintw(win, height - 2, 1, "Menu creation error!");
                wrefresh(win);
                wgetch(win);
                break;
            }

            // Assign the secondary menu to the same window
            set_menu_win(my_menu, win);
            set_menu_sub(my_menu, derwin(win, 10, width-2, 14, 2)); // Adjusted position
            curs_set(0);
            post_menu(my_menu);
            wrefresh(win);

            // Inner loop for handling the secondary menu
            while ((ch = wgetch(win)) != 'q') {
                if (ch == KEY_DOWN || ch == KEY_UP) {
                    menu_driver(my_menu, ch == KEY_DOWN ? REQ_DOWN_ITEM : REQ_UP_ITEM);
                } else if (ch == 10) { // Enter key
                    ITEM *cur_item = current_item(my_menu);
                    if (cur_item == NULL) {
                        continue; // No item selected, continue loop
                    }
                    const char *selected_option = item_name(cur_item);
                    if (strcmp(selected_option, "Exit Submenu") == 0) {
                        return 1;
                        break; // Exit the submenu loop
                    } else if (strcmp(selected_option, "DELETE") == 0) {
                        // Delete the product
                        delete_product(products_ptr, p->id);
                        save_products_to_csv("Products.csv", *products_ptr);

                        // Update products pointer and counts
                        products = *products_ptr;
                        n_products = 0;
                        while (products[n_products] != NULL) n_products++;

                        if (selected_product >= n_products) {
                            selected_product = n_products - 1;
                        }
                        return 1;
                        break; // Exit submenu to refresh main menu
                    } else if (strcmp(selected_option, "UPDATE") == 0) {
                        
                         unpost_menu(my_menu);
                        free_menu(my_menu);
                        for (int i = 0; i < n_choices2; ++i) {
                            if (my_items[i]) free_item(my_items[i]);
                        }
                        free(my_items);
                        my_menu = NULL;
                        my_items = NULL;
                        wrefresh(win);
                        // Create a submenu for selecting which field to update
                        ITEM **field_items = NULL;
                        MENU *field_menu = NULL;
                        int n_fields = 8; // Number of fields to update
                        const char *field_choices[] = {
                            "ID",
                            "Name",
                            "Description",
                            "Quantity",
                            "Price",
                            "Category",
                            "Stars",
                            "Done"
                        };

                        // Initialize the field selection menu
                        field_items = (ITEM **)calloc(n_fields + 1, sizeof(ITEM *));
                        if (!field_items) {
                            mvwprintw(win, height - 2, 1, "Memory allocation error!");
                            wrefresh(win);
                            wgetch(win);
                            break;
                        }
                        for (int i = 0; i < n_fields; ++i) {
                            field_items[i] = new_item(field_choices[i], "");
                            if (!field_items[i]) {
                                mvwprintw(win, height - 2, 1, "Memory allocation error!");
                                wrefresh(win);
                                wgetch(win);
                                break;
                            }
                        }
                        field_items[n_fields] = NULL;

                        field_menu = new_menu((ITEM **)field_items);
                        if (!field_menu) {
                            mvwprintw(win, height - 2, 1, "Menu creation error!");
                            wrefresh(win);
                            wgetch(win);
                            break;
                        }
                        set_menu_win(field_menu, win);
                        WINDOW* sub_win=derwin(win, n_fields + 2, width - 2, 14, 2);
                        set_menu_sub(field_menu, sub_win);
                        post_menu(field_menu);
                        wrefresh(win);

                        // Inner loop for field selection
                        int updating = 1;
                        while (updating) {
                            ch = wgetch(win);
                            if (ch == KEY_DOWN || ch == KEY_UP) {
                                menu_driver(field_menu, ch == KEY_DOWN ? REQ_DOWN_ITEM : REQ_UP_ITEM);
                            } else if (ch == 10) { // Enter key
                                ITEM *cur_field_item = current_item(field_menu);
                                if (cur_field_item == NULL) {
                                    continue; // No item selected, continue loop
                                }
                                const char *selected_field = item_name(cur_field_item);
                                if (strcmp(selected_field, "Done") == 0) {
                                    updating = 0;
                                    save_products_to_csv("Products.csv", products);
                                    return 1;
                                     // Exit the field selection loop
                                } else {
                                    // Prompt for new value
                                    echo();
                                    curs_set(1);
                                    char input_str[256] = {0};
                                    double input_double;
                                    int input_int;

                                    mvwprintw(win, height - 3, 1, "Enter new %s: ", selected_field);
                                    wclrtoeol(win);
                                    wrefresh(win);

                                    if (strcmp(selected_field, "ID") == 0) {
                                        wgetnstr(win, input_str, sizeof(input_str));
                                        input_int = atoi(input_str);

                                        // Check for duplicate IDs
                                        int id_exists = 0;
                                        for (int idx = 0; idx < n_products; idx++) {
                                            if (products[idx]->id == input_int && products[idx] != p) {
                                                id_exists = 1;
                                                break;
                                            }
                                        }
                                        if (id_exists) {
                                            mvwprintw(win, height - 2, 1, "ID already exists!");
                                            wclrtoeol(win);
                                            wrefresh(win);
                                            wgetch(win);
                                        } else {
                                            p->id = input_int;
                                        }
                                    } else if (strcmp(selected_field, "Quantity") == 0) {
                                        wgetnstr(win, input_str, sizeof(input_str));
                                        input_int = atoi(input_str);
                                        p->qty = input_int;
                                    } else if (strcmp(selected_field, "Price") == 0) {
                                        wgetnstr(win, input_str, sizeof(input_str));
                                        input_double = atof(input_str);
                                        p->price = input_double;
                                    } else if (strcmp(selected_field, "Stars") == 0) {
                                        wgetnstr(win, input_str, sizeof(input_str));
                                        input_double = atof(input_str);
                                        p->stars = input_double;
                                    } else {
                                        wgetnstr(win, input_str, sizeof(input_str));
                                        if (strlen(input_str) > 0) {
                                            if (strcmp(selected_field, "Name") == 0) {
                                                free(p->name);
                                                p->name = strdup(input_str);
                                            } else if (strcmp(selected_field, "Description") == 0) {
                                                free(p->desc);
                                                p->desc = strdup(input_str);
                                            } else if (strcmp(selected_field, "Category") == 0) {
                                                free(p->cat);
                                                p->cat = strdup(input_str);
                                            }
                                        }
                                    }

                                    noecho();
                                    curs_set(0);

                                    // Clear the input prompt
                                    mvwprintw(win, height - 3, 1, "                                    ");
                                    wclrtoeol(win);
                                    wrefresh(win);
                                }
                            }
                        }

                        // Cleanup field selection menu
                        unpost_menu(field_menu);
                        free_menu(my_menu);
                        for (int i = 0; i < n_fields; ++i) {
                            if (field_items[i]) free_item(field_items[i]);
                        }
                        if (field_menu) free_menu(field_menu);
                        free(field_items);
                        my_menu = NULL;
                        field_items = NULL;
                        delwin(sub_win);
                        wrefresh(sub_win);
                        wrefresh(win);

                        // Save updated products to CSV
                        // save_products_to_csv("Products.csv", products);
                        // return 1;
                        // After updating, break to refresh main menu
                        break;
                    }
                }
                wrefresh(win);
            }

            // cleanup:
            //     unpost_menu(my_menu);
            //     free_menu(my_menu);
            //     for (int i = 0; i < n_choices2; ++i) {
            //         if (my_items[i]) free_item(my_items[i]);
            //     }
            //     free(my_items);
            //     my_menu = NULL;
            //     my_items = NULL;
            //     wrefresh(win);

            // After submenu actions, refresh the main menu
            continue; // Continue the main loop to refresh the product list

        } else if (ch == 'q' || ch == 27) {
            break; // Exit the main loop
        }
    } // End of main while loop

    delwin(win);
    return 0;
}