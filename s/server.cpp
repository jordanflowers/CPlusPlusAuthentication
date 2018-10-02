//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux 
#include <iostream>
#include <string.h>         //strlen 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         //close 
#include <arpa/inet.h>      //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string>
#include <sys/time.h>       //FD_SET, FD_ISSET, FD_ZERO macros 
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <streambuf>
#include <fstream>
using namespace std;    

#define TRUE   1 
#define FALSE  0 
#define PORT 8083 
    
int main(int argc , char *argv[])  
{  
    int opt = TRUE;  
    int master_socket;
    int addrlen;
    int new_socket;
    int client_socket[30];
    int max_clients = 30;
    int activity;
    int i;
    int valread;
    int sd;
    int numConnected;
    int n;  
    int max_sd;  

    struct sockaddr_in address;  
        
    char buffer[1025];  //data buffer of 1K 
    char msg[1025]; // to grab the message without the client number
    string msgcheck = "";
    
    // Create public/private key pair for RSA
    int ret = 0;
    RSA *rsa = NULL;
    BIGNUM *bne = NULL;
    BIO *bp_public = NULL, *bp_private = NULL;

    int bits = 1024;
    unsigned long e = RSA_F4;

    // Generate RSA Key
    bne = BN_new();
    ret = BN_set_word(bne, e);
    
    rsa = RSA_new();
    ret = RSA_generate_key_ex(rsa, bits, bne, NULL);

    // Save public key
    bp_public = BIO_new_file("serverPublic.pem", "w+");
    ret = PEM_write_bio_RSAPublicKey(bp_public, rsa);

    bp_private = BIO_new_file("serverPrivate.pem", "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL);
    
    // Free:
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    RSA_free(rsa);
    BN_free(bne);


    // Key files generated...

    //set of socket descriptors 
    fd_set readfds;
        
    
    char *message = "";  
    
    //initialize all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
        
    //create a master socket 
    if((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
        
    //bind the socket to localhost
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
    numConnected = 0;
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
    
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
            
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
        numConnected++;
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
            
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
            
            //send new connection greeting message 
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            {  
                perror("send");  
            }  
                
            puts("Welcome message sent successfully");  
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                        
                    break;  
                }  
            }  
        }  
            
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                        
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                    
                // Read the message that came in 
                else
                {  
                    // set the string terminating NULL byte on the end 
                    // of the data read 
                    // valread is the number of values read
                    for(int i = valread; i > 0; i--)
                    {
                        buffer[i] = buffer[i-1];
                    }
                    buffer[0] = to_string(sd)[0];
                    buffer[valread+1] = '\0';
                    // Client (buffer[0]) sent the message (buffer[1...valread])
                    for (int i = 1; i < valread+1; i++)
                    {
                        msg[i-1] = buffer[i];
                    }
                    msgcheck = msg;

                    if (msgcheck == "needPU")
                    {
                        cout << "Grabbing my public key...\n";
                        ifstream t("serverPublic.pem");
                        string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
                        memset(&msg, 0, sizeof(msg));//clear the buffer
                        strcpy(msg, str.c_str());
                        cout << str << endl;

                        // Key Grabbed...Now lets send the message to the client.




                        cout << "Trying to send a message to client " << buffer[0] << endl;
                        send(client_socket[0], msg, strlen(msg), 0);
                        
                    }


                    memset(buffer, 0, sizeof(buffer));
                    
                }  
            }  
        }  
    }  
    close(master_socket);
    for(i = 0; i < 30; i++)
    	close(client_socket[i]);
        
    return 0;  
}  