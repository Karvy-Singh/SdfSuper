#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "cart.h"

// Function to wrap text into lines of specified width
char **wrap_text2(const char *text, int width, int *line_count) {
    char **lines = NULL;
    int count = 0;
    const char *start = text;
    while (*start) {
        int len = 0;
        const char *end = start;
        while (len < width && *end && *end != '\n') {
            len++;
            end++;
        }
        // Backtrack to the previous space if necessary
        if (len == width && *end && *end != '\n' && *end != ' ') {
            const char *space = end;
            while (space > start && *space != ' ')
                space--;
            if (space > start) {
                end = space;
                len = end - start;
            }
        }
        // Allocate and copy the line
        char *line = (char *)malloc(len + 1);
        strncpy(line, start, len);
        line[len] = '\0';
        // Add line to lines array
        lines = (char **)realloc(lines, sizeof(char *) * (count + 1));
        lines[count++] = line;
        // Move to the next line
        start = end;
        while (*start == ' ' || *start == '\n')
            start++;
    }
    *line_count = count;
    return lines;
}

// Function to print wrapped text in a window
int print_wrapped_text(WINDOW *win, int start_y, int start_x, int width, const char *text) {
    int line_count = 0;
    char **lines = wrap_text2(text, width, &line_count);
    for (int i = 0; i < line_count; ++i) {
        if (win != NULL) {
            mvwprintw(win, start_y + i, start_x, "%s", lines[i]);
        }
        free(lines[i]);
    }
    free(lines);
    return line_count;
}

void display_products(WINDOW *win, Product **products, int selected_product, int *first_visible_product) {
    int win_height, win_width;
    getmaxyx(win, win_height, win_width);

    int n_products = 0;
    while (products[n_products] != NULL) n_products++;

    // Calculate lines needed for each product
    int *product_lines = malloc(n_products * sizeof(int));
    for (int i = 0; i < n_products; ++i) {
        int name_lines = print_wrapped_text(NULL, 0, 0, win_width - 2, products[i]->name);
        int desc_lines = print_wrapped_text(NULL, 0, 0, win_width - 2, products[i]->desc);
        product_lines[i] = name_lines + desc_lines + 1; // +1 for spacing
    }

    // Adjust first_visible_product if necessary
    int total_lines = 0;
    for (int i = *first_visible_product; i <= selected_product; ++i) {
        total_lines += product_lines[i];
        if (total_lines > win_height - 2) {
            (*first_visible_product)++;
            i = *first_visible_product - 1;
            total_lines = 0;
        }
    }
    while (*first_visible_product > 0 && selected_product < *first_visible_product) {
        (*first_visible_product)--;
    }

    // Clear window and box
    werase(win);
    box(win, 0, 0);

    // Display products starting from first_visible_product
    int y = 1;
    for (int i = *first_visible_product; i < n_products && y < win_height - 1; ++i) {
        if (i == selected_product) {
            wattron(win, A_REVERSE);
        }
        y += print_wrapped_text(win, y, 1, win_width - 2, products[i]->name);
        y += print_wrapped_text(win, y, 1, win_width - 2, products[i]->desc);
        y += 1; // Add a blank line for spacing
        if (i == selected_product) {
            wattroff(win, A_REVERSE);
        }
        if (y >= win_height - 1) {
            break;
        }
    }

    free(product_lines);
}
