#ifndef ITEM_H
#define ITEM_H

typedef struct {
    char name[20];
    int currentProducer;
    int countOfStr;
} Item;

Item createItem(int currentProducer, int countOfStr, const char* name);

#endif
