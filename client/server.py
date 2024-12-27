import socket

print("Starting server...")
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("localhost", 8080))
server_socket.listen()

while True:
    print("Waiting for connection...")
    client_socket, addr = server_socket.accept()
    print("Connected to", addr)

    data = client_socket.recv(256)
    if not data:
        break
    
    data = data.decode()
    if data == "exit":
        client_socket.close()
        break

    print("Received:", data)
    client_socket.send(f"Received: {data}".encode())

server_socket.close()
