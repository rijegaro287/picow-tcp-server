import socket
import time

ip_addr = "192.168.100.70"

while True:
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip_addr, 8080))

    print('='*50)
    print('Input a message:')
    input_msg = input()

    if len(input_msg) == 0:
        continue
    if input_msg == 'exit':
        client_socket.send('exit'.encode())
        break
    
    client_socket.send(input_msg.encode())

    client_socket.settimeout(5)

    response = client_socket.recv(256)
    if not response:
        print('\n-- Connection closed by server')
        break
    else:
        print(f'\n-- Server Response: {response.decode()}\n')

    client_socket.settimeout(None)

    client_socket.close()
