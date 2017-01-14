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
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAXMAZE 1000

const char c_wall		= '#';
const char c_space	= ' ';
const char c_player	= '@';
const char c_ghost	= '%';
const char c_cookie	= '.';

struct figure {		// Player / ghosts with coodrinates (Y, X), and direction
	int y;
	int x;
	char nowDirection;
	char newDirection;
} player, ghost,blinky;

char maze[MAXMAZE];
int sizex=0, sizey = 0;

int points = 100;

WINDOW *win;

char maze_get(int x, int y) { // x=0..sizex-1,
	return maze[x + y*sizex];
}

void maze_put(int x, int y, char c) {
	maze[x + y*sizex] = c;
}

void load_maze() {
  sizex = 0; sizey = 0;
  strcpy(maze,"#####################");
  sizex = 21;
  sizey++;
  strcat(maze,"#......#     #      #");
  sizey++;
  strcat(maze,"#.##.#.### ###.#.## #");
  sizey++;
  strcat(maze,"#.#..#.........#..# #");
  sizey++;
  strcat(maze,"#.##.#.#######.# ## #");
  sizey++;
  strcat(maze,"#........           #");
  sizey++;
  strcat(maze,"#####################");
  sizey++;
}

void drawMaze(void) {

	int x, y;
	werase(win);
	for(y = 0; y < sizey; y++) {
		for(x = 0; x < sizex; x++) {

			//Coloring the ghosts
			if ( (x == ghost.x) && (y == ghost.y) ) {
				wattron(win,COLOR_PAIR(4));
				wprintw(win,"%c", c_ghost);
				wattroff(win,COLOR_PAIR(4));
			}
			else if ( (x == blinky.x) && (y == blinky.y) ) {
				wattron(win,COLOR_PAIR(5));
				wprintw(win,"%c", c_ghost);
				wattroff(win,COLOR_PAIR(5));
			}

			//Coloring the player
			else if ( (x == player.x) && (y == player.y) ) {
				wattron(win,COLOR_PAIR(2));
				wprintw(win,"%c", c_player);
				wattroff(win,COLOR_PAIR(2));
			}
			// Cookies
			else if (maze_get(x,y) == c_cookie) {
				wattron(win,COLOR_PAIR(1));
				wprintw(win,"%c", c_cookie);
				wattroff(win,COLOR_PAIR(1));
			}
			//Coloring the walls of the maze
			else if (maze_get(x,y) == c_wall) {
				wattron(win,COLOR_PAIR(3));
				wprintw(win," ");
				wattroff(win,COLOR_PAIR(3));
			}
			//Coloring the spaces
			else if (maze_get(x,y) == c_space) {
				wattron(win,COLOR_PAIR(1));
				wprintw(win,"%c", c_space);
				wattroff(win,COLOR_PAIR(1));
			}
		}
		//wprintw(win,"\n");
	}
	wrefresh(win);
}

void eatCookie(struct figure *f) {
	if (maze_get(f->x, f->y) == c_cookie) {
		points += 10;
		maze_put(f->x, f->y, c_space);
	}
}

char getTileInDirection (char d,struct figure *f) {
	switch(d) {
		case ' ': return ' '; break; // No direction
		case 'N': return maze_get(f->x,f->y-1); break;
		case 'E': return maze_get(f->x+1,f->y); break;
		case 'S': return maze_get(f->x,f->y+1); break;
		case 'W': return maze_get(f->x-1,f->y); break;
	 //	default: wprintw(win,"Invalid [%c]",d); break;
	}
	return ' ';
}

void moveFigure(struct figure *f) {

	if (f->nowDirection == ' ') {
		return;
	}
	if (getTileInDirection(f->nowDirection,f) != c_wall ) {
		switch(f->nowDirection) {
			case 'N': f->y--; break;
			case 'E': f->x++; break;
			case 'S': f->y++; break;
			case 'W': f->x--; break;
		}
	}
}

void setDirectionRandom(struct figure *f) {
	char randomDirection = "NESW"[random () % 4];
	f->nowDirection = randomDirection;
}

