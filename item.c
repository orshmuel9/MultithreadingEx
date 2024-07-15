#include "item.h"
#include <string.h>
/*
function to create an article item
*/
Item createItem(int currentProducer, int countOfStr, const char* name) {
    Item item;
    strcpy(item.name, name);
    item.currentProducer = currentProducer;
    item.countOfStr = countOfStr;
    return item;
}
