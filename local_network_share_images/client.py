import os
import time
import socket

INPUT_FOLDER = r"C:\Users\Luscias\Desktop\3DOM\Github_3DOM\Server_Connection\local_network_share_images\imgs" #r"/storage/emulated/0/DCIM/Camera"#

#HEADER = 64
PORT = 5050
#FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "!DISCONNECT"
SERVER = "192.168.43.210"
ADDR = (SERVER, PORT)


def send_img(img_bytes):
    client.send(img_bytes)


### MAIN ###
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(ADDR)

existing_files = os.listdir(INPUT_FOLDER)
transmitted_files = []
while True:
    time.sleep(1)
    for file in existing_files:
        if file not in transmitted_files:
            with open (rf"{INPUT_FOLDER}/{file}", 'rb') as img_file:
                img_bytes = img_file.read()
                send_img(img_bytes)
            print(f"Processed {file}")
            transmitted_files.append(file)
    existing_files = os.listdir(INPUT_FOLDER)

