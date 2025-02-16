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
#define ANSI_BLUE "\033[0;34m"
#define ANSI_GREEN "\033[0;32m"
#define ANSI_MAG "\033[0;35m"
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
Card* list_remove_at(CardList *list, size_t pos);

//other list & card functions
void print_card(Card* card);
void list_print(CardList *list, bool dealer);
void clear_input_buffer();

// game functions:
int bet_check(int cash, int pot);
int black_jack_check(CardList *list);
void deal(CardList *to, CardList *from, size_t how_many);
void reset_cards(CardList *player, CardList *dealer, CardList *deck);
int hit_or_stand(CardList *player, CardList *dealer, CardList *deck);
int dealer_draw(CardList *player, CardList *dealer, CardList *deck);

// MAIN:
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

    //ansi clear screen
    printf("\033[2J\033[H");

    // Game On
    int bet;
    while (1){
      
        printf(ANSI_BLUE);
        puts("");
        printf("Player's cash: %d\n", cash);
        printf("Game's pot: %d\n\n", pot);
        printf(ANSI_RST);
        // 2. Betting:
        bet = bet_check(cash, pot); // -2: quit, -1: invalid input else: player's bet
        if (bet == -2) break;
        else if (bet == -1) continue;
        else {
            cash -= bet;
            pot += bet;
        }
        // 3. Initial Deal:
        deal(&player_hand,&deck,2);
        deal(&dealer_hand,&deck,2);
        printf(ANSI_BLUE"Player's hand: "ANSI_RST);
        list_print(&player_hand,false);
        printf(ANSI_BLUE"Dealer's hand: "ANSI_RST);
        list_print(&dealer_hand,true);
        puts("");

        if (black_jack_check(&player_hand) == 21){
            printf(ANSI_GREEN"Black Jack!\n"ANSI_RST);
            cash += pot*2.5;
            pot = 0;
            reset_cards(&player_hand, &dealer_hand, &deck);
        }
        else{
            int result = hit_or_stand(&player_hand, &dealer_hand, &deck);
            // -1:loss ; 0: tie ; 1: player win ; 2: black jack 
            switch (result)
            {
            case -1: //loss
                reset_cards(&player_hand, &dealer_hand, &deck);
                printf(ANSI_MAG"you lost your bet (%d).\n"ANSI_RST, bet);
                pot = 0;
                break;
            case 0: // tie
                printf("You don't win any cash\n"ANSI_RST);
                reset_cards(&player_hand, &dealer_hand, &deck);
                break;
            case 1: // player win
                printf("You had %d!\n", cash);
                printf("You won %d!\n"ANSI_RST, pot*2);
                reset_cards(&player_hand, &dealer_hand, &deck);
                cash += pot*2;
                pot = 0;
                break; 
            case 2: // black jack
                printf(ANSI_GREEN"Black Jack!\n");
                printf("You had %d!\n", cash);
                printf("You won %d!\n"ANSI_RST, bet*2.5);
                reset_cards(&player_hand, &dealer_hand, &deck);
                cash += pot*2.5;
                pot = 0;
                break; 
            // default:
            //     break;
            }
        }
    }
 
    return 0;
}

void my_assert(bool condition, const char *error_message) {
    if (condition) {
        printf(ANSI_RED);
        fprintf(stderr, "%s\n", error_message);
        printf(ANSI_RST);
        exit(EXIT_FAILURE);
    }
}

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

void clear_input_buffer(){
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        // Keep reading characters until we encounter a newline or EOF
    }
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
    if (pot == 0 && cash < 10){
        printf(ANSI_RED"Out of cash to bet. Game Over!\n\n"ANSI_RST);
        return -2;
    }
    int bet;
    printf(ANSI_BLUE"Do you want to bet? [Y/N]?"ANSI_RST);
    //clear_input_buffer();
    char ch;
    scanf(" %c", &ch);
    clear_input_buffer();

    switch (ch)
    {
    case 'N':
    case 'n':
        //ansi clear screen
        printf("\033[2J\033[H");
        printf(ANSI_MAG"You chose to quit.\nYou earned %d cash!\n"ANSI_RST,cash);
        return -2;
    case 'Y':
    case 'y':
        //ansi clear screen
        printf("\033[2J\033[H");
        printf(ANSI_BLUE);
        printf("Player's cash: %d\n", cash);
        printf("Game's pot: %d\n\n", pot);
        break;
    default:
        //ansi clear screen
        printf("\033[2J\033[H");
        printf(ANSI_RED"Invalid input\nplease try again\n"ANSI_RST);
        return -1;
    }

    printf(ANSI_BLUE"please enter your bet (in multiplications of 10): "ANSI_RST);
    scanf(" %d", &bet);

    if (bet == 0 && pot == 0) {
        // bet with 0 cash
        printf(ANSI_RED"Invalid input:\nYou can't bet '0' because there is no money in the pot\nPlease enter a valid bet\n"ANSI_RST);
        return -1;
    }
    if (bet < 0 || bet % 10 != 0) {
        // invalid integer 
        printf(ANSI_RED"Invalid input:\nBet must be a positive multiplication of 10\nPlease enter a valid bet\n"ANSI_RST);
        return -1;
    }
    if (bet > cash) {
        // invalid bet
        //ansi clear screen
        printf("\033[2J\033[H"); 
        printf(ANSI_RED"Invalid input:\nYou don't have enough cash\nPlease enter a valid bet\n"ANSI_RST);
        return -1;
    }
    //ansi clear screen
    printf("\033[2J\033[H");    
    printf(ANSI_BLUE);
    printf("Player's cash: %d\n", cash-bet);
    printf("Game's pot: %d\n\n", pot+bet);
    printf(ANSI_RST);
    return bet;
}

