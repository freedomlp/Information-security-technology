import base64
import binascii
from pyDes import *

def _example_des_():
	from time import time

	# example of DES encrypting in CBC mode with the IV of "\0\0\0\0\0\0\0\0"
	print ("Example of DES encryption using CBC mode\n")
	k = des("ABCDEFGH", ECB, "\0\0\0\0\0\0\0\0")
	
	data = "I love you!I love you!I love you!I love you!"
	
	
	print ("Key      : 4142434445464748")
	print ("Data     : %r" % data)

	d = k.encrypt(data, padmode=PAD_PKCS5)
	ret = binascii.b2a_hex(d)
	print ("Encrypted: %r" % ret)

	d = k.decrypt(d)
	print ("Decrypted: %r" % d)

_example_des_()
