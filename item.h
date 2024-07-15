#ifndef ITEM_H
#define ITEM_H

typedef struct {
    char name[20]; // because news,sports,weather are 6,6,7
    int currentProducer;
    int countOfStr;
} Item;

Item createItem(int currentProducer, int countOfStr, const char* name);

#endif
