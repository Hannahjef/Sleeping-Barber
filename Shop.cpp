
#include "Shop.h"

void Shop::init()
{
   for (int i = 0; i < total_barbers; i++)
   {
      Barber *c = new Barber(i);
      barber_list.push_back(c);
   }
   pthread_mutex_init(&mutex_, NULL);
   pthread_cond_init(&cond_customers_waiting_, NULL);
   pthread_cond_init(&cond_barber_sleeping_, NULL);
}
Shop::~Shop()
{
for(int i=0;i<total_barbers;i++)
{
   delete barber_list[i];
}
}
string Shop::int2string(int i)
{
   stringstream out;
   out << i;
   return out.str();
}

void Shop::printBarber(int person, string message)
{
   cout << "Barber  [" << person + 1 << "]: " << message << endl;
}

void Shop::printCustomer(int person, string message)
{
   cout << "Customer[" << person + 1 << "]: " << message << endl;
}

int Shop::get_cust_drops() const
{
   return cust_drops_;
}

int Shop::visitShop(int id)
{
   pthread_mutex_lock(&mutex_);
   // If all chairs are full then leave shop
   if (waiting_chairs_.size() == max_waiting_cust_)
   {
      printCustomer(id, "leaves the shop because of no available waiting chairs.");
      ++cust_drops_;
      pthread_mutex_unlock(&mutex_);
      return -1;
   }
   int barber_assigned = -1; // barber value to be retuned

   // If someone is being served or transitioning waiting to service chair
   // then take a chair and wait untill barber get assigned
   // if(sleepin)
   if (resting_barber_.empty())
   {
      waiting_chairs_.push(id);
      printCustomer(id, "takes a waiting chair. # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size()));
      pthread_cond_wait(&cond_customers_waiting_, &mutex_);
      barber_assigned = customers_getting_served[id]; // get the barber id from the customers_getting_served map
   }
   // If any babers are sleeping wake them up and signal them
   else
   {
      barber_assigned = resting_barber_.front(); // get baber id
      resting_barber_.pop();
      customers_getting_served.insert({id, barber_assigned}); // push to the barber id to customers_getting_served;
     
   }

// Move to servicing chair when baber is assigned
   printCustomer(id, "moves to the service chair ["+int2string(barber_assigned+1)+"], # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size()));
   barber_list[barber_assigned]->current_customer = id; // update Customer's id to Barber
   barber_list[barber_assigned]->customer_in_chair = 1; // signal that cutsomer has moved to chair
   

   // signal customer in case he is sleeping
   pthread_cond_signal(&barber_list[customers_getting_served[id]]->barber_cond);
  barber_list[barber_assigned]->in_Service = 1;

   pthread_mutex_unlock(&mutex_);
   return barber_assigned; // return barber id
}

void Shop::leaveShop(int customerId, int barber_id)
{
   pthread_mutex_lock(&mutex_);

   // Wait for service to be completed
   printCustomer(customerId, "wait for barber["+int2string(barber_id+1)+"] to be done with the hair-cut");
   while (barber_list[barber_id]->in_Service != 0)
   {
      pthread_cond_wait(&barber_list[barber_id]->finished_signal, &mutex_);
   }

   // Pay the barber and signal barber appropriately
   barber_list[barber_id]->money_paid_ = true;
   pthread_cond_signal(&barber_list[barber_id]->barber_cond);


   printCustomer(customerId, "says good-bye to the barber["+int2string(barber_id+1)+"].");
   pthread_mutex_unlock(&mutex_);
}

void Shop::helloCustomer(int id)
{
   pthread_mutex_lock(&mutex_);

   // If no customers in queue than barber can sleep
   if (waiting_chairs_.empty() && barber_list[id]->current_customer == -1)
   {
      printBarber(id, "sleeps because of no customers.");
      resting_barber_.push(id);
      pthread_cond_wait(&barber_list[id]->barber_cond,&mutex_);
   }

  // wait for customer to come to the chair
   while (barber_list[id]->customer_in_chair == -1)
   {
      pthread_cond_wait(&barber_list[id]->barber_cond, &mutex_);
   }
   
   //start service
   barber_list[id]->in_Service = 1;
   printBarber(id, "starts a hair-cut service for customer[" + int2string(barber_list[id]->current_customer+1)+"]");
   pthread_mutex_unlock(&mutex_);
}

void Shop::byeCustomer(int id)
{
   pthread_mutex_lock(&mutex_);
   // Hair Cut-Service is done so signal customer and wait for payment
   barber_list[id]->in_Service = 0;
   printBarber(id, "says he's done with a hair-cut service for [" + int2string(barber_list[id]->current_customer+1)+"]");
   barber_list[id]->money_paid_ = false;

   //signal customer that he has finished
   pthread_cond_signal(&barber_list[id]->finished_signal);

   // wait untill customer signals after paying
   while (barber_list[id]->money_paid_ == false)
   {
      pthread_cond_wait(&barber_list[id]->barber_cond, &mutex_);
   }
   // removes customer from chair after payment done
     barber_list[id]->customer_in_chair = -1;

   // Remove customer from map
   customers_getting_served.erase(barber_list[id]->current_customer);
   barber_list[id]->current_customer = -1;

   // Signal to customer if some one is waiting in queue
   printBarber(id, "calls in another customer");
   if (!waiting_chairs_.empty())
   {
      int customer_id = waiting_chairs_.front();
      barber_list[id]->current_customer = customer_id;
      customers_getting_served.insert({barber_list[id]->current_customer, id});
      barber_list[id]->in_Service = 1;
      waiting_chairs_.pop();
      pthread_cond_signal(&cond_customers_waiting_);
   }

   pthread_mutex_unlock(&mutex_); // unlock
}
