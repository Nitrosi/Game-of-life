/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    (FR) : Bonjour, tout d'abord, merci d'avoir pris le temps de télécharger mon jeu. Celui-ci a été réalisé en quelques jours sur mon temps libre grâce à la 
    librairie Ncurses. Pour vous déplacer, utilisez les flèches directionnelles. Pour changer l'état d'une cellule, appuyez sur espace, pour lancer la simulation,
    appuyez sur p et pour accélérer ou ralentir celle-ci, appuyez sur Z ainsi que S. 
    Si vous avez des propositions pour améliorer mon code, contactez-moi sur mon GitHub je suis ouvert à tout. 
    Ce code est complètement open source et vous pouvez en faire ce que vous voulez. 

*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ncurses.h>
#include<stdio.h> 
#include<stdlib.h>
#include <time.h>
#include <omp.h>

void afficherjeu(WINDOW *game_win, WINDOW *Above_Window,int h, int l, unsigned char **tableaudonne, int statue, int curseurL, int curseurH, int valeuràcompter, int credit, int game_size_H, int game_size_L, int window_pos_H, int window_pos_L){
    
    int i,j;
    
    if(credit == 0){
		wattron(Above_Window, COLOR_PAIR(3));
		mvwprintw(Above_Window, 0, (l*2-15)/2, "# Jeu de la Vie #");
		wattroff(Above_Window, COLOR_PAIR(3));
	}
	else{
		box(Above_Window, 0, 0);
		wattron(Above_Window, COLOR_PAIR(3));
		mvwprintw(Above_Window, 0, (l*2-14)/2, "# By Nitrosi #");
		wattroff(Above_Window, COLOR_PAIR(3));
	}
    
    mvwprintw(Above_Window, 1, 1, "Statut ('p') :");

    mvwprintw(Above_Window, 2, 1, "Accélérer/Ralentir : 'Z' / 'S'");
    //mvwprintw(Above_Window, 2, 1, "curseur : H = %d L = %d   " , curseurH , curseurL);
    
    mvwprintw(Above_Window, 3, 1, "Vitesse : %d/10 ", valeuràcompter);
    //mvwprintw(Above_Window, 3, 1, "pos curseur : H = %d L = %d   ", window_pos_H + curseurH , window_pos_L + curseurL);
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
            if(tableaudonne[i + window_pos_H][j + window_pos_L] == 0){
                wattron(game_win, COLOR_PAIR(1));
                mvwprintw(game_win, i+1, j * 2 + 1,"  ");
                wattroff(game_win, COLOR_PAIR(1));
            }
            else{
                wattron(game_win, COLOR_PAIR(2));
                mvwprintw(game_win, i+1, j * 2 + 1,"[]");
                wattroff(game_win, COLOR_PAIR(2));
                }
        }
    }
    
    
    if(tableaudonne[window_pos_H + curseurH ][window_pos_L + curseurL] == 0){
        wattron(game_win, COLOR_PAIR(4));
        mvwprintw(game_win, curseurH + 1,curseurL * 2 +1,"()");
        wattroff(game_win, COLOR_PAIR(4));
    }
    else{
        wattron(game_win, COLOR_PAIR(5));
        mvwprintw(game_win, curseurH + 1,curseurL * 2 +1,"()");
        wattroff(game_win, COLOR_PAIR(5));
    }
        
    
    
    wrefresh(game_win);
    wrefresh(Above_Window);
    
    
}

