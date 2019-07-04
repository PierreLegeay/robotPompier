#include <stdio.h>
#include <stdlib.h>
#include "fonctions.h"
#include <unistd.h>



void LoadMap(char *map, s_Room *room)  //Fonction permettant de lire le fichier txt
{

  int i, j;
  char carac;

  FILE *monFichier = fopen(map, "r"); //Ouverture du fichier

  fscanf(monFichier, "%d:%d\n", &(room->sizeX), &(room->sizeY)); // Lecture de la taille de la salle

  room->map = malloc(sizeof(char *) * room->sizeX); // Allocation dynamique du tableau a 2 dimensions
  for (i = 0; i < room->sizeX; i++)
    room->map[i] = malloc(sizeof(char) * room->sizeY);

  for (j = 0; j < room->sizeY; j++)
  {
    for (i = 0; i < room->sizeX + 1; i++)
    { // On lit un caractère plus loin pour arriver au "\n"
      carac = fgetc(monFichier);
      if (carac != '\n')
        room->map[i][j] = carac;//Affectation du caractère à sa position dans le tableau
    }
  }

  fclose(monFichier); // Fermeture du fichier
}

void PrintMap(s_Room *room, s_Robot *robot, s_PrintSDL *screen) //Fonction permettant d'afficher la carte
{
  int i, j;
  SDL_Event event;
  if (screen->activate == 0)  // On verfie que l'affichage SDL est desactivé
  {
    for (j = 0; j < room->sizeY; j++)
    {
      for (i = 0; i < room->sizeX; i++)
      {
        if (i == robot->pos.x && j == robot->pos.y) // Si i et j correspondent à la position du robot on affiche le sprite du robot
          printf("%c", robot->design); 
        else
          printf("%c", room->map[i][j]);// Sinon on affiche tous les autres caractères
      }
      printf("\n");
    }
    ShowData(robot, screen, room); // On affiche les informations concernant les points de vie et le nombre de déplacements
  }
  else if (screen->activate == 1)  //On verifie que l'affichage SDL est activé
  {
    SDL_FillRect(screen->backg, NULL, screen->white); //On remplie le fond de la fenetre en blanc
    for (j = 0; j < room->sizeY; j++)
    {
      for (i = 0; i < room->sizeX; i++)
      {
        screen->position.x = i * SPRITE_SIZE; //On renseigne les champs du Rect de destination 
        screen->position.y = j * SPRITE_SIZE; //en fonction de la taille des SPRITE

        if (i == robot->pos.x && j == robot->pos.y && robot->gotStopFire == 0)
          SDL_BlitSurface(screen->tabImg[1], NULL, screen->backg, &(screen->position));//On affiche le sprite du robot ne possedant pas l'extincteur
        
        else if (i == robot->pos.x && j == robot->pos.y && robot->gotStopFire == 1)
          SDL_BlitSurface(screen->tabImg[8], NULL, screen->backg, &(screen->position));//On affiche le sprite du robot possedant l'extincteur

        else if (room->map[i][j] == SPRITE_WALL)
          SDL_BlitSurface(screen->tabImg[0], NULL, screen->backg, &(screen->position));//On affiche le sprite correspondant au mur

        else if (room->map[i][j] == SPRITE_FIRELEVEL1)
          SDL_BlitSurface(screen->tabImg[2], NULL, screen->backg, &(screen->position));//On affiche le sprite correspondant au feu de niveau 1

        else if (room->map[i][j] == SPRITE_FIRELEVEL2)
          SDL_BlitSurface(screen->tabImg[3], NULL, screen->backg, &(screen->position));//On affiche le sprite correspondant au feu de niveau 2

        else if (room->map[i][j] == SPRITE_FIRELEVEL3)
          SDL_BlitSurface(screen->tabImg[4], NULL, screen->backg, &(screen->position));//On affiche le sprite correspondant au feu de niveau 3

        else if (room->map[i][j] == SPRITE_STOPFIRE)
          SDL_BlitSurface(screen->tabImg[5], NULL, screen->backg, &(screen->position));//On affiche le sprite correspondant a l'extincteur
      }
    }
    ShowData(robot, screen, room);//On affiche les infos concernants les points de vie et le nombre de déplacements
    SDL_Flip(screen->backg);//On raffraichit la fenetre SDL
    SDL_PollEvent(&event);
    switch (event.type) //Si l'utilisateur quitte le programme au cours du déplacement du robot
    {
    case SDL_QUIT: //On ferme la fenetre
      exit(0);
      break;
    }
  }
}

