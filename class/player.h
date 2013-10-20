class player : public _sprite
 {
  private:
   enum { N_WE=52 };
   int KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYJUMP,KEYSHOOT; // Move player keyboard config
   int plat_layer;
   int max_we;
   int pos_x,pos_y,glob_x,glob_y,scroll_x,scroll_y,jump_y,restart_x,restart_y;
   int scroll_center_x,scroll_center_y,max_x,max_y;
   int *stage,*lives,*score,*ckpoint,*interlude,*epilogue,n_stage;
   int armo_x[6],armo_y[6];
   int shild_x[2],shild_y[2];
   int plat_y,plat_stair,plat_wall;
   int dx,dy;
   int dress,armour_count,shild_count,gold_frame,bigfrm,gauge_frame;
   bonus *bshild;
   int gauge;
   bool magicpw;
   bool canFire,side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool stop,move,gold,magicfire,magicshoot,jump_up,jump_down,jump_hit_up,jump_hit_down,down,climb,fire1,fire2,fire_down,fire_up,fall,turn,dead,restart,ready_go;
   bool hittable,hitted,armour_down,armour_side,shild_down,shild_side;
   bool shild,powerup;
   bool key_taked;
   BITMAP **frames_col[2]; // Mirror collision frames for player
   timer *ti[8];
   weapon *weap[N_WE];
   magic  *magi;
   SAMPLE *sound[15];
   map *plat_map;
   map *hight_map;
   map *back_map;
   osd *_osd;

   void addGlobY(int);
   void addGlobX(int);
   void shootWeapon();
   void shootMagic();
   void resetArmour();
   void resetShild();
   void moveArmour();
   void moveShild();
 	
  public:
   player(char *,int,int,BITMAP *,int i=0, player *p=NULL);
   ~player();
   void drawSprite();
   void drawShild(bool);
   void drawCloak();
   void drawGauge();
   void timeSprite();
   void platformSprite();
   void moveSprite();
   void animSprite();
   void setOSD(osd *);
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void takeBonus(int,int extra=0);
   void loadSample(char *,int);
   void resetPlayer(int,int,bool first=false);
   BITMAP *getActFrameColP(bool sh=false);
   int getActPlatLayer();
   bool *getMagicpwP();
   int *getMagicGaugeP();
   weapon *getWeapon(int);
   magic *getMagic();
   void setScrollCenterX(int);
   void setScrollCenterY(int);
   void setNStage(int);
   void setStageP(int *);
   void setInterludeP(int *);
   void setEpilogueP(int *);
   void setCkPointP(int *);
   void setLivesP(int *);
   void setScoreP(int *);
   void setShildP(bonus *);
   void setShildHit(bool);
   void setShild(bool);
   void setDress(int);
   void setSide(int);
   void setKeyboardLayout(int,int,int,int,int,int);
   int getKeyShoot();
   int getKeyJump();
   bool getReadyGo();
   bool getShild();
   bool getPowerUp();
   bool getKeyTaked();
   int getDress();
   int getPlatYPos();
   int getActWeapon();
   int getPosX();
   int getPosY();
   int getNWeapon();
   int getNMagic();
   int getGlobX();
   int getGlobY();
   int *getScrollXP();
   int *getScrollYP();
   int *getGlobXP();
   int *getGlobYP();
   BITMAP *getFrame(int);
   void setHitted(bool);
   bool isHittable();
   bool isGold();
   bool isMagicFire();
   void calcMax();
   void debug();
 };

