#include <stddef.h> //size_t
#include <stdlib.h> //malloc, exit, srand and rand
#include <stdbool.h> // true & false
#include <stdio.h>
#include <stdarg.h>
#include <string.h> // strcmp
#include <stdint.h> //uint8_t
#include <math.h> // pow
#include <time.h> // time

#define ANSI_RED "\033[0;31m"
#define ANSI_RST "\033[0;30m"

typedef struct Card Card;
struct Card {
    uint8_t data;
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
void list_init(CardList *list);
Card* new_card(uint8_t data);
void list_push(CardList *list, Card *card);
//remove card at a specific position "pos"
Card*  list_remove_at(CardList *list, size_t pos);

//other list & card functions
void print_card(Card* card);
void list_print(CardList *list, bool dealer);
int list_sum(CardList *list);

// game functions:
int bet_check(int cash, int pot);
void deal(CardList *to, CardList *from, size_t how_many);


//MAIN
int main(void) {
    CardList deck, player_hand, dealer_hand;
     
    // Deck creation:
    list_init(&deck);
    for (int i=1; i<14 ; i++){
        list_push(&deck, new_card((i<<4) | 1));
        list_push(&deck, new_card((i<<4) | 2));
        list_push(&deck, new_card((i<<4) | 4));
        list_push(&deck, new_card((i<<4) | 8));
    }

    // 1. Game Initiation:
    int cash, pot;
    cash = 1000;
    pot = 0;
    list_init(&player_hand);
    list_init(&dealer_hand);
    srand(time(NULL));

    // Game On
    int bet;
    while (1){
        puts("");
        printf("Player's cash: %d\n", cash);
        printf("Game's pot: %d\n", pot);
        // 2. Betting:
        bet = bet_check(cash, pot);
        if (bet == -2) break;
        else if (bet == -1) continue;
        else {
            cash -= bet;
            pot += bet;
        }
        // 3. Initial Deal:
        deal(&player_hand,&deck,2);
        deal(&dealer_hand,&deck,2);
        printf("Player's hand: ");
        list_print(&player_hand,false);
        printf("Dealer's hand: ");
        list_print(&dealer_hand,true);

    }
 
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
    uint8_t value, suit;
    value = card->data >> 4;
    suit = card->data & 15; // 00001111 = 15 is the mask to only get the suit 
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

void list_print(CardList *list, bool dealer) {
    Card *it = list->head;
    size_t i=0;
    while (it) {
        if (dealer==true && i==list->len-1){
            printf("??");
            break;
        }
        print_card(it);
        it = it->next;
        if (it == NULL) break;
        printf(" & ");
        i++;
    }
    printf("\n");
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

int bet_check(int cash, int pot){
    if (pot ==0 && cash < 10){
        printf("you have no more cash: GGAME OVER!\n\n");
        return -2;
    }

    int bet;
    char input[7]; // buffer to store user input
    printf("please enter your bet (in multiplications of 10) or quit ('q'):");
    fgets(input, sizeof(input), stdin);  // Read the input as a string
    // Remove newline character that fgets() might include:
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "q") == 0) {
        printf("You chose to quit.\nCongratulation!you earned %d\n",cash);
        return -2;
    }
    bet = atoi(input);
    if (bet == 0 && strcmp(input, "0") != 0) {
        // Check if the input was something like "abc", "q12", or "0abc"
        printf("Invalid input\nPlease enter an integer\n");
        return -1;
    }
    if (strcmp(input, "0") == 0 && pot == 0) {
        printf("Invalid input: you can't bet '0' because there is no money in the pot\nPlease enter a valid bet\n");
        return -1;
    }
    if (bet < 0 || bet % 10 != 0) {
        printf("Invalid input: the bet must be positive and in multiplication of 10\nPlease enter a valid bet\n");
        return -1;
    }
    return bet;
}

void deal(CardList *to, CardList *from, size_t how_many){
    my_assert(how_many > (from->len), "not enough cards in list to deal from");
    for (size_t i=1; i<=how_many; i++){
        list_push(to, list_remove_at(from,rand() % from->len));
    }
}