void FindStopFireAndStart(s_Room *room) //Fonction permettant de trouver la Position de depart et l'extincteur
{

  int i, j;

  for (j = 0; j < room->sizeY; j++) //On parcourt le tableau
  {
    for (i = 0; i < room->sizeX; i++)
    {
      if (room->map[i][j] == SPRITE_STOPFIRE)
      { // On cherche la position de l'extincteur
        room->stopFire.x = i; //Affectation de la position de l'extincteur
        room->stopFire.y = j;
      }
      else if (room->map[i][j] == SPRITE_DEPART)
      { // On cherche la case du depart
        room->start.x = i;//Affectation de la position du départ
        room->start.y = j;
      }
    }
  }
}
void RobotInit(s_Room *room, s_Robot *robot) // Initalisation du Robot
{ 
  robot->design = SPRITE_ROBOT; //On definit le design du robot comme étant le sprite de départ
  robot->countMove = 0; //Variable permettant de compter les déplacements du robot
  robot->findThePath = 1;
  robot->pos.x = room->start.x; //On renseigne le robot sur sa positin de départ
  robot->pos.y = room->start.y;
  robot->lifePoints = 10;//Initialisation des points de vie du robot
  robot->detectedFire = 0;// 0 si le robot n'a pas detecté de feu et 1 si il en a detecté
  robot->gotStopFire = 0; // 0 si il ne possède pas l'extincteur et 1 si il le possède
}
void StructSDLInit(s_PrintSDL *screen, s_Room *room)
{
  screen->backg = SDL_SetVideoMode(SPRITE_SIZE * room->sizeX, (SPRITE_SIZE * room->sizeY) + 4 * SPRITE_SIZE, 32, SDL_HWSURFACE);//Creation de la fenetre SDL
  screen->white = SDL_MapRGB(screen->backg->format, 255, 255, 255);//On definit la couleur blanche
  SDL_FillRect(screen->backg, NULL, screen->white);//Remplissage de la fenetre
  screen->tabImg[0] = SDL_LoadBMP("./src/wall.bmp");//Chargement du sprite mur
  screen->tabImg[1] = SDL_LoadBMP("./src/robot.bmp");//Chargement du sprite robot
  screen->tabImg[2] = SDL_LoadBMP("./src/fire1.bmp");
  screen->tabImg[3] = SDL_LoadBMP("./src/fire2.bmp");
  screen->tabImg[4] = SDL_LoadBMP("./src/fire3.bmp");
  screen->tabImg[5] = SDL_LoadBMP("./src/stopfire.bmp");
  screen->tabImg[6] = SDL_LoadBMP("./src/pdv.bmp");//Chargement de l'image pdv 
  screen->tabImg[7] = SDL_LoadBMP("./src/nbmove.bmp");//Chargement de l'image nombre de mouvements;
  screen->tabImg[8] = SDL_LoadBMP("./src/robotstopfire.bmp");//Chargement du sprite du robot possedant l'extincteur

  screen->position.x = 0;//Initialisation du rectangle de destination pour le blit
  screen->position.y = 0;
}

