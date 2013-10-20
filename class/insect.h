class insect : public _sprite
 {
  private:
   int pos_x,pos_y,scroll_x,scroll_y,jump_y; // start_y è la lunghezza a partire da start_x dove l'oggetto deve apparire (l'oggetto appare da start_x fino a start_x+start_y)
   int plat_y,plat_stair,plat_wall;
   int *ctrl_x,*ctrl_y;
   int energy;
   int dx,dy,dd,ws;
   int type;
   bool can_hit,hittable;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool restart,move,hitted,dead,explode;
   timer *ti[5];
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;
 	
  public:
   insect(char *,int,int,BITMAP *,int i=0,insect *p=NULL);
   ~insect();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,insect *);
   BITMAP *getActFrameColP();
   bool inScreen();
   void setHitted(bool b);
   bool canHit();
   bool isHittable();
   int getPlatYPos();
   int getScore();
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   void setType(int);
   int *getPosXP();
   int *getPosYP();
   void resetObj(bool);
   void debug();
 };

insect::insect(char *filename, int sx, int sy, BITMAP *bmp, int i, insect *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  dd=-1; type=0;
  resetObj(true);
  ti[0]=new timer(2); // Anim time
  ti[1]=new timer(1); // Explode time
  ti[2]=new timer(random(500)+50); // Restart time
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

insect::~insect()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<2;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void insect::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void insect::assignSample(int n,insect *p)
 {
  sound[n]=p->sound[n];
 }

void insect::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void insect::animSprite()
 {
  if(!enabled) return;
  if(!inScreen() && !restart)
   {
    act_frame=0;
    dead=1;
    hitted=0;
    explode=0;
    move=0;
    energy=1;
    if(*ctrl_x>=start_x && *ctrl_x<=start_x+start_y) { dead=0; restart=1; }
    return;
   }
  if(dead && *ctrl_x>=start_x && *ctrl_x<=start_x+start_y) { dead=0; restart=1; return; }
  if(restart) if(ti[2]->isOn())
   {
  	move=1; restart=0;
  	if(type==0)
  	 {
  	  if(*ctrl_y<340) pos_y=*ctrl_y-random(80); else pos_y=340-random(80);
     }
  	else
  	 {
	  pos_y=(*ctrl_y-10)-random(90);
     }
  	if(random(10)%4==0) pos_x=*ctrl_x-384; else pos_x=*ctrl_x+384;
  	
  	if(*ctrl_x<pos_x) side=1; else side=0;
  	dy=0; act_frame=1;
   	if(side) dx=-2; else dx=2;
   }
  if(explode) 
   {
   	if(ti[1]->isOn()) act_frame++;
  	if(act_frame==12)
  	 {
  	  act_frame=0;
  	  dead=1;
  	  hitted=0;
  	  explode=0;
  	  move=0;
  	  energy=1;
  	 }
  	return;
   }
  if(move) { if(ti[0]->isOn()) { act_frame++; dy+=dd; } if(dy<-ws) dd=1; if(dy>ws) dd=-1; if(act_frame>4) act_frame=1; }
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { explode=1; move=0; act_frame=4; play_sample(sound[1],120,150,1000,0); } else explode=0; ti[2]->reset(); return; }
  pos_x+=dx; if(ti[1]->isOn()) pos_x+=dx/2;
  pos_y+=dy;
 }

void insect::setType(int t) { type=t; }

void insect::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  //pivot_sprite(rend,car,rend_x,rend_y,piv_cx,piv_cy,itofix(ang1));
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(act_frame>=1 && act_frame<=3) can_hit=true; else can_hit=false;
  if(act_frame<4) hittable=true; else hittable=false;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void insect::platformSprite()
 {
 	if(!enabled) return;
 	/*if(!jump_down && !jump_up && !climb)
 	{
 	 if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
 	 if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
 	 if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
 	}
 	//plat_y=plat_map->getActPlatform();
 	plat_wall=0;//plat_map->getActWall(direction);
 	plat_stair=0;//plat_map->getActStair();*/
 }

BITMAP *insect::getActFrameColP() { return frames_collision[act_frame]; }

bool insect::inScreen()
 {
  if(X>=-384-100 && X<=384+384+100) return true;
  return false;
 }

int insect::getPlatYPos() { return plat_y; }

void insect::setCtrlXP(int *p) { ctrl_x=p; }

void insect::setCtrlYP(int *p) { ctrl_y=p; }

int *insect::getPosXP() { return &pos_x; }

int *insect::getPosYP() { return &pos_y; }

int insect::getScore() { if(energy==1) return 100; else return 0; }

void insect::setPlatformMap(map *p) { plat_map=p; }

void insect::setHightMap(map *p) { hight_map=p; }

void insect::setBackgroundMap(map *p) { back_map=p; }

void insect::setHitted(bool b) { hitted=b; }

bool insect::canHit() { return can_hit; }

bool insect::isHittable() { return hittable; }

void insect::resetObj(bool enb)
 {
  ws=random(1)+1;
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  energy=1;
  dx=0; dy=0; plat_y=start_y;
  act_frame=0;
  move=0; hitted=0; dead=1; explode=0; restart=0;
  side=1; direction=0;
 }

void insect::debug()
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
