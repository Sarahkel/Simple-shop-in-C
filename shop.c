// GMIT Data Analytics 
// Adapted from: Multi-paradigm programming - Dr. Dominic Carr


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// set up structs

struct Product {
    char* name;
    double price;
};

struct ProductStock {
    struct Product product;
    int quantity;
};

struct Customer {
    char* name;
    double budget;
    struct ProductStock shoppingList[10];
    int index;
};

struct Shop {
    double cash;
    struct ProductStock stock[20];
    int index;
};

// methods for printing various things

void printProduct(struct Product p)
{
    printf("PRODUCT NAME: %s \nPRODUCT PRICE: %.2f\n", p.name, p.price);   
    printf("----------------\n");
};

void printCustomer(struct Customer c)
{
    printf("CUSTOMER NAME: %s \nCUSTOMER BUDGET: %.2f\n", c.name, c.budget);   
    printf("----------------\n");

    for(int i = 0; i < c.index; i++)
    {
        //printProduct(c.shoppingList[i].product);
        printf("%s ORDERS %d of %s\n", c.name, c.shoppingList[i].quantity, c.shoppingList[i].product.name);
        double cost = c.shoppingList[i].quantity * c.shoppingList[i].product.price;
        // printf("The cost to %s will be E%.2f\n", c.name, cost);
        // printf("----------------\n");
    }
};

void printShop(struct Shop s)
{
    printf("\n\nShop has %.2f in cash\n\n", s.cash);
    for (int i = 0; i < s.index; i++)
    {
        printProduct(s.stock[i].product);
        printf("THE SHOP HAS %d of the above\n\n", s.stock[i].quantity);
    }
}

// helper methods

double findProductPrice(struct Shop s, char* n)
{
    for (int i = 0; i < s.index; i++)
    {

        struct Product product = s.stock[i].product;
        char* name = product.name;
        //contains
        if (strstr(n,name) != NULL ){
            return product.price;
        }
    }

    return -1;
}


// method to create and stock Shop

struct Shop createAndStockShop()
{
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("stock.csv", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    read = getline(&line, &len, fp);
    double cashInShop = atof(line);
    struct Shop shop = { cashInShop };

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        // printf("%s IS A LINE\n", line); 
        char* n = strtok(line, ",");
        char* p = strtok(NULL, ",");
        char* q = strtok(NULL, ",");
        int quantity = atoi(q);
        double price = atof(p);
        char* name = malloc(sizeof(char) * 50);
        strcpy( name, n );

        struct Product product = { name, price };
        struct ProductStock stockItem = { product, quantity };
        shop.stock[shop.index++] = stockItem;
        // printf("NAME OF PRODUCT %s PRICE %.2f QUANTITY %d\n", name, price, quantity);
    }

    return shop;
}


// method for user to choose how they want to order

int requestMode()
{
    // print menu and ask for choice
    printf("Please choose 1 or 2\n");
    printf("1. I have submitted my order in advance. Please process my .csv\n");
    printf("2. I would like to order now\n");
    char mode[2];
    fgets(mode, sizeof(mode), stdin);
    // https://stackoverflow.com/questions/53900369/fgets-keeps-getting-skipped-before-entering-loop
    // remove newline characters
    mode[strcspn(mode, "\n")] = 0;

    
    if (strcmp(mode,"1") == 0) {
        printf("You entered 1");
        int modeI = atoi(mode);
        return modeI;
    }
    else if (strcmp(mode,"2") == 0) {
        printf("You entered 2\n");
        int modeI = atoi(mode);
        return modeI;
    }
    else {
        printf("Sorry, we don't do that here. Please come again some other time \n\n");
        int modeI = atoi(mode);
        return modeI;        
    }
}

// method to fulfill the order

void fulfillOrder(struct Customer* c, struct Shop* s)
{

    for (int i = 0; i < c->index; i++)

    {
        // check the ShopStock by looping through shopstock and matching it with current item on shoppinglist
        for (int n = 0; n < s->index; n++)
        {
            // once you find the product, process order
            if (strcmp(s->stock[n].product.name, c->shoppingList[i].product.name) == 0)
            {
                //verify shop has enough stock
                if (s->stock[n].quantity >= c->shoppingList[i].quantity) {
                    //calculate the cost
                    double cost = c->shoppingList[i].quantity * c->shoppingList[i].product.price;
                    // printf("Cost of item on shoppinglist is %.2f \n", cost);

                    // verify customer has enough budget
                    if (c->budget > cost) {
                        //printf("Customers budget is %.2f and customer can afford cost %.2f\n", c->budget, cost);
                        c->budget -= cost;
                        //printf("Customers budget has reduced to %.2f \n", c->budget);
                        
                        //increase the shop cash
                        s->cash += cost;
                        //printf("THE SHOP'S CASH has increased to %.2f \n", s->cash);

                        //diminish the productstock accordingly
                        s->stock[n].quantity -= c->shoppingList[i].quantity;
                    }
                    else {
                    printf("Sorry, you do not have enough money to purchase this amount of %s", c->shoppingList[i].product.name);

                    }

              
                }
                else {
                    printf("The shop DOES NOT HAVE enough of %s\n. Please come back another time for this item", c->shoppingList[i].product.name);
                }
            }
        }

    }

    // include appropriate error messages

}