player::player(char *filename, int rsx, int rsy, BITMAP *bmp, int i, player *p) : _sprite(filename,bmp,rsx,rsy,i,p)
 {
  BITMAP *aux;
  char str[255];

  // Load extra data and mirror collisions frame
  aux = load_bmp(strcat(strcpy(str,filename),".bmp"),NULL);
  if(!aux) { allegro_message("File \"%s\" not found.\n\nGame closed!",str); exit(0); }

  frames_col[0] = frames_collision;
  frames_col[1] = (BITMAP **)malloc(sizeof(BITMAP *)*(rows*columns));

  for(int i=0;i<rows;i++)
   for(int j=0;j<columns;j++)
    {
     frames_col[1][j+columns*i] = create_bitmap_ex(8,width,height);
     clear_to_color(frames_col[1][j+columns*i],0);
     for(int h=0;h<frames[j+columns*i]->h;h++)
      for(int w=0;w<frames[j+columns*i]->w*4;w+=4)
       if(makecol32(frames[j+columns*i]->line[h][w+2],
                    frames[j+columns*i]->line[h][w+1],
                    frames[j+columns*i]->line[h][w]) != 16711935) frames_col[1][j+columns*i]->line[h][(((frames[j+columns*i]->w-1)*4)>>2)-(w>>2)] = 1;
    }
  bigfrm = 175;
  frames[bigfrm]=create_bitmap(104,59);
  blit(aux,frames[bigfrm],0,541,0,0,frames[bigfrm]->w,frames[bigfrm]->h);
  frames_col[0][bigfrm]=create_bitmap_ex(8,104,59);
  frames_col[1][bigfrm]=create_bitmap_ex(8,104,59);
  clear_to_color(frames_col[0][bigfrm],0);
  clear_to_color(frames_col[1][bigfrm],0);
  for(int h=0;h<frames[bigfrm]->h;h++)
   for(int w=0;w<frames[bigfrm]->w*4;w+=4)
    if(makecol32(frames[bigfrm]->line[h][w+2],
                 frames[bigfrm]->line[h][w+1],
                 frames[bigfrm]->line[h][w])!=16711935)
                  {
                   frames_col[0][bigfrm]->line[h][w>>2]=1;
                   frames_col[1][bigfrm]->line[h][(((frames[bigfrm]->w-1)*4)>>2)-(w>>2)]=1;
                  }
  destroy_bitmap(aux);

  // Reset player  
  powerup=0;
  resetPlayer(rsx,rsy,true);
  // Weapon
  weap[0]=new weapon("data/obj/player/weapon.bmp",screen);
  weap[0]->loadSample("data/sound/fx/hard.wav",0);
  weap[0]->loadSample("data/sound/fx/flame.wav",1);
  weap[0]->setIndex(0);
  max_we=2; // Numero di armi usate per l'arma corrente su un totale di N_WE sprite
  for(int i=1;i<N_WE;i++)
   {
    weap[i]=new weapon("",screen,i,weap[0]);
    weap[i]->assignSample(0,weap[0]);
    weap[i]->assignSample(1,weap[0]);
    weap[i]->setIndex(i);
   }
  for(int i=0;i<N_WE;i++)
   {
    weap[i]->setPlayerXP(&X);
 	weap[i]->setPlayerYP(&Y);
   }
  // Magic
  magi=new magic("data/obj/player/magic.bmp",screen);
  magi->loadSample("data/sound/fx/hard.wav",0);
  magi->loadSample("data/sound/fx/magic01.wav",1);
  magi->loadSample("data/sound/fx/magic01.wav",2);
  magi->loadSample("data/sound/fx/magic03.wav",3);
  magi->setPlayerXP(&X);
  magi->setPlayerYP(&Y);

  ti[0]=new timer(2);   // Move time
  ti[1]=new timer(2);   // Fire time
  ti[2]=new timer(3);   // Flashing time
  ti[3]=new timer(130); // Invisible time
  ti[4]=new timer(4);   // Dead/Gold time
  ti[5]=new timer(16);  // Gold time
  ti[6]=new timer(70);  // Magicfire time (cambiare il tick a secondo dell'arma)
  ti[7]=new timer(1);   // Gauge anim time
 }

player::~player()
 {
  for(int i=0;i<N_WE;i++) delete(weap[i]);
  for(int i=0;i<8;i++)    free(ti[i]);
  for(int i=0;i<14;i++)   destroy_sample(sound[i]);
  delete(magi);
  for(int i=0;i<rows*columns;i++) destroy_bitmap(frames[i]);
  for(int i=0;i<rows*columns;i++) { destroy_bitmap(frames_col[0][i]); destroy_bitmap(frames_col[1][i]); }
  destroy_bitmap(frames[bigfrm]);
  destroy_bitmap(frames_col[0][bigfrm]);
  destroy_bitmap(frames_col[1][bigfrm]);
 }

void player::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void player::resetPlayer(int rx,int ry,bool first)
 {
  restart_x=rx; restart_y=ry;
  magicpw=0;
  gauge=0;
  canFire=1;
  act_frame=0; dress=0; // 0=Armour, 1=Naked 2=Magic armour 3=Dead
  gauge_frame=19;
  gold=0; magicfire=0; magicshoot=0; stop=1; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire1=0; fire2=0; fire_down=0; fire_up=0; fall=0; turn=0; dead=0; restart=0; ready_go=0;
  hittable=1; hitted=0; armour_down=0; armour_count=0; shild_down=0; shild_count=0; jump_hit_up=0; jump_hit_down=0;
  side=0; direction=0;
  dx=0; dy=0; jump_y=0;
  shild=0; shild_side=0; armour_side=0; key_taked=0;
  scroll_center_x=192; scroll_center_y=112;
  scroll_x=0; scroll_y=0;
  pos_x=glob_x=restart_x; pos_y=glob_y=restart_y; plat_y=restart_y;
  if(!first) { platformSprite(); drawSprite(); }
 }

void player::timeSprite()
 {
  for(int i=0;i<8;i++) ti[i]->time();
 }

void player::takeBonus(int bonus, int extra)
 {
  if(bonus==1) { if(dress==2) dress=2; else dress=0; play_sample(sound[4],120,150,1000,0); hittable=1; } // Normal armour
  if(bonus==2) { if(dress==0 || dress==2) { gold=1; ti[5]->reset(); ti[4]->reset(); gold_frame=87; act_frame=41; } else dress=0; play_sample(sound[4],120,150,1000,0); hittable=1; } // Gold armour
  if(bonus==3) {
  	            gauge=0; magicfire=0; stop_sample(sound[7]); // Azzera magic power se cambio arma
  	            for(int i=0;i<N_WE;i++) max_we=weap[i]->setActWeapon(extra,powerup); _osd->setActWeapon(extra,powerup); magi->setActWeapon(extra);
  	           } // Change Weapon
  if(bonus==4) { play_sample(sound[6],120,150,1000,0); if(extra==0) *score+=200; if(extra==1) *score+=500; if(extra==2) *score+=800; if(extra==3) *score+=1000; if(extra==3) *score+=100; } // Bonus punti
  if(bonus==5) shild=1; // Scudo
  if(bonus==7) { powerup=1; for(int i=0;i<N_WE;i++) max_we=weap[i]->setActWeapon(getActWeapon(),powerup); _osd->setActWeapon(getActWeapon(),powerup);  magi->setActWeapon(getActWeapon()); } // Powerup armi
  if(bonus==10) { (*stage)++; *ckpoint=0; if(*stage==8) *interlude=1; if(*stage==n_stage) { *epilogue=1; (*stage)--; }if(*stage>=8 && *stage<=14) { glob_x=4000; glob_y=0; } else { glob_x=0; glob_y=0; } restart=1; key_taked=1; } // Key (next level)
 }

