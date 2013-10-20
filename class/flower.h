class flower : public _sprite
 {
  private:
   enum { N_BU=3 };
   enum { _NONE=0, _RISE=1, _WALK=2, _SHOOT=3, _CLOSE=7, _EXPLODE=8 };
   bool have_vase;
   int dx,dy,pos_x,pos_y,scroll_x,scroll_y,gravity;
   int min_x,max_x;
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
   flower(char *,int,int,BITMAP *,int i=0,flower *p=NULL);
   ~flower();
   void setMinX(int);
   void setMaxX(int);
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
   void assignSample(int,flower *);
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

flower::flower(char *filename, int sx, int sy, BITMAP *bmp, int i, flower *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  min_x=max_x=0;
  // Bullet (assegno i frame ai proiettili prendendoli dalla memoria già allocata)
  // Per adesso nessun "sound" è assegnato ai proiettili
  //BU[0]->loadSample("data/sound/fx/hard.wav",0);  	
  for(int i=0;i<N_BU;i++)
   {
    BU[i]=new bullet("",bmp,i,48,51,3);
    //BU[i]->assignSample(0,BU[0]);
 	for(int j=0;j<rows*columns;j++)
 	 {
 	  BU[i]->setFrameP(j,frames[j]);
 	  BU[i]->setFrameColP(j,frames_collision[j]);
 	 }
   }
  resetObj(true);
  ti[0]=new timer(5);   // Anim time
  ti[1]=new timer(400+random(200)); // Restart time
  ti[2]=new timer(200); // Shoot time
  ti[3]=new timer(10);  // Shoot anim time
  ti[4]=new timer(0);
 }

flower::~flower()
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

void flower::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void flower::assignSample(int n,flower *p)
 {
  sound[n]=p->sound[n];
 }

void flower::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void flower::shootBullet()
 {
  //play_sample(sound[0],120,150,1000,0);
  for(int i=0;i<N_BU;i++)
   if(!BU[i]->isFired())
    BU[i]->shoot(pos_x-10,pos_y+7,*ctrl_x,*ctrl_y+7,side);
 }

void flower::animSprite()
 {
  if(!enabled) return;
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { act_frame=15; action=_EXPLODE; play_sample(sound[1],120,150,1000,0); ti[2]->reset();} } // TODO suono esplosione da fare
 	if(action==_NONE) 
 	 {
 	 	if(ti[1]->isOn())
 	 	 {
 	 	 	dx=0; dy=0; energy=2; side=1; pos_x=start_x; pos_y=start_y;
 	 	 	act_frame=0;
 	 	 	action=_RISE;
 	 	 	ti[0]->reset();
 	 	 }
   }
 	if(action==_RISE) 
 	 {
    if(ti[0]->isOn()) act_frame++; if(act_frame>10) { action=_WALK; if(random(1)) { dx=1; side=1; } else { dx=-1; side=0; } ti[0]->reset(); ti[2]->reset(); }
    plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
    pos_y=plat_y;
 	 }
 	if(action==_WALK) 
 	 {
 	 	if(ti[2]->isOn()) { action=_SHOOT; act_frame=24; ti[3]->reset(); }
 	 	if(ti[0]->isOn()) act_frame++; if(act_frame==15) act_frame=12;
 	 	if(pos_x>max_x) { dx=-1; side=0; } if(pos_x<min_x) { dx=1; side=1; }
 	  pos_x+=dx;
    plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
    pos_y=plat_y;
   }
 	if(action==_SHOOT) 
 	 {
 	 	if(ti[3]->isOn()) { act_frame++; if(act_frame==25) shootBullet(); }
 	 	if(act_frame==26) { act_frame=12; action=_WALK; ti[0]->reset(); }
 	 }
 	if(action==_EXPLODE) 
 	 {
 	 	if(ti[0]->isOn()) act_frame++;
 	 	if(act_frame==21) { act_frame=0; action=_NONE; }
 	 } 	 
 	/*if(action==_JUMP_UP) 
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
 	if(action==_RUN) 
 	 {
 	 	if(side) pos_x-=dx; else pos_x+=dx;
 	 	if(ti[0]->isOn()) act_frame++;
 	 	if(act_frame>15) act_frame=8;
 	 	if(random(200)==0) { action=_WALK; dx=1; } // Controllare dx=1;
 	 	pos_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	 }*/
 }

void flower::drawSprite(int sx,int sy)
 {
 	if(!enabled) return;
 	if(!action)
 	 {
 	 	can_hit=hittable=is_hard=false;
 	 	// Bullet
    for(int i=0;i<N_BU;i++) { BU[i]->drawSprite(sx,sy); BU[i]->timeSprite(); BU[i]->animSprite(); }
 	 	return;
 	 }
 	X=-sx+pos_x-31; Y=-sy+pos_y-12;
 	hittable=false;
 	is_hard =false;
 	can_hit =false;
 	if(act_frame>=7  && act_frame<=14) { can_hit=true; hittable=true; }
  if(act_frame>=24 && act_frame<=25) { can_hit=true; hittable=true; }
 	if(inScreen())
 	 {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }

 	// Bullet
  for(int i=0;i<N_BU;i++) { BU[i]->drawSprite(sx,sy); BU[i]->timeSprite(); BU[i]->animSprite(); }
 }

void flower::platformSprite()
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

bullet *flower::getBullet(int n) { return BU[n]; }

BITMAP *flower::getActFrameColP() { return frames_collision[act_frame]; }

bool flower::inScreen()
 {
 	if(X>=-72 && X<=384) return true;
 	else return false;
 }

int flower::getPlatYPos() { return plat_y; }

int *flower::getPosXP() { return &pos_x; }

int *flower::getPosYP() { return &pos_y; }

int flower::getScore() { if(energy==1) { if(have_vase) { vase->setAttached(false); have_vase=0; } return 200; } else return 0; }

int flower::getNBullet() { return N_BU; }

void flower::setPlatformMap(map *p) { plat_map=p; }

void flower::setHightMap(map *p) { hight_map=p; }

void flower::setBackgroundMap(map *p) { back_map=p; }

void flower::setHitted(bool b) { hitted=b; }

void flower::setCtrlXP(int *p) { ctrl_x=p; }

void flower::setCtrlYP(int *p) { ctrl_y=p; }

bool flower::canHit() { return can_hit; }

bool flower::isHittable() { return hittable; }

bool flower::isHard() { return is_hard; }

void flower::setMinX(int mx) { min_x=mx; }

void flower::setMaxX(int mx) { max_x=mx; }

void flower::setHaveVase(bool b) { have_vase=b; }

bool flower::getHaveVase() { return have_vase; }

void flower::attachVase(bonus *p)
 {
  if(random(500)==0 && isEnabled() && !getHaveVase() && (action==_RISE || action==_WALK) && !inScreen())
   {
    vase=p;
    vase->resetObj(true,11);
    vase->setVaseX(getPosXP()); vase->setVaseY(getPosYP());
    vase->setVasepx(0); vase->setVasepy(-32);
    vase->setAttached(true);
    setHaveVase(true);
   }
 }

void flower::resetObj(bool enb)
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

void flower::debug()
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
