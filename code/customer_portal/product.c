#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "product.h"


Product **load_products2() {
  FILE *file = fopen(CSVFILE, "r");
  if (file == NULL) {
    perror("Unable to open file");
    exit(1);
  }

  char line[1024];
  Product **products = (Product **)malloc(sizeof(Product *));
  products[0] = NULL;
  int prod_ptr = 0;

  while (fgets(line, sizeof(line), file)) {
    Product *prod = (Product *)malloc(sizeof(Product));

    line[strcspn(line, "\n")] = 0; // Remove newline character

    char *token = strtok(line, ","); // Split by comma
    prod->id = atoi(token);
    token = strtok(NULL, ",");
    prod->name = strdup(token);
    token = strtok(NULL, ",");
    prod->desc = strdup(token);
    token = strtok(NULL, ",");
    prod->qty = atoi(token);
    token = strtok(NULL, ",");
    prod->price = atof(token);
    token = strtok(NULL, ",");
    prod->cat = strdup(token);
    token = strtok(NULL, ",");
    prod->stars = atof(token);
    while (strtok(NULL, ",") != NULL);

    products = (Product **)realloc(products, sizeof(Product *) * (prod_ptr + 2));
    products[prod_ptr++] = prod;
    products[prod_ptr] = NULL;
  }

  fclose(file);
  return products;
}

void write_products(Product **prods) {
    FILE *file = fopen(CSVFILE, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    do {
        fprintf(file, "%d, %s, %s, %d, %lf, %s, %lf\n", (*prods)->id, (*prods)->name, (*prods)->desc, (*prods)->qty, (*prods)->price, (*prods)->cat, (*prods)->stars);
    }while(*(++prods) != NULL);

    fclose(file);
}



void free_products2(Product **products) {
    for (int i = 0; products[i] != NULL; ++i) {
        free(products[i]->name);
        free(products[i]->desc);
        free(products[i]->cat);
        free(products[i]);
    }
    free(products);
}
