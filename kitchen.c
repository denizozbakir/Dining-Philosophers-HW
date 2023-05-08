/** Deniz Ozbakir, S025399
 * This is the kitchen simulation code for OzuRest.
 * French chef and 3 students from gastronomy department are preparing delicious meals in here
 * You need to solve their problems.
**/
#include "meal.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GLOVE_COUNT 3
#define POT_SIZE 3
#define APPRENTICE_COUNT 3
#define MEALS_TO_PREPARE 4
#define REQUIRED_INGREDIENTS 3

struct meal Menu[4] = {
    {"Menemen", {{"Tomato", 3}, {"Onion", 4}, {"Egg", 1}}, 10},
    {"Chicken Pasta", {{"Pasta", 2}, {"Chicken", 5}, {"Curry", 2}}, 8}, 
    {"Beef Steak", {{"Beef", 7}, {"Pepper", 3}, {"Garlic", 2}}, 13}, 
    {"Ali Nazik", {{"Eggplant", 4}, {"Lamb Meat", 4}, {"Yoghurt", 1}}, 10}
}; 

int potC=0;
int meal_counter = 0;
int meal_ing_counter = 0;

// Define all required mutexes here
pthread_mutex_t mealingM=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gloveM[GLOVE_COUNT]=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t potM=PTHREAD_MUTEX_INITIALIZER;

void put_gloves(int apprentice_id) {
    printf("Apprentice %d is picking gloves \n", apprentice_id);
    // Implement a mutex lock mechanism for gloves here
     if(apprentice_id==2) { // If the last apprentice, lock the 3rd and 1st gloves  
        pthread_mutex_lock(&gloveM[0]); // Lock the 1st glove
        pthread_mutex_lock(&gloveM[2]); // Lock the 3rd glove
    } else if(apprentice_id==0) { // If the 1st apprentice, lock the 1st and next gloves
        pthread_mutex_lock(&gloveM[0]); //lock the glove which corresponds to the apprentice    1st apprentice (0) -> 1st(0) and next glove, 2nd apprentice (1) -> 2nd(1) and next glove, 3rd apprentice (2) -> 3rd(2) and next glove
        pthread_mutex_lock(&gloveM[1]); // lock the next glove
    } else{ // If the 2nd apprentice, lock the 2nd and next gloves
        pthread_mutex_lock(&gloveM[1]); // Lock the 2nd glove
        pthread_mutex_lock(&gloveM[2]); // Lock the 3rd glove
    }
    printf("Apprentice %d has picked gloves\n", apprentice_id);
}

void remove_gloves(int apprentice_id) {
    // Implement a mutex unlock mechanism for gloves here
    if(apprentice_id==2) { // If the last apprentice, unlock the 3rd and 1st gloves  
        pthread_mutex_unlock(&gloveM[0]); // unlock the 1st glove
        pthread_mutex_unlock(&gloveM[2]); // unlock the 3rd glove
    } else if(apprentice_id==0) { // If the 1st apprentice, lock the 1st and next gloves
        pthread_mutex_unlock(&gloveM[0]); // u7nlock the glove which corresponds to the apprentice    1st apprentice (0) -> 1st(0) and next glove, 2nd apprentice (1) -> 2nd(1) and next glove, 3rd apprentice (2) -> 3rd(2) and next glove
        pthread_mutex_unlock(&gloveM[1]); // unlock the next glove
    } else{ // If the 2nd apprentice, unlock the 2nd and next gloves
        pthread_mutex_unlock(&gloveM[1]); // Unlock the 2nd glove
        pthread_mutex_unlock(&gloveM[2]); // Unlock the 3rd glove
    }
    printf("Apprentice %d has removed gloves\n", apprentice_id);
}

