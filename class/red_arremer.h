class red_arremer : public _sprite
 {
  private:
   bool have_vase;
   int pos_x,pos_y,scroll_x,scroll_y,jump_y;
   int plat_y,plat_stair,plat_wall;
   int *ctrl_x,*ctrl_y;
   int dx,dy;
   int speed,energy;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool can_hit,hittable;
   bool stop,move,fly,dead,attack,jump_up,jump_down,down,fire1,fire2,fall,turn,rise,unrise,hitted,explode,sleep;
   timer *ti[5];
   SAMPLE *sound[4];
   bonus *vase;
   map *plat_map;
   map *hight_map;
   map *back_map;

  public:
   red_arremer(char *,int,int,BITMAP *,int i=0,red_arremer *p=NULL);
   ~red_arremer();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,red_arremer *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getPlatYPos();
   void attachVase(bonus *);
   void setHaveVase(bool);
   bool getHaveVase();
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   int getScore();
   int *getPosXP();
   int *getPosYP();
   void setHitted(bool);
   bool canHit();
   bool isHittable();
   bool isSleep();
   void resetObj(bool);
   void debug();
 };

red_arremer::red_arremer(char *filename, int sx, int sy, BITMAP *bmp, int i, red_arremer *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  resetObj(true);
  ti[0]=new timer(3);   // Anim time
  ti[1]=new timer(280+random(80)); // Attack time
  ti[2]=new timer(400+random(150)); // Move time
  ti[3]=new timer(2);   // Explode time
  ti[4]=new timer(3);   // Stop time
 }

red_arremer::~red_arremer()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<2;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void red_arremer::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void red_arremer::assignSample(int n,red_arremer *p)
 {
  sound[n]=p->sound[n];
 }

