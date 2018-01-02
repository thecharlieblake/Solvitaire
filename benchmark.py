import json
import math
import os
import sys
import atexit
from subprocess import run, TimeoutExpired, CalledProcessError, DEVNULL
from abc import ABCMeta, abstractmethod

tempRulesFilename = "temp_rules.json"
timeoutsLimit = 4
timeoutSeconds = 1

class SolitaireRulesGen(metaclass=ABCMeta):

    def __init__(self, name):
        self.name = name
        self.baseJson = json.load(open('resources/' + name + '.json'))

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

def cleanup():
    os.remove(tempRulesFilename)
    run("pkill solvitaire", shell=True)

atexit.register(cleanup)

# Loops through each canonical solitaire
for rulesGen in [SpanishPatienceRulesGen(),
                 FreeCellRulesGen(),
                 BlackHoleRulesGen(),
                 BakersDozenRulesGen()]:

    # Loops through the levels
    for level in range(1, 13):

        # Calls the solitaire rules generator function with the level
        rulesGen.genRules(level)

        # Runs the solver with the rules and 20 random seeds. If more than 4
        # don't return in 5 seconds, output that this is the max level
        timeouts = 0
        for attempt in range(20):
            try:
                run("./cmake-build-release/bin/solvitaire --rules "
                        + tempRulesFilename + " --random " + str(attempt + 1),
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
                sys.exit()
            except:
                print("Unknown error in script")
                sys.exit()
                
            run("pkill solvitaire", shell=True)
                
            if timeouts > timeoutsLimit:
                break;

        if timeouts > timeoutsLimit:
            print("Could not solve " + rulesGen.name + " at level " + str(level)
            + " within time constraint\n")
            print("Game description at level " + str(level) + ":")
            print(str(rulesGen) + "\n")
            break
        else:
            print("Solved " + rulesGen.name + " at level " + str(level)
                    + " ...")
