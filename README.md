# General Solitaire Solver
A solver for a range of perfect-information, single player solitaire card games

## Project Outline

Most focus in games Artificial Intelligence focusses on games involving two or
more players. However, many people enjoy single-player games, sometimes
called “Solitaire Games”. Examples are many variants of card Solitaire games
(called Patience in the UK), the game Peg-Solitaire, and many others which still
fall into the category despite not having “solitaire” in the name.

A particularly important class of games are “perfect information” games, games
like Chess and Go where chance plays no role and all players have available all
the information relevant to the game. Creating world-class players for games
like this represent some of the highest achievements in Artificial Intelligence.
Not all card Solitaire games have this property, but many do, such as
“Freecell”, “King Albert”, and “Black Hole”. The game Peg-Solitaire has this
property too, as do many others. Unfortunately, good solvers are not available
for most solitaire games.

This project is to build high quality solvers for solitaire games of perfect
information. The goal is to study how to do this efficiently for a variety
of games, so the aim will be to build a solver or solvers for multiple
games.

The exact specification is negotiable with the student. It could be for card
solitaire games, peg-solitaire games, or others. The focus could be on a
greater or smaller range of games, and also could be on modelling games so
that a variety of games can be expressed in (e.g.) JSON and then solved by a
multipurpose solver.

Similarly the solving technology could be negotiated, so it could be
specially written code, or the use of existing search tools like constraint
solvers or planning programs.

### Supervisors
Ian Gent

### Artefact(s)
A solver or multiple solvers for a variety of games. 

### Background
["Search in the Patience Game Black Hole"](https://pn.host.cs.st-andrews.ac.uk/blackhole-revised-final.pdf)
