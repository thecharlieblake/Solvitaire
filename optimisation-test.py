import sys
from subprocess import run, CalledProcessError, DEVNULL, PIPE, Popen

runCount = 1000

# Loops through each canonical solitaire

for optimisation in [
#    "-no-reduced-state",
#    "-no-pile-symmetry",
#    "-no-auto-foundations",
    "-no-card-just-moved"
]:

    for preset in [
        "simple-alpha-star",
        "simple-bakers-dozen",
        "simple-black-hole",
        "simple-canfield",
        "simple-flower-garden",
        "simple-fortunes-favor",
        "simple-free-cell",
        "simple-somerset",
        "simple-spanish-patience",
        "simple-spider"]:

        differs = []

        for attempt in range(runCount):
            try:
                op_p = Popen("./cmake-build-release/bin/solvitaire" + optimisation
                    + " --type " + preset + " --random " + str(attempt)
                    + " --classify",
                    shell=True,
                    stdout=PIPE,
                    stderr=DEVNULL)
                out_str = op_p.stdout.readline()
                if out_str == b'Solved\n':
                    op_res = True
                elif out_str == b'No solution\n':
                    op_res = False
                else:
                    raise Exception('Invalid return value!')

                no_op_p = Popen("./cmake-build-release/bin/solvitaire"
                    + " --type " + preset + " --random " + str(attempt)
                    + " --classify",
                    shell=True,
                    stdout=PIPE,
                    stderr=DEVNULL)
                out_str = no_op_p.stdout.readline()
                if out_str == b'Solved\n':
                    no_op_res = True
                elif out_str == b'No solution\n':
                    no_op_res = False
                else:
                    raise Exception('Invalid return value!')

                if op_res != no_op_res:
                    differs.append(attempt)

            except CalledProcessError:
                print("Error running " + preset + " with seed " + str(attempt) + "!")
                sys.exit()
            except Exception as e:
                print("Unknown error in script: ", e)
                sys.exit()

            run("pkill solvitaire", shell=True)

        if not differs :
            print(preset + ": No difference in solutions for " + optimisation)
        else :
            print(preset + ": Optimisation " + optimisation + " differs for the following seeds:")
            print(differs)