void MoveRobot(s_Room *room, s_Robot *robot, enum direction dir, s_PrintSDL *screen)//Fonction permettant de déplacer le robot 
{
  if (robot->lifePoints <= 0) {//Si le robot n'a plus de points de vie on ferme la fenetre
    printf("Le robot est mort\n");
    exit(0);
    
  }
  if ((dir == left) && (robot->pos.x - 1 >= 0) && robot->lifePoints > 0) //On regarde si le robot est en vie et qu'il ne sorte pas de la carte
  {                                                                                               // move left
    robot->pos.x--;//On decremente sa position en x 
    LifePoint(room, robot, robot->pos.x, robot->pos.y);//On appelle cette fonction pour verifier si il perd des points de vies
    PrintMap(room, robot, screen);//On affiche la carte
    CheckFire(room, robot, screen);//On verifie si la case sur laquelle il est arrivé contient du feu
    usleep(FPS);//Delai pour pouvoir voir les deplacements un à un
  }
  else if ((dir == right) && (robot->pos.x + 1 < room->sizeX) && robot->lifePoints > 0) //On regarde si le robot est en vie et qu'il ne sorte pas de la carte 
  {                                                                       // move right
    robot->pos.x++;//On incremente sa position en x
    LifePoint(room, robot, robot->pos.x, robot->pos.y);//On appelle cette fonction pour verifier si il perd des points de vies
    PrintMap(room, robot, screen);//On affiche la carte
    CheckFire(room, robot, screen);//On verifie si la case sur laquelle il est arrivé contient du feu
    usleep(FPS);//Delai pour pouvoir voir les deplacements un à un
  }
  else if ((dir == up) && (robot->pos.y - 1 >= 0) && robot->lifePoints > 0)
  {                                                              // move up
    robot->pos.y--;//On decremente sa position en y
    LifePoint(room, robot, robot->pos.x, robot->pos.y);
    PrintMap(room, robot, screen);
    CheckFire(room, robot, screen);
    usleep(FPS);
  }
  else if ((dir == down) && (robot->pos.y + 1 < room->sizeY) && robot->lifePoints > 0)
  {                                                                       // move down
    robot->pos.y++;
    LifePoint(room, robot, robot->pos.x, robot->pos.y);
    PrintMap(room, robot, screen);
    CheckFire(room, robot, screen);
    usleep(FPS);
  }
}

//Dans cette fonction en regarde si le robot peut aller dans une direction(s'il n'y a pas de mur et qu'on ne sorte pas de la map)
//Sert surtout pour la fonction GoTo
int CanGo(s_Room *room, int **tabVal, enum direction dir, int i, int j)
{
  //down
  if (((j + 1 < room->sizeY) && (room->map[i][j + 1] != SPRITE_WALL) && dir == down && ((tabVal[i][j + 1] == -1) || (tabVal[i][j + 1] == -2))))
    return 1;
  //up
  else if (((j - 1) >= 0) && (room->map[i][j - 1] != SPRITE_WALL) && dir == up && ((tabVal[i][j - 1] == -1) || (tabVal[i][j - 1] == -2)))
    return 1;
  //right
  else if (((i + 1) < room->sizeX) && (room->map[i + 1][j] != SPRITE_WALL) && dir == right && ((tabVal[i + 1][j] == -1) || (tabVal[i + 1][j] == -2)))
    return 1;
  //left
  else if (((i - 1 >= 0) && (room->map[i - 1][j] != SPRITE_WALL) && dir == left && ((tabVal[i - 1][j] == -1) || (tabVal[i - 1][j] == -2))))
    return 1;
  return 0;
}

//Fonction de debug qui permet d'afficher le tableau de valeur de la fonction GoTo
void PrintTemp(s_Room *room, int **tabVal)
{
  int i, j;

  for (j = 0; j < room->sizeY; j++)
  {
    for (i = 0; i < room->sizeX; i++)
    {
      printf("%d", tabVal[i][j]);//On affiche le tableau
    }
    printf("\n");
  }
}

//Fonction permettant d'ajouter des valeurs dans le tableau d'entier de la fonctions GoTo
void PutNumber(s_Room *room, int **tabVal, enum direction dir, int i, int j, int val)
{
  if ((tabVal[i][j] == -1 || tabVal[i][j] == -2) && room->map[i][j] != SPRITE_WALL)//On place des nombres si la case est à -1 ou que la case 
  {                                                                                // est à l'emplacement du robot et bien sur il faut 
    tabVal[i][j] = val;                                                            //que dans la map de la salle cette position ne soit pas un mur
  }                                                                                //Cette fonction permet aussi d'évitez d'ecraser des valeurs deja
}                                                                                  //renseigné du tableau

