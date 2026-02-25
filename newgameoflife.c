#include <ncurses.h>
#include<stdio.h> 
#include<stdlib.h>
#include <time.h>

void afficherjeu(WINDOW *game_win, WINDOW *Above_Window,int h, int l, int **tableaudonne, int statue, int curseurX, int curseurY, int valeuràcompter, int credit){
	
	int i,j;
	
	werase(game_win);
    werase(Above_Window);
	
    box(game_win, 0, 0);
    box(Above_Window, 0, 0);
    
    if(credit == 0){
		wattron(Above_Window, COLOR_PAIR(3));
		mvwprintw(Above_Window, 0, (l*2-15)/2, "# Jeu de la Vie #");
		wattroff(Above_Window, COLOR_PAIR(3));
	}
	else{
		wattron(Above_Window, COLOR_PAIR(3));
		mvwprintw(Above_Window, 0, (l*2-14)/2, "# By Nitrosi #");
		wattroff(Above_Window, COLOR_PAIR(3));
	}
    
    mvwprintw(Above_Window, 1, 1, "Statue ('p') :");

    mvwprintw(Above_Window, 2, 1, "Accélérer/Ralentir : 'Z' / 'S'");
    mvwprintw(Above_Window, 3, 1, "Vitesse : %d/20", (20 - valeuràcompter));
    mvwprintw(Above_Window, 1, (l*2-10), "Exit : 'e'");
    
    if(statue == 0){
		wattron(Above_Window, COLOR_PAIR(4));
		mvwprintw(Above_Window, 1, 15, "EN PAUSE");
		wattroff(Above_Window, COLOR_PAIR(4));
	}
	else{
		wattron(Above_Window, COLOR_PAIR(3));
		mvwprintw(Above_Window, 1, 15, "EN COURS");
		wattroff(Above_Window, COLOR_PAIR(3));
	} 
    
    for(i=0;i<h;i++){
		for(j=0;j<l;j++){
			if(tableaudonne[i][j] == 0){
				wattron(game_win, COLOR_PAIR(1));
				mvwprintw(game_win, i + 1, j * 2 + 1,"  ");
				wattroff(game_win, COLOR_PAIR(1));
			}
			else{
				wattron(game_win, COLOR_PAIR(2));
				mvwprintw(game_win, i + 1, j * 2 + 1,"[]");
				wattroff(game_win, COLOR_PAIR(2));
				}
		}
	}
	
	
	if(tableaudonne[curseurY-1][curseurX/2] == 0){
		wattron(game_win, COLOR_PAIR(4));
		mvwprintw(game_win, curseurY,curseurX,"()");
		wattroff(game_win, COLOR_PAIR(4));
	}
	else{
		wattron(game_win, COLOR_PAIR(5));
		mvwprintw(game_win, curseurY,curseurX,"()");
		wattroff(game_win, COLOR_PAIR(5));
	}
		
	
	
	wrefresh(game_win);
    wrefresh(Above_Window);
    
    
}

int valeurtableauS(int h, int l, int** hauteur, int Htest, int Ltest){
	if(Htest<0){
		return 0;
	}
	if(Ltest<0){
		return 0;
	}
	if(Htest>=h){
		return 0;
	}
	if(Ltest>=l){
		return 0;
	}
	if(hauteur[Htest][Ltest] == 0){
		return 0;
	}
	else{
		return 1;
	}
}

void Av(int h, int l, int** hauteur){
	int i;
	int j;
	int count = 0;
	int** tableautemp;
	
	
	tableautemp = calloc(h, sizeof(int*));
    for(i=0;i<h;++i){
		tableautemp[i] = calloc(l,sizeof(int));
	}
	
	for(i=0;i<h;++i){
		for(j=0;j<l;++j){
			count = valeurtableauS(h,l,hauteur,i-1,j) + valeurtableauS(h,l,hauteur,i-1,j-1) + valeurtableauS(h,l,hauteur,i-1,j+1) + valeurtableauS(h,l,hauteur,i,j-1)+valeurtableauS(h,l,hauteur,i,j+1)+valeurtableauS(h,l,hauteur,i+1,j) + valeurtableauS(h,l,hauteur,i+1,j-1) + valeurtableauS(h,l,hauteur,i+1,j+1);
			if(hauteur[i][j] == 0 && count == 3){
				tableautemp[i][j] = 1;
			}
			else if(hauteur[i][j] == 1 && (count == 2 || count == 3)){
                tableautemp[i][j] = 1;
            }
            else {
                tableautemp[i][j] = 0;
            }
		}
	}
	
	for(i=0;i<h;++i){
		for(j=0;j<l;++j){
			hauteur[i][j] = tableautemp[i][j];
		}
	}
	for(i=0; i<h; ++i){
        free(tableautemp[i]);
    }
    free(tableautemp);
}

