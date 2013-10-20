class little_devil : public _sprite
 {
  private:
   enum { N_BU=3 };
   enum { _NONE=0, _FLY_UP=1, _FLY_DOWN=2, _FLY_LEFT=3, _FLY_RIGHT=4, _STOP=5, _GO_UP=6, _EXPLODE=7 };
   bool have_vase;
   int dx,dy,pos_x,pos_y,scroll_x,scroll_y,gravity;
   int *ctrl_x,*ctrl_y;
   int plat_y,plat_stair,plat_wall;
   int life,energy;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool can_hit,hittable,is_hard;
   bool hitted;
   timer *ti[5];
   bullet *BU[N_BU];
   SAMPLE *sound[4];
   bonus *vase;
   map *plat_map;
   map *hight_map;
   map *back_map;

   void shootBullet();
 	
  public:
   little_devil(char *,int,int,BITMAP *,int i=0,little_devil *p=NULL);
   ~little_devil();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   void loadSample(char *,int);
   void assignSample(int,little_devil *);
   BITMAP *getActFrameColP();
   bullet *getBullet(int); 	 
   bool inScreen();
   int getPlatYPos();
   void attachVase(bonus *);
   void setHaveVase(bool);
   bool getHaveVase();
   int getNBullet(); 	 
   int getScore();
   int *getPosXP();
   int *getPosYP();
   void setHitted(bool);
   bool canHit();
   bool isHittable();
   bool isHard();
   void resetObj(bool);
   void debug();
 };

little_devil::little_devil(char *filename, int sx, int sy, BITMAP *bmp, int i, little_devil *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  // Bullet (assegno i frame ai proiettili prendendoli dalla memoria già allocata)
  // Per adesso nessun "sound" è assegnato ai proiettili
  //BU[0]->loadSample("data/sound/fx/hard.wav",0);  	
  for(int i=0;i<N_BU;i++)
   {
  	BU[i]=new bullet("",bmp,1,38,40);
    //BU[i]->assignSample(0,BU[0]);
  	for(int j=0;j<rows*columns;j++)
     {
      BU[i]->setFrameP(j,frames[j]);
      BU[i]->setFrameColP(j,frames_collision[j]);
     }
   }
  resetObj(true);
  ti[0]=new timer(3); // Anim time
  ti[1]=new timer(1); // Move time
  ti[2]=new timer(2); // Explode time
  ti[3]=new timer(10); // Shoot time
  ti[4]=new timer(100); // Change time
 }

little_devil::~little_devil()
 {
  for(int i=0;i<N_BU;i++) delete(BU[i]);
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<3;i++) destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void little_devil::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void little_devil::assignSample(int n,little_devil *p)
 {
  sound[n]=p->sound[n];
 }

void little_devil::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void little_devil::shootBullet()
 {
 	int n=-1;
 	
 	//play_sample(sound[0],120,150,1000,0);
 	for(int i=0;i<N_BU;i++) if(!BU[i]->isFired()) { n=i; break; }
 	if(n!=-1)
 	 {
  	if(side) BU[n]->shoot(pos_x-23,pos_y+7,*ctrl_x,*ctrl_y+7,side);
  	else     BU[n]->shoot(pos_x+23,pos_y+7,*ctrl_x,*ctrl_y+7,side);
   }
 }

void little_devil::animSprite()
 {
  if(!enabled) return;
  if(pos_x<*ctrl_x-400 || pos_x>*ctrl_x+400 || pos_y>990) action=_NONE;
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { act_frame=10; action=_EXPLODE; play_sample(sound[1],120,150,1000,0); ti[2]->reset();} } // TODO suono esplosione da fare
  if(index==0) printf("py %d | cy: %d | Act:%d\n",pos_y,*ctrl_y,action);
 	if(action==_NONE) 
 	 {
 	 	if(*ctrl_x>4200 && *ctrl_x<6300)
  	 if(random(200)==0)
  	  {
 	  	 energy=1;
 	  	 pos_x=*ctrl_x-random(360)+180; pos_y=*ctrl_y+random(200)+80; dx=0; dy=-(random(2)+1); side=1; action=_GO_UP;
 	 	  }
   }
 	if(action==_GO_UP) 
 	 {
 	 	if(ti[0]->isOn()) act_frame++; if(act_frame>6) act_frame=4;
    pos_x+=dx; pos_y+=dy;
 	 	if(pos_y<=*ctrl_y-30) { if(pos_x<*ctrl_x) dx=1; else dx=-1; action=random(4)+1; }
 	 }
 	if(action==_STOP) 
 	 {
 	 	if(ti[0]->isOn()) act_frame++; if(act_frame>6) act_frame=4;
 	 	//if(random(1000)) shootBullet();
 	 	if(ti[4]->isOn())
 	 	 {
 	 	 	if(pos_x<*ctrl_x) { dx=1; side=1; } else { dx=-1; side=0; }
 	 	 	action=random(4)+1; ti[1]->setTick(random(1));
 	 	 	ti[4]->setTick(random(80)+50); ti[4]->reset();
 	 	 }
 	 }
 	if(action==_FLY_DOWN)
 	 {
 	 	dy=1;
 	 	if(dx) if(ti[0]->isOn()) act_frame++; if(act_frame>3) act_frame=1;
 	 	else   if(ti[0]->isOn()) act_frame++; if(act_frame>6) act_frame=4;
 	 	if(ti[1]->isOn()) { pos_x+=dx; pos_y+=dy; }
 	 	if(ti[4]->isOn())
 	 	 {
 	 	 	if(pos_x<*ctrl_x) { dx=1; side=1; } else { dx=-1; side=0; }
 	 	 	action=random(4)+1; ti[1]->setTick(random(1));
 	 	 	ti[4]->setTick(random(80)+50); ti[4]->reset();
 	 	 }
 	 }
 	if(action==_FLY_UP)
 	 {
 	 	dy=-1;
 	 	if(dx) if(ti[0]->isOn()) act_frame++; if(act_frame>3) act_frame=1;
 	 	else   if(ti[0]->isOn()) act_frame++; if(act_frame>6) act_frame=4;
 	 	if(ti[1]->isOn()) { pos_x+=dx; pos_y+=dy; }
 	 	if(ti[4]->isOn())
 	 	 {
 	 	 	if(pos_x<*ctrl_x) { dx=1; side=1; } else { dx=-1; side=0; }
 	 	 	action=random(4)+1; ti[1]->setTick(random(1));
 	 	 	ti[4]->setTick(random(80)+50); ti[4]->reset();
 	 	 }
 	 }
 	if(action==_FLY_LEFT)
 	 {
 	 	dy=1;
 	 	if(ti[0]->isOn()) act_frame++; if(act_frame>3) act_frame=1;
 	 	if(ti[1]->isOn()) { pos_y+=dy; }
 	 	pos_x+=dx;
 	 	if(ti[4]->isOn()) { action=_STOP; ti[4]->setTick(random(5)+5); ti[4]->reset(); }
 	 }
 	if(action==_FLY_RIGHT)
 	 {
 	 	dy=-1;
 	 	if(ti[0]->isOn()) act_frame++; if(act_frame>3) act_frame=1;
 	 	if(ti[1]->isOn()) { pos_x+=dx; pos_y+=dy; }
 	 	pos_x+=dx;
    if(ti[4]->isOn()) { action=_STOP; ti[4]->setTick(random(5)+5); ti[4]->reset(); }
 	 }
 	if(action==_EXPLODE) 
 	 {
 	 	if(ti[2]->isOn()) act_frame++;
 	 	if(act_frame==13)
 	 	 {
 	 	  act_frame=0;
 	 	  pos_x=*ctrl_x-180; pos_y=*ctrl_y+280;
 	 	  action=_NONE;
 	 	 }
 	 }
 }

