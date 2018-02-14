import json
import math
import os
import sys
import atexit
from subprocess import run, TimeoutExpired, CalledProcessError, DEVNULL
from abc import ABCMeta, abstractmethod
from prettytable import PrettyTable

tempRulesFilename = "temp_rules.json"
timeoutsLimit = 4
timeoutSeconds = 1

runtype = "release"
if (len(sys.argv) >= 2
    and (sys.argv[1] == "--debug" or sys.argv[1] == "-d")):
    runtype = "debug"
    print("Running in debug mode...")

flag = ""
if (len(sys.argv) >= 2
    and (sys.argv[1] == "--no-card-symmetry" or sys.argv[1] == "-cs")):
    flag = "-no-card-symmetry"
    print("Running in no-card-symmetry mode...")

if (len(sys.argv) >= 2
    and (sys.argv[1] == "--no-pile-symmetry" or sys.argv[1] == "-ps")):
    flag = "-no-pile-symmetry"
    print("Running in no-pile-symmetry mode...")

if (len(sys.argv) >= 2
    and (sys.argv[1] == "--no-symmetry" or sys.argv[1] == "-ps")):
    flag = "-no-symmetry"
    print("Running in no-symmetry mode...")

if (len(sys.argv) >= 2
    and (sys.argv[1] == "--no-reduced-state" or sys.argv[1] == "-rc")):
    flag = "-no-reduced-state"
    print("Running in no-reduced-state mode...")

if (len(sys.argv) >= 2
    and (sys.argv[1] == "--no-auto-foundations" or sys.argv[1] == "-af")):
    flag = "-no-auto-foundations"
    print("Running in no-auto-foundations mode...")

class SolitaireRulesGen(metaclass=ABCMeta):

    def __init__(self, name):
        self.name = name
        self.baseJson = json.load(open('resources/rules-presets' + name + '.json'))

    def genRules(self, level):
        self.alterFieldsToChange(level)
        with open(tempRulesFilename, "w") as tempOut:
            json.dump(self.baseJson, tempOut)

    @abstractmethod
    def alterFieldsToChange(self, level):
        pass

    def __str__(self):
        return json.dumps(self.baseJson, indent=4)

class SpanishPatienceRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("spanish-patience")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = level

class FreeCellRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("free-cell")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = math.ceil(level * 0.61)
        self.baseJson["cells"] = math.ceil(level * 0.3)

class BlackHoleRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("black-hole")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = math.ceil((4*level - 1)/3)

class BakersDozenRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("bakers-dozen")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = level

class FortunesFavorRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("fortunes-favor")

    def alterFieldsToChange(self, level):
        # Because aces start up we must skip lv 1
        if (level == 1): level = 2;
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = math.ceil(12*level/13)
        self.baseJson["stock size"] = level * 4 - 4 - self.baseJson["tableau piles"]["count"]

class FlowerGardenRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("flower-garden")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = math.ceil(6*level/13)
        self.baseJson["reserve size"] = math.ceil(16*level/13)

class CanfieldRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("canfield")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        cardsNotInTableau = (level - 1) * 4
        reserveSize = math.ceil(cardsNotInTableau * 0.7)
        stockSize = (level - 1) * 4 - reserveSize
        self.baseJson["reserve size"] = reserveSize
        self.baseJson["stock size"] = stockSize

class SomersetRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("somerset")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = math.ceil(10*level/13)

class AlphaStarRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("alpha-star")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = math.ceil(12*level/13)

class SpiderRulesGen(SolitaireRulesGen):

    def __init__(self):
        super().__init__("spider")

    def alterFieldsToChange(self, level):
        self.baseJson["max rank"] = level
        self.baseJson["tableau piles"]["count"] = math.ceil(10*level/13)
        self.baseJson["stock size"] = math.ceil(60*level/13)

def cleanup():
    os.remove(tempRulesFilename)
    run("pkill solvitaire", shell=True)

atexit.register(cleanup)

summaryResults = []

# Loops through each canonical solitaire
for rulesGen in [
    BlackHoleRulesGen()
    ,SpanishPatienceRulesGen()
    ,FreeCellRulesGen()
    ,SomersetRulesGen()
    ,FortunesFavorRulesGen()
    ,FlowerGardenRulesGen()
    ,CanfieldRulesGen()
    ,BakersDozenRulesGen()
    ,AlphaStarRulesGen()
    ,SpiderRulesGen()
]:

    # Loops through the levels
    for level in range(1, 14):

        # Calls the solitaire rules generator function with the level
        rulesGen.genRules(level)

        # Runs the solver with the rules and 20 random seeds. If more than 4
        # don't return in 5 seconds, output that this is the max level
        timeouts = 0
        for attempt in range(20):
            try:
                run("./cmake-build-" + runtype + "/bin/solvitaire" + flag
                    + " --rules " + tempRulesFilename + " --random "
                    + str(attempt + 1),
                    shell=True,
                    check=True,
                    stdout=DEVNULL,
                    stderr=DEVNULL,
                    timeout=timeoutSeconds)
            except TimeoutExpired:
                timeouts += 1
            except CalledProcessError:
                print("Error running " + rulesGen.name + " at level "
                        + str(level) + " with seed " + str(attempt + 1) + "!")
                print(str(rulesGen) + "\n")
                sys.exit()
            except:
                print("Unknown error in script")
                sys.exit()
                
            run("pkill solvitaire", shell=True)
                
            if timeouts > timeoutsLimit:
                break

        if timeouts > timeoutsLimit:
            print("Could not solve " + rulesGen.name + " at level " + str(level)
            + " within time constraint\n")
            print("Game description at level " + str(level) + ":")
            print(str(rulesGen) + "\n")
            summaryResults.append((rulesGen.name, level-1))
            break
        else:
            print("Solved " + rulesGen.name + " at level " + str(level)
                    + " ...")
            if level == 13:
                summaryResults.append((rulesGen.name, level))

# Prints results summary table
t = PrettyTable(['Level', 'Solitaire'])
for (name, level) in summaryResults:
    t.add_row([level, name])
print(t)
