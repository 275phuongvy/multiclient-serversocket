# Transmitting arrays of strings between multi Clients and Server by using TCP Socket Programming in C

Author: Vy P Tran

General purpose: Multiple clients try to send multiple arrays of strings (including characters and special symbols) to server so that the server can sort the array of strings by using mergesort algorithm and then send back the sorted arrays to clients. Specially, server has to handle the concurrent requests from multiple clients at the same time by using fork()


1/ To compile the code:

cc –o client1 client1.c 

cc –o client2 client2.c

cc –o client3 client3.c

cc –o server server.c


2/ To run the code: 

For server:  ./server

For one of clients:  

Client 1:  ./client1 IPaddress 

Client 2:  ./client2 IPaddress

Client3:  ./client 3 IPaddress

Note: IPaddress is the address of the server which you want to connect


Please download and read README.docx to get more information on how to compile the program and each line of code works
