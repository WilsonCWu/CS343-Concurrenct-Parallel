u++ q1voter.cc q1printer.cc q1tallyVotesMC.cc q1main.cc -DMC -g
./a.out 3 3 3 3 1
u++ q1voter.cc q1printer.cc q1tallyVotesSEM.cc q1main.cc -DSEM -g
./a.out 3 3 3 3 1
u++ q1voter.cc q1printer.cc q1tallyVotesBAR.cc q1main.cc -DBAR -g
./a.out 3 3 3 3 1