void GoTo(s_Room *room, s_Robot *robot, s_Pos *dest, s_PrintSDL *screen)//Fonction principale du programme qui permet d'ammener le robot à un point voulu
{
  int i, j;
  int block;
  int **tabVal = malloc(sizeof(int *) * room->sizeX); // Allocation dynamique du tableau a 2 dimensions
  for (i = 0; i < room->sizeX; i++)
    tabVal[i] = malloc(sizeof(int) * room->sizeY);

  int valeur = 0;//On initalise la valeur à 0 qui sera la valeur de depart de la construction du tableau
  for (i = 0; i < room->sizeX; i++)
  {
    for (j = 0; j < room->sizeY; j++)
    {
      tabVal[i][j] = -1;//On met tout le tableau à -1
    }
  }
  tabVal[dest->x][dest->y] = 0;//On met la position de destination à 0
  tabVal[robot->pos.x][robot->pos.y] = -2;//On met la position du robot à -2
  block = 0;
  if (robot->pos.x != dest->x || robot->pos.y != dest->y)//on verifie que le robot n'est pas deja sur la position de destination
  {
    while (tabVal[robot->pos.x][robot->pos.y] == -2 && !block)//On place des valeurs tant que le robot n'a pas été trouvé ou qu'on soit bloqué
    {                                                          //Exemple : position de destination entouré de mur
      block = 1;
      for (i = 0; i < room->sizeX; i++)
      {
        for (j = 0; j < room->sizeY; j++) // On parcourt le tableau
        {
          if (tabVal[i][j] == valeur) //On cherche la valeur à entourer de nombre 
          {
            if (CanGo(room, tabVal, right, i, j))//(right) si la position n'est pas un mur
            {
              PutNumber(room, tabVal, right, i + 1, j, valeur + 1);//alors on place à droite de cette valeur une valeur+1 supérieur
              block = 0;//Si on a reussit à placer une valeur le chemin n'est donc pas bloqué
            }

            if (CanGo(room, tabVal, down, i, j))//(down)
            {
              PutNumber(room, tabVal, down, i, j + 1, valeur + 1);
              block = 0;
            }

            if (CanGo(room, tabVal, up, i, j))//(up)
            {
              PutNumber(room, tabVal, up, i, j - 1, valeur + 1);
              block = 0;
            }

            if (CanGo(room, tabVal, left, i, j))//(left)
            {
              PutNumber(room, tabVal, left, i - 1, j, valeur + 1);
              block = 0;
            }
          }
        }
      }
      valeur++;
      if (block == 1) // on verifie après le parcours du tableau qu'on a reussi ou non a placer des valeurs
      {
        printf("il n' y a pas de chemin possible\n");
        robot->findThePath = 0;
      }
    }
  }

  while (((robot->pos.x != dest->x) || (robot->pos.y != dest->y)) && !block) //Parcous du robot on part cette fois ci,
  {                                                                          //de la case du robot qui vaut valeur
                                                                            //puis on regarde autour pour trouver la case comportant valeur -1

    if (CheckCell(room, tabVal, valeur - 1, robot->pos.x, robot->pos.y - 1))//On verifie si la case du dessus comporte valeur -1
    {//si oui
      MoveRobot(room, robot, up, screen);//on deplacer le robot d'une case en haut
      robot->countMove++;//on ajoute un deplacement
      valeur--;//puis on decrement valeur pour trouver une case inferier
    }
    else if (CheckCell(room, tabVal, valeur - 1, robot->pos.x + 1, robot->pos.y))//right
    {
      MoveRobot(room, robot, right, screen);
      robot->countMove++;
      valeur--;
    }

    else if (CheckCell(room, tabVal, valeur - 1, robot->pos.x - 1, robot->pos.y))//gauche
    {
      MoveRobot(room, robot, left, screen);
      robot->countMove++;
      valeur--;
    }

    else if (CheckCell(room, tabVal, valeur - 1, robot->pos.x, robot->pos.y + 1))//bas
    {
      MoveRobot(room, robot, down, screen);
      robot->countMove++;
      valeur--;
    }

  }
  //a la fin de la boucle si la pos du robot correspond a celle de l'extincteur on change son sprite
  if (robot->pos.x == room->stopFire.x && robot->pos.y == room->stopFire.y)
  {
    robot->design = SPRITE_GOTSTOPFIRE;//changement du sprite
    robot->gotStopFire = 1;  // Possede l'extincteur

    PrintMap(room, robot, screen);
    room->map[room->stopFire.x][room->stopFire.y] = ' ';//case vide correspond au vielle extincteur
  }
  free(tabVal);//on libere le tableau
}

