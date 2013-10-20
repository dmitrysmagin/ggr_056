class ghost : public _sprite
 {
  private:
   int pos_x,pos_y,scroll_x,scroll_y,jump_y; // start_y è la lunghezza a partire da start_x dove l'oggetto deve apparire (l'oggetto appare da start_x fino a start_x+start_y)
   int plat_y,plat_stair,plat_wall;
   int *ctrl_x,*ctrl_y;
   int energy,speed;
   int dx,dy;
   bool can_hit,hittable;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool restart,move,near,far,hitted,dead,explode;
   timer *ti[5];
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;

  public:
   ghost(char *,int,int,BITMAP *,int i=0,ghost *p=NULL);
   ~ghost();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,ghost *);
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

ghost::ghost(char *filename, int sx, int sy, BITMAP *bmp, int i, ghost *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  resetObj(true);
  ti[0]=new timer(10);  // Anim time
  ti[1]=new timer(2);   // Explode time
  ti[2]=new timer(random(100)+50); // Far time
  ti[3]=new timer(random(200)+150); // Move time
  ti[4]=new timer(0);
 }

ghost::~ghost()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<2;i++)            destroy_sample(sound[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void ghost::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void ghost::assignSample(int n,ghost *p)
 {
  sound[n]=p->sound[n];
 }

void ghost::timeSprite()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void ghost::animSprite()
 {
 	if(!enabled) return;
 	if(!inScreen())
 	 {
 	  act_frame=0;
 	  dead=1;
 	  hitted=0;
 	  explode=0;
 	  near=0;
 	  move=0;
 	  far=0;
 	  energy=2;
 	  return;
   }
 	if(dead && *ctrl_x>=start_x && *ctrl_x<=start_x+start_y) { dead=0; pos_y=random(184)+468; if(random(10)%2==0) pos_x=*ctrl_x-384; else pos_x=*ctrl_x+384; speed=random(2)+1; restart=1; return; }
 	if(restart) if(ti[2]->isOn()) { restart=0; far=1; if(*ctrl_x<pos_x) side=1; else side=0; }
 	if(explode) 
 	 {
 	 	if(ti[1]->isOn()) act_frame++;
 	 	if(act_frame==15)
 	 	 {
 	 	  act_frame=0;
 	 	  dead=1;
 	 	  hitted=0;
 	 	  explode=0;
 	 	  near=0;
 	 	  move=0;
 	 	  far=0;
 	 	  energy=2;
 	 	 }
 	 	return;
 	 }
 	if(far)
   {
 	  if(ti[0]->isOn()) act_frame++; if(act_frame>2) act_frame=1;
 	  if(side) dx=-speed; else dx=speed; dy=0;
 	  if(pos_x>*ctrl_x-100 && pos_x<*ctrl_x+100 && ti[3]->isOn()) { near=1; far=0; }
 	 }
 	if(near) { dx=0; dy=0; if(ti[0]->isOn()) act_frame++;
 		         if(act_frame==9)
 		          {
 		           int ang;
 		           
 		           near=0; move=1;
 		           if(*ctrl_x<pos_x) side=1; else side=0;
 		           ang=angle2P(pos_x,pos_y,*ctrl_x,*ctrl_y);
   	           dx=fixtoi(fixmul(itofix(2),(fixcos(itofix(ang)))));
   	           dy=-fixtoi(fixmul(itofix(2),(fixsin(itofix(ang)))));
   	          }
 		       }
 	if(move) { if(ti[0]->isOn()) act_frame++; if(act_frame>10) act_frame=9; }
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { explode=1; move=0; far=0; near=0; act_frame=11; play_sample(sound[1],120,150,1000,0); } else explode=0; ti[2]->reset(); return; }
 	pos_x+=dx;
 	pos_y+=dy;
 }

void ghost::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  //pivot_sprite(rend,car,rend_x,rend_y,piv_cx,piv_cy,itofix(ang1));
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(act_frame>=9 && act_frame<=10) can_hit=true;  else can_hit=false;
  if(act_frame>=8 && act_frame<=10) hittable=true; else hittable=false;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
 }

void ghost::platformSprite()
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

BITMAP *ghost::getActFrameColP() { return frames_collision[act_frame]; }

bool ghost::inScreen()
 {
 	if(X>=-62-384 && X<=384+384) return true;
 	if(Y>=400 && Y<=700) return true;
 	else return false;
 }

int ghost::getPlatYPos() { return plat_y; }

void ghost::setCtrlXP(int *p) { ctrl_x=p; }

void ghost::setCtrlYP(int *p) { ctrl_y=p; }

int *ghost::getPosXP() { return &pos_x; }

int *ghost::getPosYP() { return &pos_y; }

int ghost::getScore() { if(energy==1) return 100; else return 0; }

void ghost::setPlatformMap(map *p) { plat_map=p; }

void ghost::setHightMap(map *p) { hight_map=p; }

void ghost::setBackgroundMap(map *p) { back_map=p; }

void ghost::setHitted(bool b) { hitted=b; }

bool ghost::canHit() { return can_hit; }

bool ghost::isHittable() { return hittable; }

void ghost::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  energy=2;
  dx=0; dy=0; plat_y=start_y;
  act_frame=0;
  move=0; near=0; far=0; hitted=0; dead=1; explode=0; restart=0;
  side=1; direction=0;
 }

void ghost::debug()
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
