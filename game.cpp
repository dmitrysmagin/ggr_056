#include <allegro.h>
#include <stdio.h>

#include "util.h"
#include "class/_sprite.h"
#include "class/timer.h"
#include "class/osd.h"
#include "class/mobile.h"
#include "class/map.h"
#include "class/particle.h"
#include "class/flash.h"
#include "class/fog.h"
#include "class/water.h"
#include "class/fire.h"
#include "class/rain.h"
#include "class/snow.h"
#include "class/thunder.h"
#include "class/bonus.h"
#include "class/weapon.h"
#include "class/magic.h"
#include "class/bullet.h"
#include "class/player.h"
#include "class/blade.h"
#include "class/rockfall.h"
#include "class/zombie.h"
#include "class/skeleton.h"
#include "class/volture.h"
#include "class/red_arremer.h"
#include "class/red_arremer_king.h"
#include "class/plant_mk2.h"
#include "class/stalagmite.h"
#include "class/ghost.h"
#include "class/bat.h"
#include "class/giant.h"
#include "class/firewall.h"
#include "class/insect.h"
#include "class/fire_grizzly.h"
#include "class/clam.h"
#include "class/turtle.h"
#include "class/demon.h"
#include "class/little_devil.h"
#include "class/big_crab.h"
#include "class/pork.h"
#include "class/magician.h"
#include "class/flower.h"
#include "class/immobile.h"
#include "game.h"

int main()
 {
  init();
  Intro();
  while(1)
   {
    Title();
    while(!key[KEY_ESC] && !GAME_OVER)
     {
   	  checkGame();
   	  if(GAME_OVER) break;
  	  if(L_B)   layer_back();
  	  if(L_0)   layer_0();
  	  if(L_1)   layer_1();
  	  if(L_S)   layer_s();
  	  if(L_H)   layer_h();
  	  if(L_OSD) layer_OSD(50);
  	  Player();
  	  Enemies();
  	  Collision();
  	  if(DEBUG) debug(VSCR,50); // Debug
  	  Scroll();
  	  checkTimer();
  	  // Print all to screen
  	  rePaint(MFPS);
     }
    clear_keybuf(); wait(500);
   }
  return 0;
 }
END_OF_MAIN();
