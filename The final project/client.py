import sys
import socket

if len(sys.argv) != 3:
    print("Usage: python chatclient.py [host] [port number")
    sys.exit()

host = sys.argv[1]
port = int(sys.argv[2])
print('Welcome to the socketChat client.')

# get nameUsers
nameUsers = input('Choose your nameUsers: ')

# create socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # get host and connect socket
    host_ip = socket.gethostbyname(host)
    sock.connect((host_ip, port))
    #sock.send(bytes("Hi! Starting!\n", 'UTF-8'))
    msg = input("%s> " % nameUsers)

    while (True):#(msg != 'quit'):
        while len(msg) > 1024:
            print('Your message was too long. Maximum 1024 characters.')
            msg = input("%s> " % nameUsers)
        #sock.send("b%s> %s" % (nameUsers,msg))
        #sock.send(msg.encode()) # work
        sock.send(bytes(msg+'\n', 'cp866')) # work
            
        print('[waiting for response...]')
        response = sock.recv(1024).decode('cp866')
        if 'quit' in response:
            print("Received quit signal. Closing connection.")
            sys.exit()
        print(response)

        msg = input("%s> " % nameUsers)
    
    # react to quit message
    print("Closing Connection.")
    # send quit to server
    sock.send(bytes('quit\n', 'cp866'))
except socket.gaierror:
    print('Could not find host. Exiting.')
    sys.exit()
finally:
    print(sys.stderr, 'closing socket')
    sock.close()