void player::shootWeapon()
 {
  int ind,n=-1;

  //play_sample(sound[8],120,150,1000,0); 
  play_sample(sound[8+getActWeapon()],120,150,1000,0);
  if(!weap[0]->getMulti()) // One bullet a time
   {
    ind=weap[0]->minIndex(getActWeapon(),powerup);
    for(int i=ind;i<max_we+ind;i++) if(!weap[i]->isFired()) { n=i; break; }	
    if(n!=-1)
     {
   	  if(fire1 || fire2) // Normale o abbassato
       if(down)
  	    if(side) weap[n]->shoot(plat_map->getScrollX()+pos_x-4,plat_map->getScrollY()+pos_y+10,side,0,plat_layer,true,false,false,powerup);
  	    else     weap[n]->shoot(plat_map->getScrollX()+pos_x+4,plat_map->getScrollY()+pos_y+10,side,0,plat_layer,true,false,false,powerup);
       else 
        if(side) weap[n]->shoot(plat_map->getScrollX()+pos_x-4,plat_map->getScrollY()+pos_y-4,side,0,plat_layer,false,false,false,powerup);
        else     weap[n]->shoot(plat_map->getScrollX()+pos_x+4,plat_map->getScrollY()+pos_y-4,side,0,plat_layer,false,false,false,powerup);
 	   if(fire_up) // In alto
        weap[n]->shoot(plat_map->getScrollX()+pos_x,plat_map->getScrollY()+pos_y-4,side,1,plat_layer,false,true,false,powerup);
 	   if(fire_down) // In basso
        weap[n]->shoot(plat_map->getScrollX()+pos_x,plat_map->getScrollY()+pos_y+6,side,2,plat_layer,false,false,true,powerup);
     }
   }
  else // Multi bullet a time
   {
   	ind=weap[0]->minIndex(getActWeapon(),powerup);
   	for(int i=ind;i<max_we+ind;i+=weap[0]->getMulti())
   	 {
   	  for(int j=0;j<weap[0]->getMulti();j++)
   	   {
   	    n=i;
   	    if(weap[i+j]->isFired()) { n=-1; break; }    
   	   }
   	  if(n!=-1) break;
   	 }
    if(n!=-1)
 	   {
 	 	  for(int j=0;j<weap[0]->getMulti();j++)
 	 	   {
 	 	    if(fire1 || fire2) // Normale o abbassato
  	     if(down)
  	      if(side) weap[n]->shoot(plat_map->getScrollX()+pos_x-4,plat_map->getScrollY()+pos_y+10,side,0,plat_layer,true,false,false,powerup);
  	      else     weap[n]->shoot(plat_map->getScrollX()+pos_x+4,plat_map->getScrollY()+pos_y+10,side,0,plat_layer,true,false,false,powerup);
         else 
          if(side) weap[n]->shoot(plat_map->getScrollX()+pos_x-4,plat_map->getScrollY()+pos_y-4,side,0,plat_layer,false,false,false,powerup);
          else     weap[n]->shoot(plat_map->getScrollX()+pos_x+4,plat_map->getScrollY()+pos_y-4,side,0,plat_layer,false,false,false,powerup);
 	 	    if(fire_up) // In alto
         weap[n]->shoot(plat_map->getScrollX()+pos_x,plat_map->getScrollY()+pos_y-4,side,1,plat_layer,false,true,false,powerup);
 	 	    if(fire_down) // In basso
         weap[n]->shoot(plat_map->getScrollX()+pos_x,plat_map->getScrollY()+pos_y+6,side,2,plat_layer,false,false,true,powerup);
        n++;
       }
     }
   }
 }

void player::shootMagic() //pollo
 {
  magi->shoot(plat_map->getScrollX()+pos_x-4,plat_map->getScrollY()+pos_y-4,side,0,plat_layer,false,false,false);
 }

void player::resetArmour()
 {
  armour_side=side;
  armour_down=1;
  armour_count=0;
  armo_x[0]=X; armo_y[0]=Y-10;
  armo_x[1]=X; armo_y[1]=Y-10;
  armo_x[2]=X; armo_y[2]=Y-5;
  armo_x[3]=X; armo_y[3]=Y-5;
  armo_x[4]=X; armo_y[4]=Y;
  armo_x[5]=X; armo_y[5]=Y;
 }

void player::moveArmour()
 {
  int ss,dd,gg;
 	
  if(armour_side) ss=-1; else ss=1;
  armour_count++; if(armour_count==85) armour_down=0;
  gg=armour_count/5;
  armo_x[0]-=ss*2; armo_y[0]-=2-gg;
  armo_x[1]+=ss*2; armo_y[1]-=2-gg;
  armo_x[2]-=ss*2; armo_y[2]-=1-gg;
  armo_x[3]+=ss*2; armo_y[3]-=1-gg;
  armo_x[4]-=ss*1; armo_y[4]-=0-gg;
  armo_x[5]+=ss*1; armo_y[5]-=0-gg;
 }

