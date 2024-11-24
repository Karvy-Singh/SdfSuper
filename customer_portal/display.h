#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include "product.h"

// Function to display products in the content window
void display_products(WINDOW *win, Product **products, int selected_product, int *first_visible_product);

// Utility functions for text wrapping
char **wrap_text(const char *text, int width, int *line_count);
int print_wrapped_text(WINDOW *win, int start_y, int start_x, int width, const char *text);

#endif // DISPLAY_H
