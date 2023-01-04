// HANDLE ECCEZIONE QUANDO rest = 0 !!!!!!!!!!!



/* SERVER TO RECEIVE AN IMAGE FROM CLIENT */

// https://beej.us/guide/bgnet/pdf/bgnet_a4_c_1.pdf
// https://lenngro.github.io/how-to/2021/01/05/Simple-TCPIP-Server-Cpp/
// https://stackoverflow.com/questions/4223711/a-more-elegant-way-to-use-recv-and-vectorunsigned-char
// OPENCV       https://answers.opencv.org/question/197414/sendreceive-vector-mat-over-socket-cc/
// https://cplusplus.com/forum/windows/67754/
// https://www.youtube.com/watch?v=iVq4aecyQto
// https://isocpp.org/wiki/faq/serialization
// https://www.appsloveworld.com/cplus/100/1341/image-sending-through-socket-c-server-and-c-client
// https://bytes.com/topic/c/answers/960920-how-transfer-images-client-server-using-tcp-ip-sockets
// https://stackoverflow.com/questions/51227324/c-sending-a-vector-across-a-socket-connection
// https://courses.cs.vt.edu/cs2604/fall02/binio.html
// https://www.youtube.com/watch?v=2iZu01UHxfE
// https://www.w3schools.blog/cpp-vector
// https://github.com/sinadalvand/SocketCpp/blob/base/client.cpp

#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

using namespace std;

int out_file_counter = 0;

void ReceivingImgs(int n_imgs, int server, int bufsize){
    cout << "Receiving imgs... " << n_imgs << "/" << server << "/" << bufsize << "/" << endl;
    for(int k=0; k<n_imgs; k++){

        cout << k << "/" << n_imgs << endl;

        int rest;
        int n_bytes_rest = recv(server, &rest, sizeof(rest), 0);
        int rest_conv = ntohl(rest);
        cout << "n_bytes_rest: " << n_bytes_rest << "N of bytes in 'rest': " << rest_conv << endl;

        int n_chunks;
        int n_bytes_chunks = recv(server, &n_chunks, sizeof(n_chunks), 0);
        int n_chunks_conv = ntohl(n_chunks);
        n_chunks = n_chunks_conv;
        cout << "n_bytes_chunks: " << n_bytes_chunks << " N of chunks: " << n_chunks << endl;


        vector<char> v;
        int v_size = n_chunks * bufsize + rest_conv;
        v.resize(v_size);

        

        int c = 0;
        char control;
        int control_bytes;
        while(c < n_chunks){
            do{
            int received_bytes = recv(server, &v[0 + c*bufsize], bufsize, 0);
            control_bytes = received_bytes;
            if(control_bytes == bufsize){
                control = 1;
                send(server, &control, 1, 0);}
            else if(control_bytes == bufsize){
                control = 0;
                send(server, &control, 1, 0);}
            }
            while (control_bytes != bufsize);
            c++;            
            //cout << "[SERVER] Received_bytes: " << control_bytes << endl;
            
        }

        /* Receiving the "rest" bytes */
        do{
        int received_bytes = recv(server, &v[n_chunks*bufsize], rest_conv, 0);
        control_bytes = received_bytes;
        if(control_bytes == rest_conv){
            control = 1;
            send(server, &control, 1, 0);}
        else if(control_bytes == rest_conv){
            control = 0;
            send(server, &control, 1, 0);}
        }
        while (control_bytes != rest_conv);
        //cout << "[SERVER] Received_bytes: " << control_bytes << endl;

        /* Saveing the file */
        string file_name = "./imgs/output" + to_string(out_file_counter) +".jpg";
        fstream file;
        file.open(file_name, ios::out|ios::binary);
        int i = 0;
        
        int max = v.size(); //int max = v.size();
        while(i < max){
            file.put(v.at(i));
            i++;
        }
        cout << "OK \n";
        file.close();
        cout << "Saved img: " << file_name << endl;
        out_file_counter++;

        



    }
}













