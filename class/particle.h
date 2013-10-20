class particle
 {
  private:
   enum { _COL=52, _ROW=1, _XPIX=22, _YPIX=22 };
   enum { _NONE=0, _READY=1, _RUN=2 };
   int dx,dy,start_x,start_y,pos_x,pos_y,scroll_x,scroll_y,gravity,X,Y,_i;
   int *ctrl_x,*ctrl_y;
   int plat_y,plat_stair,plat_wall;
   int act_frame,action,life,alpha;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool can_hit,hittable,is_hard;
   bool active,hitted;
   timer *ti[5];
   SAMPLE *sound[4];
   BITMAP *frame[_COL*_ROW];
   BITMAP *frame_col[_COL*_ROW];
   BITMAP *out;
   map *plat_map;
   map *hight_map;
   map *back_map;

   void loadSFXFrames(char *);
   void assignSFXFrame(particle *);
   void setOutBitmap(BITMAP *);
 	
  public:
   particle(char *,int,int,BITMAP *,int i=0,particle *p=NULL);
   ~particle();
   void setStartX(int);
   void setStartY(int);
   bool isActive();
   void drawSFX(int,int);
   void timeSFX();
   void platformSFX();
   void animSFX();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   void loadSample(char *,int);
   void assignSample(int,particle *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getPlatYPos();
   int getX();
   int getY();
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

particle::particle(char *filename, int sx, int sy, BITMAP *out, int i, particle *p)
 {
  _i=i;
  if(!_i) loadSFXFrames(filename); else assignSFXFrame(p);
  setOutBitmap(out);
  start_x=sx; start_y=sy;
  resetObj(true);
  ti[0]=new timer(8); // Anim time
  ti[1]=new timer(0); // Up time
  ti[2]=new timer(0); // Down time
  ti[3]=new timer(0); // Left time
  ti[4]=new timer(0); // Right time
 }

particle::~particle()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(_i==0)
   {
    for(int i=0;i<3;i++)         destroy_sample(sound[i]);
 	for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame[i]);
 	for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame_col[i]);
   }
 }

void particle::loadSFXFrames(char *filename)
 {
  BITMAP *aux;

  aux=load_bmp(filename,NULL);
  //for(int i=0;i<_COL;i++) frame[i]=create_sub_bitmap(aux,46*i+i,0,46,46);
  for(int i=0;i<_ROW;i++)
   for(int j=0;j<_COL;j++)
    {
     //frame[j+_COL*i]=create_sub_bitmap(aux,46*j+j,46*i+i,46,46);
     frame[j+_COL*i]=create_bitmap(_XPIX,_YPIX);
     blit(aux,frame[j+_COL*i],_XPIX*j+j,_YPIX*i+i,0,0,frame[j+_COL*i]->w,frame[j+_COL*i]->h);
     frame_col[j+_COL*i]=create_bitmap_ex(8,_XPIX,_YPIX);
     clear_to_color(frame_col[j+_COL*i],0);
     for(int h=0;h<frame[j+_COL*i]->h;h++)
      for(int w=0;w<frame[j+_COL*i]->w*4;w+=4)
       if(makecol32(frame[j+_COL*i]->line[h][w+2],
                    frame[j+_COL*i]->line[h][w+1],
                    frame[j+_COL*i]->line[h][w])!=16711935)
        frame_col[j+_COL*i]->line[h][w>>2]=1;
    }
  destroy_bitmap(aux);
 }

void particle::assignSFXFrame(particle *p)
 {
  for(int i=0;i<_ROW*_COL;i++)
   {
    frame[i]=p->frame[i];
    frame_col[i]=p->frame_col[i];
   }
 }

void particle::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void particle::assignSample(int n,particle *p)
 {
  sound[n]=p->sound[n];
 }

void particle::setOutBitmap(BITMAP *bmp) { out=bmp; }

