class red_arremer_king : public _sprite
 {
  private:
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
   map *plat_map;
   map *hight_map;
   map *back_map;
   bonus *exit_key;
	
  public:
   red_arremer_king(char *,int,int,BITMAP *,int i=0,red_arremer_king *p=NULL);
   ~red_arremer_king();
   void setExitKeyP(bonus *);
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,red_arremer_king *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getPlatYPos();
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   int *getPosXP();
   int *getPosYP();
   int getScore();
   void setHitted(bool);
   bool canHit();
   bool isHittable();
   void resetObj(bool);
   void debug();
 };

red_arremer_king::red_arremer_king(char *filename, int sx, int sy, BITMAP *bmp, int i, red_arremer_king *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  resetObj(true);
  ti[0]=new timer(3);   // Anim time
  ti[1]=new timer(280+random(80)); // Attack time
  ti[2]=new timer(400+random(150)); // Move time
  ti[3]=new timer(2);   // Explode time
  ti[4]=new timer(3);   // Stop time
 }

red_arremer_king::~red_arremer_king()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<2;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void red_arremer_king::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void red_arremer_king::assignSample(int n,red_arremer_king *p)
 {
  sound[n]=p->sound[n];
 }

void red_arremer_king::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void red_arremer_king::animSprite()
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
 	 	  exit_key->setStartX(pos_x); exit_key->setStartY(pos_y-10); exit_key->resetObj(true,10);
 	 	 }
 	 	return;
 	 }
 	if(hitted) { play_sample(sound[0],120,150,1000,0); dy=0; hitted=0; act_frame=5; energy--; if(energy==0) { explode=1; play_sample(sound[1],120,150,1000,0); }else explode=0; move=0; attack=0; fly=1; if(sleep) dy-=3; sleep=0; stop=1; ti[4]->reset(); return; }
 	if(sleep)  if(abs(*ctrl_x-pos_x)<100 && abs(*ctrl_y-pos_y)<80) { fly=1; sleep=0; }
 	if(sleep)  act_frame=0;
 	if(fly)
 	 {
 	  if(ti[0]->isOn()) { act_frame++; if(act_frame>4) act_frame=1; }
 	  if(*ctrl_x<pos_x) side=1; else side=0;
 	  if(!move && !attack)
 	 	 {
 	 	 	int dxx,dyy;

 	    dxx=abs(*ctrl_x-pos_x);
 	    dyy=abs(*ctrl_y-pos_y);
 	 	  if(dyy>60)  dy=0; else dy=-3;
 	 	  if(dyy>130) dy=3;
 	 	  if(dxx<120) { if(*ctrl_x<pos_x) dx=1; else dx=-1; } else dx=0;
      if(dxx>150) { if(*ctrl_x<pos_x) dx=-1; else dx=+1; }
 	    if(dxx<18)  { move=1; dx=-dx*3; }
 	    if(ti[2]->isOn())  { move=1; attack=0; if(*ctrl_x>pos_x) dx=3; else dx=-3; }
 	    if(ti[1]->isOn())
 	     {
 	     	int ang;

 	     	attack=1; move=0;
 	     	ang=angle2P(pos_x,pos_y,*ctrl_x,*ctrl_y);
 	     	dx=fixtoi(fixmul(itofix(speed),(fixcos(itofix(ang)))));
 	     	dy=-fixtoi(fixmul(itofix(speed),(fixsin(itofix(ang)))));
 	     }
 	   }
 	  if(move)   { if(abs(*ctrl_x-pos_x)>140) { move=0; attack=0; stop=0; } } // Move left or right (in flight)
 	  if(attack)
 	   {
 	   	if(*ctrl_y+16<pos_y) { attack=0; move=0; stop=0; }
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

void red_arremer_king::drawSprite(int sx,int sy)
 {
 	if(!enabled) return;
 	X=-sx+pos_x-38; Y=-sy+pos_y-12;
 	if(act_frame>=0 && act_frame<6) can_hit=true; else can_hit=false;
  if(act_frame<6) hittable=true; else hittable=false;
 	if(inScreen())
 	 {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void red_arremer_king::platformSprite()
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

BITMAP *red_arremer_king::getActFrameColP() { return frames_collision[act_frame]; }

bool red_arremer_king::inScreen()
 {
  if(X>=-76 && X<=384) return true;
  else return false;
 }

int red_arremer_king::getPlatYPos() { return plat_y; }

void red_arremer_king::setExitKeyP(bonus *p) { exit_key=p; }

void red_arremer_king::setCtrlXP(int *p) { ctrl_x=p; }

void red_arremer_king::setCtrlYP(int *p) { ctrl_y=p; }

int *red_arremer_king::getPosXP() { return &pos_x; }

int *red_arremer_king::getPosYP() { return &pos_y; }

int red_arremer_king::getScore() { if(energy==1) return 500; else return 0; }

void red_arremer_king::setPlatformMap(map *p) { plat_map=p; }

void red_arremer_king::setHightMap(map *p) { hight_map=p; }

void red_arremer_king::setBackgroundMap(map *p) { back_map=p; }

void red_arremer_king::setHitted(bool b) { hitted=b; }

bool red_arremer_king::canHit() { return can_hit; }

bool red_arremer_king::isHittable() { return hittable; }

void red_arremer_king::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  dx=0; dy=0; plat_y=start_y;
  act_frame=0;
  energy=12; speed=6; // attack speed
  stop=0; move=0; fly=0; attack=0; dead=0; jump_up=0; jump_down=0; down=0; fire1=0; fire2=0; fall=0; turn=0; rise=0; unrise=0; hitted=0; explode=0; sleep=1;
  side=1; direction=0;
  plat_wall=0;
 }

void red_arremer_king::debug()
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