void player::resetShild()
 {
  shild_side=side;
  shild_down=1;
  shild_count=0;
  shild_x[0]=X; shild_y[0]=Y-10;
  shild_x[1]=X; shild_y[1]=Y;
 }

void player::moveShild()
 {
  int ss,dd,gg;
 	
  if(shild_side) ss=-1; else ss=1;
  shild_count++; if(shild_count==85) shild_down=0;
  gg=shild_count/5;
  shild_x[0]-=ss*2; shild_y[0]-=2-gg;
  shild_x[1]+=ss*1; shild_y[1]-=0-gg;
 }

void player::moveSprite()
 {
  if(restart) { if(ti[3]->isOn()) { if(!key_taked) (*lives)--; ready_go=1; } }
  if(Y>224 && !dead) { dead=1; dress=1; setHitted(true); }
  if(dead) return; // Dead

  if(dress==2) magicpw=1; else { magicpw=0; gauge=0; magicfire=0; } // Attiva magia per gold armour
  if(climb) { canFire=0; } // Aggiusta bug "spara a fine scala"
  if(!key[KEYSHOOT]) canFire=1;
  if(magicpw)
   {
   	if(!key[KEYSHOOT]) if(gauge==76 && !climb) { stop_sample(sound[7]); magicfire=1; ti[5]->reset(); ti[4]->reset(); }
   	if(key[KEYSHOOT] && !magicfire)
   	 {
   	  if(!climb)        gauge++; if(gauge>76) gauge=76; if(gauge==25) play_sample(sound[7],120,150,1000,0); }
   	  else { if(!climb) gauge-=15; if(gauge<0) gauge=0; if(gauge<=30 && gauge>5 && !magicfire) { stop_sample(sound[7]); play_sample(sound[0],120,150,1000,0); }
   	 }
   }
  if(magicfire) return;
  if(gold) { gauge=0; magicfire=0; return; } // Nessun aumento della barra mentre ottengo gold armour

// 	if(restart) { if(ti[3]->isOn()) { if(!key_taked) (*lives)--; ready_go=1; } }
// 	if(_Y>224 && !dead) { dead=1; dress=1; setHitted(true); }
// 	if(dead) return; // Dead
 	if(jump_hit_up || jump_hit_down) return; // Hitted
 	if(climb) // Sulla scala
 	 {
    if(key[KEYUP])           { addGlobY(-2); }
    if(key[KEYDOWN])         { addGlobY(2);  }
    if(plat_stair==0)        { climb=0; }
    jump_up=0; jump_down=0; fire1=0; fire2=0; fire_down=0; fire_up=0;
    return;
 	 }
 	if(!jump_up && !jump_down && !fire1 && !fire2 && !fire_down && !fire_up && !climb) // Al suolo
 	 {
      if(!key[KEYLEFT])                                    { move=0; stop=1; }
      if(!key[KEYRIGHT])                                   { move=0; stop=1; }
      if(!key[KEYDOWN])                                    { down=0; stop=1; }
      if(key[KEYLEFT]  && !key[KEYRIGHT] && !key[KEYDOWN]) { move=1; down=0; stop=0; if(!side) turn=1; direction=1; dx=-2; }
      if(key[KEYRIGHT] && !key[KEYLEFT] && !key[KEYDOWN])  { move=1; down=0; stop=0; if(side)  turn=1; direction=0; dx=2;  }
      if(key[KEYLEFT]  &&  key[KEYRIGHT])                  { move=1; down=0; stop=0; if(side)  turn=1; direction=0; dx=2;  }
      if(key[KEYUP]    &&  plat_stair==1)                  { climb=1; stop=0; move=0; act_frame=18; glob_x=(glob_x/plat_map->getTileH()*plat_map->getTileH())+16; }
      if(key[KEYDOWN]) {
    	                if(plat_stair==2) { climb=1; stop=0; move=0; act_frame=23; glob_x=(glob_x/plat_map->getTileH()*plat_map->getTileH())+16; }
    	                else              {
							               if(key[KEYLEFT])  { if(!side) turn=1; }
    	                                   if(key[KEYRIGHT]) { if(side)  turn=1; }
    	                                   down=1; move=0; stop=0;
    	                                  }
    	               }
      if(key[KEYJUMP] && !fall) { jump_up=1; down=0; dy=-4; key[KEYJUMP]=0; jump_y=0; }
      if(key[KEYSHOOT] && canFire) {
    	                            move=0; stop=0;
    								if(!down)      { fire1=1; fire2=0; fire_up=0; act_frame=10; }
    								if(down)       { fire2=1; fire1=0; fire_up=0; act_frame=12; }
    								if(key[KEYUP]) { fire_up=1; fire1=0; fire2=0; act_frame=14; }
    	                            shootWeapon();
    	                            down=0;
    	                            canFire=0;
    	                            //key[KEYSHOOT]=0;
    	                           }
    }
  if((jump_up || jump_down) && !fire1 && !fire2 && !fire_down && !fire_up && !climb) // In volo
 	 {
    if(!key[KEYLEFT])    { stop=1; }
    if(!key[KEYRIGHT])   { stop=1; }
    if(!key[KEYDOWN])    { down=0; stop=1; }
    if(key[KEYLEFT]  && !key[KEYRIGHT]) { if(!side) turn=1; }
    if(key[KEYRIGHT] && !key[KEYLEFT])  { if(side)  turn=1; }
    if(key[KEYLEFT]  &&  key[KEYRIGHT]) { if(side)  turn=1; }
    if(key[KEYUP])       { }
    if(key[KEYDOWN])     { }
    if(key[KEYJUMP])      { key[KEYJUMP]=0; }
    if(key[KEYSHOOT] && canFire) {
                                  stop=0;
    							  if(!down)         { fire1=1; fire2=0; fire_up=0; fire_down=0; act_frame=10; }
    							  if(down)          { fire2=1; fire1=0; fire_up=0; fire_down=0; act_frame=12; }
    							  if(key[KEYUP])   { fire_up=1; fire1=0; fire2=0; fire_down=0; act_frame=14; }
    							  if(key[KEYDOWN]) { fire_down=1; fire_up=0; fire1=0; fire2=0; act_frame=16; }
                                  shootWeapon();
                                  down=0;
                                  canFire=0;
                                  //key[KEYSHOOT]=0;
                                 }
   }
 }

