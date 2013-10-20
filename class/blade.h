class blade : public _sprite
 {
  private:
   int afr,coun,maxy,pos_x,pos_y,scroll_x,scroll_y,jump_y,max_y,min_y;
   int plat_y,plat_stair,plat_wall;
   int dx,dy;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool stop,move,jump_up,jump_down,down,climb,fire1,fire2,fire_down,fire_up,fall,turn;
   timer *ti[5];
   int count;
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;

  public:
   blade(char *,int,int,int,int,BITMAP *,int fr=0,int i=0,blade *p=NULL);
   ~blade();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setMaxY(int);
   void setCount(int);
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,blade *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getPlatYPos();
   int *getPosXP();
   int *getPosYP();
   void resetObj(bool);
   void debug();
 };

blade::blade(char *filename, int sx, int sy, int my, int co, BITMAP *bmp, int fr, int i, blade *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  maxy=my; afr=fr; coun=co;
  resetObj(true);
  ti[0]=new timer(16); // Restart time
  ti[1]=new timer(0);
  ti[2]=new timer(0);
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

blade::~blade()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<1;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void blade::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void blade::assignSample(int n,blade *p)
 {
  sound[n]=p->sound[n];
 }

void blade::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void blade::animSprite()
 {
  if(!enabled) return;
  if(ti[0]->isOn()) count++;
  if(count==10) { count=0; jump_down=1; jump_up=0; dy=1; jump_y=0; }
  if(jump_down)
   {
   	if(pos_y>=max_y) { jump_y=0; jump_up=1; jump_down=0; move=0; dy=-1; if(inScreen()) play_sample(sound[0],120,150,1000,0); return; }
  	if(pos_y%5==0) dy++; // Gravità
   }
  if(jump_up)
   {
  	if(pos_y==min_y) { jump_y=0; jump_up=0; jump_down=0; dy=0; }
  	//if(jump_y%5==0) dy++; // Gravità
   }
  pos_y+=dy;
 }

void blade::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void blade::platformSprite()
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
 
BITMAP *blade::getActFrameColP() { return frames_collision[act_frame]; }

bool blade::inScreen()
 {
  if(X>=-32 && X<=384) return true;
  else return false;
 }

int blade::getPlatYPos() { return plat_y; }

int *blade::getPosXP() { return &pos_x; }

int *blade::getPosYP() { return &pos_y; }

void blade::setPlatformMap(map *p) { plat_map=p; }

void blade::setHightMap(map *p) { hight_map=p; }

void blade::setBackgroundMap(map *p) { back_map=p; }

void blade::setMaxY(int my) { maxy=my; }

void blade::setCount(int co) { coun=co; }

void blade::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  min_y=start_y;
  dx=0; dy=0; plat_y=start_y; max_y=maxy;
  act_frame=afr;
  count=coun;
  stop=1; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire1=0; fire2=0; fire_down=0; fire_up=0,fall=0,turn=0;
  side=0; direction=0;
 }

void blade::debug()
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
