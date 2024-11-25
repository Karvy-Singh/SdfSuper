#ifndef PRODUCT_H
#define PRODUCT_H

#define CSVFILE "Products.csv"

typedef struct {
  int id;
  char *name;
  char *desc;
  int qty;
  double price;
  char *cat;
  double stars;
} Product;

Product **load_products2();
void write_products(Product **prods);

// Function to free loaded products
void free_products2(Product **products);

#endif // PRODUCT_H