void little_devil::drawSprite(int sx,int sy)
 {
  if(!enabled) return;
  if(!action)
   {
  	can_hit=hittable=is_hard=false;
  	// Bullet
    for(int i=0;i<N_BU;i++) { BU[i]->drawSprite(sx,sy); BU[i]->timeSprite(); BU[i]->animSprite(); }
 	return;
   }

  X=-sx+pos_x-31; Y=-sy+pos_y-6;
  if(act_frame>=1 && act_frame<=9) can_hit=true;  else can_hit=false;
  if(act_frame>=1 && act_frame<=9) hittable=true; else hittable=false;
  if(inScreen())
   {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }

  // Bullet
  for(int i=0;i<N_BU;i++) { BU[i]->drawSprite(sx,sy); BU[i]->timeSprite(); BU[i]->animSprite(); }
 }

void little_devil::platformSprite()
 {
 	//if(!enabled) return;
  //if(!action) return;
 	/*if(hitted || explode) return;
 	plat_wall=0;
 	if(!jump_down && !jump_up && !climb && !rise && !unrise && !hitted && !explode)
 	 {
 	  if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
 	 }
 	plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	if(!unrise) plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
 	plat_stair=0;//plat_map->getActStairEnemies(1);*/
 }

bullet *little_devil::getBullet(int n) { return BU[n]; }

BITMAP *little_devil::getActFrameColP() { return frames_collision[act_frame]; }

bool little_devil::inScreen()
 {
  if(X>=-72 && X<=384 && Y>-72 && Y<240) return true;
  else return false;
 }

int little_devil::getPlatYPos() { return plat_y; }

int *little_devil::getPosXP() { return &pos_x; }

int *little_devil::getPosYP() { return &pos_y; }

int little_devil::getScore() { if(energy==1) { if(have_vase) { vase->setAttached(false); have_vase=0; } return 50; } else return 0; }

int little_devil::getNBullet() { return N_BU; }

void little_devil::setPlatformMap(map *p) { plat_map=p; }

void little_devil::setHightMap(map *p) { hight_map=p; }

void little_devil::setBackgroundMap(map *p) { back_map=p; }

void little_devil::setHitted(bool b) { hitted=b; }

void little_devil::setCtrlXP(int *p) { ctrl_x=p; }

void little_devil::setCtrlYP(int *p) { ctrl_y=p; }

bool little_devil::canHit() { return can_hit; }

bool little_devil::isHittable() { return hittable; }

bool little_devil::isHard() { return is_hard; }

void little_devil::setHaveVase(bool b) { have_vase=b; }

bool little_devil::getHaveVase() { return have_vase; }

void little_devil::attachVase(bonus *p)
 {
  if(random(500)==0 && isEnabled() && !getHaveVase() && action==_GO_UP && !inScreen())
   {
    vase=p;
    vase->resetObj(true,11);
    vase->setVaseX(getPosXP()); vase->setVaseY(getPosYP());
    vase->setVasepx(-21); vase->setVasepy(1);
    vase->setAttached(true);
    setHaveVase(true);
   }
 }

void little_devil::resetObj(bool enb)
 {
  enabled=enb; hitted=0;
  action=_NONE;
  have_vase=false; // No vase at start
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; gravity=0;
  dx=0; dy=0; plat_y=start_y;
  energy=1;
  act_frame=0;
  life=random(300)+300; pos_x=random(2300);
  side=0; direction=0;
  plat_wall=0;
  for(int i=0;i<N_BU;i++) BU[i]->resetObj();
 }

void little_devil::debug()
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
