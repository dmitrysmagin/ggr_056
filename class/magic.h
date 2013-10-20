class magic
 {
  private:
   enum { _COL=24, _ROW=4, _XPIX=48, _YPIX=48, NUM_SPRITE=9, OFF=0, DOWN=1, HORIZ=2, LOAD=3, ON=4, DIAG_UP=5, DIAG_DOWN=6 };
   int index,plat_layer;
   int actWeapon,frame_offset,max_life,max_power;
   int scroll_x,scroll_y,jump_y,hitsoft_x,hitsoft_y,hithard_x,hithard_y,_i;
   int plat_y,plat_stair,plat_wall,power;
   int dx,dy;
   int hard_frame,soft_frame;
   int life,speedx,speedy,movedir,tick;
   int *player_x,*player_y;
   bool animated,two;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool can_hit;
   bool stop,move,jump_up,jump_down,down,climb,fire,fire2,fall,turn,rise,unrise,hard,soft;
   timer *ti[5];
   SAMPLE *sound[4];
   BITMAP *frame[_COL*_ROW];
   BITMAP *frame_col[2][_COL*_ROW];
   BITMAP *out;
   int X[NUM_SPRITE],Y[NUM_SPRITE],POSX[NUM_SPRITE],POSY[NUM_SPRITE],STATO[NUM_SPRITE];
   int ACT_FRAME[NUM_SPRITE],MIN_FRAME[NUM_SPRITE],MAX_FRAME[NUM_SPRITE];
   map *plat_map;
   map *hight_map;
   map *back_map;

   void loadSpriteFrames(char *);
   void assignSpriteFrame(magic *p);
   void setOutBitmap(BITMAP *);
 	
  public:
   magic(char *,BITMAP *,int i=0,magic *p=NULL);
   ~magic();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatLayer(int);
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,magic *);
   BITMAP *getActFrameColP(int);
   bool inScreen();
   int getPlatLayer();
   int getActWeapon();
   int setActWeapon(int);
   int getNSprites();
   void setIndex(int);
   void setPlayerXP(int *);
   void setPlayerYP(int *);
   int getTick();
   int getPlatYPos();
   int getX(int);
   int getY(int);
   int getMoveDir();
   int *getPosXP();
   int *getPosYP();
   void shoot(int,int,bool,int,int,bool,bool,bool);
   void hit(int fl=1);
   void hitSoft(int);
   void hitHard(int);
   bool isFired();
   bool isTwo();
   bool canHit();
   void debug();
 };

magic::magic(char *filename, BITMAP *out, int i, magic *p)
 {
  actWeapon=LANCE; frame_offset=0; power=0; max_power=1; tick=70;
  _i=i;
  if(!_i) loadSpriteFrames(filename); else assignSpriteFrame(p);
  setOutBitmap(out);
  scroll_x=0; scroll_y=0; jump_y=0; hitsoft_x=hitsoft_y=hithard_x=hithard_y=0;
  dx=0; dy=0; //plat_y=start_y;
  for(int i=0;i<NUM_SPRITE;i++) { ACT_FRAME[i]=0; MIN_FRAME[i]=1; MAX_FRAME[i]=4; POSX[i]=0; POSY[i]=0; }
  hard_frame=0; soft_frame=0;
  animated=true; index=0; two=false;
  life=max_life=800; speedx=0; speedy=48;
  stop=1; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire=0; fire2=0; fall=0,turn=0,rise=0,unrise=0; hard=0; soft=0;
  side=0; direction=0; movedir=0; can_hit=1;
  plat_wall=0;
  ti[0]=new timer(1); // Hard time
  ti[1]=new timer(2); // Anim time
  ti[2]=new timer(1); // Move time
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

magic::~magic() 
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(_i==0)
   {
    for(int i=0;i<4;i++)         destroy_sample(sound[i]);
    for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame[i]);
    for(int i=0;i<_COL*_ROW;i++) { destroy_bitmap(frame_col[0][i]); destroy_bitmap(frame_col[1][i]); }
   }
 }

