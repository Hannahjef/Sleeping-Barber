#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "Shop.h"
using namespace std;

void *barber(void *);
void *customer(void *);

// ThreadParam class
// This class is used as a way to pass more
// than one argument to a thread.
class ThreadParam
{
public:
    ThreadParam(Shop *shop, int id, int service_time) : shop(shop), id(id), service_time(service_time){};
    Shop *shop;
    int id;
    int service_time;
};

int main(int argc, char *argv[])
{

    // Read arguments from command line
    // TODO: Validate values
    if (argc != 5)
    {
        cout << "Usage: num_chairs num_customers service_time" << endl;
        return -1;
    }
    int num_barbers = atoi(argv[1]);
    int num_chairs = atoi(argv[2]);
    int num_customers = atoi(argv[3]);
    int service_time = atoi(argv[4]);

    // Multiple barber, one shop, many customers
    if(num_barbers<1)
    {
        num_barbers = kDefaultBarbers;
    }
    pthread_t barber_threads[num_barbers];
    pthread_t customer_threads[num_customers];
    Shop shop(num_barbers, num_chairs);

    for (int i = 0; i < num_barbers; i++)
    {
        int id = i;
        ThreadParam *barber_param = new ThreadParam(&shop, id, service_time);
        pthread_create(&barber_threads[i], NULL, barber, barber_param);
    }
    for (int i = 0; i < num_customers; i++)
    {
        usleep(rand() % 1000);
        int id = i;
        ThreadParam *customer_param = new ThreadParam(&shop, id, 0);
        pthread_create(&customer_threads[i], NULL, customer, customer_param);
    }

    // Wait for customers to finish and cancel barber
    for (int i = 0; i < num_customers; i++)
    {
        pthread_join(customer_threads[i], NULL);
    }

    for (int i = 0; i < num_barbers; i++)
    {
        pthread_cancel(barber_threads[i]);
    }

    cout << "# customers who didn't receive a service = " << shop.get_cust_drops() << endl;
    return 0;
}

void *barber(void *arg)
{
    ThreadParam *barber_param = (ThreadParam *)arg;
    Shop &shop = *barber_param->shop;
    int service_time = barber_param->service_time;
    int id = barber_param->id;
    delete barber_param;
    

    while (true)
    {
        shop.helloCustomer(id);
        usleep(service_time);
        shop.byeCustomer(id);
    }
    return nullptr;
}

void *customer(void *arg)
{
    ThreadParam *customer_param = (ThreadParam *)arg;
    Shop &shop = *customer_param->shop;
    int id = customer_param->id;
    delete customer_param;

    int baber_id = shop.visitShop(id);
    if (baber_id != -1)
    {
        shop.leaveShop(id,baber_id);
    }
    return nullptr;
}
