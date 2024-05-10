import asyncio, socket
import struct

from websockets.server import serve

clients = [ None, None ]
sockets = []

async def sock_recvall (reader, size):
    res = b""
    while len(res) < size:
        res += await reader.read(size - len(res))

    return res
async def handle_client_recv(reader, writer):
    loop = asyncio.get_event_loop()

    writer.gss_client_uuid = -1

    request = None
    while request != 'quit':
        request = await reader.read(1)
        if len(request) == 0: continue
        print(request)
        if writer.gss_client_uuid == -1:
            writer.gss_client_uuid = request[0]
            print("CLIENT UUID IS ", request[0])
            clients[writer.gss_client_uuid] = writer
        elif writer.gss_client_uuid == 0:  # SENSOR
            if request[0] == 0: # DPS310
                values = await sock_recvall(reader, 12)
                print(values)
                temperature = struct.unpack( "!f", bytes(reversed(values[0:4]) ))
                pressure    = struct.unpack( "!f", bytes(reversed(values[4:8]) ))
                altitude    = struct.unpack( "!f", bytes(reversed(values[8:12]) ))

                for socket in sockets:
                    loop.create_task( socket.send(f"DPS310: {temperature} {pressure} {altitude}\n") )

    writer.close()

async def run_server():
    print("Starting Socket Server")
    server = await asyncio.start_server(
        handle_client_recv,
        "172.20.10.7",
        5042
    )
    async with server:
        await server.serve_forever()

async def handle_websocket (ws):
    loop = asyncio.get_event_loop()
    sockets.append(ws)

    async for message in ws:
        tar, str = message.split("::")
        tar = int(tar)

        client = clients[tar]
        if client is None:
            await ws.send("ERROR: Client is none")
            continue
        
        client.write(bytes([ len(str) ]) + bytes(str, encoding="utf-8"))
        
async def run_server_socket ():
    async with serve(handle_websocket, "localhost", 8765):
        await asyncio.Future()

async def run ():
    loop = asyncio.get_event_loop()

    loop.create_task(run_server_socket())
    loop.create_task(run_server())

    await asyncio.Future()

asyncio.run(run())
