//
//  pacman.c
//  Procedural Programming Project @ Hamburg University of Technology
//
//  Created by Shayan Bahadori Rad, George Bahna and Felix Schlösser on 16.12.16.
//  Copyright © 2017 Shayan Bahadori Rad, George Bahna and Felix Schlösser. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <ncurses.h>	// Libary used for early rpg like rogue
#include <unistd.h>		// macOS port of ncruses

#define MAXSECONDS 100
#define MAXSCORE 11
#define MAXMAZE 1024

const char c_wall	= '#';
const char c_space	= ' ';
const char c_player	= '@';
const char c_ghost	= '%';
const char c_cookie	= '.';
const char c_border	= 'B';
const char c_cake	= 'o';

char maze[MAXMAZE];
int sizex=0, sizey = 0;

struct figure {		// Player / ghosts with coodrinates (Y, X), and direction
    int y;
    int x;
    char direction;
    char nextDirection;
    int  state;  // sec || 0=normal / agressive, >4 stupid, >10still, >20frightend
} player, blinky, pinky, bashful;

struct score {		// for highscore list
    char name[30];
    int cookies;
    int cakes;
    int time;
    int state;
};

struct score scoretab[MAXSCORE];
int scoreCount = 0;

char playerName[80] = "";
int cookies = 0;
int cakes = 0;
int seconds = 0;

WINDOW *menuWin;
WINDOW *gameWin;
WINDOW *subWin;

char maze_get(int x, int y) {  // x=0..sizex-1,
    if (x<0 || y<0 || x>=sizex || y>=sizey) {
        return c_border;
    }
    return maze[x + y*sizex];
}

void maze_put(int x, int y, char c) {
    maze[x + y*sizex] = c;
}

int countNumOf(char c) {
    int i;
    int numOf = 0;
    for(i = 0; i < MAXMAZE; i++) {
        if (maze[i] == c) {
            numOf++;
        }
    }
    return numOf;
}

void load_maze() {
    sizey = 0;
    sizex = 23;

    FILE *mazeSource;
    char buffer[1000];

    mazeSource = fopen("maze.txt","r");
    if (mazeSource) {
      while (fgets(buffer,1000, mazeSource)!=NULL) {
        strcpy(maze, buffer);
        sizey++;
      }
	  fclose(mazeSource);
	}
	else	{
     strcpy(maze,"#######################");	sizey++;
     strcat(maze,"#..........#..........#");	sizey++;
     strcat(maze,"#.###.####.#.####.###.#");	sizey++;
     strcat(maze,"#o# #.#  #.#.#  #.# #o#");	sizey++;
     strcat(maze,"#.###.####.#.####.###.#");	sizey++;
     strcat(maze,"#.....................#");	sizey++;
     strcat(maze,"#.###.#.#######.#.###.#");	sizey++;
     strcat(maze,"#.....#....#....#.....#");	sizey++;
     strcat(maze,"#####.#.##.#.##.#.#####");	sizey++;
     strcat(maze,"    #.#         #.#    ");	sizey++;
     strcat(maze,"#####.# ### ### #.#####");	sizey++;
     strcat(maze,"    ..  #     #  ..    ");	sizey++;
     strcat(maze,"#####.# ####### #.#####");	sizey++;
     strcat(maze,"    #.#         #.#    ");	sizey++;
     strcat(maze,"#####.#.#######.#.#####");	sizey++;
     strcat(maze,"#..........#..........#");	sizey++;
     strcat(maze,"#.###.####.#.####.###.#");	sizey++;
     strcat(maze,"#o..#.............#..o#");	sizey++;
     strcat(maze,"###.#.#.#######.#.#.###");	sizey++;
     strcat(maze,"#.....#....#....#.....#");	sizey++;
     strcat(maze,"#.########.#.########.#");	sizey++;
     strcat(maze,"#.....................#");	sizey++;
     strcat(maze,"#######################");	sizey++;
   }
}