void particle::timeSFX()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void particle::animSFX()
 {
 	if(!active) return;
 	//if(pos_x<*ctrl_x-400 || pos_x>*ctrl_x+400 || pos_y>990) action=_NONE;
  //if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; energy--; if(energy==0) { act_frame=19; action=_EXPLODE; play_sample(sound[1],120,150,1000,0); ti[2]->reset();} } // TODO suono esplosione da fare
 	if(action==_NONE) 
 	 {
 	 	// Smoke
 	 	pos_x=start_x; pos_y=start_y; life=random(80)+180; alpha=200; act_frame=0;
 	 	ti[1]->setTick(random(1)); ti[1]->reset();
 	 	ti[2]->setTick(random(20)+12); ti[2]->reset();
 	 	ti[3]->setTick(random(20)+12); ti[3]->reset();
 	 	action=_READY;
 	 	
 	 	/*if(*ctrl_x>200 && *ctrl_x<1900)
 	 	 if(random(100)==0)
 	 	  if(random(1)==0) { pos_x=*ctrl_x+200+random(200); pos_y=*ctrl_y-200; dx=-(random(1)+1); dy=random(3)+1; gravity=1; side=1; action=_JUMP_DOWN; }
 	 	  else             { if(random(1)==0) { pos_x=*ctrl_x+200+random(200); side=1; } else { pos_x=*ctrl_x-(200+random(100)); side=0; } pos_y=*ctrl_y-200; dx=2; dy=0; act_frame=8; action=_RUN; }*/
   }
  if(action==_READY) { if(random(20)==0) action=_RUN; }
 	if(action==_RUN) 
 	 {
 	 	if(ti[0]->isOn()) act_frame++; if(act_frame>16) act_frame=16;
 	 	if(ti[1]->isOn()) pos_y--;
 	  if(ti[2]->isOn()) pos_x--;
 	  if(ti[3]->isOn()) pos_x++;
 	 	life--; alpha--; if(alpha<0) alpha=0;
 	 	if(life==0) action=_NONE;
 	 }

/* 	if(action==_HIT_GROUND) 
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
 	 	//pos_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
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
 	 	//if(plat_y>pos_y+7) dx=-dx; else pos_y=plat_y; // Evita la caduta 	 	
 	 	if(random(160)==0) { dx=-dx; ti[3]->reset(); action=_SHOOT; act_frame=15; }
 	 	if(random(200)==0) { ti[0]->reset(); action=_CLOSE; act_frame=7; }
 	 }
 	if(action==_RUN) 
 	 {
 	 	if(side) pos_x-=dx; else pos_x+=dx;
 	 	if(ti[0]->isOn()) act_frame++;
 	 	if(act_frame>15) act_frame=8;
 	 	if(random(200)==0) { action=_WALK; dx=1; } // Controllare dx=1;
 	 	//pos_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	 }
 	if(action==_EXPLODE) 
 	 {
 	 	if(ti[2]->isOn()) act_frame++;
 	 	if(act_frame==25)
 	 	 {
 	 	  act_frame=0;
 	 	  action=_NONE;
 	 	 }
 	 }*/
 }

void particle::drawSFX(int sx,int sy)
 {
  if(!active) return;
  if(!action)
   {
  	can_hit=hittable=is_hard=false;
  	return;
   }
  X=-sx+pos_x-11; Y=-sy+pos_y-12;

  drawing_mode(DRAW_MODE_TRANS,out,0,0);
  set_trans_blender(0,0,0,alpha);
  draw_trans_sprite(out,frame[act_frame],X,Y);
  //putpixel(out,_X,_Y,makecol(220,220,220));
  //if(ti[1]->isOn()) putpixel(out,X,Y+1,makecol(190,190,190));
  set_trans_blender(0,0,0,255);
  drawing_mode(DRAW_MODE_SOLID,out,0,0);

/* 	if(act_frame>=1 && act_frame<=17) can_hit=true;  else can_hit=false;
  if(act_frame>=6 && act_frame<=17) hittable=true; else hittable=false;
  if(act_frame>=1 && act_frame<=5) is_hard=true;  else is_hard=false;
 	if(inScreen())
 	 {
    if(side) draw_sprite_h_flip(out,frame[act_frame],X,Y);
    else     draw_sprite       (out,frame[act_frame],X,Y);
   }*/
 }

void particle::platformSFX()
 {
 	//if(!active) return;
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

BITMAP *particle::getActFrameColP() { return frame_col[act_frame]; }

bool particle::inScreen()
 {
  if(X>=-72 && X<=384) return true;
  else return false;
 }

int particle::getPlatYPos() { return plat_y; }

int particle::getX() { return X; }

int particle::getY() { return Y; }

int *particle::getPosXP() { return &pos_x; }

int *particle::getPosYP() { return &pos_y; }

int particle::getScore() { return 0; }

void particle::setPlatformMap(map *p) { plat_map=p; }

void particle::setHightMap(map *p) { hight_map=p; }

void particle::setBackgroundMap(map *p) { back_map=p; }

void particle::setHitted(bool b) { hitted=b; }

void particle::setCtrlXP(int *p) { ctrl_x=p; }

void particle::setCtrlYP(int *p) { ctrl_y=p; }

bool particle::canHit() { return can_hit; }

bool particle::isHittable() { return hittable; }

bool particle::isHard() { return is_hard; }

bool particle::isActive() { return active; }

void particle::setStartX(int sx) { start_x=sx; }

void particle::setStartY(int sy) { start_y=sy; }

void particle::resetObj(bool act)
 {
 	active=act; hitted=0;
 	action=_NONE;
 	pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; gravity=0;
 	dx=0; dy=0; plat_y=start_y;
  alpha=255;
 	act_frame=0;
 	life=random(300)+300; //pos_x=random(2300);
 	side=0; direction=0;
 	plat_wall=0;
 }

void particle::debug()
 {
 	putpixel(out,pos_x,pos_y-30+58,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+59,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+61,makecol32(255,0,0));
  putpixel(out,pos_x,pos_y-30+62,makecol32(255,0,0));
  putpixel(out,pos_x-2,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x+2,pos_y-30+60,makecol32(255,0,0));
  putpixel(out,pos_x-1,pos_y-30+61,makecol32(255,0,0));
  putpixel(out,pos_x+1,pos_y-30+61,makecol32(255,0,0));
 }
