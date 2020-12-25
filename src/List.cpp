#pragma once

template<typename T>
struct Node {
  Node<T>* next = nullptr;
  T data;
};

template<typename T>
struct List {
  Node<T>* head = nullptr;
  
  void insert(T data) {
    Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
    node->data = data;
    node->next = head;
    head = node;
  }

  Node<T>* get_tail(Node<T>* targetHead) {
    while(targetHead && targetHead->next) {
      targetHead = targetHead->next;
    }
    return targetHead;
  }

  void insert_end(T data) {
    Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
    node->data = data;
    node->next = nullptr;

    Node<T>* tail = get_tail(head);
    tail->next = node;
  }

  void free_list() {
    Node<T>* current = head;
    Node<T>* next = nullptr;
    while(current) {
      next = current->next;
      free(current);
      current = next;
    }
    head = nullptr;
  }

  ~List() {
    free_list();
  }
};
