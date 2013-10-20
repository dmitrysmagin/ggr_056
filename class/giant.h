class giant : public _sprite
 {
  private:
   enum { N_BU=4 };
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
   giant(char *,int,int,BITMAP *,int i=0,giant *p=NULL);
   ~giant();
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
   void assignSample(int,giant *);
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

giant::giant(char *filename, int sx, int sy, BITMAP *bmp, int i, giant *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  min_x=0; max_x=0;
  // Bullet (assegno i frame ai proiettili prendendoli dalla memoria già allocata)
  // Per adesso nessun "sound" è assegnato ai proiettili
  //BU[0]->loadSample("data/sound/fx/hard.wav",0);  	
  for(int i=0;i<N_BU;i++)
   {
  	BU[i]=new bullet("",bmp,1);
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
  ti[2]=new timer(4);  // Hitted time (stop time)
  ti[3]=new timer(random(100)+100); // Fire time
  ti[4]=new timer(1);  // Move time
 }

giant::~giant()
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

void giant::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void giant::assignSample(int n,giant *p)
 {
  sound[n]=p->sound[n];
 }

void giant::timeSprite()
 {
 for(int i=0;i<5;i++) ti[i]->time();
 }

void giant::shootBullet()
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

void giant::animSprite()
 {
 	if(!enabled) return;
 	if(dead) return;
 	if(explode) 
 	 {
 	 	if(ti[1]->isOn()) act_frame++;
 	 	if(act_frame==12)
 	 	 {
 	 	  act_frame=0; dead=1;
 	 	  hitted=0;	   explode=0;
 	 	  move=0;      open=0;
 	 	  fire=0;
 	 	 }
 	 	return;
 	 }
 	near=pos_x>*ctrl_x-150 && pos_x<*ctrl_x+150 || first_hit;
 	if(ti[3]->isOn() && ((side==0 && *ctrl_x>pos_x) || (side==1 && *ctrl_x<pos_x))) { fire=1; move=0; open=1; ti[0]->reset(); act_frame=25; }
 	if(move)
 	 {
 	  if(side) dx=-1; else dx=1;
 	  if(ti[0]->isOn()) { act_frame++; }
 	  if(act_frame>4) act_frame=1;
 	  if(!near) { if(ti[4]->isOn()) pos_x+=dx; } else pos_x+=dx;
 	  if(index==0) pos_y=plat_map->getActPlatformEnemies(pos_x,0);
 	 } 	    	
 	if(stop) { fire=0; open=0; dx=0; act_frame=5; move=0; if(ti[2]->isOn()) { stop=0; act_frame=1; open=0; move=1; } }
  if(open) { stop=0; if(ti[0]->isOn()) act_frame++; if(act_frame>26) { move=1; open=0; stop=0; act_frame=1; } } 	
  if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; stop=1; act_frame=5; ti[2]->reset(); energy--; if(energy==0) { open=0; explode=1; move=0; act_frame=6; stop=0; play_sample(sound[1],120,150,1000,0); } else explode=0; ti[2]->reset(); return; }
  if(fire) { if(act_frame==26) { shootBullet(); fire=0; } }
 	if(!near)
 	 {
 	  if(pos_x<min_x) { side=0; } if(pos_x>max_x) { side=1; }
 	 	ti[0]->setTick(8);
   }
  else // giant near the player
   {
 	  if(*ctrl_x+48<pos_x && pos_x>min_x+100) side=1; if(*ctrl_x-48>pos_x && pos_x<max_x-100) side=0;
 	  if(pos_x<min_x) { side=0; } if(pos_x>max_x) { side=1; }
 	  ti[0]->setTick(4);
   }
 }

void giant::drawSprite(int sx,int sy)
 {
 	if(!enabled) return;
  //pivot_sprite(rend,car,rend_x,rend_y,piv_cx,piv_cy,itofix(ang1));
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if((act_frame>0 && act_frame<6) || (act_frame>=25 && act_frame<27)) can_hit=true; else can_hit=false;
  if(act_frame<5 || (act_frame>=25 && act_frame<27)) hittable=true; else hittable=false;
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

void giant::platformSprite()
 {
 	if(index!=0) return; // Solo il primo "giant" segue le piattaforme
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

bullet *giant::getBullet(int n) { return BU[n]; }

BITMAP *giant::getActFrameColP() { return frames_collision[act_frame]; }

bool giant::inScreen()
 {
 	if(X>=-62 && X<=384) return true;
 	else return false;
 }

int giant::getPlatYPos() { return plat_y; }

int giant::getScore() { if(energy==1) return 400; else return 0; }

int giant::getNBullet() { return N_BU; }

void giant::setCtrlXP(int *p) { ctrl_x=p; }

void giant::setCtrlYP(int *p) { ctrl_y=p; }

int *giant::getPosXP() { return &pos_x; }

int *giant::getPosYP() { return &pos_y; }

void giant::setPlatformMap(map *p) { plat_map=p; }

void giant::setHightMap(map *p) { hight_map=p; }

void giant::setBackgroundMap(map *p) { back_map=p; }

void giant::setHitted(bool b) { hitted=b; first_hit=1; }

bool giant::canHit() { return can_hit; }

bool giant::isHittable() { return hittable; }

void giant::setMaxX(int mx) { max_x=mx; }

void giant::setMinX(int mx) { min_x=mx; }

void giant::resetObj(bool enb)
 {
  enabled=enb;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  energy=16;
  dx=0; dy=0; plat_y=start_y; near=false; first_hit=0;
  act_frame=1;
  stop=0; move=1; fire=0; open=0; hitted=0; dead=0; explode=0;
  side=1; direction=0;
  for(int i=0;i<N_BU;i++) BU[i]->resetObj();
 }

void giant::debug()
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
