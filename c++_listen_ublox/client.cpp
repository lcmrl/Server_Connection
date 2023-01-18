#include <iostream>
#include <fstream> 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <vector>
#include <sys/stat.h>
#include <filesystem>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <ctime>

using namespace std;

void SendImages(vector<string> IMGS_TO_BE_PROCESSED, int client, int bufsize){
    for (int i=0; i<IMGS_TO_BE_PROCESSED.size(); i++){
        //cout << "IMGS_TO_BE_PROCESSED[i]: " << IMGS_TO_BE_PROCESSED[i] << endl;
        cout << "PROCESSED: " << i << "/" << IMGS_TO_BE_PROCESSED.size() << "   " << IMGS_TO_BE_PROCESSED[i] << endl;
        ifstream my_image(IMGS_TO_BE_PROCESSED[i], ios::in|ios::binary);

        char ch;
        std::vector <char> vec;
        vec.clear();
        while(!my_image.eof()) {
            ch = my_image.get();
            vec.emplace_back(ch);
            }
        //cout << "Processed img: " << i << endl;

        // Send to SERVER the "rest" part of bytes
        int rest = vec.size() % bufsize;
        int rest_conv = htonl(rest);
        send(client, &rest_conv, sizeof(rest_conv), 0);

        // Send to SERVER the number of chunks
        int num_chunks =  vec.size() / bufsize;
        int num_chunks_conv = htonl(num_chunks);
        send(client, &num_chunks_conv, sizeof(num_chunks_conv), 0);

        // Send the IMAGE
        int c = 0;
        char control = 0;
        while(c < num_chunks){
            do {
               send(client, &vec[0 + c*bufsize], bufsize, 0);
               recv(client, &control, 1, 0);
               }
            while(control == 0);
            c++;
            }

        do{
            send(client, &vec[num_chunks*bufsize], rest, 0);
            recv(client, &control, 1, 0);
            }
        while(control == 0);

        }
    }