void deal(CardList *to, CardList *from, size_t how_many){
    my_assert(how_many > (from->len), "not enough cards in list to deal from");
    for (size_t i=1; i<=how_many; i++){
        list_push(to, list_remove_at(from,rand() % from->len));
    }
}

// 3. Black Jack Check
int black_jack_check(CardList *list) {
    int sum_result = 0;
    int rank;
    int flag=0;

    for (Card *it = list->head; it; it = it->next) {
        rank = (it->data) >> 4;
        if (rank ==1 ) flag =1;
        if (rank>10) rank = 10;
        sum_result += rank;
    }
    if(flag && sum_result <= 11) sum_result += 10;

    return sum_result;
}

// 4. Hit or Stand:
// returns: -1:loss ; 0: tie ; 1: player win ; 2: black jack 
int hit_or_stand(CardList *player, CardList *dealer, CardList *deck){
    char choice='h';
    int sum;
    printf(ANSI_BLUE"What is your next move? Hit ('h') or Stand ('s')? "ANSI_RST);
    scanf(" %c", &choice);
    clear_input_buffer();
    //ansi clear screen
    printf("\033[2J\033[H");

    if (choice =='s'){ //strcmp returns 0 if the strings are the same
        printf(ANSI_BLUE"Your hand: "ANSI_RST);
        list_print(player, false);
        return dealer_draw(player, dealer, deck);
    }
    else if (choice == 'h'){
        deal(player, deck, 1);
        printf(ANSI_BLUE"Your hand: "ANSI_RST);
        list_print(player, false);
        printf(ANSI_BLUE"Dealer's hand: "ANSI_RST);
        list_print(dealer,true);
        puts("");
        sum = black_jack_check(player);
        if (sum==21) return 2;
        if (sum>21){
            printf(ANSI_MAG"bust!\n"ANSI_RST);
            return -1; // lose bet
        }
        return hit_or_stand(player, dealer, deck);
    }
    else{
        printf(ANSI_RED"Invalid input: type 'h' (Hit) or 's' (Stand)\n"ANSI_RST);
        printf(ANSI_BLUE"Player's hand: "ANSI_RST);
        list_print(player,false);
        printf(ANSI_BLUE"Dealer's hand: "ANSI_RST);
        list_print(dealer,true);
        puts("");
        return hit_or_stand(player, dealer, deck);

    }
}

// 5. Dealer draw:
// returns: -1:loss ; 0: tie ; 1: player win ; 2: black jack 
int dealer_draw(CardList *player, CardList *dealer, CardList *deck){
    int player_sum = black_jack_check(player);
    int dealer_sum=black_jack_check(dealer);

    while (dealer_sum <= player_sum){
        deal(dealer, deck, 1);
        dealer_sum = black_jack_check(dealer);
        if (dealer_sum >= 17) break;
    }
    printf(ANSI_BLUE"dealer's hand: "ANSI_RST);
    list_print(dealer, false);
    puts("");
    if (dealer_sum > 21){
        printf(ANSI_GREEN"dealer bust!\n"ANSI_RST);
        return 1;
    }
    if (dealer_sum < player_sum){
        printf(ANSI_GREEN"you win!\n"ANSI_RST);
        return 1;
    } 
    if (dealer_sum == player_sum){
        printf(ANSI_MAG"tie!\n"ANSI_RST);
        return 0;
    } 
    printf(ANSI_MAG"dealer win!\n"ANSI_RST);
    return -1;
}

// 6. Reset Cards:
void reset_cards(CardList *player, CardList *dealer, CardList *deck){
    deal(deck, player, player->len);
    deal(deck, dealer, dealer->len);
}
