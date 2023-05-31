import struct
import websocket


def deserialize_player(data):
    uuid = data[:15].decode('utf-8')
    data = data[15:]

    pos_x, pos_y = struct.unpack('II', data[:8])
    data = data[8:]

    n_trail = struct.unpack('I', data[:4])[0]
    data = data[4:]

    trail = []
    # for _ in range(n_trail):
    #     trail.append(struct.unpack('II', data[:8]))
    data = data[:0]

    return {'uuid': uuid, 'x':  pos_x, 'y': pos_y, 'trail': trail}


def deserialize_game_manager(data):
    rows, cols = struct.unpack('II', data[:8])
    data = data[8:]
    players = []
    while len(data) > 0:
        players.append(deserialize_player(data))
        data = []

    return {'rows': rows, 'cols': cols, 'players': players}


def receive_response(ws):
    data = ws.recv_frame().data
    print("ok")
    # print(deserialize_game_manager(data))

def send_request(ws):
    request = "some_data"
    ws.send(request)
    print(f"Send {request}")


ws = websocket.create_connection("ws://localhost:8030/game", header=["Authorization: test1234"])

while True:
    send_request(ws)
    receive_response(ws)
