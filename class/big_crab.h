class big_crab : public _sprite
 {
  private:
   enum { N_BU=3 };
   enum { _NONE=0, _WAIT=1, _JUMP_UP=2, _JUMP_DOWN=3, _STOP=4, _WALK=5, _RUN=6, _SHOOT=7, _CLOSE=8, _EXPLODE=9, _HIT_GROUND=10, _OPEN=11 };
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
   big_crab(char *,int,int,BITMAP *,int i=0,big_crab *p=NULL);
   ~big_crab();
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
   void assignSample(int,big_crab *);
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

big_crab::big_crab(char *filename, int sx, int sy, BITMAP *bmp, int i, big_crab *p) : _sprite(filename,bmp,sx,sy,i,p)
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
  ti[1]=new timer(4); // Jump up gravity time
  ti[2]=new timer(2); // Explode time
  ti[3]=new timer(10); // Shoot time
  ti[4]=new timer(100); // Stop time
 }

big_crab::~big_crab()
 {
  for(int i=0;i<N_BU;i++) delete(BU[i]);
  for(int i=0;i<5;i++)    delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<3;i++)            destroy_sample(sound[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void big_crab::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void big_crab::assignSample(int n,big_crab *p)
 {
  sound[n]=p->sound[n];
 }

void big_crab::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void big_crab::shootBullet()
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

void big_crab::animSprite()
 {
 	if(!enabled) return;
 	if(pos_x<*ctrl_x-400 || pos_x>*ctrl_x+400 || pos_y>990) action=_NONE;
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { act_frame=19; action=_EXPLODE; play_sample(sound[1],120,150,1000,0); ti[2]->reset();} } // TODO suono esplosione da fare
 	if(action==_NONE) { action=_WAIT; act_frame=5; }
 	if(action==_WAIT) 
 	 {
 	 	energy=2; // ???
 	 	//*ctrl_x
 	 	if(ti[0]->isOn()) act_frame++; if(act_frame>8) act_frame=5;
 	 	pos_x=2225; pos_y=506;
 	 	pos_x=start_x; pos_y=start_y;

  	//if(random(1)==0) { pos_x=*ctrl_x+200+random(200); pos_y=*ctrl_y-200; dx=-(random(1)+1); dy=random(3)+1; gravity=1; side=1; action=_JUMP_DOWN; }
 	 	//else             { if(random(1)==0) { pos_x=*ctrl_x+200+random(200); side=1; } else { pos_x=*ctrl_x-(200+random(100)); side=0; } pos_y=*ctrl_y-200; dx=2; dy=0; act_frame=8; action=_RUN; }
   }
 	if(action==_JUMP_DOWN) 
 	 {
 	 	act_frame=1;
    pos_x+=dx; pos_y+=dy;
    if(ti[1]->isOn()) dy+=gravity;
    if(dy>=7) dy=7;
    plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	 	if(pos_y>=plat_y) action=_HIT_GROUND;
 	 }
 	if(action==_HIT_GROUND) 
 	 {
 	 	act_frame=1;
    pos_x+=dx; dy=-dy;
    if(random(2)==0) { if(pos_x<*ctrl_x) dx=1; else dx=-1; action=_STOP; } else { act_frame=5; ti[4]->reset(); action=_JUMP_UP; }
    if(inScreen()) play_sample(sound[2],120,150,1000,0);
   }
 	if(action==_JUMP_UP) 
 	 {
 	 	int _dy=abs(dy);
 	  if(_dy<6) act_frame=(_dy%4)+1; else act_frame=1;
    pos_x+=dx; pos_y+=dy;
    if(ti[1]->isOn()) dy+=gravity;
    if(dy==0) action=_JUMP_DOWN;
 	 }
 	if(action==_STOP)
 	 {
 	 	if(ti[4]->isOn()) action=_OPEN;
 	 	pos_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	 }
 	if(action==_OPEN) 
 	 {
 	 	if(ti[0]->isOn()) act_frame++;
 	 	if(act_frame==8) action=_WALK;
 	 }
 	if(action==_CLOSE) 
 	 {
 	 	if(ti[0]->isOn()) act_frame--;
 	 	if(act_frame==5) { ti[4]->reset(); action=_STOP; }
 	 }


 	if(action==_WALK) 
 	 {
 	 	if(pos_x<*ctrl_x) side=0; else side=1;
 	 	pos_x+=dx;
 	 	if(ti[0]->isOn()) act_frame++;
 	 	if(act_frame>15) act_frame=8;
 	 	plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	 	if(plat_y>pos_y+7) dx=-dx; else pos_y=plat_y; // Evita la caduta 	 	
 	 	if(random(160)==0) { dx=-dx; ti[3]->reset(); action=_SHOOT; act_frame=15; }
 	 	if(random(200)==0) { ti[0]->reset(); action=_CLOSE; act_frame=7; }
 	 }
 	if(action==_SHOOT) 
 	 {
 	 	if(ti[3]->isOn()) { act_frame++; if(act_frame==17) shootBullet(); }
 	 	if(act_frame==18)
 	 	 {
 	 	  act_frame=8;
 	 	  action=_WALK;
 	 	 }
 	 }
 	if(action==_RUN) 
 	 {
 	 	if(side) pos_x-=dx; else pos_x+=dx;
 	 	if(ti[0]->isOn()) act_frame++;
 	 	if(act_frame>15) act_frame=8;
 	 	if(random(200)==0) { action=_WALK; dx=1; } // Controllare dx=1;
 	 	pos_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	 }
 	if(action==_EXPLODE) 
 	 {
 	 	if(ti[2]->isOn()) act_frame++;
 	 	if(act_frame==25)
 	 	 {
 	 	  act_frame=0;
 	 	  action=_NONE;
 	 	 }
 	 }
 }

void big_crab::drawSprite(int sx,int sy)
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
 	if(act_frame>=1 && act_frame<=17) can_hit=true;  else can_hit=false;
  if(act_frame>=6 && act_frame<=17) hittable=true; else hittable=false;
  if(act_frame>=1 && act_frame<=5) is_hard=true;  else is_hard=false;
 	if(inScreen())
 	 {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }

 	// Bullet
  for(int i=0;i<N_BU;i++) { BU[i]->drawSprite(sx,sy); BU[i]->timeSprite(); BU[i]->animSprite(); }
 }

