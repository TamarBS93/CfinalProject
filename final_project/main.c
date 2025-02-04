#include <stddef.h> //size_t
#include <stdlib.h> //malloc exit
#include <stdbool.h> // true & false
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h> //uint8_t
#include <math.h> // pow

#define ANSI_RED = "\e[0;31m";
#define ANSI_RST = "\e[0;30m";

typedef struct Card Card;
struct Card {
    uint8_t, data;
    Card *next;
};

typedef struct CardList CardList;
struct CardList {
    Card *head;
    size_t len;
};

//general helper functions
void my_assert(bool condition, const char *error_message);

//list & card functions
void list_init(CardList *list);list_sum
Card* new_card(uint8_t data);
void list_push(CardList *list, Card *card);
Card* list_pop(CardList *list);

//remove card at a specific position "pos"
Card*  list_remove_at(CardList *list, size_t pos);

//other list & card functions
void print_card(Card* card);
void list_print(CardList *list);
int list_sum(CardList *list);

//MAIN
int main(void) {
    CardList deck, player_hand, dealer_hand;
     
    // Deck creation:
    list_init(&deck);
    for (int i=1; i<14 ; i++){
        list(&deck, new_card((i<<4) || 1));
        list(&deck, new_card((i<<4) || 2));
        list(&deck, new_card((i<<4) || 4));
        list(&deck, new_card((i<<4) || 8));
    }
    list_print(deck);

    // list_init(&player_hand);
    // list_init(&dealer_hand);

 
    return 0;
}

//general helper functions
void my_assert(bool condition, const char *error_message) {
    if (condition) {
        fprintf(stderr, "%s\n", error_message);
        exit(EXIT_FAILURE);
    }
}

//list & card functions
void list_init(CardList *list) {
    my_assert(list == NULL, "list_init: list cannot be null");
    list->head = NULL;
    list->len = 0;
}

Card* new_card(uint8_t data) {
    Card *card = malloc(sizeof (Card));
    my_assert(card == NULL, "new_card: malloc failed");

    card->data = data;
    card->next = NULL;

    return card;
}

void list_push(CardList *list, Card *card) {
    my_assert(list == NULL, "list_push: list cannot be null");
    my_assert(card == NULL, "list_push: card cannot be null");

    card->next = list->head;
    list->head = card;
    list->len += 1;
}

void print_card(Card* card){
    int value, suit;
    value = (card->data >> 4);
    suit = card->data && 15; // 00001111 = 15 is the mask to only get the suit 
    if (suit == 1 || suit == 4) printf(ANSI_RED);

    switch (value)
    {
    case 1: printf("A"); break;
    case 11: printf("J"); break;
    case 12: printf("Q"); break;
    case 13: printf("K"); break;
    default: printf("%d",value); break;
    }
    switch (suit)
    {
    case 1: printf("\u2665"); break; // Hearts
    case 2: printf("\u2663"); break; // Clubs
    case 4: printf("\u2666"); break; // Diamond
    case 8: printf("\u2660"); break; // Spades
    default: 
        my_assert(1,"print card: error in card suit\n");
        break;
    }

    printf(ANSI_RST);

}

void list_print(CardList *list) {
    // Node *iter = list->head;
    // while (iter != NULL) {
    //     // ...
    //     iter = iter->next;
    // }

    for (Card *it = list->head; it; it = it->next) {
        printf("%d -> ", it->data);
    }
    printf("null\n");
}

//remove card at a specific position "pos"
Card*  list_remove_at(CardList *list, size_t pos) {
    Card *prev = NULL;
    Card *curr = list->head;

    for (size_t i = 0; curr != NULL && i < pos; ++i){
        prev = curr;
        curr = curr->next;
    }

    //if curr is NULL it means this position doesn't 
    //exist. return NULL for failure
    if (curr == NULL) {
        return NULL;
    }

    //if curr points to the card at the head of the list
    //update the head
    if (prev == NULL) {
        list->head = curr->next;
    }
    //else update prev->next
    else {
        prev->next = curr->next;
    }
    
    curr->next = NULL;
    list->len -=1;

    return curr;
}
