class weapon
 {
 	private:
 	 enum { _COL=25, _ROW=5, _XPIX=46, _YPIX=46 };
 	 int index,plat_layer;
   int actWeapon,frame_offset,min_frame,max_frame,grframe_min,grframe_max,max_life,grmax_life,max_power,status;
 	 int pos_x,pos_y,scroll_x,scroll_y,jump_y,X,Y,hit_x,hit_y,_i;
 	 int plat_y,plat_stair,plat_wall,power;
 	 int dx,dy;
 	 int act_frame,hard_frame,soft_frame;
 	 int life,speed,speed_d,speed_ud,movedir,gravity,multi;
 	 int *player_x,*player_y;
 	 bool animated,powerup,no_zeropower,canhit_ground;
 	 bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
 	 bool can_hit;
 	 bool stop,move,jump_up,jump_down,down,climb,fire,fire2,fall,turn,rise,unrise,hard,soft;
 	 timer *ti[5];
 	 SAMPLE *sound[4];
 	 BITMAP *frame[_COL*_ROW];
 	 BITMAP *frame_col[2][_COL*_ROW];
 	 BITMAP *out;
 	 map *plat_map;
 	 map *hight_map;
 	 map *back_map;

 	 void loadSpriteFrames(char *);
 	 void assignSpriteFrame(weapon *p);
 	 void setOutBitmap(BITMAP *);
 	
 	public:
 	 weapon(char *,BITMAP *,int i=0,weapon *p=NULL);
 	 ~weapon();
 	 void drawSprite(int,int);
 	 void timeSprite();
 	 void platformSprite();
 	 void animSprite();
 	 void setPlatLayer(int);
 	 void setPlatformMap(map *p);
 	 void setHightMap(map *p);
 	 void setBackgroundMap(map *p);
 	 void loadSample(char *,int);
 	 void assignSample(int,weapon *);
 	 BITMAP *getActFrameColP();
 	 bool inScreen();
 	 int getPlatLayer();
 	 int getActWeapon();
 	 int setActWeapon(int,bool);
 	 int minIndex(int,bool);
 	 void setIndex(int);
 	 void setPlayerXP(int *);
 	 void setPlayerYP(int *);
 	 int getPlatYPos();
 	 int getX();
 	 int getY();
 	 int getMoveDir();
 	 int *getPosXP();
 	 int *getPosYP();
 	 void shoot(int,int,bool,int,int,bool,bool,bool,bool);
 	 void hit(int fl=1);
 	 void hitSoft();
 	 void hitHard();
 	 void hitGround(int);
 	 bool isFired();
 	 int getMulti();
 	 bool canHit();
 	 void debug();
 };

weapon::weapon(char *filename, BITMAP *out, int i, weapon *p)
 {
 	actWeapon=LANCE; frame_offset=0; power=0; max_power=1;
 	_i=i;
 	if(!_i) loadSpriteFrames(filename); else assignSpriteFrame(p);
 	setOutBitmap(out);
 	pos_x=100; pos_y=100; scroll_x=0; scroll_y=0; jump_y=0; hit_x=0; hit_y=0;
 	dx=0; dy=0; //plat_y=start_y;
 	act_frame=0; hard_frame=0; soft_frame=0; min_frame=0; max_frame=0;
 	grframe_min=0; grframe_max=0;
 	animated=false; index=0; multi=0;
 	life=max_life=80; grmax_life=0; speed=6; speed_d=0; speed_ud=0; pos_x=0; gravity=0;
 	stop=1; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire=0; fire2=0; fall=0,turn=0,rise=0,unrise=0; hard=0; soft=0;
 	side=0; direction=0; movedir=0; can_hit=1;
 	plat_wall=0;
 	ti[0]=new timer(1); // Hard time
 	ti[1]=new timer(1); // Anim time
 	ti[2]=new timer(0); // Gravity time
 	ti[3]=new timer(0);
 	ti[4]=new timer(0);
 }

weapon::~weapon() 
 {
 	for(int i=0;i<5;i++) delete(ti[i]);
 	if(_i==0)
 	 {
    for(int i=0;i<2;i++) destroy_sample(sound[i]);
 	  for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame[i]);
 	  for(int i=0;i<_COL*_ROW;i++) { destroy_bitmap(frame_col[0][i]); destroy_bitmap(frame_col[1][i]); }
 	 } 	
 }