void drawMaze(void) {
    int x, y;
    werase(gameWin);
    for(y = 0; y < sizey; y++) {
        for(x = 0; x < sizex; x++) {

            //Coloring the ghosts
            if ( (x == pinky.x) && (y == pinky.y) ) {
                wattron(gameWin,COLOR_PAIR(4));
                if ((pinky.state > 0) && (pinky.state < 10))  {
                    wattron(gameWin,A_BLINK);
                }
                wprintw(gameWin,"%c", c_ghost);
                if ((pinky.state > 0) && (pinky.state < 10))  {
                    wattroff(gameWin,A_BLINK);
                }
                wattroff(gameWin,COLOR_PAIR(4));
            }
            else if ( (x == blinky.x) && (y == blinky.y) ) {
                wattron(gameWin,COLOR_PAIR(5));
                if ((blinky.state > 0) && (blinky.state < 10))  {
                    wattron(gameWin,A_BLINK);
                }
                wprintw(gameWin,"%c", c_ghost);
                if ((blinky.state > 0) && (blinky.state < 10)) {
                    wattroff(gameWin,A_BLINK);
                }
                wattroff(gameWin,COLOR_PAIR(5));
            }
            else if ( (x == bashful.x) && (y == bashful.y) ) {
                wattron(gameWin,COLOR_PAIR(6));
                if ((bashful.state > 0) && (bashful.state < 10)) {
                    wattron(gameWin,A_BLINK);
                }
                wprintw(gameWin,"%c", c_ghost);
                if ((bashful.state > 0) && (bashful.state < 10)) {
                    wattroff(gameWin,A_BLINK);
                }
                wattroff(gameWin,COLOR_PAIR(6));
            }

            //Coloring the player
            else if ( (x == player.x) && (y == player.y) ) {
                wattron(gameWin,COLOR_PAIR(2));
                wprintw(gameWin,"%c", c_player);
                wattroff(gameWin,COLOR_PAIR(2));
            }
            // Cookies
            else if (maze_get(x,y) == c_cookie) {
                wattron(gameWin,COLOR_PAIR(1));
                wprintw(gameWin,"%c", c_cookie);
                wattroff(gameWin,COLOR_PAIR(1));
            }
            // Cake
            else if (maze_get(x,y) == c_cake) {
                wattron(gameWin,COLOR_PAIR(1));
                wprintw(gameWin,"%c", c_cake);
                wattroff(gameWin,COLOR_PAIR(1));
            }
            //Coloring the walls of the maze
            else if (maze_get(x,y) == c_wall) {
                wattron(gameWin,COLOR_PAIR(3));
                wprintw(gameWin," ");
                wattroff(gameWin,COLOR_PAIR(3));
            }
            //Coloring the spaces
            else if (maze_get(x,y) == c_space) {
                wattron(gameWin,COLOR_PAIR(1));
                wprintw(gameWin,"%c", c_space);
                wattroff(gameWin,COLOR_PAIR(1));
            }
        }
    }
    wrefresh(gameWin);
}

void initGhosts(int x, int y, int s) {
    // Ghosts starting Position
    blinky.y = y;
    blinky.x = x-2;
    blinky.state = s;
    blinky.direction = ' ';

    pinky.y = y;
    pinky.x = x;
    pinky.direction = ' ';
    pinky.state = s;

    bashful.y = y;
    bashful.x = x+2;
    bashful.direction = ' ';
    bashful.state = s;
}

void eatCookie(struct figure *player) {
    if (maze_get(player->x, player->y) == c_cake) {
        cakes -= 1;
        initGhosts(11,11,5);
        maze_put(player->x, player->y, c_space);
    }
    else if (maze_get(player->x, player->y) == c_cookie) {
        cookies -= 1;
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
    }
    return ' ';
}

void moveFigure(struct figure *figure) {
    if (figure->direction == ' ') {
        return;
    }
    char c;
    c = getTileInDirection(figure->direction,figure);
    if (c == c_border) {  // teleport to the other side of the maze
        if (figure->x==0) 	{figure->x=sizex-1;}
        else if (figure->x==sizex-1) {figure->x=0;}
        else if (figure->y==0) 	{figure->y=sizey-1;}
        else if (figure->y==sizey-1) {figure->y=0;}
    }
    else if (c != c_wall ) {
        switch(figure->direction) {
            case 'N': figure->y--; break;
            case 'E': figure->x++; break;
            case 'S': figure->y++; break;
            case 'W': figure->x--; break;
        }
    }
}