void pick_ingredient(int apprentice_id, int* meal_index, int* ing_index) {
    put_gloves(apprentice_id);
    // Implement a control mechanism here using mutexes if needed
    
    *meal_index = meal_counter;
    *ing_index = meal_ing_counter;
    printf("Apprentice %d has taken ingredient %s\n", apprentice_id, Menu[*meal_index].ingredients[*ing_index].name);
    // Possible Race condition here, take your precaution
    if(meal_ing_counter<POT_SIZE-1){ // If the meal has 2 or more ings to be added, increase the meal_ing_counter by one
        pthread_mutex_lock(&mealingM); // Lock the mutex
        meal_ing_counter+=1;  // Increase the meal_ing_counter
        pthread_mutex_unlock(&mealingM); // Unlock the mutex
   } 
   else{ //if(meal_ing_counter==(POT_SIZE-1)){  If the meal has one more ing to be added, add the ing and, increase the meal_counter and reset the meal_ing_counter since you pick the last ing
        pthread_mutex_lock(&mealingM); // Lock the mutex
        meal_counter+=1; //  meal is prepared, increase the meal_counter
        meal_ing_counter=0; // Reset the meal_ing_counter
        pthread_mutex_unlock(&mealingM); // Unlock the mutex
   }
    remove_gloves(apprentice_id);
} 

void prepare_ingredient(int apprentice_id, int meal_index, int ing_index) {
    printf("Apprentice %d is preparing: %s \n", apprentice_id, Menu[meal_index].ingredients[ing_index].name);
    sleep(Menu[meal_index].ingredients[ing_index].time_to_process);
    printf("Apprentice %d is done preparing %s \n", apprentice_id, Menu[meal_index].ingredients[ing_index].name);
}

void put_ingredient(int id, int meal_index, int ing_index) {
    while(1) {
        if(potC<POT_SIZE){
        printf("Apprentince %d is trying to put %s into pot\n", id, Menu[meal_index].ingredients[ing_index].name);
        // Implement a control mechanism here using mutexes to prevent the second problem mentioned in HW file
        pthread_mutex_lock(&potM); // Lock the mutex
        printf("Apprentince %d has put %s into pot\n", id, Menu[meal_index].ingredients[ing_index].name);
        potC=potC+1; // Increase the pot count by one
        pthread_mutex_unlock(&potM); // Unlock the mutex
        // Do not forget to break the loop !
        break;
        }
    }
}

void help_chef(int apprentice_id) {
    int meal_index, meal_ingredient_index;
    pick_ingredient(apprentice_id, &meal_index, &meal_ingredient_index);
    prepare_ingredient(apprentice_id, meal_index, meal_ingredient_index);
    put_ingredient(apprentice_id, meal_index, meal_ingredient_index);
}

void *apprentice(int *apprentice_id) {
    printf("Im apprentice %d\n", *apprentice_id);
    while(1) {
        if(meal_counter == MEALS_TO_PREPARE) // If all meals are prepared chef can go home
            break;
        help_chef(*apprentice_id);
    }
    pthread_exit(NULL);
}

void *chef() {
    while(1) {
        /** Chef works but you need to implement a control mechanism for him 
         *  to prevent the second problem mentioned in HW file.
         *  As for now, he just cooks without checking the pot.
         */
         if(potC==POT_SIZE){
            pthread_mutex_lock(&potM); // Lock thepot mutex
        printf("Chef is preparing meal %s\n", Menu[meal_counter].name);
        sleep(Menu[meal_counter].time_to_prepare);
        printf("Chef prepared the meal %s\n",  Menu[meal_counter].name);
        meal_ing_counter = 0;
        meal_counter = meal_counter + 1;
        sleep(3); // Let the chef rest after preparing the meal
        potC=0; // Reset the pot count
        pthread_mutex_unlock(&potM); // Unlock pot the mutex
        }
        if(meal_counter == MEALS_TO_PREPARE) // If all meals are prepared chef can go home
            break;
            }

    pthread_exit(NULL);
}

int main() {
    pthread_t apprentice_threads[APPRENTICE_COUNT];
    pthread_t chef_thread;

    int apprentice_ids[APPRENTICE_COUNT] = {0 ,1 ,2};
    
    // Initialize Glove mutexes here
    for (size_t i = 0; i < GLOVE_COUNT; i++) 
        pthread_mutex_init(&gloveM[i],NULL); 
    // Initialize threads here
    for (size_t i = 0; i < APPRENTICE_COUNT; i++) 
        pthread_create(&apprentice_threads[i],NULL,(void*)apprentice,&apprentice_ids[i]);
    pthread_create(&chef_thread,NULL,(void*)chef,NULL);
    // Tell the main thread to wait other threads to complete
    for (size_t i = 0; i < APPRENTICE_COUNT; i++)
        pthread_join(apprentice_threads[i],NULL);

    pthread_join(chef_thread, NULL);

    return 0;
}
