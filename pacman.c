#include <stdio.h>
#include <ncurses.h>
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

const char wall		= '#';	// Maybe add "Obstruction" structm but how?
const char space	= ' ';	// Maybe add "Walkable" struct
const char player	= '@';
const char cookie	= '.';

struct position {		// Player with coodrinates (Y, X)
	int y;
	int x;
};
struct position playerPos;

char direction = ' ';
char newDirection = ' ';

int points = 0;

void drawMaze(void) {
	int x, y;
	for(y = 0; y < Y_SIZE; y++) {
		for(x = 0; x < X_SIZE; x++) {
			if ( (x == playerPos.x) && (y == playerPos.y) ) {
				attron(COLOR_PAIR(2));
					printw("%c", player);
				attroff(COLOR_PAIR(2));
			}
			else if (maze [y][x] == wall){	
				attron(COLOR_PAIR(3));
					printw(" ");
				attroff(COLOR_PAIR(3));
			}
			else if (maze [y][x] == space){	
				attron(COLOR_PAIR(1));
					printw("%c", maze[y][x]);
				attroff(COLOR_PAIR(1));
			}
			else if (maze [y][x] == cookie){	
				attron(COLOR_PAIR(1));
					printw("%c", maze[y][x]);
				attroff(COLOR_PAIR(1));
			}
		}
		printw("\n");
	}
	printw("'w', 'a', 's', 'd' to move. 'x' to Exit.");      
}

char tileInDirection (char d) {
	if (d==' ') return ' ';
	if (d == 'N') {
		return maze[playerPos.y-1] [playerPos.x];
	}
	if (d == 'E') {
		return maze[playerPos.y] [playerPos.x+1];
	}
	if (d == 'S') {
		return maze[playerPos.y+1] [playerPos.x];
	}
	if (d == 'W') {
		return maze[playerPos.y] [playerPos.x-1];
	}
	printw("Invalid [%c]",d);
	return ' ';
}

void eatCookie() {
	if (maze[playerPos.y] [playerPos.x] == cookie) {
		points+= 10;
		maze[playerPos.y] [playerPos.x] = space;	
	}
}

void movePlayer(direction) {
	
	if (direction == ' ') {
		return;
	}
	
	eatCookie();
		
	if (tileInDirection(direction) != wall ) {	
		switch(direction) {
			case 'N': playerPos.y--; break;
			case 'E': playerPos.x++; break;
		    case 'S': playerPos.y++; break;
		    case 'W': playerPos.x--; break;   
		}
	}
}


char playerDirection(char input) {
	if (input == 'w') {
		if (direction == ' ') {
			direction = 'N';
		}
		else {
			newDirection = 'N';
		}
	}	
	else if (input == 'a') {
		if (direction == ' ') {
			direction = 'W';
		}
		else {
			newDirection = 'W';
		}
	}	
	else if (input == 's') {
		if (direction == ' ') {
			direction = 'S';
		}
		else {
			newDirection = 'S';
		}
	}
	else if (input == 'd') {
		if (direction == ' ') {
			direction = 'E';
		}
		else {
			newDirection = 'E';
		}
	}
	else {
		printw("Invalid input");
	} 
}

int main(void) {
	initscr();
	raw();
	//cbreak();
	noecho();
	timeout(0);
	
	start_color();	
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLUE);

	playerPos.y = 5;
	playerPos.x = 10;
	
	int c = 0;
	int k = 0;
	drawMaze();
	
	char input;
	do {
		input = getch();
		if ((input=='w') || (input=='a') || (input=='s') || (input=='d')) {
	    	playerDirection(input);
	    }
	    if ( (c % 16) == 0) {		// evry tenth loop redraw the maze.
	    	erase();	
	    	movePlayer(direction);
	    	if ((newDirection != ' ') && (tileInDirection(newDirection) != wall)) {
	    		direction = newDirection;
	    		newDirection = ' ';
	    	}
	    	printw("%d", points);
	   		drawMaze();
	    }
	    
	   	usleep(10000);
	   			
		// printw("%c",input);
		c++;
	} while(input != 'x');
	
	endwin();
	return 0;
}