void player::animSprite()
 {
  // Hit hard se "tocco" il terreno con le armi (solo per moto a sinistra e destra)
  // Fare evantuali aggiustamente (se servono nelle varie stage) cambiando il valore di alt
  int alt=999;

  for(int i=0;i<N_WE;i++) if(weap[i]->isFired())
   {
   	if(weap[i]->getY()+scroll_y > plat_map->getActPlatLayerValue(weap[i]->getX()+scroll_x+20,weap[i]->getY()+scroll_y,weap[i]->getPlatLayer(),alt)+10 && weap[i]->getMoveDir()==0)
   	 weap[i]->hitHard();
   }

  if(restart) return;
  if(gold && !jump_down && !jump_up && !fall)
   {
   	dress=2;
   	if(act_frame<41) { act_frame=41; gold_frame=87; ti[4]->reset(); ti[5]->reset(); }
   	if(ti[5]->isOn()) act_frame++; if(act_frame==44) { gold=0; act_frame=41; }
   	if(act_frame<=42) if(ti[4]->isOn()) { gold_frame++; if(gold_frame==89) gold_frame=87; }
   	if(act_frame>42)  if(ti[4]->isOn()) gold_frame++;
   	return;
   }

  if(magicfire && !jump_down && !jump_up && !fall && !climb)
   {
    shootMagic();
 	if(act_frame<43) { act_frame=43; ti[6]->reset(); ti[6]->setTick(magi->getTick()); }
 	if(ti[6]->isOn()) act_frame++; if(act_frame==44) { magicfire=0; act_frame=43; }
 	return;
   }

  if(dead)
   {
    if(ti[4]->isOn() && act_frame>21) { act_frame--; ti[3]->reset(); }
   	if(ti[3]->isOn()) { ti[3]->reset(); act_frame=20; restart=1; }
  	return;
   }
  if(hitted)
   {
   	stop=1; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire1=0; fire2=0; fire_down=0; fire_up=0; fall=0; turn=0;
   	if(dress==1) dress=3;
  	if(dress==0) { dress=1; resetArmour(); }
  	if(dress==2) { dress=0; resetArmour(); }
  	hittable=0; hitted=0; direction=!side;
  	jump_hit_up=1; jump_hit_down=0; jump_y=0; dy=-4; dx=2; act_frame=24;
  	if(shild)
  	 { shild=0; bshild->setStartX(glob_x); bshild->setStartY(glob_y-38); bshild->resetObj(true,5); }
  	ti[3]->reset();
   }
  if(!hittable) { if(ti[3]->isOn()) hittable=1; }
  if(jump_hit_down)
   {
    if(!plat_wall) { if(side) addGlobX(dx); else addGlobX(-dx); }
    addGlobY(dy);
    if(glob_y>=plat_y && dress==3) { dead=1; glob_y=plat_y; return; }
    if(glob_y>=plat_y) { jump_hit_up=0; jump_hit_down=0; dy=0; glob_y=plat_y; play_sample(sound[1],120,150,1000,0); return; }
    if(glob_y%5==0 && dy<7) dy++; // Gravità
    if(dress==3) if(ti[4]->isOn() && act_frame>21) act_frame--; // Dead
    return;
   }
  if(jump_hit_up)
   {
    if(!plat_wall) { if(side) addGlobX(dx); else addGlobX(-dx); }
    jump_y++; if(jump_y==21) { jump_hit_up=0; jump_hit_down=1; dy=2; }
    if(jump_y%5==0 && dy<7) dy++; // Gravità
    addGlobY(dy);
    return;
   }
  if(turn) { if(plat_wall) dx=0; side=!side; turn=0; }
  if(stop) act_frame=0;
  if(climb)
   {
   	if(act_frame<18) act_frame=18;
   	if(key[KEYUP] || key[KEYDOWN]) if(ti[1]->isOn()) { act_frame++; if(act_frame>21) act_frame=18; }
   	jump_up=0; jump_down=0; fire1=0; fire2=0; fire_down=0; fire_up=0;
   	return;
   }
  if(move && !jump_up && !jump_down)
   {
   	if(!plat_wall) addGlobX(dx);
   	if(ti[0]->isOn()) { act_frame++; if(act_frame>6) act_frame=1; }   	
   }
  if(jump_down)
   {
    if(!fire1 && !fire2 && !fire_up && !fire_down)
     if(move && !fire1 && !fire2 && !fire_up && !fire_down) act_frame=8; else act_frame=9;
 	if(move) if(!plat_wall) addGlobX(dx);
 	if(glob_y>=plat_y) { jump_up=0; jump_down=0; move=0; dy=0; glob_y=plat_y; play_sample(sound[1],120,150,1000,0); return; }
 	if(glob_y%5==0 && dy<7) dy++; // Gravità
   }
  if(jump_up)
   {
  	if(!fire1 && !fire2 && !fire_up && !fire_down)
     if(move) act_frame=8; else act_frame=9;
 	if(move) if(!plat_wall) addGlobX(dx);
 	jump_y++; if(jump_y==21) { jump_up=0; jump_down=1; dy=2; }
 	if(jump_y%5==0 && dy<7) dy++; // Gravità
   }
  if(down) act_frame=7;
  if(fall && !fire1 && !fire2 && !fire_up && !fire_down) { act_frame=0; }
  if(fire1)     { if(ti[1]->isOn()) { act_frame++; if(act_frame>11) { act_frame=11; fire1=0;     } } }
  if(fire2)     { if(ti[1]->isOn()) { act_frame++; if(act_frame>13) { act_frame=13; fire2=0;     } } }
  if(fire_up)   { if(ti[1]->isOn()) { act_frame++; if(act_frame>15) { act_frame=15; fire_up=0;   } } }
  if(fire_down) { if(ti[1]->isOn()) { act_frame++; if(act_frame>17) { act_frame=17; fire_down=0; } } }
  addGlobY(dy);
 }

