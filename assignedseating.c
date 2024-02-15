//Programming Assignment 2
//Simi Patel
//NID: si379286

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//I will be using 1-based indexing
#define INITSIZE 10
#define MAXLEN 51
#define MAXROWS 100001

typedef struct order {
    int s_seat;
    int e_seat;
    char *name;
} order;

typedef struct theaterrow {
    order** list_orders;
    int max_size;
    int cur_size;
} theaterrow;

order* make_order(int start, int end, char* this_name){
    order*new=malloc(sizeof(order));
    new->s_seat=start;
    new->e_seat=end;
    new->name=malloc(sizeof(char)*strlen(this_name)+1);
    strcpy(new->name,this_name);
    return new;
}

theaterrow* make_empty_row(){
    theaterrow*empty = malloc(sizeof(theaterrow));
    empty->list_orders = malloc(INITSIZE * sizeof(order*));
    empty->max_size = INITSIZE;
    empty->cur_size = 0;
    return empty;
}

int conflict(order* order1, order* order2){
    //checks if order one is sandwiched in between the other
    //or checks to see if the end of the other order is sandwiched between the other 
    if (((order1->s_seat <= order2->s_seat)&&(order2->s_seat <= order1->e_seat))||((order1->s_seat<=order2->e_seat)&&(order2->e_seat <= order1->e_seat))){
        return 1;//conflict was detected
    }
    return 0;//no conflict
}

int can_add_order(theaterrow* this_row, order* this_order){
    //going through an entire row and check for conflicts
    for (int i=0;i<this_row->cur_size;i++){
        if (conflict(this_row->list_orders[i],this_order)==1){
            return 0;//can't add to this row
        }
    }
    //if the return isn't done through the loop, it allows for a return after the loop(no conflicts were found)
    return 1; //can add to row(no conflicts were found)
}

void add_order(theaterrow* this_row, order* this_order){
    if (can_add_order(this_row,this_order)==1){
        //allocating the memory
        if (this_row->cur_size==this_row->max_size){
            this_row->max_size *=2;
            this_row->list_orders=realloc(this_row->list_orders,this_row->max_size*sizeof(order*));
        }
        this_row->list_orders[this_row->cur_size++]=this_order;
    }
}

int contains(order* myorder, int seat_no){
    //checks to see if the order has the seat number 
    //sandwiched method 
    if ((myorder->s_seat <= seat_no)&&(seat_no<= myorder->e_seat)){
        return 1; //seat is sandwiched
    }
    return 0; //seat in not in between an order
}

char* get_owner(theaterrow** theater, int row, int seat_num){
    //if the row exists, or if the row has nothing in it
    if (theater[row]==NULL){
        return NULL;
    }
    //goes through each order in the row to find one that might contain the seat_num
    for (int i = 0; i < theater[row]->cur_size; i++) {
        if (contains(theater[row]->list_orders[i],seat_num)){
            return theater[row]->list_orders[i]->name;
        }
    }
    return NULL;//no owner was found
}
char* get_row_owner(theaterrow* this_row, int seat_num) {
    //if the row exists, or if the row has nothing in it
    if (this_row == NULL){ 
        return NULL;
    }
    //goes through each order in the row to find one that might contain the seat_num 
    for (int i = 0; i < this_row->cur_size; i++) {
        if (contains(this_row->list_orders[i],seat_num)){
            return this_row->list_orders[i]->name;
        }
    }
    return NULL;//no owner was found
}

void free_order(order* this_order){
    free(this_order->name);
    free(this_order);
}

void free_row(theaterrow* this_row){
    for (int i = 0; i < this_row->cur_size; i++) {
        free_order(this_row->list_orders[i]);
    }
    free(this_row->list_orders);
    free(this_row);
}

int main(){
    theaterrow** amc = calloc(MAXROWS +1, sizeof(theaterrow*));
    //defensive coding
    if (amc == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 0;
    }

    char action[10];
    while (scanf("%s", action) != EOF) {
        //when the input is BUY
        if (strcmp(action, "BUY") == 0) {
            //initalize variables for inputs
            int row, start, end;
            char name[MAXLEN];
            scanf("%d%d%d%s", &row, &start, &end, name);
            //make memory for the order 
            order* new_order = make_order(start, end, name);
            if (amc[row] == NULL)
                amc[row] = make_empty_row();
            //checks for conflicts
            if (can_add_order(amc[row], new_order)==1) {
                //if no conflicts, add and print a success
                add_order(amc[row], new_order);
                printf("SUCCESS\n");
            } else {
                //print failure and ignore the order made(done by freeing)
                printf("FAILURE\n");
                free_order(new_order);
            }
        //input is a lookup
        } else if (strcmp(action, "LOOKUP") == 0) {
            //creating variables for input
            int row, seat;
            scanf("%d%d", &row, &seat);
            //creating memory for storing the new owner
            char* owner = get_owner(amc, row, seat);
            if (owner == NULL)
                printf("No one\n");
            else
                printf("%s\n", owner);
        } else if (strcmp(action, "QUIT") == 0) {
            break;
        }
    }
    
    //freeing the theater
    //iterating through all the rows and freeing the rows
    for (int i = 1; i <= MAXROWS; i++) {
        //prevent double freeing
        if (amc[i] != NULL)
            free_row(amc[i]);
    }
    //finally free the theater itself
    free(amc);
    return 0;
}