int main()
{
    /* ---------- INITIALIZING VARIABLES ---------- */

    /*  
       1. client is a file descriptor to store the values 
       returned by the socket system call and the accept 
       system call.
       2. portNum is for storing port number on which
       the accepts connections
       3. isExit is bool variable which will be used to 
       end the loop
       4. The client reads characters from the socket 
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
    */

    int client;
    int portNum = 61111; // NOTE that the port number is same for both client and server
    bool isExit = false;
    int bufsize = 1024; //1024
    char buffer[bufsize];
    char ip[] = "127.0.0.1";

    struct sockaddr_in server_addr;

    client = socket(AF_INET, SOCK_STREAM, 0);


    /* LISTING FILE IN THE IMG DIRECTORY
    string IMG_FOLDER = "/home/pi/Desktop/photos/imgs";

    vector<string> IMG_LIST;
    int max =10;
    for (int i = 0; i < max; i++){
    cout << "SOCKET LOOP: " << i << endl;
    sleep(1);

    vector<string> CURRENT_IMGS;
    for (const auto& entry : filesystem::directory_iterator(IMG_FOLDER))
        {
        filesystem::path outfilename = entry.path();
        string outfilename_str = outfilename.string();
        CURRENT_IMGS.emplace_back(outfilename_str); 
        }

    vector<string> NOT_PROCESSED_IMAGES;
    for (int i=0; i<CURRENT_IMGS.size(); i++)
        {
        bool found = ((find(IMG_LIST.begin(), IMG_LIST.end(), CURRENT_IMGS[i])) == IMG_LIST.end());
        if (found==1)
            {
            NOT_PROCESSED_IMAGES.emplace_back(CURRENT_IMGS[i]);
            }

     // SEND IMAGES
     SendImages(NOT_PROCESSED_IMAGES);

     for(int i=0; i<NOT_PROCESSED_IMAGES.size(); i++){cout << NOT_PROCESSED_IMAGES[i] << endl;}
     IMG_LIST.insert(IMG_LIST.end(), NOT_PROCESSED_IMAGES.begin(), NOT_PROCESSED_IMAGES.end());
     NOT_PROCESSED_IMAGES.clear();

        }

    //bool found = ((find(IMG_LIST.begin(), IMG_LIST.end(), "ciao")) != IMG_LIST.end());
    //for(int i=0; i<NOT_PROCESSED_IMAGES.size(); i++){cout << NOT_PROCESSED_IMAGES[i] << endl;}
    }
    */

    //string IMG_FOLDER = "/home/pi/Desktop/photos/imgs";

    //for (const auto& entry : filesystem::directory_iterator(IMG_FOLDER))
    //{
    //filesystem::path outfilename = entry.path();
    //string outfilename_str = outfilename.string();
    //cout << outfilename_str << endl; 
    //}

    /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    /* --------------- socket() function ------------------*/

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

    cout << "\n=> Socket client has been created..." << endl;
    
    /* 
        The variable serv_addr is a structure of sockaddr_in. 
        sin_family contains a code for the address family. 
        It should always be set to AF_INET.
        htons() converts the port number from host byte order 
        to a port number in network byte order.
    */

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);

    // this function returns returns 1 if the IP is valid
    // and 0 if invalid
    // inet_pton converts IP to packets
    // inet_ntoa converts back packets to IP
    inet_pton(AF_INET, &ip[0], &server_addr.sin_addr);

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        cout << "=> Connection to the server " << inet_ntoa(server_addr.sin_addr) << " with port number: " << portNum << endl;


    /* ---------- CONNECTING THE SOCKET ---------- */
    /* ---------------- connect() ---------------- */

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        cout << "=> Connection to the server port number: " << portNum << endl;

    /* 
        The connect function is called by the client to 
        establish a connection to the server. It takes 
        three arguments, the socket file descriptor, the 
        address of the host to which it wants to connect 
        (including the port number), and the size of this 
        address. 
        This function returns 0 on success and -1 
        if it fails.
        Note that the client needs to know the port number of
        the server but not its own port number.
    */

    cout << "=> Awaiting confirmation from the server..." << endl; //line 40
    //std::vector <char> provaciao;
    int ricevuto;
    //ricevuto = recv(client, &provaciao, bufsize, 0);
    //cout << ricevuto << endl;
    //string str(provaciao.begin(),provaciao.end());
    //cout << str << endl;
    //printf("%s\n",str.c_str());


    ricevuto = recv(client, &buffer, bufsize, 0);
    cout << ricevuto << endl;
    cout << buffer << endl;

    //https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    //https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-strftime-convert-formatted-time
    //https://www.programiz.com/cpp-programming/library-function/ctime/clock
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    cout << "finished at" << std::ctime(&start_time) << endl;



    cout << "=> Connection confirmed, you are good to go...";

    //cout << "\n\n=> Enter # to end the connection\n" << endl;



    /* LISTING FILE IN THE IMG DIRECTORY */
/*
    string IMG_FOLDER = "/home/pi/Desktop/github_lcmrl/RaspberryPiBlack/photos";

    vector<string> IMG_LIST;
    int max =2000;
    for (int i = 0; i < max; i++){
    cout << "SOCKET LOOP: " << i << endl;
    sleep(1);

    vector<string> CURRENT_IMGS;
    for (const auto& entry : filesystem::directory_iterator(IMG_FOLDER)){
        filesystem::path outfilename = entry.path();
        string outfilename_str = outfilename.string();
        CURRENT_IMGS.emplace_back(outfilename_str); 
    }

    vector<string> NOT_PROCESSED_IMAGES;
    for (int i=0; i<CURRENT_IMGS.size(); i++) {
        bool found = ((find(IMG_LIST.begin(), IMG_LIST.end(), CURRENT_IMGS[i])) == IMG_LIST.end());
        if (found==1 && CURRENT_IMGS[i].substr(CURRENT_IMGS[i].size()-3)=="jpg") {
            NOT_PROCESSED_IMAGES.emplace_back(CURRENT_IMGS[i]);
            cout << "check" << CURRENT_IMGS[i].substr(CURRENT_IMGS[i].size()-3) << endl;
        }
    }

    int n_imgs = NOT_PROCESSED_IMAGES.size();
    int n_imgs_conv = htonl(n_imgs);
    cout << "N not processed images: " << n_imgs << endl;
    send(client, &n_imgs_conv, sizeof(n_imgs), 0);




    // SEND IMAGES
    SendImages(NOT_PROCESSED_IMAGES, client, bufsize);

    //for(int i=0; i<NOT_PROCESSED_IMAGES.size(); i++){cout << NOT_PROCESSED_IMAGES[i] << endl;}
    IMG_LIST.insert(IMG_LIST.end(), NOT_PROCESSED_IMAGES.begin(), NOT_PROCESSED_IMAGES.end());
    NOT_PROCESSED_IMAGES.clear();
*/    


    //bool found = ((find(IMG_LIST.begin(), IMG_LIST.end(), "ciao")) != IMG_LIST.end());
    //for(int i=0; i<NOT_PROCESSED_IMAGES.size(); i++){cout << NOT_PROCESSED_IMAGES[i] << endl;}