void magic::loadSpriteFrames(char *filename)
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

void magic::assignSpriteFrame(magic *p)
 {
  for(int i=0;i<_ROW*_COL;i++)
   {
    frame[i]=p->frame[i];
    frame_col[0][i]=p->frame_col[0][i];
    frame_col[1][i]=p->frame_col[1][i];
   }
 }

void magic::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void magic::assignSample(int n,magic *p)
 {
  sound[n]=p->sound[n];
 }

void magic::setOutBitmap(BITMAP *bmp) { out=bmp; }

void magic::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void magic::animSprite()
 {
  if(hard) { if(ti[0]->isOn()) hard_frame++; if(hard_frame>27) { hard=0; hard_frame=0; } }
  if(soft) { if(ti[0]->isOn()) soft_frame++; if(soft_frame>29) { soft=0; soft_frame=0; } }
  if(!fire) return;
  for(int i=0;i<NUM_SPRITE-1;i++)
   {
    // pollo
    if(actWeapon==LANCE)
     {
      if(fire) { if(side) dx=-speedx; else dx=speedx; dy=speedy; life--; }
      if(life<=0 || !inScreen()) fire=0;
      if(STATO[i]==DOWN)
       {
        if(ti[2]->isOn())
         {
          POSX[i]+=dx;
	      POSY[i]+=dy;
	     }
	    if(Y[i]>*player_y-24) { Y[i]=0; STATO[i]=LOAD; ACT_FRAME[i]=MIN_FRAME[i]=5; MAX_FRAME[i]=8; }
	   }
 	  if(STATO[i]==LOAD)
 	   {
 	    Y[i]++;
 	    if(Y[i]>=16) { STATO[i]=HORIZ; }
	   }
 	  if(STATO[i]==HORIZ)
 	   {
 	    if(ti[2]->isOn())
 	     {
 	      if(i%2==0) POSX[i]+=dy/2; else POSX[i]-=dy/2;
	      POSY[i]+=dx;
	     }
	    if(X[i]<-48 || X[i]>384+48) STATO[i]=OFF; // Out of the screen
	   }
	 }
    if(actWeapon==AXE)
     {
 	  if(fire) { if(side) dx=-speedx; else dx=speedx; dy=speedy; life--; }
 	  if(life<=0 || !inScreen()) fire=0;
 	  if(STATO[i]==ON)
 	   {
 	    if(ti[2]->isOn())
 	     {
 	      // bho
	     }
	   }
	 }
    if(actWeapon==SWORD)
     {
 	  if(fire) { if(side) dx=-speedx; else dx=speedx; dy=speedy; life--; }
 	  if(life<=0 || !inScreen()) fire=0;
 	  if(STATO[i]==DIAG_UP)
 	   {
        if(i%2==0) POSX[i]+=dx; else POSX[i]-=dx;
        POSY[i]+=dy;
	    if(Y[i]<*player_y-100) { Y[i]=0; STATO[i]=DIAG_DOWN; }// ACT_FRAME[i]=MIN_FRAME[i]=73; MAX_FRAME[i]=74; }
	   }
 	  if(STATO[i]==DIAG_DOWN)
 	   {
 	   	int ddy=dy>>1;
        if(side) if(i%2==0) POSX[i]-=ddy; else POSX[i]+=ddy;
        else     if(i%2==0) POSX[i]+=ddy; else POSX[i]-=ddy;
	    POSY[i]-=dy;
	    if(Y[i]>*player_y-5) { Y[i]=0; STATO[i]=HORIZ;  }// ACT_FRAME[i]=MIN_FRAME[i]=73; MAX_FRAME[i]=74; }
	   }
 	  if(STATO[i]==HORIZ)
 	   {
        if(side) if(i%2==0) POSX[i]-=dy; else POSX[i]+=dy;
        else     if(i%2==0) POSX[i]+=dy; else POSX[i]-=dy;
	    if(X[i]>*player_x-10 && X[i]<*player_x+10) STATO[i]=OFF;
	   }
	 }
   }
 }