int valeurtableauS(int h, int l, unsigned char** hauteur, int Htest, int Ltest){
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

void Av(int game_size_H, int game_size_L, unsigned char** hauteur , unsigned char** tableautemp){
    #pragma omp parallel 
    for(int i=0;i<game_size_H;++i){
        for(int j=0;j<game_size_L;++j){
            int count = valeurtableauS(game_size_H,game_size_L,hauteur,i-1,j) + valeurtableauS(game_size_H,game_size_L,hauteur,i-1,j-1) + valeurtableauS(game_size_H,game_size_L,hauteur,i-1,j+1) + valeurtableauS(game_size_H,game_size_L,hauteur,i,j-1)+valeurtableauS(game_size_H,game_size_L,hauteur,i,j+1)+valeurtableauS(game_size_H,game_size_L,hauteur,i+1,j) + valeurtableauS(game_size_H,game_size_L,hauteur,i+1,j-1) + valeurtableauS(game_size_H,game_size_L,hauteur,i+1,j+1);
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
    
}

int main() {
    int l = 35;   //largeur de la fenêtre du jeu
    int h = 35;      //hauteur de la fenêtre du jeu
    
    int i;   //variable utilisée dans toutes les boucles for 
    int ch;   //variable qui stocke la touche pressée
    
    int statue = 0;   //variable booléenne qui décrit si le jeu simule ou pas
    int credit = 0;      //variable booléenne qui décrit si la ligne des crédits est affichée
    
    int game_size_L = 1000;   //largeur globale du jeu (en largeur)
    int game_size_H = 1000;   //largeur globale du jeu (en hauteur)
    
    int curseurL = 17;   //position du curseur sur la fenêtre (en hauteur)
    int curseurH = 17;   //position du curseur sur la fenêtre (en largeur)
    
    int window_pos_L = 500;   //position du pixel en haut à gauche de l'écran (en largeur)
    int window_pos_H = 500;   //position du pixel en haut à gauche de l'écran (en hauteur)
    
    int compteurralentiseur = 0;   //compteur qui permet de ralentir le jeu
    int valeuràcompter = 5;   //valeur que le compteur doit atteindre pour mettre à jour la simulation
    
    struct timespec delay;
    delay.tv_sec = 0;          
    delay.tv_nsec = 10000000;
    
    unsigned char **tableaudonne;   //tableau qui stocke les données du jeu
    unsigned char **tableautemp;   //tableau temporaire utilisé pour la simulation
    unsigned char **swap_ptr;
    int conditionarret = 0 ;   //variable qui arrête la boucle while si elle est égale à 1


    // Initialisation du jeu
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    
    
    // Initialisation des couleurs
    
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_RED);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
    clear();
    refresh();
  
    // Initialise le tableau de données qui stocke les données du jeu
    
    tableaudonne = calloc(game_size_H, sizeof(unsigned char*));
    for(i=0;i<game_size_H;++i){
        tableaudonne[i] = calloc(game_size_L,sizeof(unsigned char));
    }
    
    
    tableautemp = calloc(game_size_H, sizeof(unsigned char*));
    for(i=0;i<game_size_H;++i){
        tableautemp[i] = calloc(game_size_L,sizeof(unsigned char));
    }
    
    // Affichage du jeu 
    
    WINDOW *game_win  = newwin(h+2, l*2+2 , 7, 2);
    WINDOW *Above_Window  = newwin(5,l*2+2, 2, 2);
    
    box(game_win, 0, 0);
    box(Above_Window, 0, 0);

    // Boucle infinie qui exécute le jeu

    while(conditionarret == 0) {

        afficherjeu(game_win, Above_Window,h,l,tableaudonne,statue,curseurL, curseurH,valeuràcompter, credit, game_size_H, game_size_L, window_pos_H, window_pos_L);
        nanosleep(&delay, NULL);
        refresh();
        
        if(statue == 1){
            if(compteurralentiseur >= (10-valeuràcompter)*4){
                    compteurralentiseur = 0;
                    Av(game_size_H,game_size_L,tableaudonne,tableautemp);
                    
                    swap_ptr = tableaudonne;
                    tableaudonne = tableautemp;
                    tableautemp = swap_ptr;
                    
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
                if(curseurH > 1){
                    curseurH--;
                }
                else if(curseurH == 1 && window_pos_H > 0){
                    window_pos_H--;
                }
                else if(curseurH == 1 && window_pos_H == 0){
                    curseurH--;
                }
                break;
             case KEY_DOWN:
                if(curseurH < h-2){
                    curseurH++;
                }
                else if(curseurH == h-2 && window_pos_H < game_size_H - h){
                    window_pos_H++;
                }
                else if(curseurH == h-2 && window_pos_H == game_size_H - h ){
                    curseurH++;
                }
                break;
             case KEY_LEFT:
                if(curseurL > 1){
                    curseurL--;
                }
                else if(curseurL == 1 && window_pos_L > 0){
                    window_pos_L--;
                }
                else if(curseurL == 1 && window_pos_L == 0){
                    curseurL--;
                }
                break;
             case KEY_RIGHT:
                if(curseurL < l-2){
                    curseurL++;
                }
                else if(curseurL == l-2 && window_pos_L < game_size_L - l){
                    window_pos_L++;
                }
                else if(curseurL == l-2 && window_pos_L == game_size_L - l){
                    curseurL++;
                }
                break;
             case ' ':
                if(tableaudonne[window_pos_H + curseurH][window_pos_L + curseurL] == 0){
                    tableaudonne[window_pos_H + curseurH][window_pos_L + curseurL] = 1;
                }
                else if(tableaudonne[window_pos_H + curseurH][window_pos_L + curseurL] == 1){
                    tableaudonne[window_pos_H + curseurH][window_pos_L + curseurL] = 0;
                }
                break;
             case 'z':
                if(valeuràcompter < 10){
                    valeuràcompter++;
                }
                break;
             case 's':
                if(valeuràcompter > 1){
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
    for(i=0; i<game_size_H; ++i){
        free(tableaudonne[i]);
    }
    free(tableaudonne);
    for(i=0; i<game_size_H; ++i){
        free(tableautemp[i]);
    }
    free(tableautemp);

    getch();
    endwin();
    return 0;
}
