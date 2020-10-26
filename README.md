# Solvitaire: A general solver for perfect-information solitaire games
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) [![GitHub release](https://d25lcipzij17d.cloudfront.net/badge.svg?id=gh&type=6&v=0.10.2)](https://github.com/thecharlesblake/Solvitaire/releases/tag/v0.10/2)
[![DOI](https://zenodo.org/badge/103662666.svg)](https://zenodo.org/badge/latestdoi/103662666)<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-4-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

## Install

The easiest way to install (and run) Solvitaire is by using the supplied
Docker image. For more information about installing Docker, head to 
<https://www.docker.com/get-started>

Currently instructions for installing/building outside of the docker image are not
provided, but the required setup/dependencies should be clear from the docker file
(they aren't complex). Please get in touch if you would like this documented
properly, or pre-built binaries to be provided.

With Docker installed, simply run:

```
$ ./docker-install.sh
```

This command downloads a docker image containing an environment in which
Solvitaire can be built and run, without the user needing to worry about having
the correct dependencies/versions.

It also adds a pre-made CMake build system and a built binary to your directory
so you don't have to wait for an initial build to compile. Rather convenient!

The downside to this approach is that the Docker image is unfortunately quite
large, and pulling it may take a minute or two (although you only have to do
this once!) If you don't wish to use docker, the steps contained in
_Dockerfile_ should be a good guide to the required dependencies, although bear
in mind that on older Linux systems the required versions of dependencies may
not be available through _rpm_ / _deb_.

(Note: on one system I had to run `sudo usermod -aG docker $USER`
and then restart my machine before docker would allow me sufficient
permissions. Just making a note of this here in case others encounter a similar
problem)

## Using the Docker image

To use the docker image, simply run:

```
$ ./enter-container.sh
```

This command will start the docker container and enter an interactive session
within the container's filesystem. The files you'll see there are simply a
mounted version of the root directory, so any changes you make in the
container will be reflected in your filesystem, and vice versa. When
you exit the interactive session the container will automatically be stopped.

If you don't wish to enter an interactive session, you can run:

```
$ ./enter-container.sh "command_to_be_run"
```

This will spin up a container, run the command, and tear the container down,
with no noticeable overhead. Combining this with the fact that the repo's
files are mounted in the Docker image, means that a user can edit
Solvitaire's files on their local system using their own tools/editor, and
then build/execute Solvitaire in the controlled Docker environment - the
best of both worlds!

For more information on Docker images, see
<https://docs.docker.com/get-started/part2/#recap-and-cheat-sheet-optional>

## Build

If the above instructions have been followed, you should already have a
built binary in the root directory. 

To (re-)build Solvitaire within the container, simply run:

```
$ ./build.sh [--release|--debug] [--solvitaire|--unit-tests]
(default args = "--release" "--solvitaire")
```

If you wish to run a build from outside the container, run:

```
$ ./enter-container.sh ./build.sh
```

For more build information and options, examine _build.sh_ and
_CMakeLists.txt._

## Run

To run Solvitaire from within the container, simply run either:

```
$ ./solvitaire
$ ./solvitaire-debug

Or from outside the container:

$ ./enter-container.sh "./solvitaire"
```

and for the unit tests (note, these must be
run inside src/test for the integration tests to pass):

```
$ cd src/test
$ ../../unit-tests
$ ../../unit-tests-debug

Or from outside the container:

$ ./enter-container.sh "cd src/test; ../../unit-tests"
```

to see what solvitaire can do, use the `--help` command:

```
$ ./enter-container.sh "./solvitaire --help"
Usage: solvitaire [options] input-file1 input-file2 ...
options:
  --help                    produce help message
  --type arg                specify the type of the solitaire game to be solved
                            from the list of preset games. Must supply either 
                            this 'type' option, or the 'custom-rules' option
  --available-game-types    outputs a list of the different preset game types 
                            that can be solved
  --describe-game-rules arg outputs the JSON that describes the rules of the 
                            supplied preset game type
  --custom-rules arg        the path to a JSON file describing the rules of the
                            solitaire to be solved. Must supply either 'type' 
                            or 'custom-rules' option
  --random arg              create and solve a random solitaire deal based on a
                            seed. Must supply either 'random','solvability', 
                            'benchmark' or list of deals to be solved.
  ... etc ...
```

...
for example, if you wish to generate a random deal (with seed 1) of the game Klondike
and attempt to solve it, run:

```
$ ./enter-container.sh "./solvitaire --type klondike --random 1"
  [info] Attempting to solve with seed: 1...
  Deal:
  --- Foundations ---------
  []	[]	[]	[]	
  --- Tableau Piles -------
  9D	##	##	##	##	##	##	
  	AS	##	##	##	##	##	
  		JC	##	##	##	##	
  			6D	##	##	##	
  				JH	##	##	
  					AH	##	
  						JD	
  --- Stock | Waste -------
  6H	[]	
  KS		
  10C		
  QH		
  ...	
  ===================================
  Solution Type: unsolvable
  States Searched: 417235
  Unique States Searched: 164359
  Backtracks: 417234
  Dominance Moves: 63707
  States Removed From Cache: 0
  Final States In Cache: 100652
  Final Buckets In Cache: 196613
  Maximum Search Depth: 46
  Final Search Depth: 0
  Time Taken (milliseconds): 903
```

...
full documentation of solvitaire is not yet available, but information can be provided
to those who reach out over email (see below).

## Help

If you have any problems getting these steps to work, don't hesitate to get in
touch via <thecharlieblake@gmail.com>

User [@galcohensius]( https://github.com/galcohensius ) has also provided a helpful
cheat-sheet for Windows users to help get set up on Solvitaire, which can be found
in [docs/windows_cheat_sheet]( docs/windows_cheat_sheet.md ).

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="http://thecharlesblake.co.uk/"><img src="https://avatars1.githubusercontent.com/u/17354715?v=4" width="100px;" alt=""/><br /><sub><b>Charlie Blake</b></sub></a><br /><a href="https://github.com/thecharlieblake/Solvitaire/commits?author=thecharlesblake" title="Code">💻</a> <a href="#design-thecharlesblake" title="Design">🎨</a> <a href="#ideas-thecharlesblake" title="Ideas, Planning, & Feedback">🤔</a></td>
    <td align="center"><a href="http://ian.gent"><img src="https://avatars0.githubusercontent.com/u/2893913?v=4" width="100px;" alt=""/><br /><sub><b>Ian Gent</b></sub></a><br /><a href="https://github.com/thecharlieblake/Solvitaire/commits?author=turingfan" title="Code">💻</a> <a href="#design-turingfan" title="Design">🎨</a> <a href="#ideas-turingfan" title="Ideas, Planning, & Feedback">🤔</a></td>
    <td align="center"><a href="http://www.shlomifish.org/"><img src="https://avatars1.githubusercontent.com/u/3150?v=4" width="100px;" alt=""/><br /><sub><b>Shlomi Fish</b></sub></a><br /><a href="https://github.com/thecharlieblake/Solvitaire/commits?author=shlomif" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/galcohensius"><img src="https://avatars1.githubusercontent.com/u/25342140?v=4" width="100px;" alt=""/><br /><sub><b>Gal Cohensius</b></sub></a><br /><a href="https://github.com/thecharlieblake/Solvitaire/commits?author=galcohensius" title="Documentation">📖</a></td>
  </tr>
</table>

<!-- markdownlint-enable -->
<!-- prettier-ignore-end -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