void player::drawSprite()
 {
  pos_x=scroll_center_x;
  if(glob_x<scroll_center_x) pos_x=glob_x; 
  if(scroll_x==max_x) pos_x=glob_x-scroll_x;
  pos_y=scroll_center_y;
  if(glob_y<scroll_center_y) pos_y=glob_y;
  if(scroll_y==max_y) pos_y=glob_y-scroll_y;

  if(glob_x<=scroll_center_x) scroll_x=0; else scroll_x=glob_x-scroll_center_x;
  if(scroll_x>max_x) scroll_x=max_x;
  if(glob_y<=scroll_center_y) scroll_y=0; else scroll_y=glob_y-scroll_center_y;
  if(scroll_y>max_y) scroll_y=max_y;

  if(pos_x<22)  { pos_x=22;  glob_x=22; }
  if(pos_x>366) { pos_x=366; glob_x=max_x+366; }
  if(pos_y<0)   { pos_y=0;   glob_y=0; }
  if(pos_y>250) { pos_y=250; glob_y=max_y+250; }
  if(Y<=-5) { glob_y+=2; } // Risolve bug uscita da schermo in alto e non c'è scroll verticale

  X=pos_x-23; Y=pos_y-12;
  if(!hittable && dress!=3) // Flashing (manca ancora l'animazione completa del mantello)
   {
   	if(ti[2]->isOn() || act_frame==24)
     {
   	  drawShild(true);
   	  drawCloak();
      if(side) draw_sprite_h_flip(screen,frames[act_frame+dress*25],X,Y);
      else     draw_sprite       (screen,frames[act_frame+dress*25],X,Y);
      drawShild(false);
     }
   }
  else // Not flashing
   {
   	drawShild(true);
   	drawCloak();
    drawGauge();
   	if(side) draw_sprite_h_flip(screen,frames[act_frame+dress*25],X,Y);
    else     draw_sprite       (screen,frames[act_frame+dress*25],X,Y);
    drawShild(false);
   }

  // Armour hitted
  if(armour_down) moveArmour();
  if(armour_down && armour_side)
   {
    for(int i=0;i<6;i++)
     draw_sprite_h_flip(screen,frames[i+6*dress+3*25],armo_x[i],armo_y[i]);
   }
  if(armour_down && !armour_side)
   {
    for(int i=0;i<6;i++)
     draw_sprite(screen,frames[i+6*dress+3*25],armo_x[i],armo_y[i]);
   }

  // Shild hitted
  if(shild_down) moveShild();
  if(shild_down && shild_side)
   {
    for(int i=0;i<2;i++)
     draw_sprite_h_flip(screen,frames[94],shild_x[i],shild_y[i]);
   }
  if(shild_down && !shild_side)
   {
    for(int i=0;i<2;i++)
     draw_sprite(screen,frames[94],shild_x[i],shild_y[i]);
   }

  // Risolve bug animazione scudo
  if(act_frame==41 && shild) if(side) draw_sprite_h_flip(screen,frames[7+5*25],X,Y);
                             else     draw_sprite       (screen,frames[7+5*25],X,Y);
  // Gold armour taked
  if(gold)
   {
    if(shild)
      {
   	   if(side) { if(act_frame!=8) draw_sprite_h_flip(screen,frames[7+5*25],X,Y); else draw_sprite_h_flip(screen,frames[8+5*25],X,Y); }
       else     { if(act_frame!=8) draw_sprite       (screen,frames[7+5*25],X,Y); else draw_sprite       (screen,frames[8+5*25],X,Y); }
      }
   	if(gold_frame==91) draw_sprite(screen,frames[bigfrm],X-30,Y-14);
   	else draw_sprite(screen,frames[gold_frame],X,Y);
   }

  // Magic power fired
  if(magicfire)
   {
	// Parte di pugno mancante al frame
	if(act_frame==43)
     if(side) draw_sprite_h_flip(screen,frames[act_frame+3*25],X,Y-46);
     else     draw_sprite       (screen,frames[act_frame+3*25],X,Y-46);

    if(shild)
      {
	   if(act_frame==43)
   	    if(side) { if(act_frame!=8) draw_sprite_h_flip(screen,frames[7+5*25],X,Y); else draw_sprite_h_flip(screen,frames[8+5*25],X,Y); }
        else     { if(act_frame!=8) draw_sprite       (screen,frames[7+5*25],X,Y); else draw_sprite       (screen,frames[8+5*25],X,Y); }
      }
   }

  // Weapon
  // ... are in "game.h" file (in player() and layer_s() )
 }

