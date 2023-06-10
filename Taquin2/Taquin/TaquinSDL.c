#include "TaquinSDL.h"
#include "AStar.h"
#include <time.h>
#include "SDL_gfxprimitives.h"
#include "Taquin.h"
#include "SDL_ttf.h"


// Fonction permettant de cr�er le taquin en SDL � partir de sa taille et du fichier BMP � utiliser
// Cette fonction cr�e aussi la fen�tre SDL et la stocke dans la structure TaquinSDL
int createTaquinSDL(TaquinSDL * pTaquinSDL,int hauteur, int largeur, char * pathBMPfile)
{
	// Test pour v�rifier que les donn�es pass�es ne sont pas corrompues
	if(!pTaquinSDL) return 0;

	// initialisation de la fen�tre SDL
	pTaquinSDL->pWindow = NULL;
	// initialisation de l'image � NULL
	pTaquinSDL->pFond = NULL;
	// On cr�e le taquin qui sera utilis� pour jouer
	pTaquinSDL->taquin.plateau = NULL;
	createTaquin(&(pTaquinSDL->taquin),largeur,hauteur);

	// On met � jour la taille du taquin
	largeur = pTaquinSDL->taquin.largeur;
	hauteur = pTaquinSDL->taquin.hauteur;

	// On initialise la SDL
	// On initialise non seulement le mode vid�o mais aussi la gestion du temps pour pouvoir utiliser SDL_GetTicks()
	if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ))
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}

	if (!IMG_Init(IMG_INIT_PNG))
	{
		printf("[-] ERROR - Failed to initialise SDL_Image (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	if (TTF_Init()) {
		printf("Erreur de ttf (%s)\n", TTF_GetError);
		return EXIT_FAILURE;
	}

	// On charge le BMP qui servira de fond au taquin
	pTaquinSDL->pFond= SDL_LoadBMP(pathBMPfile);
	if(!pTaquinSDL->pFond) 
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}

	// Initialisation de la fen�tre

	// On r�cup�re la taille d'une case
	pTaquinSDL->resX = pTaquinSDL->pFond->w/largeur;
	pTaquinSDL->resY = pTaquinSDL->pFond->h/hauteur;

	// On cr�e la fen�tre en fonction de la r�solution de l'image d'entr�e
	pTaquinSDL->pWindow = SDL_SetVideoMode(pTaquinSDL->resX*largeur, pTaquinSDL->resY*hauteur, 32, SDL_DOUBLEBUF|SDL_HWSURFACE);
	if(!pTaquinSDL->pWindow)
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}


	SDL_WM_SetCaption("Taquin","Taquin" );

	SDL_WM_SetIcon(NULL,NULL);

	return 1;
	
}