void red_arremer::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void red_arremer::animSprite()
 {
 	if(!enabled) return;
 	if(dead) return;
 	if(explode) 
 	 {
 	 	if(ti[3]->isOn()) act_frame++;
 	 	if(act_frame==13)
 	 	 {
 	 	  act_frame=13;
 	 	  explode=0;
 	 	  dead=1;
 	 	  move=0; dy=0; dx=0;
 	 	 }
 	 	return;
 	 }
 	if(hitted) { play_sample(sound[0],120,150,1000,0); dy=0; hitted=0; act_frame=5; energy--; if(energy==0) { explode=1; play_sample(sound[1],120,150,1000,0); }else explode=0; move=0; attack=0; fly=1; if(sleep) dy-=3; sleep=0; stop=1; ti[4]->reset(); return; }
 	if(sleep)  if(abs(*ctrl_x-pos_x)<100 && abs(*ctrl_y-pos_y)<80) { fly=1; sleep=0; }
 	if(sleep)  act_frame=0;
 	if(fly)
 	 {
 	 	//textprintf_ex(screen,font,0,0,makecol32(255,0,0),-1,"ABS: %d pos_x:%d ctrl_x:%d",abs(*ctrl_x-pos_x),pos_x,*ctrl_x);
 	  if(ti[0]->isOn()) { act_frame++; if(act_frame>4) act_frame=1; }
 	  if(*ctrl_x<pos_x) side=1; else side=0;
 	 	if(!move && !attack)
 	 	 {
 	 	  if(abs(*ctrl_y-pos_y)>60) dy=0; else dy=-3;
 	 	  if(abs(*ctrl_y-pos_y)>130) dy=3;
 	    if(abs(*ctrl_x-pos_x)<120) { if(*ctrl_x<pos_x) dx=1; else dx=-1; } else dx=0;
 	    if(abs(*ctrl_x-pos_x)>150) { if(*ctrl_x<pos_x) dx=-1; else dx=+1; }
 	    if(abs(*ctrl_x-pos_x)<18)  { move=1; dx=-dx*3; }
 	    if(ti[2]->isOn())  { move=1; attack=0; if(*ctrl_x>pos_x) dx=3; else dx=-3; }
 	    if(ti[1]->isOn())  { attack=1; move=0; dx=2*(*ctrl_x/pos_x+1); dy=2*(*ctrl_y/pos_y+1); if(*ctrl_x<pos_x) dx=-dx; }
 	   }
 	  if(move)   { if(abs(*ctrl_x-pos_x)>140) { move=0; attack=0; stop=0; } } // Move left or right (in flight)
 	  if(attack)
 	   {
 	   	if(*ctrl_y<pos_y) { attack=0; move=0; stop=0; }
 	   }
 	 }
 	if(stop) { dx=0;dy=0; act_frame=5; if(ti[4]->isOn()) stop=0; attack=0; move=0; }
  pos_y+=dy;
  pos_x+=dx;
 	//if(!life || plat_wall) { unrise=1; move=0; rise=0; act_frame=8; }
 	/*if(!rise && !move && !unrise && !hitted && !explode)
 	 { play_sample(sound[0],120,150,1000,0); rise=1; act_frame=0; }*/
 	//if(!move) { move=1; act_frame=8; }
 	/*if(climb) // Sulla scala
 	 {
    if(key[KEY_UP])           { pos_y-=2; }
    if(key[KEY_DOWN])         { pos_y+=2; }
    if(plat_stair==0)         { climb=0; }
    jump_up=0; jump_down=0; fire1=0; fire2=0;
    return;
 	 }
 	if(!jump_up && !jump_down && !fire1 && !fire2 && !climb) // Al suolo
 	 {
    if(!key[KEY_LEFT])        { move=0; stop=1; }
    if(!key[KEY_RIGHT])       { move=0; stop=1; }
    if(!key[KEY_DOWN])        { down=0; stop=1; }
    if(key[KEY_LEFT]  && !key[KEY_RIGHT]) { move=1; down=0; stop=0; if(!side) turn=1; direction=1; dx=-2; }
    if(key[KEY_RIGHT] && !key[KEY_LEFT])  { move=1; down=0; stop=0; if(side)  turn=1; direction=0; dx=2;  }
    if(key[KEY_LEFT]  &&  key[KEY_RIGHT]) { move=1; down=0; stop=0; if(side)  turn=1; direction=0; dx=2;  }
    if(key[KEY_UP] &&
       plat_stair==1)         { climb=1; stop=0; move=0; act_frame=18; plat_map->setScrollX((plat_map->getActTileX()-6)*plat_map->getTileW()+8); hight_map->setScrollX(plat_map->getScrollX()); back_map->setScrollX(plat_map->getScrollX()/2); }
    if(key[KEY_DOWN])         { 
    	                          if(plat_stair==2) { climb=1; stop=0; move=0; act_frame=23; plat_map->setScrollX((plat_map->getActTileX()-6)*plat_map->getTileW()+8); hight_map->setScrollX(plat_map->getScrollX()); back_map->setScrollX(plat_map->getScrollX()/2); }
    	                          else { down=1; move=0; stop=0; }
    	                        }
    if(key[KEY_ALT] && !fall) { jump_up=1; down=0; dy=-4; key[KEY_ALT]=0; jump_y=0;}
    if(key[KEY_LCONTROL])     { 
    											     move=0; stop=0;
    											     if(!down)       { fire1=1; fire2=0; act_frame=10; }
    											     if(down)        { fire2=1; fire1=0; act_frame=12; }
    											     if(key[KEY_UP]) { fire1=0; fire2=0; act_frame=14; }
    	                         down=0;
    	                         play_sample(sound[0],120,150,1000,0);
    	                         key[KEY_LCONTROL]=0;
    	                        }
   }
  if((jump_up || jump_down) && !fire1 && !fire2 && !climb) // In volo
 	 {
    if(!key[KEY_LEFT])    { stop=1; }
    if(!key[KEY_RIGHT])   { stop=1; }
    if(!key[KEY_DOWN])    { down=0; stop=1; }
    if(key[KEY_LEFT]  && !key[KEY_RIGHT]) { if(!side) turn=1; }
    if(key[KEY_RIGHT] && !key[KEY_LEFT])  { if(side)  turn=1; }
    if(key[KEY_LEFT]  &&  key[KEY_RIGHT]) { if(side)  turn=1; }
    if(key[KEY_UP])       { }
    if(key[KEY_DOWN])     { }
    if(key[KEY_ALT])      { key[KEY_ALT]=0; }
    if(key[KEY_LCONTROL]) { 
    											 stop=0;
    											 if(!down)         { fire1=1; fire2=0; act_frame=10; }
    											 if(down)          { fire2=1; fire1=0; act_frame=12; }
    											 if(key[KEY_UP])   { fire1=0; fire2=0; act_frame=14; }
    											 if(key[KEY_DOWN]) { fire1=0; fire2=0; act_frame=16; }
    	                     down=0;
    	                     play_sample(sound[0],120,150,1000,0);
    	                     key[KEY_LCONTROL]=0;
    	                    }
   }*/
 }

