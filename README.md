# Solvitaire: A general solver for perfect-information solitaire games

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) [![GitHub release](https://d25lcipzij17d.cloudfront.net/badge.svg?id=gh&type=6&v=0.09)](https://github.com/thecharlesblake/Solvitaire/releases/tag/v0.09)

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

## Help

If you have any problems getting these steps to work, don't hesitate to get in
touch via <thecharlesblake@live.co.uk>

