class volture : public _sprite
 {
  private:
   int pos_x,pos_y,scroll_x,scroll_y,jump_y;
   int plat_y,plat_stair,plat_wall;
   int energy;
   int dx,dy;
   bool can_hit,hittable;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool stop,move,jump_up,jump_down,down,climb,fire1,fire2,fire_down,fire_up,fall,hitted,fly,dead,explode;
   timer *ti[5];
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;
	
  public:
   volture(char *,int,int,BITMAP *,int i=0,volture *p=NULL);
   ~volture();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,volture *);
   BITMAP *getActFrameColP();
   bool inScreen();
   void setHitted(bool b);
   bool canHit();
   bool isHittable();
   int getPlatYPos();
   int getScore();
   int *getPosXP();
   int *getPosYP();
   void resetObj(bool);
   void debug();
 };

volture::volture(char *filename, int sx, int sy, BITMAP *bmp, int i, volture *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  resetObj(true);
  ti[0]=new timer(4);  // Anim time
  ti[1]=new timer(2);  // Explode time
  ti[2]=new timer(5);  // Stop time
  ti[3]=new timer(10); // Fly anim time
  ti[4]=new timer(0);
 }

volture::~volture()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<2;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void volture::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void volture::assignSample(int n,volture *p)
 {
  sound[n]=p->sound[n];
 }

void volture::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void volture::animSprite()
 {
 	if(!enabled) return;
 	if(dead) return;
 	if(explode) 
 	 {
 	 	if(ti[1]->isOn()) act_frame++;
 	 	if(act_frame==35)
 	 	 {
 	 	  act_frame=35;
 	 	  explode=0;
 	 	  dead=1;
 	 	 }
 	 	return;
 	 }
 	if(!fly) if(random(2)==1) fly=1;
 	if(fly)  { if(side) dx=-2; else dx=2; if(ti[3]->isOn()) { act_frame++; if(act_frame>2) act_frame=1; } }
 	if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; act_frame=19; stop=1; energy--; if(energy==0) { explode=1; fly=0; stop=0; play_sample(sound[1],120,150,1000,0); } else explode=0; ti[2]->reset(); return; }
 	if(stop) { dx=0; act_frame=19; if(ti[2]->isOn()) stop=0; }
 	pos_x+=dx;
 	pos_y+=dy;
 	/*if(climb) // Sulla scala
 	 {
    if(key[KEY_UP])           { pos_y-=2; }
    if(key[KEY_DOWN])         { pos_y+=2; }
    if(plat_stair==0)         { climb=0; }
    jump_up=0; jump_down=0; fire1=0; fire2=0; fire_down=0; fire_up=0;
    return;
 	 }
 	if(!jump_up && !jump_down && !fire1 && !fire2 && !fire_down && !fire_up && !climb) // Al suolo
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
    											     if(!down)       { fire1=1; fire2=0; fire_up=0; act_frame=10; }
    											     if(down)        { fire2=1; fire1=0; fire_up=0; act_frame=12; }
    											     if(key[KEY_UP]) { fire_up=1; fire1=0; fire2=0; act_frame=14; }
    	                         down=0;
    	                         play_sample(sound[0],120,150,1000,0);
    	                         key[KEY_LCONTROL]=0;
    	                        }
   }
  if((jump_up || jump_down) && !fire1 && !fire2 && !fire_down && !fire_up && !climb) // In volo
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
    											 if(!down)         { fire1=1; fire2=0; fire_up=0; fire_down=0; act_frame=10; }
    											 if(down)          { fire2=1; fire1=0; fire_up=0; fire_down=0; act_frame=12; }
    											 if(key[KEY_UP])   { fire_up=1; fire1=0; fire2=0; fire_down=0; act_frame=14; }
    											 if(key[KEY_DOWN]) { fire_down=1; fire_up=1; fire1=0; fire2=0; act_frame=16; }
    	                     down=0;
    	                     play_sample(sound[0],120,150,1000,0);
    	                     key[KEY_LCONTROL]=0;
    	                    }
   }*/
 }

void volture::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  //pivot_sprite(rend,car,rend_x,rend_y,piv_cx,piv_cy,itofix(ang1));
  X=-sx+pos_x-30; Y=-sy+pos_y-12;
  if((act_frame>=0 && act_frame<3) || act_frame==19) can_hit=true; else can_hit=false;
  if(act_frame<3 || act_frame==19) hittable=true; else hittable=false;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void volture::platformSprite()
 {
  if(!enabled) return;
  if(!jump_down && !jump_up && !climb)
  {
   if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
   if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
   if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
  }
  //plat_y=plat_map->getActPlatform();
  plat_wall=0;//plat_map->getActWall(direction);
  plat_stair=0;//plat_map->getActStair();
 }

BITMAP *volture::getActFrameColP() { return frames_collision[act_frame]; }

bool volture::inScreen()
 {
  if(X>=-62 && X<=384) return true;
  else return false;
 }

int volture::getPlatYPos() { return plat_y; }

int *volture::getPosXP() { return &pos_x; }

int *volture::getPosYP() { return &pos_y; }

int volture::getScore() { if(energy==1) return 200; else return 0; }

void volture::setPlatformMap(map *p) { plat_map=p; }

void volture::setHightMap(map *p) { hight_map=p; }

void volture::setBackgroundMap(map *p) { back_map=p; }

void volture::setHitted(bool b) { hitted=b; }

bool volture::canHit() { return can_hit; }

bool volture::isHittable() { return hittable; }

void volture::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  energy=2;
  dx=0; dy=0; plat_y=start_y;
  act_frame=1;
  stop=0; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire1=0; fire2=0; fire_down=0,fire_up=0,fall=0,hitted=0,fly=0,dead=0,explode=0;
  side=1; direction=0;
 }

void volture::debug()
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
