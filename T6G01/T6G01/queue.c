#include "queue.h"

tlv_request_t *requests[MAX_DATA] = {NULL};
int front = 0;
int rear = -1;
int itemCount = 0;

tlv_request_t *peek() {
  return requests[front];
}

bool isEmpty() {
  return itemCount == 0;
}

bool isFull() {
  return itemCount == MAX_DATA;
}

int size() {
  return itemCount;
}

void insert(tlv_request_t *data) {

  if (!isFull()) {

    if (rear == MAX_DATA - 1) {
      rear = -1;
    }

    requests[++rear] = data;
    itemCount++;
  }
}

tlv_request_t *removeData() {
  tlv_request_t *data = requests[front++];
  if (front == MAX_DATA) {
    front = 0;
  }

  itemCount--;
  return data;
}