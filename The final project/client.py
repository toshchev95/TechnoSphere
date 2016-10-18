import sys
import socket
import select

def isAuthentication(sock):
    try:
        isOkay = False;

        while(isOkay == False):
            register = input('Sign up or Login? ')
            sign = ''
            if 'sign' in register.lower():
                sign = 'Sign '
            elif 'login' in register.lower():
                sign = 'login '
            else:
                continue    

            login = input('Choose your nameUsers: ')
            password = input('Your password: ')

            #string = sign + login+'+'+password
            #sock.send(bytes(string +'\n', 'cp866'))
            string = sign + login+'+'+password + '\n'
            sock.send(string.encode())
            
            response = sock.recv(512).decode('cp866')
            print("response: "+response)
            
            if 'login ok' in response:
                isOkay = True
            else:
                print('Wrong input login and password')
        return login;
    except:
        print('Authentication problems ')
        sys.exit()

def chat_client():
    if len(sys.argv) != 3:
        print("Usage: python chatclient.py [host] [port number")
        sys.exit()

    host = sys.argv[1]
    port = int(sys.argv[2])
    print('Welcome to the socketChat client.')

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        host_ip = socket.gethostbyname(host)
        sock.connect((host_ip, port))

        nameUsers = isAuthentication(sock)

        msg = input("%s> " % nameUsers)

        while ('quit' in msg) == False:
            while len(msg) > 1024:
                print('Your message was too long. Maximum 1024 characters.')
                msg = input("%s> " % nameUsers)
            #sock.send(msg.encode())
            sock.send(bytes(msg+'\n', 'cp866'))
                
            print('[waiting for response...]')
            response = sock.recv(1024).decode('cp866')
            if 'quit' in response:
                print("Received quit signal. Closing connection.")
                sys.exit()
            print(response)

            msg = input("%s> " % nameUsers)
        
        print("Closing Connection.")

        sock.send(bytes('quit\n', 'cp866'))
    except socket.gaierror:
        print('Could not find host. Exiting.')
        sys.exit()
    finally:
        print(sys.stderr, 'closing socket')
        sock.close()

if __name__ == "__main__":
    sys.exit(chat_client())