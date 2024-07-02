//=================================================================
// Implementation for DLL module.
//
// Copyright 2024 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================
#include <stdlib.h>
#include <stdio.h>
#include "doubly_linked_list.h"

LLNode* create_llnode(void* data) {

    // Your code here
    LLNode* newNode = (LLNode*) malloc(sizeof(LLNode));
    if (newNode == NULL) return NULL;
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode; // replace this
}

DLinkedList* create_dlinkedlist(void) {
    DLinkedList* newList = (DLinkedList*)malloc(sizeof(DLinkedList));
    if (newList == NULL) return NULL;
    newList->head = NULL;
    newList->tail = NULL;
    newList->size = 0;
    return newList;
}

int getSize(DLinkedList* dLinkedList){
  
  return dLinkedList->size;
}

LLNode* getHead(DLinkedList* dLinkedList){

  // Your code here

  return dLinkedList->head;
}

LLNode* getTail(DLinkedList* dLinkedList){

  // Your code here

  return dLinkedList->tail;
}

LLNode* getNext(LLNode* node){

  // Your code here

  return node->next;
}

LLNode* getPrev(LLNode* node){

  // Your code here

  return node->prev;
}

void* getData(LLNode* node){

  // Your code here

  return node->data;
}

void insertAfter(DLinkedList* dLinkedList, LLNode* prev_node, void* data){
  // if a null node to insert after is passed, return
  if (prev_node == NULL) {
    printf("the given previous node cannot be NULL");
    return;
  }

  // create the new node and set the appropriate pointers
  LLNode* node = create_llnode(data);
  if (node == NULL) {
    return;
  }
  node->next = prev_node->next;
  node->prev = prev_node;
  // if the node you are inserting after is not the tail, set the node after
  // that node's prev pointer to the added node
  // if it is the tail set the new node as the tail
  if(dLinkedList->tail == prev_node) {
    dLinkedList->tail = node;
  } else {
    node->next->prev = node;
  }
  // set the node we are inserting after's next pointer to the inserted node and increment the size
  prev_node->next = node;
  dLinkedList->size++;
  return;
}

void insertBefore(DLinkedList* dLinkedList, LLNode* next_node, void* data){
  // if a null node to insert before is passed, return
  if (next_node == NULL) {
    printf("the given next node cannot be NULL");
    return;
  }

  // create the new node and set the appropriate pointers
  LLNode* node = create_llnode(data);
  if (node == NULL) {
    return;
  }
  node->next = next_node;
  node->prev = next_node->prev;

  // if the node you are inserting after is not the head, set the node before
  // that node's next pointer to the added node
  // if it is the head set the new node as the head
  if (dLinkedList->head == next_node) {
    dLinkedList->head = node;
  } else {
    node->prev->next = node;
  }
  // set the node we are inserting before's prev pointer to the inserted node and increment the size
  next_node->prev = node;
  dLinkedList->size++;
  return;
}

void insertHead(DLinkedList* dLinkedList, void* data){
  // if there are no elements in the list, add a new element, and make it the
  // head and the tail
  if(dLinkedList->head == NULL){
    LLNode* newNode = create_llnode(data);
    if (newNode == NULL) {
    return;
    }
    dLinkedList->head = newNode;
    dLinkedList->tail = newNode;
    dLinkedList->size++; 
  }else{
    // if not, inset before the current head
    insertBefore(dLinkedList, dLinkedList->head, data);
  }
}

void insertTail(DLinkedList* dLinkedList, void* data){

  // if there are no elements in the list, add a new element, and make it the
  // head and the tail
  if (dLinkedList->size == 0) {
    LLNode* newNode = create_llnode(data);
    if (newNode == NULL) {
      return;
    }
    dLinkedList->head = newNode;
    dLinkedList->tail = newNode;
    dLinkedList->size++; 
  } else {
    // if not, insert an element after the current tail
    insertAfter(dLinkedList, dLinkedList->tail, data);
  }
}

void deleteNode(DLinkedList* dLinkedList, LLNode* Node){
  if (dLinkedList == NULL) {
    return;
  }
  // checks if the node is the head node
  if (dLinkedList->head == Node) {
    // if so, sets the new head to the next node
    dLinkedList->head = Node->next;
    // if the next node is null, meaning the removed node is the only node in the list, set the tail to null,
    // since the head node was also the tail
    if (Node->next == NULL) {
      dLinkedList->tail = NULL;
// if not, set the prev pointer of the node after the one we are removing to null
    } else {
      Node->next->prev = NULL;
    }
  // if the node is the tail node in the list
  } else if (dLinkedList->tail == Node) {
    // since the node is not the head node (else we would have entered the first if loop)
    // it cannot be the only node in the list. So we just have to set the previous node as the
    // tail and set that node's next pointer to be null
    dLinkedList->tail = Node->prev;
    Node->prev->next = NULL;
  } else {
    // otherwise if the node is in the middle of the list, set the next node's prev pointer to the removed node's prev pointer,
    // and the prev node's next pointer to the next node from the removed node
    Node->prev->next = Node->next;
    Node->next->prev = Node->prev;
  }
  // free the node and its data and subtract one from the list size
  free(Node->data);
  free(Node);
  dLinkedList->size--;
}

void destroyList(DLinkedList* dLinkedList){
  if (dLinkedList == NULL) {
    printf("Cannot destroy null list\n");
    return;
  }
  // if there are no nodes in the list just free the list
  if (dLinkedList->size == 0) {
    free(dLinkedList);
    return;
  }

  // while there are still nodes in the list, delete the head of the list, then free the list
  while (dLinkedList->size > 0) {
    deleteNode(dLinkedList, dLinkedList->head);
  }
  free(dLinkedList);
}

void reverse(DLinkedList* dLinkedList)
{
  if (dLinkedList == NULL) {
    printf("Cannot reverse null list\n");
    return;
  }
  // if the size is one or zero, the list is already reversed
  if (dLinkedList->size <= 1) {
    return;
  }
  // for each node, swap the next and prev pointers
  LLNode* curr = dLinkedList->head;
  while(curr != NULL) {
    LLNode* temp = curr->next;
    curr->next = curr->prev;
    curr->prev = temp;
    curr = temp;
  }
  // swap the original head and tail
  LLNode* temp = dLinkedList->head;
  dLinkedList->head = dLinkedList->tail;
  dLinkedList->tail = temp;
}     