void player::drawShild(bool first)
 {
  int shild_frame;

  shild_frame=act_frame+5*25;
  if(first)
   {
    if(shild && !gold && dress!=3 && act_frame<41 && (act_frame!=0 || act_frame!=7 || act_frame!=9 || act_frame!=18 || act_frame!=19 || act_frame!=20 || act_frame!=21 || act_frame==22 || act_frame==23))
     {
   	  if(side) draw_sprite_h_flip(screen,frames[shild_frame],X,Y);
      else     draw_sprite       (screen,frames[shild_frame],X,Y);
     }
   }
  else
   {
    if(shild && !gold && dress!=3 && act_frame<41 && (act_frame==0 || act_frame==7 || act_frame==9 || act_frame==18 || act_frame==19 || act_frame==20 || act_frame==21 || act_frame==22 || act_frame==23))
     {
   	  if(side) draw_sprite_h_flip(screen,frames[shild_frame],X,Y);
      else     draw_sprite       (screen,frames[shild_frame],X,Y);
     } 	
   }
 }

void player::drawCloak()
 {
  // Mantello
  if(dress==2)
   {
  	if(act_frame>=1 && act_frame<=8 || act_frame==17)
     if(side) draw_sprite_h_flip(screen,frames[act_frame+4*25],X+46,Y);
     else     draw_sprite       (screen,frames[act_frame+4*25],X-46,Y);
    if(act_frame==9)
   	 if(side) draw_sprite_h_flip(screen,frames[act_frame+4*25],X,Y+46);
     else     draw_sprite       (screen,frames[act_frame+4*25],X,Y+46);    
    if(act_frame==16)
   	 if(side) draw_sprite_h_flip(screen,frames[act_frame+4*25],X,Y-46);
     else     draw_sprite       (screen,frames[act_frame+4*25],X,Y-46);    
   }
 }

void player::drawGauge()
 {
  // Animazione aurea magica
  if(gauge==25 || gauge==69) gauge_frame=19;
  if(gauge>30)
   {
   	if(gauge==76)
   	 {
      if(side) draw_sprite_h_flip(screen,frames[gauge_frame+3+4*25],X,Y);
      else     draw_sprite       (screen,frames[gauge_frame+3+4*25],X,Y);
     }
    else
   	 {
      if(side) draw_sprite_h_flip(screen,frames[gauge_frame+4*25],X,Y);
      else     draw_sprite       (screen,frames[gauge_frame+4*25],X,Y);
     }
   }
  if(ti[7]->isOn()) gauge_frame++; if(gauge_frame>21) gauge_frame=19; 	
 }

void player::platformSprite()
 {
  // Get platform, walls and stairs
  int plat_aux;

  if(!jump_down && !jump_up && !climb && !jump_hit_down && !jump_hit_up)
   {
 	if(glob_y>plat_y) if(glob_y-plat_y<=8) glob_y=plat_y; 
 	if(glob_y<plat_y) if(plat_y-glob_y<=8) glob_y=plat_y; 
 	if(glob_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; } 
   }
  plat_y=plat_map->getActPlatform(glob_x,glob_y);
  plat_aux=plat_map->getActPlatformMobile(glob_x,glob_y);
  if(abs(plat_aux-glob_y)<abs(plat_y-glob_y)) plat_y=plat_aux;

  plat_stair=plat_map->getActStair(glob_x,glob_y);

  plat_wall=plat_map->getActWall(direction,glob_x,glob_y);
  if(!plat_wall) plat_wall=plat_map->getActWallMobile(direction,glob_x,glob_y);

  // Gestione collisioni armi terreno
  if(glob_y==plat_y && !fall && !jump_down && !jump_up && !climb && !jump_hit_down && !jump_hit_up)
   {
    if(*stage==0)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);
    if(*stage==1)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,1,true);
    if(*stage==2)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);
    if(*stage==3)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);
    if(*stage==4)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);
    if(*stage==5)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,1,true);
    if(*stage==6)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);
    if(*stage==7)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);
    if(*stage==8)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);//|| STAGE==6
    if(*stage==9)  plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,1,true); //|| STAGE==5
    if(*stage==10) plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);//|| STAGE==4
    if(*stage==11) plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);//|| STAGE==3
    if(*stage==12) plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);//|| STAGE==2
    if(*stage==13) plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,1,true); //|| STAGE==1
    if(*stage==14) plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);//|| STAGE==0
    if(*stage==15) plat_layer=plat_map->getActPlatLayer(glob_x,glob_y,0,false);
   }
 }