int displayCaseTaquin(TaquinSDL * pTaquinSDL,unsigned char caseTaquin, SDL_Rect * pDest, int x, int y, int refresh)
{
	// TODO: displayCaseTaquin
	// on d�finit o� on veut la dessiner
	int i = 0;
	int j = 0;
	

	while (caseTaquin != (pTaquinSDL->taquin).plateau[i][j] ) {
		if ((pTaquinSDL->taquin).largeur - 1 == j) {
			j = 0;
			i++;
		}
		else
			j++;
	}
	if (caseTaquin == (pTaquinSDL->taquin).plateau[i][j] && refresh==1) {
		int tmp = i;
		i= (pTaquinSDL->taquin).plateau[i][j] / pTaquinSDL->taquin.largeur;
		j = (pTaquinSDL->taquin).plateau[tmp][j] % pTaquinSDL->taquin.largeur;
	}
	
	
	// Si la case n'est pas vide ...
	// on calcule o� est la case "caseTaquin" dans l'image initiale pour -par la suite - venir d�couper la zone qui correspond � la case

	SDL_Rect* a = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));
	a->x = x;
	a->y = y;
	a->w = (pTaquinSDL->pFond->w / (pTaquinSDL->taquin).largeur) ;    
	a->h = (pTaquinSDL->pFond->h / (pTaquinSDL->taquin).hauteur);

	SDL_Rect* r = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));
	r->x = pTaquinSDL->resX * j;
	r->y = pTaquinSDL->resX * i;
	r->w = (pTaquinSDL->pFond->w / (pTaquinSDL->taquin).largeur);   
	r->h = (pTaquinSDL->pFond->h / (pTaquinSDL->taquin).hauteur);

	if (caseTaquin == 0 && refresh == 1) {
		SDL_Surface* s = SDL_CreateRGBSurface(0, a->w, a->h,32, 0, 0, 0, 0);
		SDL_FillRect(pTaquinSDL->pWindow,a, SDL_MapRGB(s->format, 0, 0, 0));
		return 0;
	}

	
	SDL_BlitSurface(pTaquinSDL->pFond, r, pTaquinSDL->pWindow, a);

	if (refresh == 0) {
		
		SDL_Surface* s = IMG_Load("./adrien.bmp");
		if (!s)
		{
			printf("Erreur de chargement de l'image : %s", SDL_GetError());
			return -1;
		}
		SDL_Rect* screct = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));
		screct->x = 0;
		screct->y = 0;
		screct->w = 400;
		screct->h = 400;

		SDL_Rect* dsrect = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));
		dsrect->x = 250;
		dsrect->y = 250;
		dsrect->w = 150;
		dsrect->h = 150;
		SDL_BlitSurface(s, screct, pTaquinSDL->pWindow, dsrect);
		SDL_FreeSurface(s);

		s = IMG_Load("./couronnee.png");
		if (!s)
		{
			printf("Erreur de chargement de l'image : %s", SDL_GetError());
			return -1;
		}

		screct->x = 0;
		screct->y = 0;
		screct->w = 254;
		screct->h = 198;

		dsrect->x = 300;
		dsrect->y = 180;
		dsrect->w = 500;
		dsrect->h = 400;
		SDL_BlitSurface(s, screct, pTaquinSDL->pWindow, dsrect);
		SDL_FreeSurface(s);

		//////////////////////////////////////////////////////////////////////////////

		SDL_Surface* message = NULL;
		TTF_Font* font_message = NULL;
		SDL_Color textColor_message = { 0, 0, 0 };
		SDL_Rect* emplacement_message = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));

		font_message = TTF_OpenFont("./ARLRDBD.TTF", 72);

		message = TTF_RenderText_Solid(font_message, "R - Relancer", textColor_message);

		emplacement_message->x = 200;
		emplacement_message->y = 50;
		emplacement_message->w = 130;
		emplacement_message->h = 200;


		SDL_BlitSurface(message, NULL, pTaquinSDL->pWindow, emplacement_message);
		SDL_FreeSurface(message);

		message = TTF_RenderText_Solid(font_message, "F - Menu", textColor_message);

		emplacement_message->x = 200;
		emplacement_message->y = 130;
		emplacement_message->w = 130;
		emplacement_message->h = 200;

		SDL_BlitSurface(message, NULL, pTaquinSDL->pWindow, emplacement_message);
		SDL_FreeSurface(message);
	}
	// On dessine la case dans la fen�tre (en d�coupant dans l'image initiale avec la zone d�finie ci-dessus)
	if(refresh) SDL_UpdateRect(pTaquinSDL->pWindow,pDest->x,pDest->y,pDest->w,pDest->h);

	return 1;
}

// fonction pour rendre le taquin dans son �tat actuel
int displayTaquinSDL(TaquinSDL * pTaquinSDL, int end)
{
	int refresh = 1;

	// Test pour v�rifier que les donn�es pass�es ne sont pas corrompues
	if(!pTaquinSDL || !pTaquinSDL->taquin.plateau || !pTaquinSDL->pWindow ) return 0;

	SDL_FreeSurface(pTaquinSDL->pWindow);


	if (end == 1) {
		refresh = 0;
			
	}
	// On dessine les cases une par une en allant d�couper l'image de fond avec un SDL_Rect
	{
		// On dessine le taquin termin� pour afficher quelque chose mais il faudra le changer
		 
		SDL_Rect* pDest = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));
		pDest->x = 0;
		pDest->y = 0;
		pDest->w = pTaquinSDL->pWindow->w;
		pDest->h = pTaquinSDL->pWindow->h;
	
		// TODO: displayTaquinSDL
		// ...
		for (int i = 0; i < pTaquinSDL->taquin.largeur; i++) {
			for (int j = 0; j < pTaquinSDL->taquin.hauteur; j++) {
				//if (pTaquinSDL->taquin.plateau[i][j] == 0)continue;
				int y;
				int x;
				x = pTaquinSDL->resX * j;
				y = pTaquinSDL->resY * i;

				displayCaseTaquin(pTaquinSDL, pTaquinSDL->taquin.plateau[i][j], pDest,x , y, refresh);
			}
		}


		

		//SDL_Delay(1000);
		// On met � jour la fen�tre compl�te
		SDL_UpdateRect(pTaquinSDL->pWindow, 0, 0, 0, 0);
	}
	return 1;
}

