import random
import requests
import json
import datetime
import random
import string
apiKey = "134a8fb6-3824-497a-a23c-1b89abe03a8a"
epochInitial = 1704067200
server = "http://localhost:8080/"
class Block:
    def __init__(self, temp=None, hum=None,node=None):
        self.temp = temp
        self.hum = hum
        self.node = node


class Node:
    def __init__(self, macaddress, token=None):
        self.macaddress = macaddress
        self.token = token
        self.data = []


class colors:
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    RESET = '\033[0m' 

def authenthicate(mac, token,ip):
    try:
        custom_headers = {
            "ApiToken" :token,
            "macaddress": mac,
            "api_key": apiKey,
            "ip":ip
        }
        response = requests.get(server+"api/node/authenticate",headers=custom_headers)
        if response.status_code == 200:
            JSONObj = response.text
            send = json.loads(JSONObj)["result"]["token"]
            return send
        else:
            return f"Error: {response.status_code}"
    except requests.RequestException as e:
        return f"An error occurred: {e}"
    
def sendData(token, data):
    try:
        custom_headers = {
            "auth_token": token
        }
        response = requests.post(server+"api/node/data",headers=custom_headers, json= data)
        if response.status_code == 200:
            return 
        else:
            return f"Error: {response.status_code}"
    except requests.RequestException as e:
        return f"An error occurred: {e}"
    

def sendConnection(token, data):
    try:
        custom_headers = {
            "auth_token": token
        }
        response = requests.post(server+"api/node/connections",headers=custom_headers, json= data)
        if response.status_code == 200:
            return 
        else:
            return f"Error: {response.status_code}"
    except requests.RequestException as e:
        return f"An error occurred: {e}"

def genmac():
    mac_digits = [random.choice('0123456789ABCDEF') for _ in range(12)]
    mac_address = ':'.join(''.join(mac_digits[i:i+2]) for i in range(0, 12, 2))
    return mac_address

def gen_token(length):
    characters = string.ascii_letters + string.digits
    token = ''.join(random.choice(characters) for _ in range(length))
    return token
def gen_ip():
    ip = '.'.join(str(random.randint(0, 255)) for _ in range(4))
    return ip


n_data = 120

n_nodes = 40
height = 10  
width = 10
search_radius = 2
epochInteval = 60  #in seconds

map = [[None for _ in range(width)] for _ in range(height)]
for i in range(height):
    for j in range(width):
        map[i][j] = Block()


def get_neighboring_values(i, j):
    neighbors = []
    for dx in range(-1, 2):
        for dy in range(-1, 2):
            x = i + dx
            y = j + dy
            if 0 <= x < len(map) and 0 <= y < len(map[0]):
                neighbors.append(map[x][y])
    return neighbors


closestConnections = {}
def getNeighbor():
    for i in range(height):
        for j in range(width):
            if(map[i][j].node is not None):
                data = {
                        "connections":[]
                        }
                for n in range(max(0, i - search_radius), min(height, i + search_radius+1)):
                    for m in range(max(0, j - search_radius), min(width, j + search_radius+1)):
                        if map[n][m].node is not None and n != i and m != j:
                            data["connections"].append(map[n][m].node.macaddress)
                            #print("\nnode:"+map[i][j].node.macaddress.split(":")[0]+" found:"+map[n][m].node.macaddress.split(":")[0])
                sendConnection(map[i][j].node.token, data)
                #print("\n-------------------------")
                

            
extremTemp = [0,0]
extremHum = [0,0]
def gentemp():
    isFirst = True
    for i in range(height):
        for j in range(width):
            temp = random.uniform(16, 30)
            hum = random.uniform(40, 70)
            map[i][j].temp = temp
            map[i][j].hum = hum
    for i in range(height):
        for j in range(width):
            neighbors = get_neighboring_values( i, j)
            avg_temp = sum(block.temp for block in neighbors) / len(neighbors)
            avg_hum = sum(block.hum for block in neighbors) / len(neighbors)
            temp = round(random.uniform(avg_temp - 1, avg_temp + 1), 1)
            hum = round(random.uniform(avg_hum - 2, avg_hum + 2), 1)
            if(isFirst):
                isFirst = False
                extremTemp[0]=temp
                extremTemp[1]=temp
                extremHum[0]=hum
                extremHum[0]=hum
            if(temp > extremTemp[0]):
                extremTemp[0] = temp
            if(temp < extremTemp[1]):
                extremTemp[1] = temp
            if(hum > extremHum[0]):
                extremHum[0] = hum
            if(hum < extremHum[1]):
                extremHum[1] = hum
            map[i][j].temp = temp
            map[i][j].hum = hum
    return
def showMap_temp():
    # Print out the temperatures
    tempinterval = (extremTemp[0]-extremTemp[1])/3
    huminterval = (extremHum[0]-extremHum[1])/3
    for i in range(height):
        for j in range(width):
            temp = map[i][j].temp
            if(temp<extremTemp[1]+tempinterval):
                print(f"{colors.YELLOW}", end="")
                print("[{:.1f}]".format(map[i][j].temp), end="")
                print(f"{colors.RESET}", end="")
            if(temp>extremTemp[1]+tempinterval and temp < extremTemp[0]-tempinterval):
                print(f"{colors.GREEN}", end="")
                print("[{:.1f}]".format(map[i][j].temp), end="")
                print(f"{colors.RESET}", end="")
            if(temp>extremTemp[0]-tempinterval):
                print(f"{colors.RED}", end="")
                print("[{:.1f}]".format(map[i][j].temp), end="")
                print(f"{colors.RESET}", end="")
        print("")

def showMap_nodes():
    for i in range(height):
        for j in range(width):
            if(map[i][j].node is not None):
                print("["+map[i][j].node.macaddress.split(":")[0]+"]", end="")
                print("", end="")
                continue
            print("[  ]", end="")
        print("")


#insert nodes into theyr spot 
for i in range(n_nodes):
    while(1):
        x = random.randint(0,height-1)
        y = random.randint(0,width-1)
        if map[x][y].node is None:
            node = Node(genmac())
            token = authenthicate(node.macaddress, gen_token(15), gen_ip())
            node.token = token
            map[x][y].node = node
            break
getNeighbor()


tempEpoch = epochInitial - epochInteval
for l in range(n_data):
    tempEpoch = tempEpoch + epochInteval
    gentemp()
    for i in range(height):
        for j in range(width):
            if(map[i][j].node is not None):
                #print("["+map[i][j].node.macaddress.split(":")[0]+"]", end=" - ")
                #print(tempEpoch)
                datajson = {
                    "timestamp": tempEpoch, 
                    "humidity": map[i][j].hum,
                    "temperature": map[i][j].temp
                }          
                map[i][j].node.data.append(datajson)
                #print(len(map[i][j].node.data))
    
    
for i in range(height):
    for j in range(width):
        if(map[i][j].node is not None):
            
            sendData(map[i][j].node.token, map[i][j].node.data)


            
showMap_nodes()
showMap_temp()




