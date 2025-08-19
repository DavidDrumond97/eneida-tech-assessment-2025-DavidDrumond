#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define CB_SIZE 128

typedef struct {
    uint8_t data[CB_SIZE];
    unsigned head;
    unsigned tail;
    unsigned count;
} circularBuffer_t;

void circularBuffer_init(circularBuffer_t *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

bool circularBuffer_is_empty(const circularBuffer_t *cb) {
    return cb->count == 0;
}

bool circularBuffer_is_full(const circularBuffer_t *cb) {
    return cb->count == CB_SIZE;
}

bool circularBuffer_push(circularBuffer_t *cb, uint8_t v) {
    if (circularBuffer_is_full(cb)) return false;
    cb->data[cb->head] = v;
    cb->head = (cb->head + 1) % CB_SIZE; // wrap
    cb->count++;
    return true;
}

bool circularBuffer_pop(circularBuffer_t *cb, uint8_t *out) {
    if (circularBuffer_is_empty(cb)) return false;
    *out = cb->data[cb->tail];
    cb->tail = (cb->tail + 1) % CB_SIZE;
    cb->count--;
    return true;
}

int main(void) {
    circularBuffer_t buf;
    circularBuffer_init(&buf);

    circularBuffer_push(&buf, 'A');
    circularBuffer_push(&buf, 'B');
    circularBuffer_push(&buf, 'C');

    uint8_t v;
    while (!circularBuffer_is_empty(&buf)) {
        circularBuffer_pop(&buf, &v);
        printf("Popped: %c\n", v);
    }
    return 0;
}