// Reads from CSV

struct Customer readShoppingList(struct Shop s)
{
    // read from csv

    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("customer.csv", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    //read name

    read = getline(&line, &len, fp);
    char* nameC = strtok(line, ",");
    char* nameCustomer = malloc(sizeof(char) * 50);
    strcpy( nameCustomer, nameC );

    //read budget

    char* bC = strtok(NULL, ",");
    double budgetCustomer = atof(bC);

    struct Customer customer = { nameCustomer, budgetCustomer };

    //read their shoppinglist 
    while ((read = getline(&line, &len, fp)) != -1) {
        char* n = strtok(line, ",");
        char* q = strtok(NULL, ",");
        int quantity = atoi(q);
        char* name = malloc(sizeof(char) * 50);
        strcpy( name, n );

        double price = findProductPrice(s, name);
        if (price == -1) {
            printf("Sorry, we do not carry %s", name);
        }
        else {
        struct Product Item = { name, price };
        struct ProductStock listItem = { Item, quantity };
        customer.shoppingList[customer.index++] = listItem;
        // printf("Added Product %s Quantity %d price %.2f", name, quantity, price );
        }
    }

    return customer;
}

// reads customer from user input

struct Customer recordOrder(struct Shop s)
{

    //get name
    printf("What is your name?\n");
    char nameC[20];
    //https://www.gamedev.net/forums/topic/589620-why-is-my-program-skipping-fgets/
    fgetc(stdin);
    fgets(nameC, sizeof(nameC), stdin);
    nameC[strcspn(nameC, "\n")] = 0;
    char* nameCustomer = malloc(sizeof(char) * 50);
    strcpy( nameCustomer, nameC );

    //get budget
    printf("What is your budget?\n");
    char bC[20];
    fgets(bC, sizeof(bC), stdin);
    // https://stackoverflow.com/questions/53900369/fgets-keeps-getting-skipped-before-entering-loop
    //bC[strcspn(bC, "\n")] = 0;
    double budgetCustomer = atof(bC);

    struct Customer customer = { nameCustomer, budgetCustomer };
    printf("The name of the customer is %s and their budget is %.2f\n", nameCustomer, budgetCustomer);


    int i=0;

    // while (i < 1) couldn't get this to work as it would always jump over one question on second loop
    // {
        //get order
        printf("What would you like?\n");
        char productName[20];
        fgets(productName, sizeof(productName), stdin);
        productName[strcspn(productName, "\n")] = 0;
        char* nameP = malloc(sizeof(char) * 50);
        strcpy( nameP, productName );

        printf("How many?\n");
        char pN[20];
        fgets(pN, sizeof(pN), stdin);
        pN[strcspn(pN, "\n")] = 0;
        int productNumber = atoi(pN);

        double price = findProductPrice(s, nameP);
        if (price == -1) {
            printf("Sorry, we do not carry %s\n", nameP);
        }
        else {
        struct Product Item = { nameP, price };
        struct ProductStock listItem = { Item, productNumber };
        customer.shoppingList[customer.index++] = listItem;
        printf("Added Product %s Quantity %d price %.2f\n", nameP, productNumber, price );
        } 

        // i++;
        // }

    return customer;
}

int main(void)
{
    // initializes shop
    struct Shop shop = createAndStockShop();

    // printShop(shop);

    // asks user in which mode they want to operate
    int mode = requestMode();
    
    //Read from CSV
    if (mode == 1){
        struct Customer customer = readShoppingList(shop);
        //create pointers
        struct Customer* CustomerCSV = &customer;
        struct Shop* ShopP = &shop;
        printf("---------\n before order\n----------\n");
        printShop(shop);
        printCustomer(customer);
        fulfillOrder(CustomerCSV,ShopP);
        printf("---------\n ****after order****\n----------\n");
        printShop(shop);
        printCustomer(customer);
    }
    //Live Mode
    else if (mode == 2){
        struct Customer customerL = recordOrder(shop);
        printCustomer(customerL);
        //create pointers
        struct Customer* CustomerLive = &customerL;
        struct Shop* ShopP = &shop;
        printf("---------\n before order\n----------\n");
        printShop(shop);
        printCustomer(customerL);
        fulfillOrder(CustomerLive, ShopP);
        printf("---------\n ****after order****\n----------\n");
        printShop(shop);
        printCustomer(customerL);
    }
    else
    {
        
    }
    
    // printCustomer(customer);
    return 0;
}