# mogue
I've begun rewriting this "game" completely from scratch in pure C for my own entertainment.
I'm no longer satisfied with the code quality of this old project and I no longer believe in the merits of object-oriented programming. As a result, I've decided to rewrite this project and leave the old version up as a relic.

So far the new version is progressing nicely, already solving one major design flaw of the old version, namely that the screen would flicker when holding down a movement key because the whole screen would be refreshed. That was a lazy hack. Now, only the characters that move are redrawn. Another thing it improves upon is cutting away needless complexity.

Another important new feature of this rewrite is that it uses ASCII escape codes instead of ncurses functions to move the cursor around the screen and other such tasks. It only uses termios for unbuffered non-echoing input.

It's very rough now, but the rewrite will take shape in time.

I'm also finally going to learn to use the `git` command instead of just using the online interface like a plebeian. It's about time.

"mogue": Norman, borrowing from Old Norse, meaning mug or tankard. (via Wiktionary)

(It's actually just 'rogue' with an M.)
