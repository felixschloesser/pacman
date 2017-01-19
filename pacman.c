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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <ncurses.h> // Libary used for early rpg like rogue
#include <unistd.h> // macPort of ncruses

#define MAXMAZE 1000

const char c_wall		= '#';
const char c_space	= ' ';
const char c_player	= '@';
const char c_ghost	= '%';
const char c_cookie	= '.';

struct figure {		// Player / ghosts with coodrinates (Y, X), and direction
	int y;
	int x;
	char direction;
	char nextDirection;
} player, ghost, blinky;

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

void eatCookie(struct figure *player) {
	if (maze_get(player->x, player->y) == c_cookie) {
		points += 10;
		maze_put(player->x, player->y, c_space);
	}
}

char getTileInDirection (char d, struct figure *figure) {
	switch(d) {
		case ' ': return ' '; break; // No direction
		case 'N': return maze_get(figure->x,figure->y-1); break;
		case 'E': return maze_get(figure->x+1,figure->y); break;
		case 'S': return maze_get(figure->x,figure->y+1); break;
		case 'W': return maze_get(figure->x-1,figure->y); break;
	 //	default: wprintw(win,"Invalid [%c]",d); break;
	}
	return ' ';
}

void moveFigure(struct figure *figure) {

	if (figure->direction == ' ') {
		return;
	}
	if (getTileInDirection(figure->direction,figure) != c_wall ) {
		switch(figure->direction) {
			case 'N': figure->y--; break;
			case 'E': figure->x++; break;
			case 'S': figure->y++; break;
			case 'W': figure->x--; break;
		}
	}
}

void setDirectionRandom(struct figure *ghost) {
	char randomDirection = "NESW"[random () % 4];
	ghost->direction = randomDirection;
}

void addValue(char *arr, int *len, char value) {
	arr[*len] = value;
    ++*len;
}

void rmValue(char *arr, int *len, char value){
	int i = 0;
	int pos = -1;
	if (*len > 0)
	{
		for(i = 0; i < *len; i++)
		{
	  		if (arr[i] == value)
			{pos = i;}
		}
	}
    if (pos > -1)
    {
 		if(pos+1==*len){
			--*len;
			return;
		}
		for(i = pos; i < *len-1; i++)
		arr[i] = arr[i+1];
		--*len;
	}
}

char sourceDirection(char d) {
   	switch(d) {
		case ' ': return ' '; break; // No direction
		case 'N': return 'S'; break;
		case 'E': return 'W'; break;
		case 'S': return 'N'; break;
		case 'W': return 'E'; break;
	 //	default: wprintw(win,"Invalid [%c]",d); break;
	}
	return ' ';
}

void blinkyAI(struct figure *figure) {
	char branches[4];
	int numOfBranches = 0;
    int d = 0;
    struct figure f1,f2;

  	for(d = 0; d < 4; d++)
	if (getTileInDirection("NESW"[d], figure) != c_wall)
	 	{addValue(branches,&numOfBranches,"NESW"[d]);}
	if ((figure->direction != ' ') && (numOfBranches > 1))
	{rmValue(branches,&numOfBranches,sourceDirection(figure->direction));}

	 switch (numOfBranches) {
	 	case 1:
	 			figure->direction = branches[0];
				break;
		case 2:
		   f1 = *figure; f1.direction = branches[0];
		   moveFigure(&f1);
		   f2 = *figure; f2.direction = branches[1];
		   moveFigure(&f2);
	 	  if (sqrt( pow(f1.x - player.x, 2) + pow(f1.y - player.y, 2)) >
	 	     sqrt( pow(f2.x - player.x, 2) + pow(f2.y - player.y, 2)))
	 	  {figure->direction =   branches[1];}
	 	  else
	 	  {figure->direction =   branches[0];}
	 	  break;
		case 4:  // open space, keep direction
		break;
	}
}


