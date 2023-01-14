#include "lab3.h"
 
//Add TA party item request to the list
int add_request(struct party_node **head, char *item, double price, char *ta){
  
   if (strcmp(item, "IDE") == 0){
     return -1;
   } 
  
     //allocate memory for dynamic types
     struct party_node *request = (struct party_node*) malloc (sizeof(struct party_node));
 
     request->item = (char *) malloc (sizeof(char)*(strlen(item) + 1));
     request->ta = (char *) malloc (sizeof(char)*(strlen(ta) + 1));
 
     strcpy(request->ta, ta);
     strcpy(request->item, item);
     request->price = price;
 
     //keep track of node before switching
     request->next = *head;
 
     //point head to new request node (LIFO)
     *head = request;
 
     return 0;
 
  
}
 
//Remove the last item added
void remove_request(struct party_node **head){
  
   //keep track of head node in dummy ptr
   if ((*head) == NULL){
     return;
   }
  
   struct party_node *dummy = *head;
 
   //point head to next node
   *head = dummy->next;
   
   free(dummy->ta);
   free(dummy->item);
   free(dummy);
 
}
 
//Sort party item requests - in place?
void make_sorted(struct party_node **head){
  struct party_node *unsorted = *head;
 struct party_node *sorted = NULL;
 
 int length = 1;
  //find length of linked list
 while (unsorted->next != NULL) {
   length++;
   unsorted = unsorted -> next;
 }
  //declaring min and other elements
 double min = unsorted->price;
 char* temp_item = unsorted->item;
 char* temp_ta = unsorted->ta;
 // free(unsorted->item)
 
 unsorted = *head;
 
 for(int i = 0; i < length; i++) {
 
   unsorted = *head;
 
   //printf("%f\n\n\n", unsorted->price);
   //finding min for this iteration
   //here there was a next
   while (unsorted != NULL){
     if (unsorted->price < min) {
       min = unsorted->price;
       temp_item = unsorted->item;
       temp_ta = unsorted->ta;
     }
     unsorted = unsorted->next;
   }
 
   //printf("%f\n\n\n", min);
  
   add_request(&sorted, temp_item, min, temp_ta);
  
   unsorted = *head;
 
   while(unsorted->next != NULL){
     if(unsorted == *head && (*head)->price == min){
         *head = unsorted->next;
       }
    
     else if(unsorted->next->price == min){
         unsorted->next = unsorted->next->next;
     }
 
     else {
       unsorted = unsorted->next;
     }
      
   }
    
   //print_list(*head);
   //print_list(sorted);
 
   min = 9999999;
  }
 //SORTING works, now we
 
//need to pass it to head
 //keep track of node before switching
 unsorted->next = *head;
 
 //point head to sorted
 *head = sorted;

  free(unsorted->item);
  free(unsorted->ta);
  free(unsorted);
}


//Trim list to fit the budget
double finalize_list(struct party_node **head, double budget){
  //Add code here
  struct party_node* curr = *head;
  struct party_node* prev = *head;
  struct party_node* temp = NULL;

   while(curr!=NULL && budget >0){
      
      if(curr==(*head) && ((curr->price) > budget)) {
      *head = (*head)->next;
      remove_request(&prev);
      curr = *head;
      prev = *head;
      }
      
      curr = curr->next;
      
     
     }
    
   curr = *head; 

   while(curr!=NULL && budget>0){
     
     
     if (curr->price <= budget){
       budget = budget-(curr->price);
       prev = curr;
       temp = NULL;
     }
     
    else{
       temp = curr;
       prev->next = curr->next;
    }
    
     curr = curr->next;
     
     if (temp!= NULL && budget > 0) {
      if (temp->price > budget){
        remove_request(&temp);
      }
     }
    temp = NULL;
     
   }
      
   
 
   return budget;

   
}

//Print the current list - hope this is helpful!
void print_list(struct party_node *head){
    int count = 1;
    printf("The current list contains:\n");
    while(head!=NULL){
        printf("Item %d: %s, %.2lf, requested by %s\n",
            count, head->item, head->price, head->ta);
        count++;
        head = head->next;
    }
    printf("\n\n");
    return;
}