void weapon::loadSpriteFrames(char *filename)
 {
 	BITMAP *aux;

  aux=load_bmp(filename,NULL);
  //for(int i=0;i<24;i++) frame[i]=create_sub_bitmap(aux,46*i+i,0,46,46);
  for(int i=0;i<_ROW;i++)
   for(int j=0;j<_COL;j++)
    {
     //frame[j+_COL*i]=create_sub_bitmap(aux,46*j+j,46*i+i,46,46);
     frame[j+_COL*i]=create_bitmap(_XPIX,_YPIX);
     blit(aux,frame[j+_COL*i],_XPIX*j+j,_YPIX*i+i,0,0,frame[j+_COL*i]->w,frame[j+_COL*i]->h);
     frame_col[0][j+_COL*i]=create_bitmap_ex(8,_XPIX,_YPIX);
     frame_col[1][j+_COL*i]=create_bitmap_ex(8,_XPIX,_YPIX);
     clear_to_color(frame_col[0][j+_COL*i],0);
     clear_to_color(frame_col[1][j+_COL*i],0);
     for(int h=0;h<frame[j+_COL*i]->h;h++)
      for(int w=0;w<frame[j+_COL*i]->w*4;w+=4)
       if(makecol32(frame[j+_COL*i]->line[h][w+2],
                    frame[j+_COL*i]->line[h][w+1],
                    frame[j+_COL*i]->line[h][w])!=16711935)
                     {
                      frame_col[0][j+_COL*i]->line[h][w>>2]=1;
                      frame_col[1][j+_COL*i]->line[h][(((frame[j+_COL*i]->w-1)*4)>>2)-(w>>2)]=1;
                     }
    }
  destroy_bitmap(aux);
 }

void weapon::assignSpriteFrame(weapon *p)
 {
 	for(int i=0;i<_ROW*_COL;i++)
 	 {
 	  frame[i]=p->frame[i];
 	  frame_col[0][i]=p->frame_col[0][i];
 	  frame_col[1][i]=p->frame_col[1][i];
 	 }
 }

void weapon::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void weapon::assignSample(int n,weapon *p)
 {
 	sound[n]=p->sound[n];
 }

void weapon::setOutBitmap(BITMAP *bmp) { out=bmp; }