void stupidAI(struct figure *figure) {
    char randomDirection = "NESW"[random () % 4];
    figure->direction = randomDirection;
}

void addValue(char *arr, int *len, char value) {
    arr[*len] = value;
    ++*len;
}

void rmValue(char *array, int *length, char value){
    int i = 0;
    int position = -1;
    if (*length > 0) {
        for(i = 0; i < *length; i++) {
            if (array[i] == value) {
                position = i;
            }
        }
    }
    if (position > -1) {
        if(position+1 == *length) {
            --*length;
            return;
        }
        for(i = position; i < *length -1; i++) {
            array[i] = array[i+1];
            --*length;
        }
    }
}

char oppositeDirection(char d) {
    switch(d) {
        case ' ': return ' '; break; // No direction
        case 'N': return 'S'; break;
        case 'E': return 'W'; break;
        case 'S': return 'N'; break;
        case 'W': return 'E'; break;
    }
    return ' '; //default
}

void ghostAI(struct figure *figure) {
    char branches[4];
    int numOfBranches = 0;
    int d = 0;
    struct figure f1,f2;
    char map[] = "NESW";

    for(d = 0; d < 4; d++) {
        if (getTileInDirection(map[d], figure) != c_wall) {
            addValue(branches,&numOfBranches,map[d]);
        }
        if ((figure->direction != ' ') && (numOfBranches > 1))	{
            rmValue(branches,&numOfBranches,oppositeDirection(figure->direction));
        }
    }
    if (numOfBranches == 1) {
        figure->direction = branches[0];}
    else {
        f1 = *figure; f1.direction = branches[0];
        moveFigure(&f1);
        f2 = *figure; f2.direction = branches[1];
        moveFigure(&f2);
        if ( sqrt( pow(f1.x - player.x, 2) + pow(f1.y - player.y, 2)) >
            sqrt( pow(f2.x - player.x, 2) + pow(f2.y - player.y, 2)) ) {
            figure->direction =   branches[1];
        }
        else {
            figure->direction =   branches[0];
        }
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


void setnextDirection(struct figure *player) {
    if (player->nextDirection != ' ') {
        if (getTileInDirection(player->nextDirection,player) != c_wall) {
            player->direction = player->nextDirection;
            player->nextDirection = ' ';
        }
    }
}

void loadScore() {
	char buffer[1024] ;
	char *fld,*line;
	int col=0;

	scoreCount = 0;
  FILE *fp;
	fp = fopen("score.csv","r");
	if(fp == NULL)
    return;
	while (((line = fgets(buffer,sizeof(buffer),fp))!=NULL) && (scoreCount < (MAXSCORE-1))) {
		col = 0;
		fld = strtok(line,";");
		while(fld != NULL) {
			col++;
			switch(col) {
        case 1 : strcpy(scoretab[scoreCount].name,fld); break;
      	case 2 : scoretab[scoreCount].cookies = atoi(fld); break;
      	case 3 : scoretab[scoreCount].cakes = atoi(fld); break;
        case 4 : scoretab[scoreCount].time = atoi(fld); break;
        case 5 : scoretab[scoreCount].state = atoi(fld); break;
      }
			fld = strtok(NULL,";");
		}
    scoreCount++;
	}
	fclose(fp);
}

void saveScore() {
	FILE *fp;
	fp = fopen("score.csv","w");
	if(fp==NULL)
    return;
	if (scoreCount > MAXSCORE) {scoreCount = MAXSCORE;}
    for (int row=0; row<scoreCount; row++) {
		fprintf(fp,"%s;",scoretab[row].name);
		fprintf(fp,"%d;%d;%d;",scoretab[row].cookies, scoretab[row].cakes, scoretab[row].time);
    fprintf(fp,"%d\n",scoretab[row].state);
    }
  	fclose(fp);
}

int checkIfGameOver(int input) {
    if ((input == 'x') || (input == 'q')) {
        return 20;
    }
    if (cookies == 0) {
        return 1;
    }
    if (seconds >= MAXSECONDS) {
        return 6;
    }
    //GAME OVER routine
    if ((pinky.x == player.x) && (pinky.y == player.y)) {
        return 11;
    }
    if ((blinky.x == player.x) && (blinky.y == player.y)) {
        return 12;
    }
    if ((bashful.x == player.x) && (bashful.y == player.y)) {
        return 13;
    }
    return 0; // continue playing
}

void showScore (void) {
  mvwprintw(subWin,1, 2, "Player:\t%s", playerName);
  mvwprintw(subWin,2, 2, "Cookies left:\t%3d", cookies);
  mvwprintw(subWin,3, 2, "Cakes left:\t%3d", cakes);
  mvwprintw(subWin,4, 2, "Time:   \t%3d", MAXSECONDS-seconds);
  wrefresh(subWin);
}

void loadGame() {
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);   // Cookies
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);	// Packman / Player
  init_pair(3, COLOR_BLUE, COLOR_BLUE);     // Walls
  init_pair(4, COLOR_MAGENTA, COLOR_BLACK);	// Pinky
  init_pair(5, COLOR_RED, COLOR_BLACK);	    // Blinky
  init_pair(6, COLOR_CYAN, COLOR_BLACK);		// Bashful

  load_maze(); // Ititializes the size of the maze
  cookies = countNumOf(c_cookie);
  cakes = countNumOf(c_cake);
  seconds = 0;

  int starty = ((LINES - sizey) / 2);	// Calculating for a center placement
  int startx = ((COLS - sizex)  / 2); // of the gameWindow

  gameWin = newwin(sizey, sizex, starty, startx);	// opens canvas window in terminal

  timeout(0);

  // Player starting Position
  player.y = 17;
  player.x = 11;

  initGhosts(11,11,15);
  drawMaze();
}

