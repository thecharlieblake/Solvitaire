#from sys import argv
#import subprocess
import json

#_, seed = argv

# command = "./KlondikeSolver /G " + seed + " /R /DC 3"
# result = subprocess.run(["./KlondikeSolver", "/G", seed, "/R", "/DC", "3"], stdout=subprocess.PIPE).stdout.decode('utf-8')

with open('sm_out.txt', 'r') as myfile:
    data = myfile.read()

lines = [line.strip('\n').replace("T", "10")[4:] for line in data.splitlines()]

obj = {
    "tableau piles": [
        list(reversed(line.split('-')[0].split() + list(map(lambda c: c.lower(), line.split('-')[1:]))))
        for line in lines[1:8]],
    "stock": list(reversed(lines[8].split())),
    "waste": list(reversed(lines[0].split())),
    "foundations": [
        list(reversed(lines[9].split())),
        list(reversed(lines[10].split())),
        list(reversed(lines[11].split())),
        list(reversed(lines[12].split()))
    ]
}

# face-up down split, waste, foundations (how do they work in solvitiare?)

print(json.dumps(obj), file=open("sol_in.txt", "wt"))
