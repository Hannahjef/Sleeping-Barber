#ifndef SHOP_H_
#define SHOP_H_
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include<map>
using namespace std;

#define kDefaultNumChairs 3
#define kDefaultBarbers 1

class Shop
{
public:
   Shop(int num_barbers, int num_chairs) : max_waiting_cust_((num_chairs > 0) ? num_chairs : kDefaultNumChairs),
                                                total_barbers((num_barbers > 0) ? num_barbers : kDefaultBarbers),cust_drops_(0)
   {
       
       init();
   };
   Shop() : max_waiting_cust_(kDefaultNumChairs), 
                 cust_drops_(0), total_barbers(kDefaultBarbers)
   {
      init();
   };
   ~Shop();

   int visitShop(int id); // return barber id if barber is assigned else -1;
   void leaveShop(int customer_id,int barber_id); 
   void helloCustomer(int id);
   void byeCustomer(int barber_id);
   int get_cust_drops() const;

private:
   const int max_waiting_cust_; // the max number of threads that can wait
   int total_barbers; // Total number of barbers in a shop
   
   queue<int> waiting_chairs_; // includes the ids of all waiting threads
   queue<int> resting_barber_; // Queue to track the resting barber
   map <int,int> customers_getting_served; // map containing current service of barber to customer
   int cust_drops_; // Customer drops
    //Barber's elements
   struct Barber
   {
      int myid; // Id of barber
       pthread_cond_t barber_cond; //condition of barber
       pthread_cond_t  finished_signal; // finished signal sent to customer
      int current_customer = -1; // holds the id of current customer servicing
      bool money_paid_ = false; // variable for tracking money payment
      int in_Service = -1; // Value 1 when active at service with a cutsomer , -1 - no customer ,0 - finished service for a customer
      int customer_in_chair =-1; // identifier to signal if cutsomer moved to service chair

      //Constructor
      Barber(int id)
      {
         myid = id;
         pthread_cond_init(&barber_cond, NULL);
          pthread_cond_init(&finished_signal, NULL);
      }
      
   };
  
   
   vector <Barber *> barber_list; // Vector containing List of Barbers with respective to their id.
   // Mutexes and condition variables to coordinate threads
   // mutex_ is used in conjuction with all conditional variables
   pthread_mutex_t mutex_;
   pthread_cond_t cond_customers_waiting_;
   pthread_cond_t cond_barber_sleeping_; // common condition variable for barber sleeping queue.

   void init(); // Condition variables and Barber vector initializer
   string int2string(int i); 
   void printBarber(int person, string message); // Print funtcion for Barber
     void printCustomer(int person, string message);//Print function for customer
};
#endif
