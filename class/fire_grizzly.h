class fire_grizzly : public _sprite
 {
  private:
   enum { N_BU=8 };
   int max_x,min_x,pos_x,pos_y,scroll_x,scroll_y,jump_y;
   int plat_y,plat_stair,plat_wall;
   int *ctrl_x,*ctrl_y;
   int energy;
   int dx,dy;
   bool can_hit,hittable;
   bool near,side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool stop,move,fire,open,hitted,first_hit,dead,explode;
   timer *ti[5];
   bullet *BU[N_BU];
   SAMPLE *sound[4];
   map *plat_map;
   map *hight_map;
   map *back_map;

   void shootBullet();
 	
  public:
   fire_grizzly(char *,int,int,BITMAP *,int i=0,fire_grizzly *p=NULL);
   ~fire_grizzly();
   void setMaxX(int);
   void setMinX(int);
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,fire_grizzly *);
   BITMAP *getActFrameColP();
   bullet *getBullet(int);
   bool inScreen();
   void setHitted(bool b);
   bool canHit();
   bool isHittable();
   int getPlatYPos();
   int getNBullet();
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   int *getPosXP();
   int *getPosYP();
   int getScore();
   void resetObj(bool);
   void debug();
 };

fire_grizzly::fire_grizzly(char *filename, int sx, int sy, BITMAP *bmp, int i, fire_grizzly *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  min_x=0; max_x=0;
  // Bullet (assegno i frame ai proiettili prendendoli dalla memoria già allocata)
  // Per adesso nessun "sound" è assegnato ai proiettili
  //BU[0]->loadSample("data/sound/fx/hard.wav",0);  	
  for(int i=0;i<N_BU;i++)
   {
   	BU[i]=new bullet("",bmp,1,36,39);
    //BU[i]->assignSample(0,BU[0]);
  	for(int j=0;j<rows*columns;j++)
     {
      BU[i]->setFrameP(j,frames[j]);
      BU[i]->setFrameColP(j,frames_collision[j]);
     }
   }
  resetObj(true);
  ti[0]=new timer(8);  // Anim time
  ti[1]=new timer(2);  // Explode time
  ti[2]=new timer(5);  // Hitted time (stop time)
  ti[3]=new timer(random(100)+100); // Fire time
  ti[4]=new timer(1);  // Move time
 }

