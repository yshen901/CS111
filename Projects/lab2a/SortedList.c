#include "SortedList.h"
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
  // If the list is non-existant or the head has been overwritten
  if (element == NULL || list == NULL || list->key != NULL)
    return;
  
  SortedListElement_t *currEle = list->next;

  // Until we reach end of list, or the next largest node
  while (currEle != list) {
    if (strcmp(currEle->key, element->key) >= 0)
      break;
    currEle = currEle->next;
  }

  if (opt_yield & INSERT_YIELD)
    sched_yield();
  
  element->prev = currEle->prev;
  element->next = currEle;

  currEle->prev->next = element;
  currEle->prev = element;
  
  return;
}

int SortedList_delete(SortedListElement_t *element) {
  // If element is null, or is the head
  if (element == NULL || element->key == NULL)
    return 1;

  // If there is corruption
  if (element->prev->next != element || element->next->prev != element)
    return 1;

  if (opt_yield & DELETE_YIELD)
    sched_yield();

  // Reassigns prev and next's pointers to element while checking for corruption
  element->prev->next = element->next;
  element->next->prev = element->prev;
  
  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char* key) {
  // If the list is non-existant or the head has been overwritten
  if (list == NULL || key==NULL || list->key != NULL)
    return NULL;

  SortedListElement_t* currEle = list->next;

  if (opt_yield & LOOKUP_YIELD)
    sched_yield();
  while(currEle != list) {
    if (strcmp(currEle->key, key) == 0)
      return currEle;
    currEle = currEle->next;
  }

  return NULL;
}

int SortedList_length(SortedList_t *list) {
  // If the list is non-existant or the head has been overwritten
  if (list == NULL || list->key != NULL)
    return -1;

  int length = 0;
  SortedListElement_t* currEle = list->next;

  if (opt_yield & LOOKUP_YIELD)
    sched_yield();
  while (currEle != list) {
    // Check for corruption before moving on
    if (currEle->prev->next != currEle || currEle->next->prev != currEle)
      return -1;
    length += 1;
    
    currEle = currEle->next;
  }
  return length;
}
