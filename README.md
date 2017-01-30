# Pacman *- for console*
This is a remake of the 1980' arcade-classic Pacman in the UNIX-Console. At this point it still lacks authentic ghost-behavoir in case of eating a cake.
## How to compile
supports macOS and linux,

* **on Mac:**
  *gcc -o pacman pacman.c -lncurses*

* **on Linux:**
  *gcc -o pacman pacman.c -lncurses -ln*

**To execute:**
*./pacman* 

##Libarys used:
ncurses (new curses) - From Wikpedia

is a programming library providing an application programming
interface (API) that allows the programmer to write text-based user interfaces
in a terminal-independent manner.
It is a toolkit for developing "GUI-like" application software
that runs under a terminal emulator.
It also optimizes screen changes, in order to reduce the latency experienced
when using remote shells.

READ ONLINE: http://site.ebrary.com/lib/alltitles/docDetail.action?docID=10278601