int main()
{
    /* ---------- INITIALIZING VARIABLES ---------- */

    /*  
       1. client/server are two file descriptors
       These two variables store the values returned 
       by the socket system call and the accept system call.
       2. portNum is for storing port number on which
       the accepts connections
       3. isExit is bool variable which will be used to 
       end the loop
       4. The server reads characters from the socket 
       connection into a dynamic variable (buffer).
       5. A sockaddr_in is a structure containing an internet 
       address. This structure is already defined in netinet/in.h, so
       we don't need to declare it again.
        DEFINITION:
        struct sockaddr_in
        {
          short   sin_family;
          u_short sin_port;
          struct  in_addr sin_addr;
          char    sin_zero[8];
        };
        6. serv_addr will contain the address of the server
        7. socklen_t  is an intr type of width of at least 32 bits
    */
    int client, server;
    int portNum = 8080;
    bool isExit = false;
    int bufsize = 1024; //1024;
    char buffer[bufsize];

    struct sockaddr_in server_addr;
    socklen_t size;

    /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    /* --------------- socket() function ------------------*/

    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0) 
    {
        cout << "\nError establishing socket..." << endl;
        exit(1);
    }

    /*
        The socket() function creates a new socket.
        It takes 3 arguments,
            a. AF_INET: address domain of the socket.
            b. SOCK_STREAM: Type of socket. a stream socket in 
            which characters are read in a continuous stream (TCP)
            c. Third is a protocol argument: should always be 0. The 
            OS will choose the most appropiate protocol.
            This will return a small integer and is used for all 
            references to this socket. If the socket call fails, 
            it returns -1.
    */

    cout << "\n=> Socket server has been created..." << endl;

    /* 
        The variable serv_addr is a structure of sockaddr_in. 
        sin_family contains a code for the address family. 
        It should always be set to AF_INET.
        INADDR_ANY contains the IP address of the host. For 
        server code, this will always be the IP address of 
        the machine on which the server is running.
        htons() converts the port number from host byte order 
        to a port number in network byte order.
    */

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    /* ---------- BINDING THE SOCKET ---------- */
    /* ---------------- bind() ---------------- */


    if ((bind(client, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) 
    {
        cout << "=> Error binding connection, the socket has already been established..." << endl;
        return -1;
    }

    /* 
        The bind() system call binds a socket to an address, 
        in this case the address of the current host and port number 
        on which the server will run. It takes three arguments, 
        the socket file descriptor. The second argument is a pointer 
        to a structure of type sockaddr, this must be cast to
        the correct type.
    */

    size = sizeof(server_addr);
    cout << "=> Looking for clients..." << endl;

    /* ------------- LISTENING CALL ------------- */
    /* ---------------- listen() ---------------- */

    listen(client, 1);

    /* 
        The listen system call allows the process to listen 
        on the socket for connections. 
        The program will be stay idle here if there are no 
        incomming connections.
        The first argument is the socket file descriptor, 
        and the second is the size for the number of clients 
        i.e the number of connections that the server can 
        handle while the process is handling a particular 
        connection. The maximum size permitted by most 
        systems is 5.
    */

    /* ------------- ACCEPTING CLIENTS  ------------- */
    /* ----------------- listen() ------------------- */

    /* 
        The accept() system call causes the process to block 
        until a client connects to the server. Thus, it wakes 
        up the process when a connection from a client has been 
        successfully established. It returns a new file descriptor, 
        and all communication on this connection should be done 
        using the new file descriptor. The second argument is a 
        reference pointer to the address of the client on the other 
        end of the connection, and the third argument is the size 
        of this structure.
    */

    int clientCount = 1;
    server = accept(client, (struct sockaddr *)&server_addr, &size);

    // first check if it is valid or not
    if (server < 0) 
        cout << "=> Error on accepting..." << endl;

    while (server > 0) 
    {
        strcpy(buffer, "=> Server connected...\n");
        send(server, buffer, bufsize, 0);
        cout << "=> Connected with the client #" << clientCount << ", you are good to go..." << endl;
        cout << "\n=> Enter # to end the connection\n" << endl;

        /* 
            Note that we would only get to this point after a 
            client has successfully connected to our server. 
            This reads from the socket. Note that the read() 
            will block until there is something for it to read 
            in the socket, i.e. after the client has executed a 
            the send().
            It will read either the total number of characters 
            in the socket or 1024
        */

        cout << "Client: \n";
        
        /* 
            Once a connection has been established, both ends 
            can both read and write to the connection. Naturally, 
            everything written by the client will be read by the 
            server, and everything written by the server will be 
            read by the client.
        */

        int max = 2000;
        for(int i; i<max; i++){
            cout << "SOCKET LOOP: " << i << endl;

            // Receiving number of imgs to be processed
            int n_imgs;
            int n_bytes = recv(server, &n_imgs, sizeof(n_imgs), 0);
            int n_imgs_conv = ntohl(n_imgs);
            cout << "n_bytes: " << n_bytes << "n_imgs_conv: " << n_imgs_conv << endl;
        
            if(n_imgs_conv>0) {
                ReceivingImgs(n_imgs_conv, server, bufsize);
                }

        }









        int rest;
        int n_bytes_rest = recv(server, &rest, sizeof(rest), 0);
        int rest_conv = ntohl(rest);
        cout << "n_bytes_rest: " << n_bytes_rest << "N of bytes in 'rest': " << rest_conv << endl;

        int n_chunks;
        int n_bytes_chunks = recv(server, &n_chunks, sizeof(n_chunks), 0);
        int n_chunks_conv = ntohl(n_chunks);
        n_chunks = n_chunks_conv;
        cout << "n_bytes_chunks: " << n_bytes_chunks << " N of chunks: " << n_chunks << endl;


        vector<char> v;
        int v_size = n_chunks * bufsize + rest_conv;
        v.resize(v_size);

        

        int c = 0;
        char control;
        int control_bytes;
        while(c < n_chunks){
            do{
            int received_bytes = recv(server, &v[0 + c*bufsize], bufsize, 0);
            control_bytes = received_bytes;
            if(control_bytes == bufsize){
                control = 1;
                send(server, &control, 1, 0);}
            else if(control_bytes == bufsize){
                control = 0;
                send(server, &control, 1, 0);}
            }
            while (control_bytes != bufsize);
            c++;            
            //cout << "[SERVER] Received_bytes: " << control_bytes << endl;
            
        }

        /* Receiving the "rest" bytes */
        do{
        int received_bytes = recv(server, &v[n_chunks*bufsize], rest_conv, 0);
        control_bytes = received_bytes;
        if(control_bytes == rest_conv){
            control = 1;
            send(server, &control, 1, 0);}
        else if(control_bytes == rest_conv){
            control = 0;
            send(server, &control, 1, 0);}
        }
        while (control_bytes != rest_conv);
        //cout << "[SERVER] Received_bytes: " << control_bytes << endl;

        /* Saveing the file */
        fstream file;
        file.open("output.jpeg", ios::out|ios::binary);
        int i = 0;
        
        max = v.size(); //int max = v.size();
        while(i < max){
            file.put(v.at(i));
            i++;
        }
        cout << "OK \n";
        file.close();


        /* ---------------- CLOSE CALL ------------- */
        /* ----------------- close() --------------- */

        /* 
            Once the server presses # to end the connection,
            the loop will break and it will close the server 
            socket connection and the client connection.
        */

        // inet_ntoa converts packet data to IP, which was taken from client
        cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
        close(server);
        cout << "\nGoodbye..." << endl;
        isExit = false;
        exit(1);
    }

    close(client);
    return 0;
}