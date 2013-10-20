class firewall : public _sprite
 {
  private:
   int coun,maxy,pos_x,pos_y,scroll_x,scroll_y,max_y,min_y;
   int plat_y,plat_stair,plat_wall;
   int dx,dy;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool stop,move,jump_up,jump_down,down,climb,fire1,fire2,fire_down,fire_up,fall,turn;
   timer *ti[5];
   int count,max_count;
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;
 	
  public:
   firewall(char *,int,int,int,int,BITMAP *,int i=0,firewall *p=NULL);
   ~firewall();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setMaxY(int);
   void setCount(int);
   void setMaxCount(int);
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,firewall *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getPlatYPos();
   int *getPosXP();
   int *getPosYP();
   void resetObj(bool);
   void debug();
 };

firewall::firewall(char *filename, int sx, int sy, int my, int co, BITMAP *bmp, int i, firewall *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  maxy=my; coun=co; max_count=30;
  resetObj(true);
  ti[0]=new timer(16); // Restart time
  ti[1]=new timer(3);  // Anim time
  ti[2]=new timer(0);
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

firewall::~firewall()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<1;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void firewall::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void firewall::assignSample(int n,firewall *p)
 {
  sound[n]=p->sound[n];
 }

void firewall::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void firewall::animSprite()
 {
  if(!enabled) return;
  if(ti[1]->isOn()) { act_frame++; if(act_frame>4) act_frame=1; }
  if(ti[0]->isOn()) count++;
  if(down) if(count>=max_count) { count=0; down=0; jump_down=0; jump_up=1; stop=0; dy=-2; if(inScreen()) play_sample(sound[0],120,150,1000,0); }
  if(jump_down) { if(pos_y>=max_y) { jump_up=0; jump_down=0; stop=0; down=1; dy=0; } }
  if(jump_up)   {	if(pos_y<=min_y) { jump_up=0; jump_down=0; stop=1; down=0; dy=0; count=0; } }
  if(stop)      { if(count==10) { jump_up=0; jump_down=1; stop=0; down=0; dy=1; } }
  pos_y+=dy;
 }

void firewall::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void firewall::platformSprite()
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
 
BITMAP *firewall::getActFrameColP() { return frames_collision[act_frame]; }

bool firewall::inScreen()
 {
  if(X>=-32 && X<=384) return true;
  else return false;
 }

int firewall::getPlatYPos() { return plat_y; }

int *firewall::getPosXP() { return &pos_x; }

int *firewall::getPosYP() { return &pos_y; }

void firewall::setPlatformMap(map *p) { plat_map=p; }

void firewall::setHightMap(map *p) { hight_map=p; }

void firewall::setBackgroundMap(map *p) { back_map=p; }

void firewall::setMaxY(int my) { maxy=my; }

void firewall::setCount(int co) { coun=co; }

void firewall::setMaxCount(int mc) { max_count=mc; }

void firewall::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0;
  max_y=start_y; min_y=start_y-maxy;
  dx=0; dy=0; plat_y=start_y;
  act_frame=0;
  count=coun;
  stop=0; down=1; move=0; jump_up=0; jump_down=0; climb=0; fire1=0; fire2=0; fire_down=0; fire_up=0,fall=0,turn=0;
  side=0; direction=0;
 }

void firewall::debug()
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
