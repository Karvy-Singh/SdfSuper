#ifndef PRODUCT_NEW_H
#define PRODUCT_NEW_H

#include <ncurses.h>

typedef struct {
    int id;
    char *name;
    char *desc;
    int qty;
    double price;
    char *cat;
    double stars;
} Product;

// Function prototypes
Product **load_products(const char *filename);
void free_products(Product **products);
int save_products_to_csv(const char *filename, Product **products);
int display_products_from_file(const char *filename, WINDOW *win);

int delete_product(Product ***products, int id);

#endif // PRODUCT_NEW_H