void weapon::timeSprite()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void weapon::animSprite()
 {
 	int plat_y;
 	
 	plat_y=plat_map->getActPlatform(pos_x,pos_y)+7;
 	if(hard) { if(ti[0]->isOn()) hard_frame++; if(hard_frame>27) { hard=0; hard_frame=0; } }
 	if(soft) { if(ti[0]->isOn()) soft_frame++; if(soft_frame>51) { soft=0; soft_frame=0; } }
 	if(!fire) return;
 	if(fire) { if(side) dx=-speed; else dx=speed; dy=speed; life--; }
 	if(life<=0 || !inScreen()) fire=0;
 	if(movedir==0) // Normal
 	 {
 		pos_x+=dx; pos_y+=speed_d;
 		if(ti[2]->isOn()) speed_d+=gravity;
  	if(canhit_ground) // L'arma cambia stato se colpisce il terreno (esempio torcia)
  	 if(pos_y>plat_y && speed_d>0 && status==0) hitGround(plat_y);
 	 }
 	if(movedir==1) // Up
 	 { 
 		pos_y-=dy; if(side) pos_x-=speed_ud; else pos_x+=speed_ud;
 		if(ti[2]->isOn()) speed-=gravity;
  	if(canhit_ground) // L'arma cambia stato se colpisce il terreno (esempio torcia)
  	 if(pos_y>plat_y && speed<0 && status==0) hitGround(plat_y);
 	 }
 	if(movedir==2) // Down
 	 {
 		pos_y+=dy; if(side) pos_x-=speed_ud; else pos_x+=speed_ud;
 		if(ti[2]->isOn()) speed+=gravity;
  	if(canhit_ground) // L'arma cambia stato se colpisce il terreno (esempio torcia)
  	 if(pos_y>plat_y && speed>0 && status==0) hitGround(plat_y);
 	 }
 	/*if(fall && !unrise) { act_frame=7; }
 	if(rise) 
 	 {
 	 	pos_y=plat_map->getActPlatformEnemies(pos_x,0);
 	 	if(ti[1]->isOn()) act_frame++;
 	 	if(act_frame==8) { rise=0; move=1; }
 	 }
 	if(unrise) 
 	 {
 	 	if(ti[1]->isOn()) act_frame--;
 	 	if(act_frame==0)
 	 	 {
 	 	 	rise=1; unrise=0;
 	 	 	pos_x=random(2300);
 	 	 	side=direction=random(1);
 	 	 	speed=random(1)+1;
 	 	 }
 	 }
 	if(move)
 	 {
 	  pos_x+=dx;
 	  if(ti[0]->isOn()) { act_frame++; if(act_frame>9) act_frame=8; }
 	 }
 	if(side) dx=-speed; else dx=speed;
  pos_y+=dy;
  life--;*/
 	/*if(turn) { if(plat_wall) dx=0; side=!side; turn=0; }
  if(stop) act_frame=0;
  if(climb)
   {
   	if(act_frame<18) act_frame=18;
   	if(key[KEY_UP] || key[KEY_DOWN]) if(ti[1]->isOn()) { act_frame++; if(act_frame>21) act_frame=18; }
   	jump_up=0; jump_down=0; fire=0; fire2=0; fire_down=0; fire_up=0;
   	return;
   }
  if(move && !jump_up && !jump_down)
   {
   	if(!plat_wall) pos_x+=dx;
   	if(timer[0]) { act_frame++; if(act_frame>6) act_frame=1; }
   }
 	if(jump_down)
 	 {
 	 	if(!fire && !fire2 && !fire_up && !fire_down)
 	 	 if(move && !fire && !fire2 && !fire_up && !fire_down) act_frame=8; else act_frame=9;
 	 	if(move) if(!plat_wall) pos_x+=dx;
 	 	if(pos_y>=plat_y) { jump_up=0; jump_down=0; move=0; dy=0; pos_y=plat_y; play_sample(sound[1],120,150,1000,0); return; }
 	 	if(pos_y%5==0) dy++; // Gravità
 	 }
 	if(jump_up)
 	 {
 	 	if(!fire && !fire2 && !fire_up && !fire_down)
 	 	 if(move) act_frame=8; else act_frame=9;
 	 	if(move) if(!plat_wall) pos_x+=dx;
 	 	jump_y++; if(jump_y==21) { jump_up=0; jump_down=1; dy=2; }
 	 	if(jump_y%5==0) dy++; // Gravità
 	 }
  if(down) act_frame=7;
  //if(fall && !fire && !fire2 && !fire_up && !fire_down) { act_frame=6; }
  if(fire)     { if(timer[1]) { act_frame++; if(act_frame>11) { act_frame=11; fire=0;     } } }
  if(fire2)     { if(timer[1]) { act_frame++; if(act_frame>13) { act_frame=13; fire2=0;     } } }
  if(fire_up)   { if(timer[1]) { act_frame++; if(act_frame>15) { act_frame=15; fire_up=0;   } } }
  if(fire_down) { if(timer[1]) { act_frame++; if(act_frame>17) { act_frame=17; fire_down=0; } } }
  //if(plat_wall) { pos_x-=dx; }
  if(pos_x<22) pos_x=22; if(pos_x>366) pos_x=366;*/
 }

