#ifndef DISPATCH_H
#define DISPATCH_H

// this function is using by the dispatch thread and it's porpuse is to move the articles from the dispatch to the is correct co editor queue
void *dispatch(void *arg);

#endif