int runGame() {
	int input;
	//open win for score
	subWin = newwin(5, 30, 0, 0);
	wbkgd(subWin, COLOR_PAIR(1));

	//Radom generator initialazation
	srand(time(NULL));

	// Timing / Ticks
	int ticks = 1;
	int state = 0; // 0 = playing

	do {
    input = getch();
    if (input != 255) {
    		setDirectionFromInput(input,&player);
  	}
    if ((ticks % 35) == 0) {
      // Blinky AI
      if (blinky.state == 0 ) {
        ghostAI(&blinky);
        moveFigure(&blinky);
      }
      else if ((blinky.state > 0) && (blinky.state <= 4)) {
        stupidAI(&blinky);
        moveFigure(&blinky);
      }

      // Pinky AI
      if (pinky.state == 0) {
        ghostAI(&pinky);
        moveFigure(&pinky);
      }
      else if ((pinky.state > 1) && (pinky.state <= 4)) {
        stupidAI(&pinky);
        moveFigure(&pinky);
      }

      // Bashful AI
      if (bashful.state == 0) {
        ghostAI(&bashful);
        moveFigure(&bashful);
      }
      else if ((bashful.state > 1) && (bashful.state <= 4)) {
        stupidAI(&bashful);
        moveFigure(&bashful);
      }
    }

    // Stats cooldown
    if (seconds < 90) {
      if ((ticks % 100 == 0) && (blinky.state > 0)) {
        blinky.state -= 1;
      }
      if ((ticks % 110 == 0) && (pinky.state > 0)) {
        pinky.state -= 1;
      }
      if ((ticks % 120 == 0) && (bashful.state > 0)) {
        bashful.state -= 1;
      }
    }

    if ((ticks % 16) == 0) {
      moveFigure(&player);
      eatCookie(&player);
      setnextDirection(&player);
      drawMaze();
      showScore();
    }
    if ((ticks % 110) == 0) {
      seconds += 1;
      showScore();
    }

    state = checkIfGameOver(input);
    usleep(7000);
    ticks++;
  } while (!state);
  timeout(-1); // resets the timeout of the getch();

  delwin(subWin);
  refresh();

  return state;
}

void showHighscore() {
  subWin = newwin(15, 32, 8, 24);
	box(subWin, 0, 0);
	wbkgd(subWin, COLOR_PAIR(1));
	int i;
	mvwprintw(subWin, 1, 2,"Player          Cookies Time");
	for (i=0; i<scoreCount; i++) {
		mvwprintw(subWin, i+3, 2,"%-19s %3d  %3d",scoretab[i].name, scoretab[i].cookies, scoretab[i].time);
	}
  wrefresh(subWin);
  timeout(20000);
  getch();
  timeout(-1);
  delwin(subWin);
}