void weapon::drawSprite(int sx, int sy)
 {
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(fire)
   {
   	if(animated && ti[1]->isOn()) { act_frame++; if(act_frame>max_frame) act_frame=min_frame; }
   	
   	// Aggiustamenti posizione e animazione per armi (SWORD e AXE)
   	if(actWeapon==SWORD)
   	 {
   	  // La spada resta "attaccatta" al player
   	  if(act_frame==15)                  if(side) { X=*player_x+9;  Y=*player_y-13; } else { X=*player_x-9;  Y=*player_y-13; } // Normale
      if(act_frame==16 || act_frame==17) if(side) { X=*player_x-27; Y=*player_y-2;  } else { X=*player_x+27; Y=*player_y-2;  } // Normale
   	  if(act_frame==40)                  if(side) { X=*player_x-9;  Y=*player_y-3;  } else { X=*player_x+9;  Y=*player_y-3;  } // Abbassato
   	  if(act_frame==41 || act_frame==42) if(side) { X=*player_x-29; Y=*player_y+12; } else { X=*player_x+29; Y=*player_y+12; } // Abbassato
   	  if(act_frame==18)                  if(side) { X=*player_x-2;  Y=*player_y-17; } else { X=*player_x+2;  Y=*player_y-17; } // Alto
   	  if(act_frame==19)                  if(side) { X=*player_x+5;  Y=*player_y-27; } else { X=*player_x-5;  Y=*player_y-27; } // Alto
   	  if(act_frame==43)                  if(side) { X=*player_x-10; Y=*player_y+13; } else { X=*player_x+10; Y=*player_y+13; } // Basso
   	  if(act_frame==44)                  if(side) { X=*player_x+10; Y=*player_y+24; } else { X=*player_x-10; Y=*player_y+24; } // Basso
   	 }
   	if(actWeapon==AXE)
   	 {
   	  if(act_frame%2==0 && powerup)
   	   {
   	    if(side) draw_sprite(out,frame_col[1][act_frame-1],X,Y);
   	    else     draw_sprite(out,frame_col[0][act_frame-1],X,Y);
   	   }
   	 }

    if(side) draw_sprite_h_flip(out,frame[act_frame],X,Y);
    else     draw_sprite       (out,frame[act_frame],X,Y);
    
    // Specil gfx for weapon TODO
    //line(out,X+12,Y+20,X-(max_life-life),Y+20,makecol32(155,255,255));
   }
  if(hard)
   {
    if(side) draw_sprite_h_flip(out,frame[hard_frame],-sx+hit_x,-sy+hit_y);
    else     draw_sprite       (out,frame[hard_frame],-sx+hit_x,-sy+hit_y);
   }
  if(soft)
   {
    if(side) draw_sprite_h_flip(out,frame[soft_frame],-sx+hit_x,-sy+hit_y);
    else     draw_sprite       (out,frame[soft_frame],-sx+hit_x,-sy+hit_y);
   }
 }

void weapon::platformSprite()
 {
 	plat_wall=0;
 	if(!jump_down && !jump_up && !climb && !rise && !unrise)
 	 {
 	  if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
 	 }
 	plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	plat_stair=0;//plat_map->getActPlatform(1);
 	if(!unrise) plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
 }

void weapon::setPlayerXP(int *p) { player_x=p; }

void weapon::setPlayerYP(int *p) { player_y=p; }

BITMAP *weapon::getActFrameColP() { return frame_col[side][act_frame]; }

int weapon::getPlatLayer() { return plat_layer; }

int weapon::getActWeapon() { return actWeapon; }

void weapon::setIndex(int i) { index=i; }

void weapon::setPlatLayer(int n) { plat_layer=n; }

int weapon::setActWeapon(int w,bool pow)
 {
 	// This function returns the number of sprites
 	actWeapon=w;
 	if(w==LANCE) { multi=0; if(pow)   return 3;    else            return 2; } 
 	if(w==KNIFE) { multi=0; if(pow)   return 5;    else	           return 3; }
 	if(w==AXE)	 { multi=0; if(pow)   return 2;    else            return 1; }
 	if(w==SWORD) { if(pow) { multi=2; return 4;  } else { multi=0; return 1; } }
	if(w==ARROW) { if(pow) { multi=3; return 9;  } else { multi=2; return 4; } }
	if(w==FLAME) { if(pow) { multi=5; return 15; } else { multi=0; return 3; } }
 }

int weapon::minIndex(int w,bool pow)
 {
 	// This function returns the min index for the weapon frame
 	actWeapon=w;
 	if(w==LANCE) { if(pow) return 2;  else return 0;  } 
 	if(w==KNIFE) { if(pow) return 8;  else return 5;  }
 	if(w==AXE)	 { if(pow) return 14; else return 13; }
 	if(w==SWORD) { if(pow) return 17; else return 16; }
	if(w==ARROW) { if(pow) return 25; else return 21; }
	if(w==FLAME) { if(pow) return 37; else return 34; }
 }

bool weapon::inScreen()
 {
  if(X>=-32 && X<=384 && Y>-40 && Y<224) return true;
  else return false;
 }

int weapon::getPlatYPos() { return plat_y; }

int weapon::getX() { return X; }

int weapon::getY() { return Y; }

int *weapon::getPosXP() { return &pos_x; }

int *weapon::getPosYP() { return &pos_y; }

int weapon::getMoveDir() { return movedir; }

void weapon::setPlatformMap(map *p) { plat_map=p; }

void weapon::setHightMap(map *p) { hight_map=p; }

void weapon::setBackgroundMap(map *p) { back_map=p; }

bool weapon::canHit() { if(life<max_life) return can_hit; else return false; } // Controllo life per aggirere il bug di riposizionamento arma

