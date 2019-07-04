#include <stdio.h>
#include <stdlib.h>
#include "fonctions.h"

int main(int argc, char **argv)
{
  // Bug fix de la démo pour le fichier appart2.txt à la ligne 359 dans le fichier fonctions.c
  s_Room room;
  s_Robot robot;
  int choice;
  int done = 0;
  int loopSDL = 1;
  SDL_Event event;
  s_PrintSDL screen;

  LoadMap(argv[1], &room);//charge le fichier
  FindStopFireAndStart(&room);//recherche de l'extincteur et du depart
  RobotInit(&room, &robot);//initialisation du robot
  do
  {
    printf("ProjetC\n-----------\nRobot Pompier\n-----------\n\n1.Version Terminal\n2.Version Grapique\n3.Quittez\n\nChoix>");
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
      screen.activate = 0;// on desactie l'affichage SDL
      PrintMap(&room, &robot, &screen);//afiche la map
      GoTo(&room, &robot, &(room.stopFire), &screen);//on va a l'extincteur
      ShutDownFire(&room, &robot, &screen);//on eteint le feu
      break;
    case 2:
      screen.activate = 1;// on active SDL
      SDL_Init(SDL_INIT_VIDEO);//demarage de sdl
      SDL_WM_SetCaption("Robot Pompier", NULL);
      StructSDLInit(&screen, &room);//initialisation de sdl
      while (loopSDL)
      {
        SDL_WaitEvent(&event); // pour pouvoir fermer le programme à la fin de la simulation
        switch (event.type)
        {
        case SDL_QUIT:
          loopSDL = 0;// si on quitte on sort de la boucle
          break;
        }

        if (done == 0) // condition pour que la simulation ne s'exectute qu'une seul fois
        {
          PrintMap(&room, &robot, &screen);//on affiche la map
          GoTo(&room, &robot, &(room.stopFire), &screen);//on va à l'extincteur
          ShutDownFire(&room, &robot, &screen);//on eteint le feu
          done = 1;
        }
      }
      SDL_Quit();//fermeture de sdl

      break;
    case 3:
      break;
    }
  } while (choice > 3 || choice < 1);

  return 0;
}
