# mogue
Now written in pure C (std=99) without ncurses.

An important new detail of this rewrite is that it uses ASCII escape codes instead of a special library for terminal display tasks. termios is used only for setting terminal attributes to allow for unbuffered and non-echoing input.

The rewrite solves the screen flickering of the old version (caused by redrawing the whole screen every update), and better cuts down on needless complexity, in my opinion.

"mogue": Norman, borrowing from Old Norse, meaning mug or tankard. (via Wiktionary)

(It's actually just 'rogue' with an M.)
