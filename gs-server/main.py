import asyncio, socket
import struct

from websockets.server import serve

clients = [ None, None ]
sockets = []

async def sock_recvall (loop, client, size):
    res = b""
    while len(res) < size:
        res += await loop.sock_recv(client, size - len(res))

    return res
async def handle_client_recv(client):
    loop = asyncio.get_event_loop()

    request = None
    while request != 'quit':
        request = await loop.sock_recv(client, 1)
        if client.gss_client_uuid == -1:
            client.gss_client_uuid = request[0]

            clients[client.gss_client_uuid] = client
        elif client.gss_client_uuid == 0:  # SENSOR
            if request[0] == 0: # DPS310
                values = await sock_recvall(loop, client, 12)
                temperature = struct.unpack( "!f", reversed(values[0:4]) )
                pressure    = struct.unpack( "!f", reversed(values[4:8]) )
                altitude    = struct.unpack( "!f", reversed(values[8:12]) )

                for socket in sockets:
                    loop.create_task( socket.send(f"DPS310: {temperature} {pressure} {altitude}\n") )

    client.close()

async def run_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('localhost', 15555))
    server.listen(8)
    server.setblocking(False)

    loop = asyncio.get_event_loop()

    while True:
        client, _ = await loop.sock_accept(server)
        client.gss_client_uuid = - 1

        loop.create_task(handle_client_recv(client))

async def handle_websocket (ws):
    loop = asyncio.get_event_loop()
    sockets.append(ws)

    async for message in ws:
        tar, str = message.split("::")
        tar = int(tar)

        client = clients[tar]
        print(bytes([ len(str) ]) + bytes(str, encoding="utf-8"))
        if client is None:
            await ws.send("ERROR: Client is none")
            continue

        loop.create_task(
            client.sock_sendall(bytes([ len(str) ]) + bytes(str))
        )
        print(message)

async def run_server_socket ():
    print("Running")
    async with serve(handle_websocket, "localhost", 8765):
        print("Created WebSocket server")
        await asyncio.Future()

async def run ():
    loop = asyncio.get_event_loop()

    print("Hello")
    loop.create_task(run_server_socket())
    loop.create_task(run_server())

    await asyncio.Future()

print("Hi !")
asyncio.run(run())