//    }




    // Once it reaches here, the client can send a message first.

    // ifstream my_image(L"./cat.jpg", std::ios::binary | std::ios::in);
    // ifstream my_image("/home/luca/Scrivania/socket_c++/mine3_images/cat.txt", ios::in | ios::binary);
    // char bufferprova[5];
    // my_image.read(bufferprova, 5);
    // cout << "File size: " << my_image.gcount() << endl;
    // cout << "File size: " << bufferprova << endl;
    // cout << "Client: ";
    // send(client, bufferprova, 5, 0);

    //ifstream my_image("/home/luca/Scrivania/socket_c++/mine3_images/cat_small.jpg", ios::in | ios::binary);
    //int newbufsize = 1024;
    //char newbuffer[bufsize];
    //my_image.read(newbuffer, newbufsize);
    //cout << "File size: " << my_image.gcount() << endl;
    //send(client, newbuffer, newbufsize, 0);
    //my_image.close();

    //unsigned char* imgdata;
    //std::ifstream file (L"/home/luca/Scrivania/socket_c++/mine3_images/luca.jpeg", std::ios::binary | std::ios::out);
    //file.write(provabuffer, provabufsize)
    //cout << "OK \n";
/*
    ifstream my_image("cat.jpeg", ios::in|ios::binary);
    ofstream out_image("xxxxxxxxxxxxxx.jpeg", ios::out|ios::binary);
    char ch;
    std::vector <char> vec;
    //const void * my_poninter = &vec;
    vec.clear();
    while(!my_image.eof())
    {
        ch = my_image.get();
        //cout << "my_image.eof()" << ch << "\n"; // my_image.eof()
        vec.emplace_back(ch);
    }
    cout << "vec.size()" << vec.size() << endl;
*/
    /*send(client, &vec[0], vec.size(), 0);*/

    //cout << "my_image.eof()" << my_image.eof();
    //int newbufsize = 1024;
    //char newbuffer[bufsize];
    //my_image.read(newbuffer, newbufsize);
    //cout << "File size: " << my_image.gcount() << endl;
    //send(client, newbuffer, newbufsize, 0);
    /*int i = 0;
    while(i < vec.size()){
        out_image.put(vec.at(i));
        i++;
    }
    my_image.close();
    out_image.close();
*/

/*
    int costante = 3000;

    int rest = vec.size() % bufsize; //vec.size()
    int rest_conv = htonl(rest);
    send(client, &rest_conv, sizeof(rest_conv), 0);

    int num_chunks =  vec.size() / bufsize;
    int num_chunks_conv = htonl(num_chunks);
    send(client, &num_chunks_conv, sizeof(num_chunks_conv), 0);

    cout << "vec.size()" << vec.size() << endl;
    cout << typeid(vec.size()).name() << endl;
    cout << typeid(bufsize).name() << endl;
    cout << typeid(num_chunks).name() << endl;




    int c = 0;
    char control = 0;
    while(c < num_chunks){
       do{
        send(client, &vec[0 + c*bufsize], bufsize, 0);
        recv(client, &control, 1, 0);
        }
        while(control == 0);
        c++;
    }
    //send(client, &vec[num_chunks*bufsize], rest, 0);

    do{
    send(client, &vec[num_chunks*bufsize], rest, 0);
    recv(client, &control, 1, 0);
    }
    while(control == 0);

*/
    /* ---------------- CLOSE CALL ------------- */
    /* ----------------- close() --------------- */

    /* 
        Once the server presses # to end the connection,
        the loop will break and it will close the server 
        socket connection and the client connection.
    */

    cout << "\n=> Connection terminated.\nGoodbye...\n";

    close(client);
    return 0;
}
