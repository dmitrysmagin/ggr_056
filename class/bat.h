class bat : public _sprite
 {
  private:
   int pos_x,pos_y,scroll_x,scroll_y,jump_y; // start_y è la lunghezza a partire da start_x dove l'oggetto deve apparire (l'oggetto appare da start_x fino a start_x+start_y)
   int plat_y,plat_stair,plat_wall;
   int *ctrl_x,*ctrl_y;
   int energy;
   int dx,dy;
   bool can_hit,hittable;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool restart,move,hitted,dead,explode;
   timer *ti[5];
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;

  public:
   bat(char *,int,int,BITMAP *,int i=0,bat *p=NULL);
   ~bat();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,bat *);
   BITMAP *getActFrameColP();
   bool inScreen();
   void setHitted(bool b);
   bool canHit();
   bool isHittable();
   int getPlatYPos();
   int getScore();
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   int *getPosXP();
   int *getPosYP();
   void resetObj(bool);
   void debug();
 };

bat::bat(char *filename, int sx, int sy, BITMAP *bmp, int i, bat *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  resetObj(true);
  ti[0]=new timer(7); // Anim time
  ti[1]=new timer(1); // Explode time
  ti[2]=new timer(random(700)+100); // Restart time
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

bat::~bat()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<2;i++)            destroy_sample(sound[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void bat::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void bat::assignSample(int n,bat *p)
 {
  sound[n]=p->sound[n];
 }

void bat::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void bat::animSprite()
 {
  if(!enabled) return;
  if(!inScreen())
   {
    act_frame=0;
    dead=1;
    hitted=0;
    explode=0;
    move=0;
    energy=2;
    // if(*ctrl_x>=start_x && *ctrl_x<=start_x+start_y) { dead=0; restart=1; } // Vedi insects.h
    return;
   }
  if(dead && *ctrl_x>=start_x && *ctrl_x<=start_x+start_y) { dead=0; restart=1; return; }
  if(restart) if(ti[2]->isOn())
   {
  	move=1; restart=0;
  	pos_y=random(284)+368; if(random(10)%2==0) pos_x=*ctrl_x-384; else pos_x=*ctrl_x+384;
  	if(*ctrl_x<pos_x) side=1; else side=0;
  	dx=0; dy=0; act_frame=1;
 	int ang;

 	ang=angle2P(pos_x,pos_y,*ctrl_x,*ctrl_y);
   	dx=fixtoi(fixmul(itofix(2),(fixcos(itofix(ang)))));
   	dy=-fixtoi(fixmul(itofix(2),(fixsin(itofix(ang)))));
   	if(dx<-1) dx=-1; if(dx>1) dx=1;
   	if(dy<-1) dy=-1; if(dy>1) dy=1;
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
 	 	  energy=2;
 	 	 }
 	 	return;
 	 }
 	if(move) { if(ti[0]->isOn()) act_frame++; if(act_frame>3) act_frame=1; }
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { explode=1; move=0; act_frame=3; play_sample(sound[1],120,150,1000,0); } else explode=0; ti[2]->reset(); return; }
 	pos_x+=dx;
 	pos_y+=dy;
 }

void bat::drawSprite(int sx,int sy)
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

void bat::platformSprite()
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

BITMAP *bat::getActFrameColP() { return frames_collision[act_frame]; }

bool bat::inScreen()
 {
  if(X>=-62-384 && X<=384+384) return true;
  if(Y>=400 && Y<=700) return true;
  else return false;
 }

int bat::getPlatYPos() { return plat_y; }

void bat::setCtrlXP(int *p) { ctrl_x=p; }

void bat::setCtrlYP(int *p) { ctrl_y=p; }

int *bat::getPosXP() { return &pos_x; }

int *bat::getPosYP() { return &pos_y; }

int bat::getScore() { if(energy==1) return 100; else return 0; }

void bat::setPlatformMap(map *p) { plat_map=p; }

void bat::setHightMap(map *p) { hight_map=p; }

void bat::setBackgroundMap(map *p) { back_map=p; }

void bat::setHitted(bool b) { hitted=b; }

bool bat::canHit() { return can_hit; }

bool bat::isHittable() { return hittable; }

void bat::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  energy=2;
  dx=0; dy=0; plat_y=start_y;
  act_frame=0;
  move=0; hitted=0; dead=1; explode=0; restart=0;
  side=1; direction=0;
 }

void bat::debug()
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
