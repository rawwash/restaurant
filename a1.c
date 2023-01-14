#include "a1.h"

void print_menu(Menu* menu){
	fprintf(stdout, "--- Menu ---\n");
	for (int i = 0; i < menu->num_items; i++){
		fprintf(stdout, "(%s) %s: %.2f\n", 
			menu->item_codes[i], 
			menu->item_names[i], 
			menu->item_cost_per_unit[i]	
		);
	}
}


void print_order(Order* order){
	for (int i = 0; i < order->num_items; i++){
		fprintf(
			stdout, 
			"%d x (%s)\n", 
			order->item_quantities[i], 
			order->item_codes[i]
		);
	}
}


void print_receipt(Order* order, Menu* menu){
	for (int i = 0; i < order->num_items; i++){
		double item_cost = get_item_cost(order->item_codes[i], menu);
		fprintf(
			stdout, 
			"%d x (%s)\n @$%.2f ea \t %.2f\n", 
			order->item_quantities[i],
			order->item_codes[i], 
			item_cost,
			item_cost * order->item_quantities[i]
		);
	}

	double order_subtotal = get_order_subtotal(order, menu);
	double order_total = get_order_total(order, menu);
	
	fprintf(stdout, "Subtotal: \t %.2f\n", order_subtotal);
	fprintf(stdout, "               -------\n");
	fprintf(stdout, "Tax %d%%: \t$%.2f\n", TAX_RATE, order_total);
	fprintf(stdout, "              ========\n");
}


Restaurant* initialize_restaurant(char* name){
  
  //malloc_call = 1
  struct Restaurant* acc_restaurant = (struct Restaurant *) malloc(sizeof(Restaurant));

  acc_restaurant->name = (char*)malloc(sizeof(char*)*strlen(name)+1);
  strcpy(acc_restaurant->name, name);
  
  acc_restaurant->menu = load_menu(MENU_FNAME);
  acc_restaurant->num_completed_orders = 0;
  acc_restaurant->num_pending_orders = 0;

  struct Queue* new_queue = (struct Queue*)malloc(sizeof(Queue));
  new_queue->head = NULL;
  new_queue->tail = NULL;
  acc_restaurant->pending_orders = new_queue;
  
  return acc_restaurant;
}


Menu* load_menu(char* fname){
  
  //declaring file and getting ready to read
  FILE *fptr = fopen(fname, "r");

  //declare temp menu
  struct Menu* temp_menu = (struct Menu*) malloc(sizeof(Menu));

  //string reading variables - malloc = 2
  char *line = (char*) malloc(sizeof(char) * (ITEM_CODE_LENGTH + MAX_ITEM_NAME_LENGTH + 100));
  

  //this 50 needs to be determined exactly somehow...it represents the # of items 
  //first declare the arrays of pointers
  temp_menu->item_names = (char**) malloc(sizeof(char*) * 50);
  temp_menu->item_codes = (char**) malloc(sizeof(char*) * 50);
  temp_menu->item_cost_per_unit = (double*) malloc(sizeof(double) * 200);

  //splitting by commas
  char* token = NULL;
  
  //counting # of items
  int item_counter = 0;    

  //read line by line
  while (fgets(line, 1000, fptr) != NULL){

    //codes
    //split from rest
    token = strtok(line, MENU_DELIM);
    
    //load into variables
    temp_menu->item_codes[item_counter] = (char*) malloc(sizeof(char) * ITEM_CODE_LENGTH);
    strcpy(temp_menu->item_codes[item_counter], token);

    //names
    token = strtok(NULL, MENU_DELIM);
    temp_menu->item_names[item_counter] = (char*) malloc(sizeof(char) * MAX_ITEM_NAME_LENGTH);
    strcpy(temp_menu->item_names[item_counter], token);

    //costs
    token = strtok(NULL, MENU_DELIM);
    //token = strtok(NULL, "$");
    //first remove the dollar sign
    token[0] = '0';


    temp_menu->item_cost_per_unit[item_counter] = atof(token);
    item_counter++;
  }

  //storing # of items
  temp_menu->num_items = item_counter;

  //close file      
  fclose(fptr);

  //freeing malloc's
  free(line);

  return temp_menu;
}