// fonction permettant de faire le rendu SDL du taquin et de jouer (gestion des �v�nements � l'int�rieur de la fonction)
int gameLoopSDL(int hauteur,int largeur, char * pathBMPfile, int minRandom, int maxRandom)
{
	int end = 0;
	int resolu = 0;
	TaquinSDL t;

	srand((unsigned)time(NULL));

	
	
	// On cr�e le taquin et la fen�tre pour le dessiner
	if(!createTaquinSDL(&t,hauteur,largeur,pathBMPfile)) return 0;

	if (hauteur == 10) {
		int size = menu(&t);
		return size;
	}

	// On boucle sur le jeu tant qu'on a pas demand� de quitter
	while(end>=0)
	{
		int tick = SDL_GetTicks();

		// On m�lange le taquin
		
		mixTaquin(&(t.taquin), minRandom, maxRandom);		

		displayTaquin(&t.taquin, 0);
		// On affiche le taquin m�lang�
		displayTaquinSDL(&t, resolu);

		end = 0;
		
		// On boucle tant que la solution n'a pas �t� trouv�e
		while(!end )
		{
			displayTaquinSDL(&t, resolu);
			// Gestion des �v�nements
			SDL_Event e;

			int dt = SDL_GetTicks()-tick;
			tick +=dt;

			while(!end && SDL_PollEvent(&e))
			{
				
				switch(e.type)
				{
					case SDL_KEYDOWN:
						{
							deplacement d = AUCUN;
							
							switch(e.key.keysym.sym)
							{
								// On d�finit le mouvement � effectuer
								
							
								case SDLK_LEFT:		
									if (resolu == 1)break;
									d = GAUCHE;
									
									break;
								
								case SDLK_RIGHT:
									if (resolu == 1)break;
									d = DROITE;
									break;
								
								case SDLK_UP:	
									if (resolu == 1)break;
									d = HAUT;
									break;
								
								case SDLK_DOWN:		
									if (resolu == 1)break;
									d = BAS;
									break;

						
								case SDLK_f:
									if (resolu == 1) {
										int size = menu(&t);
										if (size == 0) {

											freeTaquinSDL(&t);
											TTF_Quit();
											IMG_Quit();
											SDL_Quit();
											
										}
										return size;
									}
									break;
									
								case SDLK_i:  
									initTaquin(&t.taquin);
									break;
								case SDLK_r:
									if (resolu == 1) {
										end = 1;
										resolu = 0;
									}
									break;
							

								case SDLK_ESCAPE:	
									// On quitte l'application
									end = -1;
									break;
								case SDLK_F1:
									// On d�clenche la r�solution du taquin
									// r�solution SDL � faire par la suite pour laisser la main � l'utilisateur :
									// - Arr�ter la r�solution (appui sur n'importe qu'elle touche
									// - Quitter l'application (ECHAP ou CROIX en haut � droite)
									{
										
										//on initialise les variables permettant de r�cup�rer les informations issues de la r�solution
										deplacement* tabDeplacements = NULL;
										unsigned long nbDeplacements = 0;
										unsigned long nbSommetsParcourus = 0;
										unsigned long timeElapsed = 0;

										// On demande la r�solution du taquin � l'ordinateur
										if(solveTaquin(&(t.taquin),&tabDeplacements,&nbDeplacements, &nbSommetsParcourus, &timeElapsed, 0, t.pWindow))
										{
											// Si on a trouv� une solution on affiche les informations issues de la r�solution
											unsigned long i;
											int res = 0;
											printf("Nombre de deplacements = %d\n",nbDeplacements);
											printf("Nombre de sommets parcourus = %d\n",nbSommetsParcourus);
											printf("Temps ecoule = %d ms\n",timeElapsed);

											// On affiche la solution �tape par �tape
											/*for (i = 0; i < nbDeplacements; i++)
											{
												// On effectue le d�placement, on affiche le nouveau plateau et on attend un appui sur une touche pour continuer
												if(tabDeplacements[i]!=AUCUN)
												{
													if(moveTaquin(&(t.taquin),tabDeplacements[i])) displayTaquinSDL(&t, resolu);
													else break;
												}
											}*/
										}
										// Si la r�solution n'a pas fonctionn�, on affiche le taquin tel qu'il �tait avant r�solution (on efface l'icone de "progression" si elle avait �t� dessin�e)
										else displayTaquinSDL(&t, resolu);

										if(tabDeplacements) 
										{
											free(tabDeplacements);
											tabDeplacements = NULL;
										}
									}

									break;
							}
							
							if(d!=AUCUN && moveTaquin(&(t.taquin),d)) displayTaquinSDL(&t, resolu);
							
						}
						break;

					case SDL_MOUSEBUTTONDOWN:
						{
						if (resolu == 1)break;
							// On r�cup�re la position et l'�tat des boutons de la souris
							int x,y;
							Uint8 state;
							state = SDL_GetMouseState(&x,&y);

							if (x < 0)x = 0;
							if (y < 0)y = 0;
							if (x > t.taquin.largeur * t.resX)x = t.taquin.largeur * t.resX;
							if (y > t.taquin.hauteur * t.resY)x = t.taquin.hauteur * t.resY;

							if(state & SDL_BUTTON_LEFT)
							{

								deplacement d = AUCUN;

								// On r�cup�re la position de la case dans le taquin
								int posY = x/t.resX;
								int posX = y/t.resY;
								printf("%d %d\n", posX, posY);
								//printf("%d %d", x, y);
								// TODO: On d�finit le d�placement � effectuer (diff�rence en X de +/- 1 ou en Y de +/-1 => DEPLACEMENT sinon AUCUN)
								

								for (int i = 0; i <= t.taquin.largeur; i++) {
									for (int j = 0; j <= t.taquin.largeur; j++) {
										if (posX != 0 ) {
											if (t.taquin.plateau[posX - 1][posY] == 0)d = HAUT;
											
										}
										if (posY != 0) {
											if (t.taquin.plateau[posX][posY - 1] == 0)d = GAUCHE;
										}
										if (posX != t.taquin.hauteur - 1) {
											if (t.taquin.plateau[posX + 1][posY] == 0) {
												d = BAS;
											}
										}
										if (posY != t.taquin.largeur - 1) {
											if (t.taquin.plateau[posX][posY + 1] == 0 )d = DROITE;


										}
										
									}
								}
								
								// ...
									
								// On applique le d�placement
								if(moveTaquin(&(t.taquin),d)) displayTaquinSDL(&t, resolu);
							}
							
							
						}
						break;
					
					case SDL_QUIT:
						
						// On quitte
						end = -1;

						break;
				}
				
			}

			// On relache du temps processeur pour les autres processus
			SDL_Delay(1);
			
			if (!resolu) {
				
				resolu = endTaquin(&(t.taquin));
			}
			
		}

		// Si on n'a pas demand� � quitter c'est qu'on a termin� le taquin
		

		// On r�initialise le taquin pour le prochain tour de jeu
		initTaquin(&(t.taquin));
	}

	// On lib�re le taquin et les surfaces SDL
	freeTaquinSDL(&t);

	/* Shut them both down */
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();


	return 1;
}