void showGamescore(int state) {
	subWin = newwin(7, 32, 9, 23);
	box(subWin, 0, 0);
	wbkgd(subWin, COLOR_PAIR(1));
	if (state > 0) {
  	if (state == 1) {
      mvwprintw(subWin, 1,2,"******* YOU'VE WON! ********");}
  	else {
      mvwprintw(subWin, 1,2,"******* YOU'VE LOST ********");}
		switch (state) {
      case 1 : mvwprintw(subWin, 2,2,"=== CONGRATULATIONS! ==="); break;
   		case 6 : mvwprintw(subWin, 2,2,"======= TIME'S UP ======="); break;
      case 11 : mvwprintw(subWin, 2,2,"  === CAUGHT BY PINKY ==="); break;
      case 12 : mvwprintw(subWin, 2,2," === CAUGHT BY BLINKY ==="); break;
      case 13 : mvwprintw(subWin, 2,2,"  == CAUGHT BY BASHFUL =="); break;
      case 20 : mvwprintw(subWin, 2,2,"   QUIT: GAME TERMINATED   "); break;
    }
  }

	mvwprintw(subWin, 4, 2,"Player          Cookies Time");
	mvwprintw(subWin, 5, 2,"%-19s %3d  %3d",playerName, cookies, seconds);
  wrefresh(subWin);
  timeout(20000);
  getch();
  timeout(-1);
  delwin(subWin);
}

void addScore(int state) {
	strcpy(scoretab[scoreCount].name,playerName);
	scoretab[scoreCount].time = seconds;
	scoretab[scoreCount].cookies = cookies;
	scoretab[scoreCount].cakes = cakes;
	scoretab[scoreCount].state = state;
  if (scoreCount < MAXSCORE) {
    scoreCount++;
  }
}

void sortScore() {
  int i;  int j;
  struct score temp;

  for (i=0; i<scoreCount; i++) {
    for (j=i+1; j<scoreCount; j++){
      if ((scoretab[i].cookies > scoretab[j].cookies) ||
         ((scoretab[i].cookies == scoretab[j].cookies) &&
          (scoretab[i].time > scoretab[j].time))) {
            temp=scoretab[i];
            scoretab[i]=scoretab[j];
            scoretab[j]=temp;
      }
    }
  }
}

void getPlayerName() {
	subWin = newwin(3, 40, 11, 19);
	box(subWin, 0, 0);
	wbkgd(subWin, COLOR_PAIR(1));
  mvwprintw(subWin,1,1,"Player Name: [          ]");
  wrefresh(subWin);
  echo();
  curs_set(2);
  mvwgetnstr(subWin, 1, 15, playerName, 10);
  noecho();
  curs_set(0);
  delwin(subWin);
  clear();
  refresh();
}

int main(void) {
  initscr();
  clear();
  noecho();
  curs_set(0);
  raw();
  keypad(stdscr, 1);

  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  int choice;

  do {
    bkgd(COLOR_PAIR(2));
    mvaddstr(5,36, "PACMAN");
    mvaddstr(6,34, "for Console");
    mvhline(7, 30, ACS_HLINE, 20);
    refresh();

    menuWin = newwin(11, 28, 9, 25);
    wbkgd(menuWin, COLOR_PAIR(1));
    mvwaddstr(menuWin, 2, 3, "1) New Game");
    mvwaddstr(menuWin, 4, 3, "2) Highsccores");
    mvwaddstr(menuWin, 6, 3, "3) Change Player");
    mvwaddstr(menuWin, 8, 3, "q) Quit");
    box(menuWin, 0, 0);
    wrefresh(menuWin);

    choice = getch();
    switch(choice) {
      case '1': clear();
                refresh();
                loadGame();
                if (playerName[0] == '\0') {
                  getPlayerName();
                }
                int gameState = runGame();
                loadScore();
                addScore(gameState);
                sortScore();
                saveScore();
                showGamescore(gameState);
    		        delwin(gameWin);
    		        refresh();
                break;

      case '2': loadScore();
        	      showHighscore();
                break;

      case '3': getPlayerName();
                break;
    }
  } while(choice != 'q');
  delwin(menuWin);
  refresh();
  clear();
  endwin();
  return 0;
}
