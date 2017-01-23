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

#define MAXMAZE 1024

WINDOW *menuWin;
WINDOW *gameWin;
WINDOW *highscoreWin;

const char c_wall	= 	'#';
const char c_space	= ' ';
const char c_player	= '@';
const char c_ghost	= '%';
const char c_cookie	= '.';

const char c_border	= 'B';
const char c_cake	= 	'o';

struct figure {		// Player / ghosts with coodrinates (Y, X), and direction
    int y;
    int x;
    char direction;
    char nextDirection;
    int  state;  // sec || 0=normal / agressive, >4 stupid, >10still, >20frightend
} player, blinky, pinky, bashful;

char maze[MAXMAZE];
int sizex=0, sizey = 0;

int cookies = 0;
int seconds = 0;



char maze_get(int x, int y) { // x=0..sizex-1,
    if (x<0 || y<0 || x>=sizex || y>=sizey) {
        return c_border;
    }
    return maze[x + y*sizex];
}

void maze_put(int x, int y, char c) {
    maze[x + y*sizex] = c;
}

int countNumOfCookies() {
    int i;
    int numOfCookies = 0;
    
    for(i = 0; i < MAXMAZE; i++) {
        if (maze[i] == '.') {
            numOfCookies++;
        }
    }
    return numOfCookies;
}

void load_maze() {
    sizey = 0;
    sizex = 23;
    
    strcpy(maze,"#######################"); sizey++;
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
    strcat(maze,"     .  #     #  .     ");	sizey++;
    strcat(maze,"#####.#.#######.#.#####");	sizey++;
    strcat(maze,"    #.#         #.#    ");	sizey++;
    strcat(maze,"#####.# ####### #.#####");	sizey++;
    strcat(maze,"#..........#..........#");	sizey++;
    strcat(maze,"#.###.####.#.####.###.#");	sizey++;
    strcat(maze,"#o..#.............#..o#");	sizey++;
    strcat(maze,"###.#.#.#######.#.#.###");	sizey++;
    strcat(maze,"#.....#....#....#.....#");	sizey++;
    strcat(maze,"#.########.#.########.#");	sizey++;
    strcat(maze,"#.....................#");	sizey++;
    strcat(maze,"#######################");	sizey++;
    
}