void magic::drawSprite(int sx, int sy)
 {
  for(int i=0;i<NUM_SPRITE-1;i++) if(STATO[i]!=LOAD) { if(side) X[i]=-sx+POSX[i]-23+6; else X[i]=-sx+POSX[i]-23; Y[i]=-sy+POSY[i]-12+4; }
  if(side) X[NUM_SPRITE-1]=*player_x+1; else X[NUM_SPRITE-1]=*player_x-3;
  Y[NUM_SPRITE-1]=*player_y-2;
  if(fire)
   {
    // pollo
    if(actWeapon==LANCE)
     {
      for(int i=0;i<NUM_SPRITE;i++)
       {
       	if(STATO[i]!=OFF && STATO[i]!=LOAD)
       	 {
   	      if(animated && ti[1]->isOn()) { ACT_FRAME[i]++; if(ACT_FRAME[i]>MAX_FRAME[i]) ACT_FRAME[i]=MIN_FRAME[i]; }
          if(side) draw_sprite_h_flip(out,frame[ACT_FRAME[i]],X[i],Y[i]);
          else     draw_sprite       (out,frame[ACT_FRAME[i]],X[i],Y[i]);
         }
       }
     }
    if(actWeapon==AXE)
     {
      for(int i=0;i<NUM_SPRITE;i++)
       {
       	if(STATO[i]!=OFF)
       	 {
   	      if(animated && ti[1]->isOn()) { ACT_FRAME[i]++; if(ACT_FRAME[i]>MAX_FRAME[i]) ACT_FRAME[i]=MIN_FRAME[i]; }
          if(side) draw_sprite_h_flip(out,frame[ACT_FRAME[i]],X[i],Y[i]);
          else     draw_sprite       (out,frame[ACT_FRAME[i]],X[i],Y[i]);
         }
       }
     }
    if(actWeapon==SWORD)
     {
      for(int i=0;i<NUM_SPRITE;i++)
       {
       	if(STATO[i]!=OFF)
       	 {
   	      if(animated && ti[1]->isOn()) { ACT_FRAME[i]++; if(ACT_FRAME[i]>MAX_FRAME[i]) ACT_FRAME[i]=MIN_FRAME[i]; }
   	      if(i==NUM_SPRITE-1 && life<600) ACT_FRAME[i]=54;
           if(side)
            {
             if(STATO[i]!=DIAG_UP || Y[i]<*player_y)
           	  if(i%2==0) draw_sprite_h_flip(out,frame[ACT_FRAME[i]],X[i],Y[i]);
           	  else       draw_sprite       (out,frame[ACT_FRAME[i]],X[i],Y[i]);
            }
           else
            {
             if(STATO[i]!=DIAG_UP || Y[i]<*player_y)
           	  if(i%2==0) draw_sprite       (out,frame[ACT_FRAME[i]],X[i],Y[i]);
           	  else       draw_sprite_h_flip(out,frame[ACT_FRAME[i]],X[i],Y[i]);
            }
         }
       }
     }
   }
  if(hard)
   {
    if(side) draw_sprite_h_flip(out,frame[hard_frame],-sx+hithard_x,-sy+hithard_y);
    else     draw_sprite       (out,frame[hard_frame],-sx+hithard_x,-sy+hithard_y);
   }
  if(soft)
   {
    if(side) draw_sprite_h_flip(out,frame[soft_frame],-sx+hitsoft_x,-sy+hitsoft_y);
    else     draw_sprite       (out,frame[soft_frame],-sx+hitsoft_x,-sy+hitsoft_y);
   }
 }

/*void magic::platformSprite()
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
 }*/

void magic::setPlayerXP(int *p) { player_x=p; }