void red_arremer::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(act_frame>=0 && act_frame<6) can_hit=true; else can_hit=false;
  if(act_frame<6) hittable=true; else hittable=false;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void red_arremer::platformSprite()
 {
 	if(!enabled) return;
 	if(hitted || explode) return;
 	plat_wall=0;
 	if(!jump_down && !jump_up && !rise && !unrise && !hitted && !explode)
 	 {
 	  if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
 	 }
 	plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	if(!unrise) plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
 	plat_stair=0;//plat_map->getActStairEnemies(1);
 }

BITMAP *red_arremer::getActFrameColP() { return frames_collision[act_frame]; }

bool red_arremer::inScreen()
 {
 	if(X>=-32 && X<=384) return true;
 	else return false;
 }

int red_arremer::getPlatYPos() { return plat_y; }

int red_arremer::getScore() { if(energy==1) { if(have_vase) { vase->setAttached(false); have_vase=0; } return 300; } else return 0; } //*

void red_arremer::setCtrlXP(int *p) { ctrl_x=p; }

void red_arremer::setCtrlYP(int *p) { ctrl_y=p; }

int *red_arremer::getPosXP() { return &pos_x; }

int *red_arremer::getPosYP() { return &pos_y; }

void red_arremer::setPlatformMap(map *p) { plat_map=p; }

void red_arremer::setHightMap(map *p) { hight_map=p; }

void red_arremer::setBackgroundMap(map *p) { back_map=p; }

void red_arremer::setHitted(bool b) { hitted=b; }

bool red_arremer::canHit() { return can_hit; }

bool red_arremer::isHittable() { return hittable; }

bool red_arremer::isSleep() { return sleep; }

void red_arremer::setHaveVase(bool b) { have_vase=b; }

bool red_arremer::getHaveVase() { return have_vase; }

void red_arremer::attachVase(bonus *p)
 {
  if(random(500)==0 && isEnabled() && isSleep() && !getHaveVase() && !inScreen())
   {
    vase=p;
    vase->resetObj(true,11);
    vase->setVaseX(getPosXP()); vase->setVaseY(getPosYP());
    vase->setVasepx(0); vase->setVasepy(8);
    vase->setAttached(true);
    setHaveVase(true);
   }
 }

void red_arremer::resetObj(bool enb)
 {
  enabled=enb;
  have_vase=false; // No vase at start
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  dx=0; dy=0; plat_y=start_y;
  act_frame=0;
  energy=5;
  stop=0; move=0; fly=0; attack=0; dead=0; jump_up=0; jump_down=0; down=0; fire1=0; fire2=0; fall=0; turn=0; rise=0; unrise=0; hitted=0; explode=0; sleep=1;
  side=1; direction=0;
  plat_wall=0; 	
 }

void red_arremer::debug()
 {
  putpixel(screen,pos_x,pos_y-30+58,makecol32(255,0,0));
  putpixel(screen,pos_x,pos_y-30+59,makecol32(255,0,0));
  putpixel(screen,pos_x,pos_y-30+60,makecol32(255,0,0));
  putpixel(screen,pos_x,pos_y-30+61,makecol32(255,0,0));
  putpixel(screen,pos_x,pos_y-30+62,makecol32(255,0,0));
  putpixel(screen,pos_x-2,pos_y-30+60,makecol32(255,0,0));
  putpixel(screen,pos_x+2,pos_y-30+60,makecol32(255,0,0));
  putpixel(screen,pos_x-1,pos_y-30+61,makecol32(255,0,0));
  putpixel(screen,pos_x+1,pos_y-30+61,makecol32(255,0,0));
 }
