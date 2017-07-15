# mogue
Now written in pure C (without ncurses).

Controls: h/j/k/l/y/u/b/n, numpad, or arrow keys for movement. (Arrow keys can not move diagonally, however.)
\> and < go down or up a staircase (a '>' or '<') respectively.
Shift-S redraws the screen (which you mostly want to do if you resize the terminal).

If you have the scepter (a purple 'I' in the dungeon) then z + (direction) summons a zombie adjacent to you in that direction. o + (direction) summons a portal one tile away in that direction. Shift-Z summons zombies all around you. If you have the scepter when you die, you can press Shift-R to resurrect yourself, losing the scepter.

An important new detail of this rewrite is that it uses ASCII escape codes instead of a special library for terminal display tasks. termios is used only for setting terminal attributes to allow for unbuffered and non-echoing input. As it turns out, if you compile the game with cygwin and have cygwin1.dll installed or in the same folder, the exe will work with Windows.

The rewrite solves the screen flickering of the old version (caused by redrawing the whole screen every update), and better cuts down on needless complexity, in my opinion.

"mogue": Norman, borrowing from Old Norse, meaning mug or tankard. (via Wiktionary)

(It's actually just 'rogue' with an M.)