int CheckCell(s_Room *room, int **tabVal, int valeur, int x, int y)//on verifie si la cellule cible 
{
  if (x >= 0 && x < room->sizeX && y >= 0 && y < room->sizeY)  // Fix du bug de la démo je regardais une case de trop. Sur appart2.txt 
    if (tabVal[x][y] == valeur)                                // il regardait à droite avant de regarder à gauche 
      return 1;                                                // d'ou le coredumped donc il sortait de la map vu que le robot demarre à droite
  return 0;
}

int CheckFire(s_Room *room, s_Robot *robot, s_PrintSDL *screen)//fonction permettant de verrifier si la case contient du feu
{
  if (room->map[robot->pos.x][robot->pos.y] == '1' && robot->gotStopFire == 1) // si le robot arrive sur une case contenant du feu et qu'il possède l'extincteur alors il l'éteint
  {
    robot->detectedFire = 1; //feu detecté
    ClearFire(room);//on éteint le feu
    PrintMap(room, robot, screen);//on affiche la carte
  }
  else if (room->map[robot->pos.x][robot->pos.y] == '1')//si le robot croise du feu et qu'il n'a pas l'extincteur il enrengistre sa positions
  {
    robot->detectedFire = 1; //feu detecté
    robot->fire.x = robot->pos.x;//on recupere la postion du feu
    robot->fire.y = robot->pos.y;
    return 1;
  }
  return 0;
}

void ShutDownFire(s_Room *room, s_Robot *robot, s_PrintSDL *screen)
{
  int i;
  int j = 0;
  s_Pos dest;
  if (robot->gotStopFire == 1) // sir le robot à l'extincteur
  {
    if (robot->detectedFire == 1)// et qu'on a deja detecté du feu alors on va à cette position
    {
      GoTo(room, robot, &(robot->fire), screen);
    }
    else//sinon on parcourt la map à la recherche du feu
    {
      while (robot->detectedFire != 1 && j < room->sizeY)
      {
        i = 0;
        while (robot->detectedFire != 1 && i < room->sizeX)
        {
          if (room->map[i][j] != SPRITE_WALL)
          {
            dest.x = i;
            dest.y = j;
            GoTo(room, robot, &dest, screen);
          }
          i++;
        }
        j++;
      }
    }
  }
}

void LifePoint(s_Room *room, s_Robot *robot, int x, int y)//fonction permettant de deduire les points de vie du robot
{
  for (char i = '1'; i <= '3'; i++)
    if (room->map[robot->pos.x][robot->pos.y] == i)//si le robot marche sur du feu alors on en deduit les pdv correspondant
      robot->lifePoints -= i - 48;

  
}



void ClearFire(s_Room *room)//fonction qui eteint le feu
{
  int i, j;
  for (i = 0; i < room->sizeX; i++)
  {
    for (j = 0; j < room->sizeY; j++)
    {
      if ((room->map[i][j] == '1') || (room->map[i][j] == '2') || (room->map[i][j] == '3'))//on cherche toutes les cases de la carte
        room->map[i][j] = ' ';                                                            //correspondant à du feu et on les éteint
    }
  }
}
void ShowData(s_Robot *robot, s_PrintSDL *screen, s_Room *room)
{
  printf("Point de vie du robot : %d\t\t Nombre de deplacements du robot %d\n", robot->lifePoints, robot->countMove);//affichage des pdv et du nb de deplacements

  screen->position.x = 0;
  screen->position.y = room->sizeY * SPRITE_SIZE;
  SDL_BlitSurface(screen->tabImg[6], NULL, screen->backg, &(screen->position));//collage de l'image LifePoints
  screen->position.x = room->sizeX * SPRITE_SIZE / 2;
  SDL_BlitSurface(screen->tabImg[7], NULL, screen->backg, &(screen->position));//collage de l'image nb movements

}
