import subprocess
import json
import re

with open('sample100.txt', 'r') as myfile:
    data = myfile.read()

lines = data.splitlines()
deals = []
for (i, line) in enumerate(lines):
    # Get stuck starting positions
    if line[:8] == "Canfield" and lines[i+12][:5] == "Stuck":
        foundations = []
        reserve = []
        tableau = []
        stock = []
        for (j, line) in enumerate(lines[i+1:i+12]):
            cards = line[6:].replace("T", "10")

            # Reserve
            if j < 4 and cards != '':
                foundations.append(cards)
            elif j == 4:
                reserve = list(filter(None, re.compile("\x1b\[[0-9]m ?").split(cards)))
            elif j > 4 and j < 9:
                tableau.append([cards])
            elif j == 9 or j == 10:
                stock += cards.split(' ')
        deals.append({
            "foundations": foundations,
            "reserve": reserve,
            "tableau piles": tableau,
            "stock": stock
        })

for (i, deal) in enumerate(deals):
    print(json.dumps(deal), file=open("tmp.json", "wt"))
    print("Attempting deal " + str(i))
    subprocess.run(["./cmake-build-release/bin/solvitaire", "--type", "canfield", "tmp.json", "--class", "--timeout", "600000"])