void magic::setPlayerYP(int *p) { player_y=p; }

BITMAP *magic::getActFrameColP(int i) { return frame_col[side][ACT_FRAME[i]]; }

int magic::getPlatLayer() { return plat_layer; }

int magic::getActWeapon() { return actWeapon; }

int magic::getNSprites() { return NUM_SPRITE; }

void magic::setIndex(int i) { index=i; }

void magic::setPlatLayer(int n) { plat_layer=n; }

int magic::setActWeapon(int w)
 {
 	actWeapon=w;
 	return 0;
 	/*if(w==LANCE) return 3; // number of sprites
 	if(w==KNIFE)   return 5; // number of sprites
 	if(w==AXE)	   return 2; // number of sprites
 	if(w==SWORD)   return 4; // number of sprites
	if(w==ARROW)   return 8; // number of sprites*/
 }

bool magic::inScreen()
 {
  return true;
  //int i=0; // pollo
  //if(X[i]>=-32 && X[i]<=384 && Y[i]>-40 && Y[i]<224) return true;
  //else return false;
 }

int magic::getTick() { return tick; }

int magic::getPlatYPos() { return plat_y; }

int magic::getX(int i) { return X[i]; }

int magic::getY(int i) { return Y[i]; }

int *magic::getPosXP() { return &POSX[0]; } // pollo

int *magic::getPosYP() { return &POSY[0]; } // pollo

int magic::getMoveDir() { return movedir; }

void magic::setPlatformMap(map *p) { plat_map=p; }

void magic::setHightMap(map *p) { hight_map=p; }

void magic::setBackgroundMap(map *p) { back_map=p; }

bool magic::canHit() { if(life<max_life) return can_hit; else return false; } // Controllo life per aggirere il bug di riposizionamento arma

