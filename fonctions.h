#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#ifndef FONCTIONS_H
#define FONCTIONS_H
#define SPRITE_ROBOT 'R'
#define SPRITE_STOPFIRE 'E'
#define SPRITE_GOTSTOPFIRE 'P'
#define SPRITE_DEPART 'D'
#define SPRITE_WALL 'x'
#define SPRITE_FIRELEVEL1 '1'
#define SPRITE_FIRELEVEL2 '2'
#define SPRITE_FIRELEVEL3 '3'
#define FPS 100000
#define SPRITE_SIZE 16

enum direction //enumeration facilitant la syntaxe pour les deplacements
{
  left,
  right,
  up,
  down,
};

typedef struct s_Pos //structure position utilise pour les elements de decor
{
  int x;
  int y;
} s_Pos;

typedef struct s_Room//structure de la salle
{
  int sizeX, sizeY;//taille de la salle
  s_Pos stopFire;//positon de l'extincteur
  s_Pos start;//pos depart
  char **map;//carte
} s_Room;

typedef struct s_Robot
{
  s_Pos pos;//pos du robot
  s_Pos fire;//pos du feu(si rencontré)
  int countMove;//compteur de mouvement
  int findThePath;//si le robot a trouvé un chemin possible
  char design;// son spprite
  int gotStopFire;//si il possede l'extincteur
  int lifePoints;//pdv
  int detectedFire;//si il a detecté du feu

} s_Robot;

typedef struct s_PrintSDL
{
  SDL_Surface *backg;//surface sur laquelle on colle tout les élements
  Uint32 white;//couleur blanche
  SDL_Surface *tabImg[9]; //tableau de pointeurs de surface correspondant aux images
  SDL_Rect position;//la position du rectangle de destination pour le collage
  int activate;//si SDL est activé ou non

} s_PrintSDL;

void LoadMap(char *map, s_Room *room);
void PrintMap(s_Room *room, s_Robot *robot, s_PrintSDL *screen);
void FindStopFireAndStart(s_Room *room);
void RobotInit(s_Room *room, s_Robot *robot);
void MoveRobot(s_Room *room, s_Robot *robot, enum direction dir, s_PrintSDL *screen);

void GoTo(s_Room *room, s_Robot *robot, s_Pos *dest, s_PrintSDL *screen);
int CanGo(s_Room *room, int **tabVal, enum direction dir, int i, int j);
void PutNumber(s_Room *room, int **tabVal, enum direction dir, int i, int j, int val);
void PrintTemp(s_Room *room, int **tabVal);
int CheckCell(s_Room *room, int **tabVal, int valeur, int x, int y);
int CheckFire(s_Room *room, s_Robot *robot, s_PrintSDL *screen);
void ShutDownFire(s_Room *room, s_Robot *robot, s_PrintSDL *screen);
void FreeMap(s_Room *room);
void ClearFire(s_Room *room);
void ShowData(s_Robot *robot, s_PrintSDL *screen, s_Room *room);
void LifePoint(s_Room *room, s_Robot *robot, int x, int y);
void StructSDLInit(s_PrintSDL *screen, s_Room *room);

#endif
