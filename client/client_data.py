import wave
import socket
import os
import time


if(os.path.exists('./test_data/sine_wave_1khz_out.wav')):
    os.remove('./test_data/sine_wave_1khz_out.wav')

wave_file = wave.open('./test_data/sine_wave_1khz.wav', 'rb')
n_channels = wave_file.getnchannels()
n_samples = wave_file.getnframes()
sample_width = wave_file.getsampwidth()
sample_rate = wave_file.getframerate()
buffer_size = 1024

out_file = wave.open('./test_data/sine_wave_1khz_out.wav', 'wb')
out_file.setnchannels(n_channels)
out_file.setsampwidth(sample_width)
out_file.setframerate(sample_rate)
out_file.setnframes(n_samples)

print(f'Number of samples: {n_samples}')
print(f'Sample width: {sample_width} bytes')
print(f'Sample rate: {sample_rate}')


ip_addr = "192.168.100.70"
port = 8080

start = time.time()
for i in range(0, n_samples, buffer_size // sample_width):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip_addr, port))

    frames = wave_file.readframes(buffer_size // sample_width)
    # print(f'-- Frames:\n{frames.hex()}')

    client_socket.send(frames)

    client_socket.settimeout(3)

    response = client_socket.recv(buffer_size)
    if not response:
        print('-- Connection closed by server')
        break
    else:
        # print(f'-- Response:\n{response.hex()}')
        out_file.writeframes(response)

    client_socket.settimeout(None)

    client_socket.close()

end = time.time()

print(f'Elapsed time: {end - start} seconds')

wave_file.close()
out_file.close()
