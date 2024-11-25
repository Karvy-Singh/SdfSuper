#include <ncurses.h>

#ifndef CART_H
#define CART_H

#include "product.h"

// Function to add a product to the cart
void add_to_cart(Product *product);

// Function to display the cart
void display_cart(WINDOW *win);

// Function to free the cart items
void free_cart();

#endif // CART_H
