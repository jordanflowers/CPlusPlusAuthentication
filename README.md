# Encrypted Chat Program

Status: Server is able to send public key over the socket successfully to the Client. The client is able to receive it and store it into a PEM file. 


To do: 
  1. The client needs to do the same thing (send the public key of the client over to the server)
  2. The client needs to hash the password entered by the user and store it in a file with the username entered as well in this format:
        username:passwordhash
  3. The client needs to sign and encrypt the string "username:passwordhash" and send it to the server.
  4. The server needs to decrypt and verify the string with a table that the server stores with usernames and password hashes (That file should be encrypted using AES 256) 
  
