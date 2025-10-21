import socket
import struct
import uuid

class DragonicaGM:
    ip = ''
    port = 15401
    sock = None
    PACKET_HEADER_SIZE = 4

    def __init__(self, ip, port=15401, cmd_guid=uuid.UUID(version=4, hex='20e319cd-eb50-41b6-beff-723b350df2e2')) -> None:
        self.ip = ip
        self.port = port
        self.cmdGuid = cmd_guid

    def __enter__(self):
        self.conn()
        self.recv() # Skip welcome packet
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def conn(self):
        if self.sock:
            raise ValueError("Connection is already open!")
        self.sock = socket.socket()
        self.sock.connect((self.ip, self.port))

    def close(self):
        if not self.conn:
            raise ValueError("Connection is not open!")
        self.sock.close()
        self.sock = None

    def recv(self):
        if not self.conn:
            raise ValueError("Connection is not open!")

        data = self.sock.recv(self.PACKET_HEADER_SIZE)
        size = struct.unpack('I', data)[0]
        return self.sock.recv(size)

    def send(self, type, data):
        if not self.conn:
            raise ValueError("Connection is not open!")
        data_to_send = struct.pack('IH', 2 + len(data), type) + data
        self.sock.sendall(data_to_send)

