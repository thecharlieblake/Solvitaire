# Solvitaire
A solver for a variety of single-player, perfect
information solitaire card games.

## Project Overview

The solver should be able to correctly provide
solutions for some instances of solitaire games, and
be able to report other instances as unsolvable,
minimising the number of instances that are deemed
'intractable' (cannot be classified as solvable or
unsolvable within a reasonable time period). A key
study for this project will be how to do this
efficiently for a _variety_ of solitaire games.

Perfect information games are an especially
important domain in artificial intelligence, and
some of the most high-profile successes in the field
of AI have been creating good players for games like
Chess, Checkers and Go, all of which are perfect 
information games. However, relatively little work
has been done on creating programs to play
single-player perfect information games, and good
solvers do not yet exist for most solitaire games.

Hence, this project will focus on creating a solver
which is general enough to be able to play a wide
range of solitaire games, whilst also maintaining a
high level of efficiency. This efficiency will be
achieved through a combination of language-level
optimisation, and through exploring different AI
search techniques. A specialised solver will be
written to accomplish this, in code rather than
using an existing constraint solving or planning
programs.

## Objectives

### Primary Objectives
- An application for solving single-player, perfect
information solitaire instances (software artefact).
- Some kind of schema or language for describing a
_wide_ range of solitaire games.
- The application must be able to solve valid games 
expressed using the schema/language.
- Automated testing to demonstrate the correctness
of my classifications.
- An implementation of search techniques such as
'meta-moves'/'super moves' and eliminating
symmetrical states.
- An implementation of language-level/design
optimisations to improve the efficiency of my
search.
- An analysis in my report of the effects of the
above search techniques and implementation choices
on the performance of the solver.

### Secondary Objectives

- Optimise the search algorithm to eliminate
searching states in which certain moves/states
'dominate' others.
- Implement and evaluate more than one different
type of search (e.g. best first, iterative
deepening). **Objective adjusted:** This is no
longer deemed essential, as the focus in on
exhaustive search, but it may be interesting
to explore how iterative deepening performs.
- Experiment with and evaluate different designs
for the general heuristic for best-first search,
that works across a variety of games.
**Objective adjusted:** This is no longer relevant.

### Tertiary Objectives

Other tertiary objectives that satisfy the core
aims of the project would be reasonable
additions. Below are two particular instances of
interesting routes down which this project could
be taken:

- An extension of the algorithm that aims to
provide shortened solutions (i.e. ones that
minimise the number of moves taken).
**Objective adjusted:** This may be done in
tandem with the iterative deepening objective.
- Calculating the solvability ratio for some the
games my application can solve.
**Objective adjusted:** Because of good progress
made, this is now very much a target for the
project.

## Supervisor

Ian Gent

## Background

["Search in the Patience Game Black Hole"](https://pn.host.cs.st-andrews.ac.uk/blackhole-revised-final.pdf)