void magic::shoot(int px,int py,bool s,int dir,int plat_lay,bool prone,bool fireup,bool firedown)
 {
 	if(!fire)
 	 {
 	 	movedir=dir;
 	 	side=s;
	  fire=1;
	  can_hit=1;

	  if(actWeapon==LANCE)
	   {
	   	play_sample(sound[1],120,150,1000,0);
	   	animated=true; two=false; max_life=800; tick=70;
  	 	frame_offset=0; max_power=1; speedx=0; speedy=48;
	   	for(int i=0;i<NUM_SPRITE;i++)
	   	 {
	   	  MIN_FRAME[i]=1; MAX_FRAME[i]=4;
  	 	  STATO[i]=DOWN;
	   	  POSX[i]=px; POSY[i]=py-((5+i)*48); ACT_FRAME[i]=dir+1+frame_offset+(i%2);
	   	 }
	   	ACT_FRAME[NUM_SPRITE-1]=9; MIN_FRAME[NUM_SPRITE-1]=23; MAX_FRAME[NUM_SPRITE-1]=23;
	   }
	  if(actWeapon==AXE)
	   {
	   	play_sample(sound[2],120,150,1000,0);
	   	animated=true; two=false; max_life=300; tick=74;
  	 	frame_offset=0; max_power=1; speedx=0; speedy=0;
	   	for(int i=0;i<NUM_SPRITE;i++) STATO[i]=ON;

	   	MIN_FRAME[0]=54; MAX_FRAME[0]=58; ACT_FRAME[0]=54; POSY[0]=py-2;   
	   	MIN_FRAME[1]=59; MAX_FRAME[1]=63; ACT_FRAME[1]=59; POSY[1]=py-2;   
	   	MIN_FRAME[2]=30; MAX_FRAME[2]=34; ACT_FRAME[2]=30; POSY[2]=py-2-47;
	   	MIN_FRAME[3]=40; MAX_FRAME[3]=44; ACT_FRAME[3]=40; POSY[3]=py-2-47;
	   	MIN_FRAME[4]=35; MAX_FRAME[4]=39; ACT_FRAME[4]=35; POSY[4]=py-2-47;
	   	if(side)
	   	 { // Controllare le coordinate esatte
  	    POSX[0]=px+48;
  	    POSX[1]=px-47;
  	    POSX[2]=px+48;
  	    POSX[3]=px;
  	    POSX[4]=px-47;
  	   }
  	  else
	   	 {
  	    POSX[0]=px-47;
  	    POSX[1]=px+48;
  	    POSX[2]=px-47;
  	    POSX[3]=px;
  	    POSX[4]=px+48;
  	   }
	   	ACT_FRAME[NUM_SPRITE-1]=49; MIN_FRAME[NUM_SPRITE-1]=49; MAX_FRAME[NUM_SPRITE-1]=53;
	   	POSX[NUM_SPRITE-1]=px; POSY[NUM_SPRITE-1]=py;
	   	STATO[5]=STATO[6]=STATO[7]=OFF;
	   }
	  if(actWeapon==SWORD)
	   {
	   	play_sample(sound[3],120,150,1000,0);
	   	animated=true; two=false; max_life=800; tick=90;
  	 	frame_offset=72; max_power=1; speedx=4; speedy=-4;
	   	for(int i=0;i<NUM_SPRITE;i++)
	   	 {	   	 	
	   	  MIN_FRAME[i]=75; MAX_FRAME[i]=76;
	   	  if(i==4 || i==5) ACT_FRAME[i]=75; else ACT_FRAME[i]=76;
  	 	  STATO[i]=DIAG_UP;
  	 	  if(side)
  	 	   {
	   	    if(i%2==0) { POSX[i]=px+10*i; POSY[i]=py+12*i; }
	   	    else       { POSX[i]=px-10*i; POSY[i]=py+12*i; }
	   	   }
	   	  else
	   	   {
	   	    if(i%2==0) { POSX[i]=px-10*i; POSY[i]=py+12*i; }
	   	    else       { POSX[i]=px+10*i; POSY[i]=py+12*i; }
	   	   }
	   	 }

	   	MIN_FRAME[0]=73; MAX_FRAME[0]=74; ACT_FRAME[0]=frame_offset;
	   	MIN_FRAME[1]=73; MAX_FRAME[1]=74; ACT_FRAME[1]=frame_offset;
	   	ACT_FRAME[NUM_SPRITE-1]=53; MIN_FRAME[NUM_SPRITE-1]=53; MAX_FRAME[NUM_SPRITE-1]=53;
	   }

	  life=max_life;
	  power=max_power;
	  plat_layer=plat_lay;
	  ti[1]->reset(); // Resetta timer per animazione magic
	 }
 }

void magic::hit(int fl)
 {
 	if(fl) power--; //if(power<0) power=0;
 	if(power==0)
 	 {
 	  fire=0;
 	  life=0;
 	  can_hit=0;
 	  POSX[0]=0; POSY[0]=0; // pollo
 	  ACT_FRAME[0]=0;
 	 }
 }

void magic::hitSoft(int i)
 {
  if(!soft)
   {
    soft_frame=28;
 	soft=1;
    hitsoft_x=POSX[i]-23; hitsoft_y=POSY[i]-12;
   }
 }

void magic::hitHard(int i)
 {
 	if(!hard)
 	 {
 	  play_sample(sound[0],120,150,1000,0);
 	  hard_frame=25;
 	  hard=1;
 	  hithard_x=POSX[i]-23; hithard_y=POSY[i]-12;
 	 }
 }

bool magic::isFired() { return fire; }

bool magic::isTwo() { return two; }

void magic::debug()
 {
 	/*putpixel(out,pos_x,pos_y-30+58,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+59,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+61,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+62,makecol32(255,0,0));
  putpixel(out,pos_x-2,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x+2,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x-1,pos_y-30+61,makecol32(255,0,0));
  putpixel(out,pos_x+1,pos_y-30+61,makecol32(255,0,0));*/
 }
