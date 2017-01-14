# Planung ProzProg Projekt: Pacman

## a) Spezifikation

### Spielregeln
Ein Spieler kann das Objekt (Pacman) in vier Richtungen bewegen
(Nord – Süd – West - Ost). Ein Geist verfolgt Pacman, bis er ihn erreicht hat.
Während Pacman den Weg durchläuft, gewinnt er Punkte (Kekse), die als Score dargestellt werden.
Pacman kann sich nur in bestimmten Wegen bewegen. 
Wenn die Geister Pacman erreicht haben, endet das Spiel.

### Eingabe
Es kann ein alternatives Labyrinth über eine .txt Datei eingegeben werden.

Während des Spiels erfolgt die Eingabe über bestimmte Tastaturknöpfe:

* "w" Richtung Nord 
* "a" Richtung West 
* "s" Richtung Süd
* "d" Richtung Ost
* "x" Programm beenden

### Grafikausgabe
Das Programm erzeugt eine Grafik auf dem Bildschirm, in dem die Objekte (Wall, Pacman, Geister, Kekse) farbig abgebildet sind. Es gibt eine sich aktualisierende Anzeige, die die Punktzahl des Spielers anzeigt.

## b) Design

Um Dinge wie Fester, Farbe und Tastatureigabe zu händeln, wurde die ncurses Libary benutzt.

###  Ncurses Libary
**Aus Wikipedia:**
*ncurses (new curses) is a programming library providing an API that allows the programmer to write text-based user interfaces in a terminal-independent manner. It is a toolkit for developing "GUI-like" application software that runs under a terminal emulator.*

### Funktionalitäten
* Menü

* Eine Karte bzw. ein Labyrinth aus einer .txt Datei impotieren. (TODO) 

* Orginales Farbschema und volle blöcke für Labyrinth. (ncruses)

* Postionen und Richtungen des Spieler und der Geister als Struktur übergeben. (TODO Richtung)

* Geister haben eine KI und verfolgen den Spieler (TODO).

* Autentisches Steuerkozept, wie im orginalen Pacman (merkt sich Richtung und biegt bei der nächstmöglichen Kreuzung ab).

* Kekse essen und Punkte zählen.

* Highscore Liste, sort (TODO).

### Wichtige Funktionen

**1. Geist-KI**
Der rote Geist soll, wenn er an einer Kreuzung (Krezungs-Erkennungsfunktion) ist, die Distanz ( sqr((x2−x1)^2 + (y2−y1)^2 ) ) zum Spieler im Falle des Abbiegens in einer der Richtungen vergleichen. Biegt in die Richtung mit der kürzesten Distanz ab.
Falls noch viel Zeit verbleibt, bekommen einzelne Geister verschieder Charaktäre.
Siehe: <http://gameinternals.com/post/2072558330/understanding-pac-man-ghost-behavior>

**2. Bewegung**
Sobald *direction* vernünftig in der *position*-Strukur eingeabreitet ist, wird der Parameter direction entfallen.
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

**3. Den Status einer Kachel in einer Richtung relativ zum Charakter bekommen**
  char getTileInDirection (int sender, char d) {
		switch(d) {
			// No direction
			case ' ': return ' ';
								break;
			// North - 0 is Pacman, 1 is blinky. Should probably not be a bool though.
			case 'N': if (! sender) { 
									return maze[player.y-1][player.x];
								} else {
									return maze[blinky.y-1][blinky.x];
								}
								break;
			// East
			case 'E': if (! sender) {
										return maze[player.y][player.x+1];
									} else {
										return maze[blinky.y][blinky.x+1];
									}
									break;
			// South
			case 'S': if (! sender) {
									return maze[player.y+1][player.x];
								} else {
									return maze[blinky.y+1][blinky.x];
								}
								break;
			// West
			case 'W':	if (! sender) {
									return maze[player.y][player.x-1];
								} else {
									return maze[blinky.y][blinky.x-1];
								}
								break;
			// Failsafe
			default:	  wprintw(win,"Invalid [%c]",d);
								break;
		}
	}

### Strukturierte Datentypen
	struct position {		// Entity with coodrinates (Y, X)
		int y;
		int x;
	}player, blinky;