BITMAP *player::getActFrameColP(bool sh)
 {
  if(sh) return frames_col[side][act_frame+5*25];
  else   return frames_col[side][act_frame+dress*25];
 }

bool *player::getMagicpwP() { return &magicpw; }

int *player::getMagicGaugeP() { return &gauge; }

int player::getActPlatLayer() { return plat_layer; }

weapon *player::getWeapon(int n) { return weap[n]; }

magic *player::getMagic() { return magi; }

int player::getDress() { return dress; }

bool player::getShild() { return shild; }
                                           
bool player::getPowerUp() { return powerup; }
                                           
bool player::getReadyGo() { return ready_go; }

bool player::getKeyTaked() { return key_taked; }

int player::getPlatYPos() { return plat_y; }

void player::setNStage(int n) { n_stage=n; }

void player::setStageP(int *p) { stage=p; }

void player::setInterludeP(int *p) { interlude=p; }

void player::setEpilogueP(int *p) { epilogue=p; }

void player::setCkPointP(int *p) { ckpoint=p; }

void player::setLivesP(int *p) { lives=p; }

void player::setScoreP(int *p) { score=p; }

void player::setShildP(bonus *p) { bshild=p; }

void player::setScrollCenterX(int sc) { scroll_center_x=sc; }

void player::setScrollCenterY(int sc) { scroll_center_y=sc; }

int player::getActWeapon() { return weap[0]->getActWeapon(); }

int player::getPosX() { return pos_x; }

int player::getPosY() { return pos_y; }

int player::getNWeapon() { return N_WE; }

int player::getNMagic() { return magi->getNSprites(); }

int player::getGlobX() { return glob_x; }

int player::getGlobY() { return glob_y; }

int *player::getScrollXP() { return &scroll_x; }

int *player::getScrollYP() { return &scroll_y; }

int *player::getGlobXP() { return &glob_x; }

int *player::getGlobYP() { return &glob_y; }

BITMAP *player::getFrame(int i) { return frames[i]; }

void player::setHitted(bool fl)
 {
  hitted=fl;
  stop_sample(sound[7]);
  if(dress!=1) play_sample(sound[2],120,150,1000,0);
  else         play_sample(sound[3],120,150,1000,0);
 }

void player::setShildHit(bool b) {play_sample(sound[5],120,150,1000,0); resetShild(); shild=0; }

void player::setShild(bool b) { shild=b; }

void player::setDress(int d) { dress=d; }

void player::setSide(int s) { side=s; }

bool player::isGold() { return gold; }

bool player::isMagicFire() { return magicfire && !jump_down && !jump_up && !fall && !climb; }

bool player::isHittable() { return hittable; }

void player::setOSD(osd *o) { _osd=o; }

void player::setKeyboardLayout(int keyleft,int keyright,int keyup,int keydown,int keyjump,int keyshoot)
 {
  KEYLEFT  = keyleft; KEYRIGHT = keyright;
  KEYUP    = keyup;   KEYDOWN  = keydown;
  KEYJUMP  = keyjump; KEYSHOOT = keyshoot;
 }

int player::getKeyJump()  { return KEYJUMP; }

int player::getKeyShoot() { return KEYSHOOT; }

void player::setPlatformMap(map *p)
 {
  plat_map=p;
  for(int i=0;i<N_WE;i++) weap[i]->setPlatformMap(p); // Setta anche la mappa della piattaforma per le armi
 }

void player::setHightMap(map *p) { hight_map=p; }

void player::setBackgroundMap(map *p) { back_map=p; }

void player::addGlobX(int v) { glob_x+=v; }

void player::addGlobY(int v) { glob_y+=v; }

void player::calcMax()
 {
  // Calcola i limiti entro i quali il giocatore può muoversi
  max_x=plat_map->getTotCol()*plat_map->getTileW()-plat_map->getNumCol()*plat_map->getTileW();
  max_y=plat_map->getTotRow()*plat_map->getTileW()-plat_map->getNumRow()*plat_map->getTileW();
 }

void player::debug()
 {
  putpixel(screen,pos_x,pos_y,makecol32(255,0,255));
  putpixel(screen,pos_x+1,pos_y,makecol32(255,0,0));
  putpixel(screen,pos_x-1,pos_y,makecol32(255,0,0));
  putpixel(screen,pos_x,pos_y+1,makecol32(255,0,0));
  putpixel(screen,pos_x,pos_y-1,makecol32(255,0,0));
 	
  for(int i=0;i<18;i++) putpixel(screen,pos_x,pos_y-30+62-i,makecol32(255,0,0));
  putpixel(screen,pos_x-2,pos_y-30+60,makecol32(255,0,0));
  putpixel(screen,pos_x+2,pos_y-30+60,makecol32(255,0,0));
  putpixel(screen,pos_x-1,pos_y-30+61,makecol32(255,0,0));
  putpixel(screen,pos_x+1,pos_y-30+61,makecol32(255,0,0));
 }
