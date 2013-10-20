class rockfall : public _sprite
 {
  private:
   enum { _NONE=0, _SHAKE=1, _FALL=3, _SLAM=4, _BOUNCE=5 };
   int afr,coun,maxy,pos_x,pos_y,scroll_x,scroll_y,max_y,min_y;
   int plat_y,plat_stair,plat_wall;
   int dx,dy;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool jump_up,jump_down,climb,fall;
   timer *ti[5];
   int count;
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;
 	
  public:
   rockfall(char *,int,int,int,int,BITMAP *,int fr=0,int i=0,rockfall *p=NULL);
   ~rockfall();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setMaxY(int);
   void setCount(int);
   void setActualFrame(int);
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,rockfall *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getPlatYPos();
   int *getPosXP();
   int *getPosYP();
   void resetObj(bool);
   void debug();
 };

rockfall::rockfall(char *filename, int sx, int sy, int my, int co, BITMAP *bmp, int fr, int i, rockfall *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  maxy=my; afr=fr; coun=co;
  resetObj(true);
  ti[0]=new timer(16); // Restart time
  ti[1]=new timer(2);  // Shake time
  ti[2]=new timer(0);
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

rockfall::~rockfall()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<1;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void rockfall::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void rockfall::assignSample(int n,rockfall *p)
 {
  sound[n]=p->sound[n];
 }

void rockfall::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void rockfall::animSprite()
 {
  if(!enabled) return;
  if(action==_NONE) { pos_y=min_y; dy=0; count=0; pos_x=start_x; if(random(80)==0) action=_SHAKE; }
  if(action==_SHAKE)
   {
    if(ti[0]->isOn()) { pos_y+=3; count++; }
    if(ti[1]->isOn())
     {
      if(pos_x%2==0) pos_x++; else pos_x--;
      if(count==5) { count=0; dy=1; action=_FALL; }
     }
   }
  if(action==_FALL)
   {
   	if(pos_y>=max_y) { count=0; action=_SLAM; /*if(inScreen()) play_sample(sound[0],120,150,1000,0);*/ }
  	if(count%4==0) if(dy<9) dy++; // Gravità
  	count++; pos_y+=dy;
   }
  if(action==_SLAM) { count=0; action=_BOUNCE; }
  if(action==_BOUNCE)
   {
  	if(count%2==0) pos_x++;
  	if(count%5==0) { pos_x++; act_frame=1; } else { act_frame=0; }
  	if(dy>0) dy--; // Rimbalzo
  	count++;
  	pos_y-=dy;
  	if(dy==0) { action=_NONE; act_frame=0; }
   }
 }

void rockfall::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void rockfall::platformSprite()
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
 
BITMAP *rockfall::getActFrameColP() { return frames_collision[act_frame]; }

bool rockfall::inScreen()
 {
  if(X>=-32 && X<=384) return true;
  else return false;
 }

int rockfall::getPlatYPos() { return plat_y; }

int *rockfall::getPosXP() { return &pos_x; }

int *rockfall::getPosYP() { return &pos_y; }

void rockfall::setPlatformMap(map *p) { plat_map=p; }

void rockfall::setHightMap(map *p) { hight_map=p; }

void rockfall::setBackgroundMap(map *p) { back_map=p; }

void rockfall::setMaxY(int my) { maxy=my; }

void rockfall::setCount(int co) { coun=co; }

void rockfall::setActualFrame(int fr) { afr = fr; }

void rockfall::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0;
  min_y=start_y;
  dx=0; dy=0; plat_y=start_y; max_y=maxy;
  act_frame=afr;
  count=coun;
  action = _NONE;
  side=0; direction=0;
 }

void rockfall::debug()
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