void drawMaze(void) {
    
    int x, y;
    werase(gameWin);
    for(y = 0; y < sizey; y++) {
        for(x = 0; x < sizex; x++) {
            
            //Coloring the ghosts
            if ( (x == pinky.x) && (y == pinky.y) ) {
                wattron(gameWin,COLOR_PAIR(4));
                if (pinky.state > 0) {
                    wattron(gameWin,A_BLINK);
                }
                wprintw(gameWin,"%c", c_ghost);
                if (pinky.state > 0) {
                    wattroff(gameWin,A_BLINK);
                }
                wattroff(gameWin,COLOR_PAIR(4));
            }
            else if ( (x == blinky.x) && (y == blinky.y) ) {
                wattron(gameWin,COLOR_PAIR(5));
                if (blinky.state > 0) {
                    wattron(gameWin,A_BLINK);
                }
                wprintw(gameWin,"%c", c_ghost);
                if (blinky.state > 0) {
                    wattroff(gameWin,A_BLINK);
                }
                wattroff(gameWin,COLOR_PAIR(5));
            }
            else if ( (x == bashful.x) && (y == bashful.y) ) {
                wattron(gameWin,COLOR_PAIR(6));
                if (blinky.state > 0) {
                    wattron(gameWin,A_BLINK);
                }
                wprintw(gameWin,"%c", c_ghost);
                if (blinky.state > 0) {
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

void eatCookie(struct figure *player) {
    if (maze_get(player->x, player->y) == c_cake) {
        cookies -= 1;
        
        blinky.state += 12;
        pinky.state += 16;
        
        pinky.y = 11;
        pinky.x = 10;
        
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

void printScore (void) {
    mvwprintw(gameWin,2,2,"Cookies left:\t%4d", cookies);
    mvwprintw(gameWin,3,2,"Time:\t\t%4d", seconds);
    refresh();
}

int checkIfGameOver(int input) {
    if ((input == 'x') || (input == 'q')) {
        return 20;
    }
    if (cookies == 0) {
        return 1;
    }
    if (seconds < 0) {
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

void savePoints(){    // saves Count of Cookies each time and save them in text file
    
    FILE* data = fopen("Scores.txt","a");
    
    fprintf(data,"%d\t",cookies);
    
    fclose(data);
}

void bringScores(){   // brings the Score from text file
    bkgd(COLOR_PAIR(1));
    FILE *data = fopen("Scores.txt", "r");
    
    //read file into array
    int* numberArray = (int*)malloc(10*sizeof(int));
   
    int i=0;
    do{
        bkgd(COLOR_PAIR(1));
        fscanf(data, "%d", &numberArray[i]);
        bkgd(COLOR_PAIR(1));
        printw("\n%d.  try: %d",i+1, numberArray[i]);
        i++;
    }while(numberArray[i]!='\0');
    free(numberArray);
}

int runGame(char playerName[]) {
    int input;
    
    //Radom generator initialazation
    srand(time(NULL));
    
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);		// Cookies
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);	// Packman / Player
    init_pair(3, COLOR_BLUE, COLOR_BLUE);		// Walls
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);	// Pinky
    init_pair(5, COLOR_RED, COLOR_BLACK);		// Blinky
    init_pair(6, COLOR_CYAN, COLOR_BLACK);		// Bashful
    
    load_maze(); // Ititializes the size of the maze
    cookies = countNumOfCookies();
    seconds = 100;
    
    int starty = ((LINES - sizey) / 2);	// Calculating for a center placement
    int startx = ((COLS - sizex)  / 2); // of the gameWindow
    
    refresh();
    
    gameWin = newwin(sizey, sizex, starty, startx);	// opens canvas window in terminal
    timeout(0);
    
    // Player starting Position
    player.y = 17;
    player.x = 11;
    
    // Ghosts starting Position
    blinky.y = 11;
    blinky.x = 11;
    blinky.state = 12;
    blinky.direction = ' ';
    
    pinky.y = 11;
    pinky.x = 9;
    pinky.direction = ' ';
    pinky.state = 16;
    
    bashful.y = 11;
    bashful.x = 13;
    bashful.direction = ' ';
    bashful.state = 19;
    
    // Timing / Ticks
    int ticks = 1;
    
    int gameState = 0; // 0 = playing
    drawMaze();
    
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
        if ((ticks % 100 == 0) && (blinky.state > 0)) {
            blinky.state -= 1;
        }
        if ((ticks % 100 == 0) && (pinky.state > 0)) {
            pinky.state -= 1;
        }
        if ((ticks % 100 == 0) && (bashful.state > 0)) {
            bashful.state -= 1;
        }
        
        if ((ticks % 16) == 0) {
            moveFigure(&player);
            eatCookie(&player);
            setnextDirection(&player);
            
            printScore();
            drawMaze();
        }
        if ((ticks % 110) == 0) {
            seconds -= 1;
            printScore();
        }
        
        gameState = checkIfGameOver(input);
        usleep(7000);
        ticks++;
    } while (!gameState);
    
    if (gameState == 1) {
        mvwprintw(gameWin,3,23,"***** - CONGRATULATIONS! - *****");
        mvwprintw(gameWin,4,23,"======  - YOU'VE WON! -  =======");
    }
    else if (gameState == 6) {
        mvwprintw(gameWin,3,23,"      YOU LOST - TIME'S UP      ");
    }
    else if (gameState == 11) {
        mvwprintw(gameWin,3,23,"********* YOU'VE LOST **********");
        mvwprintw(gameWin,4,23,"======  CAUGTH BY PINKY  =======");
    }
    else if (gameState == 12) {
        mvwprintw(gameWin,3,23,"********* YOU'VE LOST **********");
        mvwprintw(gameWin,4,23,"======  CAUGTH BY BLINKY  ======");
    }
    else if (gameState == 13) {
        mvwprintw(gameWin,3,23,"********* YOU'VE LOST **********");
        mvwprintw(gameWin,4,23,"=====  CAUGTH BY BASHFUL  ======");
    }
    else if (gameState == 20) {
        mvwprintw(gameWin,3,23,"      QUIT: GAME TERMINATED     ");
    }
    refresh();
    delwin(gameWin);
    timeout(-1);
    savePoints();
    return cookies;
}

void quit(void) {
    delwin(menuWin);
    endwin();
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
        menuWin = newwin(11, 28, 9, 25);
        bkgd(COLOR_PAIR(2));
        wbkgd(menuWin, COLOR_PAIR(1));
        
        
        mvaddstr(5,33, "PACMAN");
        mvaddstr(6,35, "for Console");
        mvwaddstr(menuWin, 3, 3, "1) New Game");
        mvwaddstr(menuWin, 5, 3, "2) Highsccores");
        mvwaddstr(menuWin, 7, 3, "3) Quit");
        
        
        box(menuWin, 0, 0);
        mvhline(7, 30, ACS_HLINE, 20);
        
        refresh();
        wrefresh(menuWin);
        
        choice = getch();
        
        switch(choice) {
            case '1': clear ();
                FILE* data = fopen("Scores.txt","a");
                bkgd(COLOR_PAIR(1));
                echo();
                char mesg[]="Player Name: ";
                char playerName[80];
                int row,col;
                
                getmaxyx(stdscr,row,col);
                mvprintw(row/2,(col-strlen(mesg))/2,"%s",mesg);
                
                getstr(playerName);
               // fprintf(data,"%s",playerName);
                
                mvprintw(row/2+2, (col-26-strlen(mesg)), "Welcome %s.", playerName);
                
                usleep(50000);
                clear();
                noecho();
                
                runGame(playerName);
                delwin(gameWin);
                
                break;
                
            case '2': highscoreWin = newwin(11, 28, 9, 25);
                //wbkgd(highscoreWin, COLOR_PAIR(1));
                
                // mvwaddstr(highscoreWin, 1, 3, "HIGHSCORES");
               // mvwaddnstr(highscoreWin, 5, 3, "%d", numm );
                
                box(highscoreWin, 0, 0);
                
                
                
                bringScores();
                
               
        }
    }while(choice != '3');
    quit();
    return 0;
}
