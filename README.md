
![](https://raw.githubusercontent.com/taylorkrn/Cplusplus-Server/main/Server_Client_Connection.png)


# Creating a simple server with listen and response

## A local server recieves and prints a message before also sending a confirmation answer

- Create Socket and Bind Server to Socket
- Tell Server to listen for clients and enter infinite while loop waiting for Clients
- Run 'telnet localhost 47000' in the Shell of a linux operating system
- The 'client' connects to the local 'server' and sends the *request*
- The function *handle_connection* is executed, the server simply repeats the request back to the Client
- The connection is closed and the server waits for a new client (A client can also have arrived during the process and a new thread is then created)

