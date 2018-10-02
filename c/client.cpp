#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <thread>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <fstream>

using namespace std;

// Global variable for the client socket descripter
int clientSd;


int main(int argc, char *argv[])
{
    ofstream out;
    // Usage
    if(argc != 3)
    {
        cerr << "Usage: ip_address port" << endl; exit(0); 
    } 

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
    bp_public = BIO_new_file("clientPublic.pem", "w+");
    ret = PEM_write_bio_RSAPublicKey(bp_public, rsa);

    bp_private = BIO_new_file("clientPrivate.pem", "w+");
    ret = PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL);
    
    // Free:
    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    RSA_free(rsa);
    BN_free(bne);


    // Key files generated...

    // Gets the IP and Port
    char *serverIp = argv[1]; 
    int port = atoi(argv[2]);

    // Create a message buffer 
    char msg[1500]; 

    // Setup a socket and connection tools 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    clientSd = socket(AF_INET, SOCK_STREAM, 0);

    // Try to connect
    int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));

    if(status < 0)
    {
        cout << "Error connecting to socket!" << endl;
    }

    cout << "Connected to the server!" << endl;
    
    // For sending messages
    while(1)
    {
        cout << ">> What would you like to do?\n1)Public Key Encryption\n2)Symmetric Key Encryption\n3)Digital Signature\n4)Hash some text\n";
        int in;
        string data;
        char receivedMessage[1500];
        
        EVP_PKEY *server_public;



        cin >> in;
        switch (in)
        {
            case 1:


                // Need to get server's public key.
                data = "needPU";
                memset(&msg, 0, sizeof(msg));//clear the buffer
                strcpy(msg, data.c_str());
                send(clientSd, (char*)&msg, strlen(msg), 0);

                cout << "Resetting receivedMessage.\n";
                memset(receivedMessage, 0, sizeof(receivedMessage));
                
                // Try to receive a message from the server.
                cout << "Trying to receive the server's public key.\n";
                recv(clientSd, (char*)&receivedMessage, sizeof(receivedMessage), 0);

                
                out.open("serverPublic.pem");
                data = receivedMessage;
                cout << "message: \n" << receivedMessage << endl;
                // data contains the server's public key. We will create a file containing this information.
                out << data;
                out.close();
                // We now have the server's public key.


                cout << "Enter the message to encrypt:\n";

                getline(cin, data);
                memset(&msg, 0, sizeof(msg));//clear the buffer
                strcpy(msg, data.c_str());

                // We have the message...Now we encrypt
                FILE *pFile = fopen ("serverPublic.pem" , "r");
                //server_Public = PEM_read_PUBKEY(pFile, NULL, NULL, NULL);
        
                

                if(data == "exit")
                {
                    send(clientSd, (char*)&msg, strlen(msg), 0);
                    break;
                }
                send(clientSd, (char*)&msg, strlen(msg), 0);

                // Clear the buffer
                memset(&msg, 0, sizeof(msg));

                break;
            case 2:
                getline(cin, data);
                memset(&msg, 0, sizeof(msg));//clear the buffer
                strcpy(msg, data.c_str());

                

                if(data == "exit")
                {
                    send(clientSd, (char*)&msg, strlen(msg), 0);
                    break;
                }
                send(clientSd, (char*)&msg, strlen(msg), 0);

                // Clear the buffer
                memset(&msg, 0, sizeof(msg));

                break;
            case 3:
                getline(cin, data);
                memset(&msg, 0, sizeof(msg));//clear the buffer
                strcpy(msg, data.c_str());

                

                if(data == "exit")
                {
                    send(clientSd, (char*)&msg, strlen(msg), 0);
                    break;
                }
                send(clientSd, (char*)&msg, strlen(msg), 0);

                // Clear the buffer
                memset(&msg, 0, sizeof(msg));
                
                break;
            case 4:
                getline(cin, data);
                memset(&msg, 0, sizeof(msg));//clear the buffer
                strcpy(msg, data.c_str());

                

                if(data == "exit")
                {
                    send(clientSd, (char*)&msg, strlen(msg), 0);
                    break;
                }
                send(clientSd, (char*)&msg, strlen(msg), 0);

                // Clear the buffer
                memset(&msg, 0, sizeof(msg));
                
                break;
            default:
                cerr << "Not a correct option.\n";
                break;
        }
        
        
    }

    close(clientSd); // Close Socket
    cout << "Connection closed" << endl;
    return 0;    
}