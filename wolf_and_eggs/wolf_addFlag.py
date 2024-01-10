#!/usr/bin/env python3

from pwn import *

# elf = ELF("./alpha_assault.elf")
# libc = ELF("./libc.so.6")

port = 1337
# s = process(["./ld-linux.so.2", elf.path], env={"LD_PRELOAD":"./libc.so.6"})
# s = process(elf.path)

class Standart_User:
	def __init__(self, ip):
		self.s = remote(ip, port)

		random_string = ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(8))
		self.username = random_string
		self.password = random_string
		self.flag = f"{''.join(random.choice(string.ascii_letters.upper() + string.digits) for _ in range(31))}="

		self.register()


	def register(self):
		# register
		self.s.sendline(b'2')
		self.s.sendline(self.username.encode())
		self.s.sendline(self.password.encode())

		# login
		self.s.sendline(self.username.encode())	
		self.s.sendline(self.password.encode())


	def store(self):
		self.s.sendline(b'1')
		self.s.sendline(b'0')
		self.s.sendline(b'secret')
		self.s.sendline(self.flag.encode())


	def exit(self):
		self.s.sendline(b'5')


def main():

	try:
		ip = sys.argv[1]
	except:
		print(f"USAGE:\t{sys.argv[0]} <victim-ip-address>")
		sys.exit(1)

	for _ in range(10):
		Mike = Standart_User(ip)
		Mike.store()
		Mike.exit()


if __name__=="__main__":
	main()