int main() {
	int l = -1;
	int h = -1;
	int i;
	int ch;
	int statue = 0;
	int credit = 0;
	char input_l[3];
	char input_h[3];
	
	char buffer[3];
	
	int premierpassageini = 0;
	
	int curseurX = 1;
	int curseurY = 1;
	
	int compteurralentiseur = 0;
	int valeuràcompter = 10;
	
	struct timespec delay;
    delay.tv_sec = 0;          
    delay.tv_nsec = 40000000;
	
	int **tableaudonne;
	int conditionarret = 0 ;
	int commande = 0;

	// Initialisation du jeu
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);
    
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_RED);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
    clear();
    refresh();
    initscr();
    while(l < 20 || h < 10 || h>=LINES || l >= COLS){
		
		WINDOW *Ini  = newwin(6,20*2, 2, 2);
		box(Ini, 0, 0);
		
		if(premierpassageini == 1){
			if(l < 20 || h < 10){
				wattron(Ini, COLOR_PAIR(4));
				mvwprintw(Ini, 1, 5, "Veuiller Respecter Taille Mini");
				wattroff(Ini, COLOR_PAIR(4));
			}
			else{
				wattron(Ini, COLOR_PAIR(4));
				mvwprintw(Ini, 1, 5, "Veuiller agrandir le terminal");
				wattroff(Ini, COLOR_PAIR(4));
			}
		}
		
		wattron(Ini, COLOR_PAIR(3));
		mvwprintw(Ini, 0, 11, "# Jeu de la Vie #");
		wattroff(Ini, COLOR_PAIR(3));
		mvwprintw(Ini, 2, 1, "Initialisation du jeu :");
		mvwprintw(Ini, 3, 1, "Hauteur (Minimum 10) : ");
		echo();           
		curs_set(1);      
		mvwgetnstr(Ini, 3, 24,buffer,2 ); 
		h = atoi(buffer); 
		mvwprintw(Ini, 4, 1, "Largeur (Minmum 20) : ");
		mvwgetnstr(Ini, 4, 23,buffer,2 );
		l = atoi(buffer);
		initscr();
			
		
		premierpassageini = 1;
		
	}
	
	
	
	noecho();         
	curs_set(0);
    refresh();
    nodelay(stdscr, TRUE);
  
	tableaudonne = calloc(h, sizeof(int*));
    for(i=0;i<h;++i){
		tableaudonne[i] = calloc(l,sizeof(int));
	}
	
	// Affichage du jeu 
	
	WINDOW *game_win  = newwin(h+2, l*2+2 , 7, 2);
    WINDOW *Above_Window  = newwin(5,l*2+2, 2, 2);

	while(conditionarret == 0) {
		clear();
		afficherjeu(game_win, Above_Window,h,l,tableaudonne,statue,curseurX, curseurY,valeuràcompter, credit);
		nanosleep(&delay, NULL);
		refresh();
		
		if(statue == 1){
			if(compteurralentiseur >= valeuràcompter){
					compteurralentiseur = 0;
					Av(h,l,tableaudonne);
				}
			else{
				compteurralentiseur++;
			}
			}
		
		ch = getch();
		switch(ch) {
            case 'p':
                if(statue == 0){
					statue = 1;
				}
				else{
					statue = 0;
				}
                break;
             case KEY_UP:
                if(curseurY > 1){
					curseurY = curseurY-1;
				}
                break;
             case KEY_DOWN:
                if(curseurY < h){
					curseurY = curseurY+1;
				}
                break;
             case KEY_LEFT:
                if(curseurX > 2){
					curseurX = curseurX-2;
				}
                break;
             case KEY_RIGHT:
                if(curseurX < l*2-1){
					curseurX = curseurX+2;
				}
                break;
             case ' ':
                if(tableaudonne[curseurY-1][curseurX/2] == 0){
					tableaudonne[curseurY-1][curseurX/2] = 1;
				}
				else if(tableaudonne[curseurY-1][curseurX/2] == 1){
					tableaudonne[curseurY-1][curseurX/2] = 0;
				}
                break;
             case 's':
                if(valeuràcompter < 20){
					valeuràcompter++;
				}
                break;
             case 'z':
                if(valeuràcompter > 0){
					valeuràcompter--;
				}
                break;
             case 'e':
                conditionarret = 1;
                break;
             case 'c':
                if(credit == 0){
					credit = 1;
				}
				else{
					credit = 0;
				}
                break;
        }
	}
   
	
    getch();
    endwin();
    return 0;
}
