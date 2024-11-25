#include <stdlib.h>
#include <string.h>
#include "cart.h"
#include "display.h"

// Simple linked list to store cart items
typedef struct CartItem {
    Product *product;
    struct CartItem *next;
} CartItem;

static CartItem *cart_head = NULL;

// Function to create a deep copy of a product
Product *copy_product(Product *original) {
    Product *copy = (Product *)malloc(sizeof(Product));
    copy->name = strdup(original->name);
    copy->desc = strdup(original->desc);
    copy->price = original->price;
    copy->id = original->id;
    copy->cat = strdup(original->cat);
    copy->qty = original->qty;
    copy->stars = original->stars;
    return copy;
}

void add_to_cart(Product *product) {
    CartItem *new_item = (CartItem *)malloc(sizeof(CartItem));
    new_item->product = copy_product(product);  // Use the copy_product function
    new_item->next = cart_head;
    cart_head = new_item;
}

void display_cart(WINDOW *win) {
    int y = 1;
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, y++, 2, "My Cart:");
    CartItem *current = cart_head;
    if (current == NULL) {
        mvwprintw(win, y++, 2, "Your cart is empty.");
    } else {
        while (current != NULL) {
            mvwprintw(win, y++, 2, "%s - $%.2f", current->product->name, current->product->price);
            current = current->next;
        }
    }
    wrefresh(win);
}

// Function to free the cart items
void free_cart() {
    CartItem *current = cart_head;
    while (current != NULL) {
        CartItem *next = current->next;
        // Free the product data
        free(current->product->name);
        free(current->product->desc);
        free(current->product->cat);
        free(current->product);
        // Free the cart item
        free(current);
        current = next;
    }
    cart_head = NULL;
}