void ghostAI(struct figure *ghost) {

	int numOfBranches = 0;
	float distanceToPlayer;
	char direction;

	if (getTileInDirection('N', ghost) != c_wall)
	 	numOfBranches++;
	if (getTileInDirection('E', ghost) != c_wall)
	 	numOfBranches++;
	if (getTileInDirection('S', ghost) != c_wall)
		numOfBranches++;
	if (getTileInDirection('W', ghost) != c_wall)
		numOfBranches++;


	 switch (numOfBranches) {			//dead end -> turn other way
	 	case 1: if (ghost->direction == 'N')
							ghost->direction = 'S';
						if (ghost->direction == 'E')
							ghost->direction = 'W';
						if (ghost->direction == 'S')
							ghost->direction = 'N';
						if (ghost->direction == 'W')
							ghost->direction = 'E';
						break;

		// A TURN OR A STRAIGHT
		case 2:	if (getTileInDirection('W', ghost) != c_wall) {

							if (getTileInDirection('N', ghost) != c_wall) {		// West - North turn
								if (ghost->direction == 'E') // going to the West
									ghost->direction = 'N';

								if (ghost->direction == 'S')	// going to the North
									ghost->direction = 'W';
							}
							if (getTileInDirection('S', ghost) != c_wall) {		// West - South turn
								if (ghost->direction == 'E') // going to the West
									ghost->direction = 'S';

								if (ghost->direction == 'N')	// going to the South
									ghost->direction = 'W';
							}
						}

						if (getTileInDirection('E', ghost) != c_wall) {

							if (getTileInDirection('N',ghost) != c_wall) {			// East - North turn
								if (ghost->direction == 'W') // going to the East
									ghost->direction = 'N';

								if (ghost->direction == 'S')	// going to the North
									ghost->direction = 'E';
								}
							if (getTileInDirection('S',ghost) != c_wall) {		// East - South turn
								if (ghost->direction == 'W') // going to the East
									ghost->direction = 'S';

								if (ghost->direction == 'N')	// going to the South
									ghost->direction = 'E';
							}
						}
						break;

		// MORE THAN THREE BRANCHES
		case 3:
		case 4:	if (getTileInDirection('N', ghost) != c_wall)	{
							distanceToPlayer =  sqrt( pow(ghost->x - player.x, 2) + pow(ghost->y+1 - player.y, 2));
							direction = 'N';
						}

						if (getTileInDirection('E', ghost) != c_wall)	{
							if (distanceToPlayer <= sqrt( pow(ghost->x-1 - player.x, 2) + pow(ghost->y - player.y, 2)) ) {
								distanceToPlayer =  sqrt( pow(ghost->x-1 - player.x, 2) + pow(ghost->y - player.y, 2));
								direction = 'E';
							}
						}
						if (getTileInDirection('S', ghost) != c_wall)	{
							if (distanceToPlayer <= sqrt( pow(ghost->x - player.x, 2) + pow(ghost->y-1 - player.y, 2)) ){
								distanceToPlayer = sqrt( pow(ghost->x - player.x, 2) + pow(ghost->y-1 - player.y, 2));
								direction = 'S';
							}
						}
						if (getTileInDirection('W', ghost) != c_wall)	{
							if (distanceToPlayer <= sqrt( pow(ghost->x-1 - player.x , 2) + pow(ghost->y - player.y, 2)) ){
								distanceToPlayer = sqrt( pow(ghost->x-1 - player.x , 2) + pow(ghost->y - player.y, 2));
								direction = 'W';
							}
						}
						//printf("Distance %f", distanceToPlayer);
						ghost->direction = direction;
						break;
	}
}

void setDirectionFromInput(int input,struct figure *player) {
	switch(input) {
		case 'w':
		case KEY_UP:	if (player->direction == ' ') {
										player->direction = 'N';
									}
									else {
										player->nextDirection = 'N';
									}
									break;
		case 'a':
		case KEY_LEFT:	if (player->direction == ' ') {
											player->direction = 'W';
										}
										else {
											player->nextDirection = 'W';
										}
										break;
		case 's':
		case KEY_DOWN:	if (player->direction == ' ') {
											player->direction = 'S';
										}
										else {
											player->nextDirection = 'S';
										}
										break;
		case 'd':
		case KEY_RIGHT:	if (player->direction == ' ') {
											player->direction = 'E';
										}
										else {
											player->nextDirection = 'E';
										}
										break;
	}
}



void setnextDirection(struct figure *player)
{
	if (player->nextDirection != ' ') {
		if (getTileInDirection(player->nextDirection,player) != c_wall) {
			player->direction = player->nextDirection;
			player->nextDirection = ' ';
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
	if ((input == 'x') || (input == 'q')) {
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
    ghost.direction = ' ';

	blinky.y = 1;
	blinky.x = 11;
	blinky.direction = ' ';

	// Timing / Ticks
	int ticks = 0;

	int gameState = 0; // 0 = playing, 1 = won, 7 = defeated by ghost, 10 = terminated
	drawMaze();

	do {
		input = getch();
	  	if (input != 255) {
			setDirectionFromInput(input,&player);
		}
		if ((ticks % 30) == 0) {		// evey tenth loop redraws the maze
			//setDirectionRandom(&ghost);
			ghostAI(&ghost);
    		moveFigure(&ghost);

			blinkyAI(&blinky);
        	moveFigure(&blinky);
	  }
	  if ((ticks % 16) == 0) {		// evey tenth loop redraws the maze;
			moveFigure(&player);
			eatCookie(&player);
			setnextDirection(&player);

			printPoints();
			drawMaze();
	  }
		if ((ticks % 100) == 0) {
			//points = points - 5;
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
