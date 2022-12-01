import socket
import threading

OUT_FOLDER = r"C:\Users\Luscias\Desktop\3DOM\Github_3DOM\Server_Connection\local_network_share_images\out_folder"

### CONSTANTS ###
HEADER = 64
PORT = 5050
SERVER = socket.gethostbyname(socket.gethostname()) # or hardcoded SERVER = "192.168.82.218" # type "ipconfig" on the cmd of Windows to know your local network, copy and paste here the IPv4 Address
ADDR = (SERVER, PORT)
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "!DISCONNECT"
print("SERVER IPv4: {}".format(SERVER))


### FUNCTIONS ###
#def handle_client(conn, addr):
#    print(f"[NEW CONNECTION] {addr} connected.")
#
#    connected = True
#    while connected:
#        msg_lenght = conn.recv(HEADER).decode(FORMAT)
#        if msg_lenght:
#            msg_lenght = int(msg_lenght)
#            msg = conn.recv(msg_lenght).decode(FORMAT)
#            if msg == DISCONNECT_MESSAGE:
#                connected = False
#
#            print(f"[{addr}] {msg}")
#            conn.send("Msg received".encode(FORMAT))
#    conn.close()

def handle_client(conn, addr):
    print(f"[NEW CONNECTION] {addr} connected.")

    file_counter = 0
    connected = True
    while connected:
        img_bytes = conn.recv(40960000)
        if img_bytes:
            file_counter += 1
            with open(rf"{OUT_FOLDER}\{file_counter}.jpg", 'wb') as out_file:
                out_file.write(img_bytes)
            #msg_lenght = int(msg_lenght)
            #msg = conn.recv(msg_lenght).decode(FORMAT)
            #if msg == DISCONNECT_MESSAGE:
            #    connected = False

            #print(f"[{addr}] {msg}")
            #conn.send("Msg received".encode(FORMAT))
    conn.close()


def start():
    server.listen()
    print(f"[LISTENING] Server is listening on {SERVER}")
    while True:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(conn, addr))
        thread.start()
        print(f"[ACTIVE CONNECTIONS] {threading.activeCount() - 1}")


### MAIN ###
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # AF_INET is the type of connection and stay for IPv4 address, SOCK_STREAM is the protocol to stream the data
server.bind(ADDR)

print("[STARTING] server is starting...")
start()