void setDirectionFromInput(int input,struct figure *f) {
	switch(input) {
		case 'w':
		case KEY_UP:	if (f->nowDirection == ' ') {
							f->nowDirection = 'N';
						}
						else {
							f->newDirection = 'N';
						}
						break;

		case 'a':
		case KEY_LEFT:	if (f->nowDirection == ' ') {
							f->nowDirection = 'W';
						}
						else {
							f->newDirection = 'W';
						}
						break;

		case 's':
		case KEY_DOWN:	if (f->nowDirection == ' ') {
						    f->nowDirection = 'S';
						}
						else {
							f->newDirection = 'S';
						}
						break;

		case 'd':
		case KEY_RIGHT:	if (f->nowDirection == ' ') {
							f->nowDirection = 'E';
						}
						else {
							f->newDirection = 'E';
						}
						break;
	}
}

void setNewDirection(struct figure *f)
{
	if (f->newDirection != ' ') {
		if (getTileInDirection(player.newDirection,&player) != c_wall) {
			player.nowDirection = player.newDirection;
			player.newDirection = ' ';
		}
	}
}

void printPoints (void) {
	mvprintw(2,2,"%4d", points);
	refresh();
}

void debug (void) {
	mvprintw(4,4,"%d %d", ghost.x, ghost.y);
}

int checkIfGameOver(int input) {

	if (input == 'x') {
		return 10;
	}
	if (points > 350) {
		return 1;
	}
	if (points < 0) {
		return 6;
	}
	//GAME OVER routine
	if ( (ghost.x == player.x) && (ghost.y == player.y) ) {
		return 7;
	}
	if ( (blinky.x == player.x) && (blinky.y == player.y) ) {
		return 7;
	}
	return 0;
}


int main(void) {
	int starty,startx;
	int input;

	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	//Radom generator initialazation
	srand(time(NULL));

	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);		// Cookies
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);	// Packman / Player
	init_pair(3, COLOR_BLUE, COLOR_BLUE);		// Walls
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);	// Ghost
	init_pair(5, COLOR_RED, COLOR_BLACK);		// Blinky

	load_maze();

	starty = (LINES - sizey) / 2;	/* Calculating for a center placement */
	startx = (COLS - sizex) / 2;	/* of the window		*/
	printw("'w', 'a', 's', 'd' to move. 'x' to Exit");
	refresh();

	win = newwin(sizey, sizex, starty, startx);
	timeout(0);

	// Player starting Position
	player.y = 5;
	player.x = 10;

	// Ghost starting Position
	ghost.y = 1;
	ghost.x = 10;
	// Ghost starting Position
	blinky.y = 1;
	blinky.x = 11;

	// Timing / Ticks
	int ticks = 0;

	int gameState = 0; // 0 = playing, 1 = won, 7 = defeated by ghost, 10 = terminated
	drawMaze();

	do {
		input = getch();
	  	if (input != 255) {
			setDirectionFromInput(input,&player);
		}
		if ((ticks % 16) == 0) {		// evey tenth loop redraws the maze
			setDirectionRandom(&ghost);
      moveFigure(&ghost);

			setDirectionRandom(&blinky);
      moveFigure(&blinky);

			moveFigure(&player);
			eatCookie(&player);
			setNewDirection(&player);

			printPoints();
			drawMaze();
	  }
		if ((ticks % 100) == 0) {
			points = points - 5;
			printPoints();
		}
		gameState = checkIfGameOver(input);
		usleep(10000);
	  ticks++;
	} while (!gameState);

	if (gameState == 1) {
		mvprintw(5,29,"CONGRATS - YOU WON!");
  }
	else if (gameState == 6) {
		mvprintw(5,29,"YOU LOST ALL  POINTS");
  }
	else if (gameState == 7) {
		mvprintw(5,29,"YOU LOST - GAME OVER");
	}
	else if (gameState == 10) {
		mvprintw(5,29,"QUIT: GAME TERMINATED");
  }
	refresh();
	sleep(2);
	delwin(win);
	endwin();
	return 0;
}