// fonction pour lib�rer le taquins et les surfaces SDL utilis�es
int freeTaquinSDL(TaquinSDL * t)
{
	// test pour savoir si les donn�es pass�es ne sont pas corrompues
	if(!t) return 0;

	// On lib�re la fen�tre SDL
	if(t->pWindow) SDL_FreeSurface(t->pWindow);

	// On lib�re l'image de fond
	if(t->pFond) SDL_FreeSurface(t->pFond);

	// On lib�re le taquin (mode console)
	freeTaquin(&(t->taquin));

	return 1;
}



int menu(TaquinSDL* t) {

	printf("Menu");

	int size = 2;

	SDL_Rect* a = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));
	a->x = 0;
	a->y = 0;
	a->w = t->resX * t->taquin.largeur;
	a->h = t->resY * t->taquin.hauteur;

	SDL_Surface* s = SDL_CreateRGBSurface(0, a->w, a->h, 32, 26, 232, 241, 0);
	SDL_FillRect(t->pWindow, a, SDL_MapRGB(s->format, 26, 232, 241));


	//////////////////////////////////////////////////////////////////////////


	SDL_Surface* message = NULL;
	TTF_Font* font = NULL;
	SDL_Color textColor = { 255, 255, 255 };
	SDL_Rect* emplacement = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));

	font = TTF_OpenFont("./ARLRDBD.TTF", 72);


	message = TTF_RenderText_Solid(font, "  MENU   ", textColor);

	emplacement->x = 250;
	emplacement->y = 50;
	emplacement->w = 130;
	emplacement->h = 200;


	SDL_BlitSurface(message, NULL, t->pWindow, emplacement);
	SDL_FreeSurface(message);

	//////////////////////////////////////////////////////////////////////////

	font = TTF_OpenFont("./ARLRDBD.TTF", 40);


	message = TTF_RenderText_Solid(font, "TAILLE GRILLE : ", textColor);

	emplacement->x = 50;
	emplacement->y = 250;
	emplacement->w = 100;
	emplacement->h = 170;


	SDL_BlitSurface(message, NULL, t->pWindow, emplacement);
	SDL_FreeSurface(message);


	/////////////////////////////////////////////////////////////////////////

	SDL_Surface* message_size = NULL;
	TTF_Font* font_size = NULL;
	SDL_Color textColor_size = { 255, 255, 255 };
	SDL_Rect* emplacement_size = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));

	font_size = TTF_OpenFont("./ARLRDBD.TTF", 40);

	char sizec[4] = { size + '0', ' ' };
	
	message_size = TTF_RenderText_Solid(font_size, sizec, textColor_size);

	emplacement_size->x = 400;
	emplacement_size->y = 250;
	emplacement_size->w = 130;
	emplacement_size->h = 200;


	SDL_BlitSurface(message_size, NULL, t->pWindow, emplacement_size);
	SDL_FreeSurface(message_size);

	//////////////////////////////////////////////////////////////////////////

	font = TTF_OpenFont("./ARLRDBD.TTF", 30);


	message = TTF_RenderText_Solid(font, "(down or up)", textColor);

	emplacement->x = 480;
	emplacement->y = 250;
	emplacement->w = 100;
	emplacement->h = 170;


	SDL_BlitSurface(message, NULL, t->pWindow, emplacement);
	SDL_FreeSurface(message);

	//////////////////////////////////////////////////////////////////////////

	font = TTF_OpenFont("./ARLRDBD.TTF", 40);


	message = TTF_RenderText_Solid(font, "P - PLAY ", textColor);

	emplacement->x = 50;
	emplacement->y = 350;
	emplacement->w = 100;
	emplacement->h = 170;


	SDL_BlitSurface(message, NULL, t->pWindow, emplacement);
	SDL_FreeSurface(message);

	//////////////////////////////////////////////////////////////////////////

	font = TTF_OpenFont("./ARLRDBD.TTF", 40);


	message = TTF_RenderText_Solid(font, "Tips - Touch F1 when playling ", textColor);

	emplacement->x = 50;
	emplacement->y = 450;
	emplacement->w = 100;
	emplacement->h = 170;


	SDL_BlitSurface(message, NULL, t->pWindow, emplacement);
	SDL_FreeSurface(message);

	//////////////////////////////////////////////////////////////////////////

	font = TTF_OpenFont("./ARLRDBD.TTF", 40);


	message = TTF_RenderText_Solid(font, "Escape - QUIT ", textColor);

	emplacement->x = 50;
	emplacement->y = 550;
	emplacement->w = 100;
	emplacement->h = 170;


	SDL_BlitSurface(message, NULL, t->pWindow, emplacement);
	SDL_FreeSurface(message);

	//////////////////////////////////////////////////////////////////////////

	SDL_UpdateRect(t->pWindow, a->x, a->y,a->w, a->h);
	int play = 0;

	int tick = SDL_GetTicks();

	

	while (!play) {
		
		// Gestion des �v�nements
		SDL_Event e;

		int dt = SDL_GetTicks() - tick;
		tick += dt;

		while (!play && SDL_PollEvent(&e))
		{

			switch (e.type)
			{
				case SDL_KEYDOWN:
				{
					switch (e.key.keysym.sym)
					{
						// On d�finit le mouvement � effectuer


					case SDLK_p:
						play =1;
						break;

					case SDLK_DOWN:
						size--;
						if (size < 2)size = 2;
						break;

					case SDLK_UP:
						size++;
						if (size > 9)size = 9;
						break;
					case SDLK_ESCAPE:
						TTF_Quit();
						IMG_Quit();
						SDL_Quit();
						size = 0;
						play = 1;
						return size;
						break;

					default:
						break;
					}
				}

				//Clean le fond
				message_size =  SDL_CreateRGBSurface(0, emplacement_size->w, emplacement_size->h, 32, 26, 232, 241, 0);
				SDL_FillRect(t->pWindow, emplacement_size, SDL_MapRGB(message_size->format, 26, 232, 241));

				sizec[0] =  size + '0';

				message_size = TTF_RenderText_Solid(font_size, sizec, textColor_size);

				SDL_BlitSurface(message_size, NULL, t->pWindow, emplacement_size);
				SDL_FreeSurface(message_size);

				SDL_UpdateRect(t->pWindow, a->x, a->y, a->w, a->h);
			}
		}
		

	}
	
	TTF_CloseFont(font);
	TTF_CloseFont(font_size);

	return size;
}