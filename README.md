# Sleeping-Barber
Extended Sleeping barber problem

## Aim of the Problem
The problem uses concepts of synchronization and thread management to simulate the interactions in a barbershop. It aims to ensure that customers are assigned to available barbers, customers leave if there are no available chairs, and barbers sleep if there are no customers. The objective is to create a program that models this scenario and ensures correct interactions between customers and barbers in a multi-threaded environment.

## Overview
The problem objective is to extend the sleeping-barber problem to a multiple sleeping barbers problem. In this scenario, there's a barbershop with a waiting room containing n waiting chairs and a barber room with m barber chairs. Customers visit the barbershop and receive a haircut from one of the available barbers. The problem involves managing the interaction between customers and barbers while ensuring that customers get served promptly.

The program takes command line arguments to specify the number of barbers, the number of waiting chairs, the number of customers, and the service time for each barber. It then simulates the interactions between customers and barbers, with customers arriving at random intervals and getting served by available barbers.

The main challenge is to implement synchronization mechanisms to manage access to shared resources like waiting chairs and barber chairs, ensuring that customers and barbers coordinate correctly. The program uses pthreads for multithreading and requires careful management of threads, mutexes, and condition variables to achieve the desired behavior.

