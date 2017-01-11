/*
ncurses (new curses) - From Wikpedia
is a programming library providing an application programming
interface (API) that allows the programmer to write text-based user interfaces
in a terminal-independent manner.
It is a toolkit for developing "GUI-like" application software
that runs under a terminal emulator.
It also optimizes screen changes, in order to reduce the latency experienced
when using remote shells.

READ ONLINE: http://site.ebrary.com/lib/alltitles/docDetail.action?docID=10278601
*/

#include <stdio.h>
#include <ncurses.h> // Libary used for early rpg like rogue
#include <stdlib.h>
#include <time.h>

#define X_SIZE 21
#define Y_SIZE 7


char maze[Y_SIZE][X_SIZE] = {
	"#####################",
	"#......#     #      #",
	"#.##.#.### ###.#.## #",
	"#.#..#.........#..# #",
	"#.##.#.#######.# ## #",
	"#........           #",
	"#####################"
};

const char wall		= '#';	// Maybe add "Obstruction" structure but how?
const char space	= ' ';	// Maybe add "Walkable" struct
const char pacman	= '@';
const char ghost	= '%';
const char cookie	= '.';

struct position {		// Player with coodrinates (Y, X)
	int y;
	int x;
}player, blinky;

char direction = ' ';
char newDirection = ' ';

int points = 0;

WINDOW *win;

void drawMaze(void) {
	int x, y;
	for(y = 0; y < Y_SIZE; y++) {
		for(x = 0; x < X_SIZE; x++) {

			//Coloring the ghost
			if ( (x == blinky.x) && (y == blinky.y) ) {
				wattron(win,COLOR_PAIR(4));
				wprintw(win,"%c", ghost);
				wattroff(win,COLOR_PAIR(4));
			}
			//Coloring the player
			else if ( (x == player.x) && (y == player.y) ) {
				wattron(win,COLOR_PAIR(2));
				wprintw(win,"%c", pacman);
				wattroff(win,COLOR_PAIR(2));
			}
			// Cookies
			else if (maze [y][x] == cookie) {
				wattron(win,COLOR_PAIR(1));
				wprintw(win,"%c", maze[y][x]);
				wattroff(win,COLOR_PAIR(1));
			}
			//Coloring the walls of the maze
			else if (maze [y][x] == wall) {
				wattron(win,COLOR_PAIR(3));
				wprintw(win," ");
				wattroff(win,COLOR_PAIR(3));
			}
			//Coloring the spaces
			else if (maze [y][x] == space) {
				wattron(win,COLOR_PAIR(1));
				wprintw(win,"%c", maze[y][x]);
				wattroff(win,COLOR_PAIR(1));
			}
		}
	}
	wrefresh(win);
}

void eatCookie() {
	if (maze[player.y] [player.x] == cookie) {
		points+= 10;
		maze[player.y] [player.x] = space;
	}
}


// Optaining the state of a tile in a specific direction
char getTileInDirection (int sender, char d) {
	if (d == ' ') return ' '; 		// No direction

	else if (d == 'N') {					// North
		if (! sender) {
			return maze[player.y-1] [player.x];
		} else {
			return maze[blinky.y-1] [blinky.x];
		}
	}
	else if (d == 'E') { 					// East
		if (! sender) {
			return maze[player.y] [player.x+1];
		} else {
			return maze[blinky.y] [blinky.x+1];
		}
	}
	else if (d == 'S') { 					// South
		if (! sender) {
			return maze[player.y+1] [player.x];
		} else {
			return maze[blinky.y+1] [blinky.x];
		}
	}
	else if (d == 'W') { 					// West
		if (! sender) {
			return maze[player.y] [player.x-1];
		} else {
			return maze[blinky.y] [blinky.x-1];
		}
	}
	else {
		wprintw(win,"Invalid [%c]",d);
		return ' ';
	}
}

// 0 for player 1 for ghost
void moveTo(int sender,char direction) {

	if (direction == ' ') {
		return;
	}
	if (getTileInDirection(sender, direction) != wall ) {
		if (sender == 0) {
			eatCookie();
			// moving Pacman
			switch(direction) {
				case 'N': player.y--; break;
				case 'E': player.x++; break;
			  case 'S': player.y++; break;
			  case 'W': player.x--; break;
			}
		}
		else if (sender == 1) {
		// moving Blinky
			switch(direction) {
				case 'N': blinky.y--; break;
				case 'E': blinky.x++; break;
			  case 'S': blinky.y++; break;
			  case 'W': blinky.x--; break;
			}
		}

	}
}

void moveGhost() {
	char randomDirection = "NESW"[random () % 4];
	moveTo(1,randomDirection);
	return;
}

void playerDirection(int input) {
	switch(input) {
		case 'w':
		case KEY_UP:	if (direction == ' ') {
							direction = 'N';
						}
						else {
							newDirection = 'N';
						}
						break;

		case 'a':
		case KEY_LEFT:	if (direction == ' ') {
							direction = 'W';
						}
						else {
							newDirection = 'W';
						}
						break;

		case 's':
		case KEY_DOWN:	if (direction == ' ') {
							direction = 'S';
						}
						else {
							newDirection = 'S';
						}
						break;

		case 'd':
		case KEY_RIGHT:	if (direction == ' ') {
							direction = 'E';
						}
						else {
							newDirection = 'E';
						}
						break;
	}
}


int main(void) {

	int starty,startx;
  int input;

	initscr();
	raw();

	noecho();
	keypad(stdscr, TRUE);

	//Radom generator initialazation
	srand(time(NULL));
	//Initializeing the colors
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);		// Cookies
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);	// Packman / Player
	init_pair(3, COLOR_BLUE, COLOR_BLUE);			// Walls
	init_pair(4, COLOR_RED, COLOR_BLACK);			// Blinky


	starty = (LINES - Y_SIZE) / 2;	// finding the middle of the window
	startx = (COLS - X_SIZE) / 2;		// same

	printw("'w', 'a', 's', 'd' to move. 'x' to Exit");
	refresh();

	win = newwin(Y_SIZE, X_SIZE, starty, startx);

	// Player starting Position
	player.y = 5;
	player.x = 10;

	// Ghost starting Position
	blinky.y = 1;
	blinky.x = 10;

	// Timing / Ticks
	timeout(0);
	int ticks = 0;

	drawMaze();

	do {
		input = getch();
	  playerDirection(input);

	  if ( (ticks % 16) == 0) {		// evey tenth loop redraws the maze
			werase(win);
			moveGhost();
	    moveTo(0, direction); // 0 for player as the sender of

			if ((newDirection != ' ') && (getTileInDirection(0, newDirection) != wall)) {
	    	direction = newDirection;
	    	newDirection = ' ';
	    }
	    mvprintw(2,2,"%d", points);

			//GAME OVER routine
			if ( (blinky.x == player.x) && (blinky.y == player.y) ) {
				mvprintw(starty-2, startx, "GAME OVER");
				drawMaze();
				usleep(1500000);
				endwin();
				return 0;
			}
			drawMaze();
		}

	  usleep(10000);

		ticks++;
	} while(input != 'x');

	endwin();
	return 0;
}