void weapon::shoot(int px,int py,bool s,int dir,int plat_lay,bool prone,bool fireup,bool firedown,bool pow)
 {
 	if(!fire)
 	 {
 	 	no_zeropower=false;
 	 	grframe_min=grframe_max=grmax_life=0;
 	 	powerup=pow;
 	 	movedir=dir;
 	 	side=s;
	  fire=1;
	  can_hit=1;
	  if(actWeapon==LANCE)
	   {
	   	canhit_ground=false; gravity=0; status=0;
	   	animated=false; multi=0; speed_d=0; speed_ud=0; max_life=80;
  	 	if(pow) { frame_offset=44; max_power=2; speed=7; } else { frame_offset=0; max_power=1;  speed=6; }
	   	pos_x=px; pos_y=py; act_frame=dir+1+frame_offset;
	   }
	  if(actWeapon==KNIFE)
	   {
	   	canhit_ground=false; gravity=0; status=0;
	   	animated=false; multi=0; speed_d=0; speed_ud=0; max_life=80; max_power=1;
   	 	if(pow) { frame_offset=31; speed=10; } else { frame_offset=3; speed=8; }
	   	pos_x=px+random(8)-4; pos_y=py+random(10)-6; act_frame=dir+1+frame_offset;
	   }
	  if(actWeapon==AXE)
	   {
	   	canhit_ground=false; gravity=0; status=0;
	   	max_power=4; frame_offset=6; animated=true; min_frame=7; max_frame=14; multi=0; speed_d=-3; speed_ud=0; speed=6; max_life=80;
  	 	if(pow) { ti[1]->setTick(0); } else { ti[1]->setTick(1); }
	   	pos_x=px; pos_y=py; act_frame=dir+1+frame_offset;
	   }
	  if(actWeapon==SWORD)
	   {
	   	canhit_ground=false; gravity=0; status=0;
	   	if(pow)
	   	 {
	   	 	multi=2;
	   	 	if(index%multi==1)
	   	 	 {
	   	 	 	no_zeropower=true;
	   	 	  frame_offset=14; animated=true; min_frame=15; max_frame=17; speed_d=0; speed_ud=0; speed=0;
	  	    max_power=6; max_life=6; ti[1]->setTick(1);
	  	    if(prone)    { frame_offset=39; min_frame=40; max_frame=42; }
 	 	      if(fireup)   { frame_offset=17; min_frame=18; max_frame=19; max_life=9; ti[1]->setTick(3); }
          if(firedown) { frame_offset=42; min_frame=43; max_frame=44; max_life=9; ti[1]->setTick(3); }
          if(side) { pos_x=px-23; pos_y=py+2; } else  { pos_x=px+23; pos_y=py+2; } act_frame=1+frame_offset;
         }
        else
         {
	   	    animated=false; speed_d=0; speed_ud=0; speed=6; max_life=11;
	   	    frame_offset=35;
 	  	    if(prone)    frame_offset=36;
 	 	      if(fireup)   { frame_offset=37; max_life=13; }
          if(firedown) { frame_offset=38; max_life=13; }
    	   	pos_y=py+2;
    	   	if(frame_offset==35 || frame_offset==36) if(side) pos_x=px-23; else pos_x=px+23;
    	   	if(frame_offset==37) if(side) pos_x=px-12; else pos_x=px+12;
    	   	if(frame_offset==38) if(side) pos_x=px-6;  else pos_x=px+6;
    	   	act_frame=1+frame_offset;
         }
	     }
	    else
	     {
	     	no_zeropower=true;
	  	  frame_offset=14; animated=true; min_frame=15; max_frame=17; multi=0; speed_d=0; speed_ud=0; speed=0;
	  	  max_power=2; max_life=6; ti[1]->setTick(1);
	  	  if(prone)    { frame_offset=39; min_frame=40; max_frame=42; }
 	 	    if(fireup)   { frame_offset=17; min_frame=18; max_frame=19; max_life=9; ti[1]->setTick(3); }
        if(firedown) { frame_offset=42; min_frame=43; max_frame=44; max_life=9; ti[1]->setTick(3); }
        if(side) { pos_x=px-23; pos_y=py+2; } else  { pos_x=px+23; pos_y=py+2; } act_frame=1+frame_offset;
       }
     }
	  if(actWeapon==ARROW)
	   {
	   	canhit_ground=false; gravity=0; status=0;
	   	animated=false; if(pow) multi=3; else multi=2;
	   	if(index%multi==0) { if(pow) frame_offset=69; else frame_offset=19; speed_d=0;  speed_ud=0;  }
	   	if(index%multi==1) { if(pow) frame_offset=53; else frame_offset=28; speed_d=-1; speed_ud=-1; }
	   	if(index%multi==2) { if(pow) frame_offset=53; else frame_offset=28; speed_d=-3; speed_ud=-3; }
   	 	if(pow) { speed=8; max_life=120; max_power=1; } else { speed=6; max_life=120; max_power=1; }
	   	pos_x=px; pos_y=py; act_frame=dir+1+frame_offset;
	   }
	  if(actWeapon==FLAME)
	   {
	   	ti[2]->setTick(4); // Gravity time
	   	canhit_ground=true; gravity=2; status=0; max_life=120;
	   	max_power=1; frame_offset=74; animated=true; min_frame=75; max_frame=79; speed_d=-3; speed_ud=0;
	   	if(pow)
	   	 {
	   	 	multi=5;
	   	 	if(index%multi==0)
	   	 	 {
	   	 	  grmax_life=56;
	   	 	  frame_offset=99; min_frame=100; max_frame=101;
	   	 	  grframe_min=102; grframe_max=120;
	   	 	  ti[1]->setTick(1); speed=5;
	   	 	 }


	   	 	 // da fare TODO TO DO
	   	 	else
	   	 	 {
	   	 	  grmax_life=56;
	   	 	  //frame_offset=99; min_frame=100; max_frame=101;
	   	 	  frame_offset=min_frame=max_frame=0; // Non buono per le collisioni poichè il frame 0 è vuoto
	   	 	  grframe_min=102; grframe_max=120;
	   	 	  ti[1]->setTick(1); speed=index%multi;
	   	 	 }
	   	 	 
	   	 	 
	   	 	 
	   	 	 
	   	 }
	   	else
	   	 {
	   	 	multi=0;
	   	 	grmax_life=36;
	   	 	frame_offset=74; min_frame=75; max_frame=79;
	   	 	grframe_min=80; grframe_max=89;
	   	 	ti[1]->setTick(2); speed=4;
	   	 }
	   	pos_x=px; pos_y=py+2; act_frame=dir+1+frame_offset;
	   }
	  life=max_life;
	  power=max_power;
	  plat_layer=plat_lay;
	  ti[1]->reset(); // Resetta timer armi animate
	  ti[2]->reset(); // Resetta gravity timer
	 }
 }