void big_crab::platformSprite()
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

bullet *big_crab::getBullet(int n) { return BU[n]; }

BITMAP *big_crab::getActFrameColP() { return frames_collision[act_frame]; }

bool big_crab::inScreen()
 {
  if(X>=-72 && X<=384) return true;
  else return false;
 }

int big_crab::getPlatYPos() { return plat_y; }

int *big_crab::getPosXP() { return &pos_x; }

int *big_crab::getPosYP() { return &pos_y; }

int big_crab::getScore() { if(energy==1) { if(have_vase) { vase->setAttached(false); have_vase=0; } return 200; } else return 0; }

int big_crab::getNBullet() { return N_BU; }

void big_crab::setPlatformMap(map *p) { plat_map=p; }

void big_crab::setHightMap(map *p) { hight_map=p; }

void big_crab::setBackgroundMap(map *p) { back_map=p; }

void big_crab::setHitted(bool b) { hitted=b; }

void big_crab::setCtrlXP(int *p) { ctrl_x=p; }

void big_crab::setCtrlYP(int *p) { ctrl_y=p; }

bool big_crab::canHit() { return can_hit; }

bool big_crab::isHittable() { return hittable; }

bool big_crab::isHard() { return is_hard; }

void big_crab::setHaveVase(bool b) { have_vase=b; }

bool big_crab::getHaveVase() { return have_vase; }

void big_crab::attachVase(bonus *p)
 {
 	if(random(500)==0 && isEnabled() && !getHaveVase() && (action==_RUN || action==_WALK) && !inScreen())
 	 {
 	  vase=p;
    vase->resetObj(true,11);
    vase->setVaseX(getPosXP()); vase->setVaseY(getPosYP());
    vase->setVasepx(0); vase->setVasepy(-32);
    vase->setAttached(true);
    setHaveVase(true);
   }
 }

void big_crab::resetObj(bool enb)
 {
  enabled=enb; hitted=0;
  action=_NONE;
  have_vase=false; // No vase at start
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; gravity=0;
  dx=0; dy=0; plat_y=start_y;
  energy=2;
  act_frame=0;
  life=random(300)+300; pos_x=random(2300);
  side=0; direction=0;
  plat_wall=0;
  for(int i=0;i<N_BU;i++) BU[i]->resetObj();
 }

void big_crab::debug()
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