Order* build_order(char* items, char* quantities) {
 
  struct Order* new_order = (struct Order*)malloc(sizeof(Order));
 
  int len_of_arr;
 
  //NUM_ITEMS
  if (ITEM_CODE_LENGTH - 1 != 0){
    new_order->num_items = strlen(items) / (ITEM_CODE_LENGTH - 1);
    len_of_arr = new_order->num_items;
  }


  //ITEM_CODES
  char **head = (char **)malloc(sizeof(char*)*len_of_arr);

  for(int i = 0; i < len_of_arr; i++){
    *(head+i) = (char*)malloc(sizeof(char)*(ITEM_CODE_LENGTH));
  }
 
  for(int i = 0; i <len_of_arr; i++) {
    for(int j = 0; j < ITEM_CODE_LENGTH - 1; j++){
        head[i][j] = items[(i*(ITEM_CODE_LENGTH - 1)) + j];
      if(j == ITEM_CODE_LENGTH - 2){
        head[i][j+1] = '\0';
      }
    }
  }

  new_order->item_codes = head;
  
  //ITEM_QUANTITIES
  int* int_quantities = (int*)malloc(sizeof(int)*len_of_arr);
  char* char_quantities = (char*)malloc(sizeof(char)*strlen(quantities)+1);
  char* token = NULL;
  strcpy(char_quantities, quantities);

  token = strtok(char_quantities, MENU_DELIM);
  int counter = 0;

  while(counter < len_of_arr){
    int_quantities[counter] = atoi(token);
    token = strtok(NULL, MENU_DELIM);
    counter++;
  }
 
  new_order->item_quantities = int_quantities;

  //freeing malloc's
  free(char_quantities);
  
  return new_order;
}


void enqueue_order(Order* order , Restaurant* restaurant){
  QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
  new_node->order = order;
  new_node->next = NULL;

  if(restaurant->num_pending_orders == 0){
    restaurant->pending_orders->head = new_node;
    restaurant->pending_orders->tail = new_node;
  }
  
  else {
    restaurant->pending_orders->tail->next = new_node;
    restaurant->pending_orders->tail = new_node;
  }
  
  //add a pending order
  restaurant->num_pending_orders = restaurant->num_pending_orders + 1;
  
}

Order* dequeue_order(Restaurant* restaurant){
  Order* temp = restaurant->pending_orders->head->order;
  
  if(restaurant->pending_orders->head == restaurant->pending_orders->tail){
    free(restaurant->pending_orders->head);
    restaurant->pending_orders->head = NULL;
    restaurant->pending_orders->tail = NULL; 
    }
  else{
    QueueNode* temp2 = restaurant->pending_orders->head;
    restaurant->pending_orders->head = restaurant->pending_orders->head->next;
    free(temp2);
  }
  
  restaurant->num_pending_orders = restaurant->num_pending_orders - 1;
  restaurant->num_completed_orders = restaurant->num_completed_orders + 1;
  
  return temp;
}

double get_item_cost(char* item_code, Menu* menu){

  double cost = 0.0;

  //printf("%s\n", item_code);
  //printf("..");wwwsw3ww

  for (int i = 0; i < menu->num_items; i++){
    if (strcmp(item_code, menu->item_codes[i]) == 0){
      cost = menu->item_cost_per_unit[i];
    }
  }
  
  return cost;
}

double get_order_subtotal(Order* order, Menu* menu){
  double *order_subtotal = (double*) malloc(sizeof(double));
  *order_subtotal = 0;

  for(int i = 0; i < order->num_items ; i++){
    for(int j = 0; j < menu->num_items ; j++){
        if(strcmp(order->item_codes[i], menu->item_codes[j]) == 0 && order->item_codes[i] != NULL && menu->item_codes[j] != NULL){
          *order_subtotal += order->item_quantities[i] * get_item_cost(menu->item_codes[j], menu);         
        } 
    }   
  }

  double temp = *order_subtotal;
  free(order_subtotal);
  
  return temp;
}


double get_order_total(Order* order, Menu* menu){
  return (get_order_subtotal(order, menu) * (TAX_RATE) / 100) + (get_order_subtotal(order, menu));
}

int get_num_completed_orders(Restaurant* restaurant){
  return restaurant->num_completed_orders;
}

int get_num_pending_orders(Restaurant* restaurant){
  return restaurant->num_pending_orders;
}



void clear_order(Order** order){

  for(int i = 0; i < (*order)->num_items; i++){
    free(*(((**order).item_codes)+i));
  }

  free((*order)->item_codes);
  
  free((*order)->item_quantities);
 
  free(*order);

  *order = NULL;

}

void clear_menu(Menu** menu){

  for(int i = 0; i < (*menu)->num_items; i++){
    free((*menu)->item_codes[i]);
    free((*menu)->item_names[i]);
  }

  free((*menu)->item_codes);
  free((*menu)->item_names);
  free((*menu)->item_cost_per_unit);
  free(*menu);

  *menu = NULL;
}


void close_restaurant(Restaurant** restaurant) {
  //freeing name
  free((*restaurant)->name);
  
  //freeing menu
  clear_menu((&((*restaurant)->menu)));
  
  //freeing Queue pending orders
  free((*restaurant)->pending_orders->head);
  free((*restaurant)->pending_orders->tail);
  free((*restaurant)->pending_orders);

  //free restaurant
  free(*restaurant);
  
  *restaurant = NULL;
}