fire_grizzly::~fire_grizzly()
 {
  for(int i=0;i<N_BU;i++) delete(BU[i]);
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<3;i++)            destroy_sample(sound[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
 	for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void fire_grizzly::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void fire_grizzly::assignSample(int n,fire_grizzly *p)
 {
  sound[n]=p->sound[n];
 }

void fire_grizzly::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void fire_grizzly::shootBullet()
 {
 	int n=-1;
 	
 	//play_sample(sound[0],120,150,1000,0);
 	for(int i=0;i<N_BU;i++) if(!BU[i]->isFired()) { n=i; break; }
 	if(n!=-1)
 	 {
 	 	if(fire)
 	 	 {
  	  BU[n]->shoot(pos_x,pos_y,*ctrl_x,*ctrl_y+7,side);
  	 }
   }
 }

void fire_grizzly::animSprite()
 {
 	if(!enabled) return;
 	if(dead) return;
 	if(explode) 
 	 {
 	 	if(ti[1]->isOn()) act_frame++;
 	 	if(act_frame==15)
 	 	 {
 	 	  act_frame=0; dead=1;
 	 	  hitted=0;	   explode=0;
 	 	  move=0;      open=0;
 	 	  fire=0;
 	 	 }
 	 	return;
 	 }
 	near=pos_x>*ctrl_x-150 && pos_x<*ctrl_x+150 || first_hit;
 	if(ti[3]->isOn() && ((side==0 && *ctrl_x>pos_x) || (side==1 && *ctrl_x<pos_x))) { fire=1; move=0; open=1; ti[0]->reset(); act_frame=6; }
 	if(move)
 	 {
 	  if(side) dx=-1; else dx=1;
 	  if(ti[0]->isOn()) { act_frame++; }
 	  if(act_frame>4) act_frame=1;
 	  if(!near) { if(ti[4]->isOn()) pos_x+=dx; } else pos_x+=dx;
 	  pos_y=plat_map->getActPlatformEnemies(pos_x,0);
 	 } 	    	
 	if(stop) { fire=0; open=0; dx=0; act_frame=5; move=0; if(ti[2]->isOn()) { stop=0; act_frame=1; open=0; move=1; } }
  if(open) { stop=0; if(ti[0]->isOn()) act_frame++; if(act_frame>7) { move=1; open=0; stop=0; act_frame=1; } } 	
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; stop=1; act_frame=5; ti[2]->reset(); energy--; if(energy==0) { open=0; explode=1; move=0; act_frame=8; stop=0; play_sample(sound[1],120,150,1000,0); } else explode=0; ti[2]->reset(); return; }
  if(fire) { if(act_frame==7) { shootBullet(); fire=0; } }
 	if(!near)
 	 {
 	  if(pos_x<min_x) { side=0; } if(pos_x>max_x) { side=1; }
 	 	ti[0]->setTick(8);
   }
  else // fire_grizzly near the player
   {
 	  if(*ctrl_x+48<pos_x && pos_x>min_x+100) side=1; if(*ctrl_x-48>pos_x && pos_x<max_x-100) side=0;
 	  if(pos_x<min_x) { side=0; } if(pos_x>max_x) { side=1; }
 	  ti[0]->setTick(4);
   }
 }

void fire_grizzly::drawSprite(int sx,int sy)
 {
//if(index==0) textprintf_ex(screen,font,0,70,makecol32(255,0,0),-1,"pos_x: %d: min_x:%d max_x:%d",pos_x,min_x,max_x);
 	if(!enabled) return;
  //pivot_sprite(rend,car,rend_x,rend_y,piv_cx,piv_cy,itofix(ang1));
  X=-sx+pos_x-30; Y=-sy+pos_y-30;
  if((act_frame>0 && act_frame<6) || (act_frame>=6 && act_frame<8)) can_hit=true; else can_hit=false;
  if(act_frame<5 || (act_frame>=6 && act_frame<8)) hittable=true; else hittable=false;
  if(inScreen())
   {
   	if(act_frame==3) play_sample(sound[2],120,150,1000,0);
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
   }
  // Bullet
  for(int i=0;i<N_BU;i++)
   {
   	BU[i]->drawSprite(sx,sy);
    BU[i]->timeSprite();
 	  BU[i]->animSprite();
 	  //BU[i]->platformSprite();
    //BU[i]->drawSprite(plat_map->getScrollX());
   }
 }

void fire_grizzly::platformSprite()
 {
 	if(index!=0) return; // Solo il primo "fire_grizzly" segue le piattaforme
 	if(!enabled) return;
 	if(explode || dead) return;
 	plat_wall=0;
 	if(!hitted && !explode)
 	 {
 	  if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
 	  //if(pos_y<plat_y) { fall=1; dy++; if(dy>6) dy=6; dx=0; } else { fall=0; dy=0; }
 	 }
 	plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	//if(!unrise) plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
 	plat_stair=0;//plat_map->getActStairEnemies(1);
 }

bullet *fire_grizzly::getBullet(int n) { return BU[n]; }

BITMAP *fire_grizzly::getActFrameColP() { return frames_collision[act_frame]; }

bool fire_grizzly::inScreen()
 {
  if(X>=-62 && X<=384) return true;
  else return false;
 }

int fire_grizzly::getPlatYPos() { return plat_y; }

int fire_grizzly::getScore() { if(energy==1) return 500; else return 0; }

int fire_grizzly::getNBullet() { return N_BU; }

void fire_grizzly::setCtrlXP(int *p) { ctrl_x=p; }

void fire_grizzly::setCtrlYP(int *p) { ctrl_y=p; }

int *fire_grizzly::getPosXP() { return &pos_x; }

int *fire_grizzly::getPosYP() { return &pos_y; }

void fire_grizzly::setPlatformMap(map *p) { plat_map=p; }

void fire_grizzly::setHightMap(map *p) { hight_map=p; }

void fire_grizzly::setBackgroundMap(map *p) { back_map=p; }

void fire_grizzly::setHitted(bool b) { hitted=b; first_hit=1; }

bool fire_grizzly::canHit() { return can_hit; }

bool fire_grizzly::isHittable() { return hittable; }

void fire_grizzly::setMaxX(int mx) { max_x=mx; }

void fire_grizzly::setMinX(int mx) { min_x=mx; }

void fire_grizzly::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  energy=20;
  dx=0; dy=0; plat_y=start_y; near=false; first_hit=0;
  act_frame=1;
  stop=0; move=1; fire=0; open=0; hitted=0; dead=0; explode=0;
  side=1; direction=0;
  for(int i=0;i<N_BU;i++) BU[i]->resetObj();
 }

void fire_grizzly::debug()
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
