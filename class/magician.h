class magician : public _sprite
 {
  private:
   enum { N_BU=6 };
   enum { _NONE=0, _START=1, _OPEN=2, _SHOOT=3, _CLOSE=4, _EXPLODE=5 };
   bool have_vase;
   int dx,dy,pos_x,pos_y,scroll_x,scroll_y,gravity;
   int *ctrl_x,*ctrl_y;
   int plat_y,plat_stair,plat_wall;
   int energy,counter;
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
   magician(char *,int,int,BITMAP *,int i=0,magician *p=NULL);
   ~magician();
   void setPosX(int);
   void setPosY(int);
   void startAction();
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
   void assignSample(int,magician *);
   BITMAP *getActFrameColP();
   bullet *getBullet(int); 	 
   bool inScreen();
   int getPlatYPos();
   void attachVase(bonus *);
   void setHaveVase(bool);
   bool getHaveVase();
   int getNBullet();
   void hitAllBullets();
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

magician::magician(char *filename, int sx, int sy, BITMAP *bmp, int i, magician *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  // Bullet (assegno i frame ai proiettili prendendoli dalla memoria già allocata)
  // Per adesso nessun "sound" è assegnato ai proiettili
  //BU[0]->loadSample("data/sound/fx/hard.wav",0);  	
  for(int i=0;i<N_BU;i++)
   {
   	BU[i]=new bullet("",screen,1,24,27,2);
    //BU[i]->assignSample(0,BU[0]);
  	for(int j=0;j<rows*columns;j++)
     {
      BU[i]->setFrameP(j,frames[j]);
      BU[i]->setFrameColP(j,frames_collision[j]);
     }
   }
  resetObj(true);
  ti[0]=new timer(3); // Anim time
  ti[1]=new timer(1); 
  ti[2]=new timer(1); 
  ti[3]=new timer(1); 
  ti[4]=new timer(1);
 }

magician::~magician()
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

void magician::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void magician::assignSample(int n,magician *p)
 {
 	sound[n]=p->sound[n];
 }

void magician::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void magician::shootBullet()
 {
 	for(int i=0;i<N_BU;i++) if(!BU[i]->isFired())
 	 {
 	 	BU[i]->shoot(pos_x-10,pos_y+7,*ctrl_x,*ctrl_y+7,side);
 	 	BU[i]->setAngle(i*42);
 	 }
 }

void magician::animSprite()
 {
  if(!enabled) return;
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { act_frame=7; action=_EXPLODE; play_sample(sound[1],120,150,1000,0); ti[0]->reset();} } // TODO suono esplosione da fare
 	if(action==_START)
 	 {
 	 	energy=1; act_frame=0; counter=0;
 	 	if(pos_x<*ctrl_x) side=0; else side=1;
 	 	action=_OPEN;
   }
 	if(action==_OPEN)
 	 {
 	 	if(pos_x<*ctrl_x) side=0; else side=1;
    if(ti[0]->isOn()) act_frame++; if(act_frame==5) { act_frame=3; counter++; }
 	  if(counter==4) action=_SHOOT;	
 	 }
 	if(action==_SHOOT) 
 	 {
 	 	if(pos_x<*ctrl_x) side=0; else side=1;
 	  if(ti[0]->isOn()) act_frame++; if(act_frame==7) { shootBullet(); action=_CLOSE; }
 	 }
 	if(action==_CLOSE)
 	 {
    if(ti[0]->isOn()) act_frame--; if(act_frame==0) action=_NONE;
 	 }
 	if(action==_EXPLODE) 
 	 {
 	 	if(ti[0]->isOn()) act_frame++;
 	 	if(act_frame==14) { act_frame=0; action=_NONE; }
 	 }
 }

void magician::drawSprite(int sx,int sy)
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
 	if(act_frame>=3 && act_frame<=6) can_hit=true;  else can_hit=false;
  if(act_frame>=5 && act_frame<=6) hittable=true; else hittable=false;
 	if(inScreen())
 	 {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }

 	// Bullet
  for(int i=0;i<N_BU;i++) { BU[i]->drawSprite(sx,sy); BU[i]->timeSprite(); BU[i]->animSprite(); }
 }

void magician::platformSprite()
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

bullet *magician::getBullet(int n) { return BU[n]; }

BITMAP *magician::getActFrameColP() { return frames_collision[act_frame]; }

bool magician::inScreen()
 {
 	if(X>=-72 && X<=384) return true;
 	else return false;
 }

void magician::hitAllBullets() { for(int i=0;i<N_BU;i++) BU[i]->hit(); }
	
int magician::getPlatYPos() { return plat_y; }

int *magician::getPosXP() { return &pos_x; }

int *magician::getPosYP() { return &pos_y; }

int magician::getScore() { if(energy==1) { if(have_vase) { vase->setAttached(false); have_vase=0; } return 200; } else return 0; }

int magician::getNBullet() { return N_BU; }

void magician::setPlatformMap(map *p) { plat_map=p; }

void magician::setHightMap(map *p) { hight_map=p; }

void magician::setBackgroundMap(map *p) { back_map=p; }

void magician::setHitted(bool b) { hitted=b; }

void magician::setCtrlXP(int *p) { ctrl_x=p; }

void magician::setCtrlYP(int *p) { ctrl_y=p; }

bool magician::canHit() { return can_hit; }

bool magician::isHittable() { return hittable; }

bool magician::isHard() { return is_hard; }

void magician::startAction() { action=_START; }
	
void magician::setPosX(int px) { pos_x=px; }

void magician::setPosY(int py) { pos_y=py; }
	
void magician::setHaveVase(bool b) { have_vase=b; }

bool magician::getHaveVase() { return have_vase; }

void magician::attachVase(bonus *p)
 {
  if(random(500)==0 && isEnabled() && !getHaveVase() && (action==_OPEN || action==_SHOOT) && !inScreen())
   {
    vase=p;
    vase->resetObj(true,11);
    vase->setVaseX(getPosXP()); vase->setVaseY(getPosYP());
    vase->setVasepx(0); vase->setVasepy(-32);
    vase->setAttached(true);
    setHaveVase(true);
   }
 }

void magician::resetObj(bool enb)
 {
 	enabled=enb; hitted=0;
 	action=_NONE;
 	have_vase=false; // No vase at start
 	pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; gravity=0;
 	dx=0; dy=0; plat_y=start_y;
  energy=1;
  counter=0;
 	act_frame=0;
 	side=0; direction=0;
 	plat_wall=0;
 	for(int i=0;i<N_BU;i++) BU[i]->resetObj();
 }

void magician::debug()
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