void weapon::hit(int fl)
 {
 	if(fl) power--; //if(power<0) power=0;
 	if(power==0)
 	 {
 	  fire=0;
 	  life=0;
 	  can_hit=0;
 	  pos_x=0; pos_y=0;
 	  act_frame=0;
 	 }
 }

void weapon::hitSoft()
 {
 	//play_sample(sound[0],120,150,1000,0);
 	soft_frame=50;
 	power--; if(power==0) can_hit=0;
 	soft=1;
 	hit_x=pos_x-23; hit_y=pos_y-12; // Aggiustare queste coordinate per SWORD
 	hit(0);
 }

void weapon::hitHard()
 {
 	play_sample(sound[0],120,150,1000,0);
 	hard_frame=25;
 	
 	// Aggiusta animazione spada per hitHard
 	if(!no_zeropower) power=0;
 	
 	can_hit=0;
 	hard=1;
 	hit_x=pos_x-23; hit_y=pos_y-12; // Aggiustare queste coordinate per SWORD
 	hit(0);
 }

void weapon::hitGround(int plat_y)
 {
 	// L'arma ha colpito il terreno, cambierà stato ed animazione
 	play_sample(sound[1],120,150,1000,0); // Suono per fiamma (TODO: diversificare per arma TODO)
 	status=1; // status==0 non ha ancora colpito il terreno, status==1 ha colpito il terreno
 	pos_y=plat_y-7;
 	act_frame=min_frame=grframe_min; max_frame=grframe_max;
 	life=grmax_life;
 	gravity=0; speed_d=0; speed=0; dx=0;
 }

bool weapon::isFired() { return fire; }

int weapon::getMulti() { return multi; }

void weapon::debug()
 {
 	putpixel(out,pos_x,pos_y-30+58,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+59,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+61,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+62,makecol32(255,0,0));
  putpixel(out,pos_x-2,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x+2,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x-1,pos_y-30+61,makecol32(255,0,0));
  putpixel(out,pos_x+1,pos_y-30+61,makecol32(255,0,0));
 }
