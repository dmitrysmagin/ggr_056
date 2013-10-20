class water
 {
 	private:
 	 int start_x,start_y,pos_x,pos_y,scroll_x,scroll_y,act_x,act_y,jump_y,X,Y,_i;
 	 int plat_y,plat_stair,plat_wall;
 	 int *ctrl_x,*ctrl_y;
 	 int dx,dy,alpha,wlen;
 	 int r,g,b,count;
 	 int speed,energy;
 	 bool active,stop,ison,waveon;
 	 int act_frame,act_color;
 	 int lev,levdx; // Livello dell'acqua (altezza oscillazione onda)
 	 timer *ti[5];
 	 SAMPLE *sound[4];
 	 //BITMAP *frame[100];
 	 //BITMAP *frame_col[100];
 	 BITMAP *out,*water_bmp,*aux_bmp;
 	 map *plat_map;
 	 map *hight_map;
 	 map *back_map;

 	 void setOutBitmap(BITMAP *);
 	
 	public:
 	 water(int,int,BITMAP *,int i=0,water *p=NULL);
 	 ~water();
 	 void drawSFX(int,int);
 	 void timeSFX();
 	 void platformSFX();
 	 void animSFX();
 	 void setPlatformMap(map *p);
 	 void setHightMap(map *p);
 	 void setBackgroundMap(map *p);
   void setScrollSpeed(int);
   void setAlpha(int);
   void setWaterLen(int);
   void setWaveSpeed(int);
 	 void loadSample(char *,int);
 	 void assignSample(int,water *);
 	 //BITMAP *getActFrameColP();
 	 bool inScreen();
 	 bool isOn();
 	 int getPlatYPos();
 	 int getX();
 	 int getY();
 	 void setStartX(int);
 	 void setStartY(int);
 	 void setCtrlXP(int *);
   void setCtrlYP(int *);
 	 int *getPosXP();
 	 int *getPosYP();
 	 void resetSFX(bool);
 	 void debug();
 };

water::water(int sx, int sy, BITMAP *out, int i, water *p)
 {
 	_i=i;
 	start_x=sx; start_y=sy;
 	setOutBitmap(out);
 	resetSFX(true);
 	// Crea water
 	water_bmp=load_bitmap("data/sfx/water.bmp",NULL);
 	aux_bmp=create_bitmap(1,40);
 	ti[0]=new timer(6); // Scroll water time
 	ti[1]=new timer(50); // Up down water time
 	ti[2]=new timer(0);
 	ti[3]=new timer(0);
 	ti[4]=new timer(0);
 }

water::~water()
 {
 	for(int i=0;i<5;i++) delete(ti[i]);
 	destroy_bitmap(water_bmp);
 	destroy_bitmap(aux_bmp);
 	if(_i==0)
 	 {
    for(int i=0;i<0;i++) destroy_sample(sound[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame_col[i]);
 	 }
 }

void water::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void water::setOutBitmap(BITMAP *bmp) { out=bmp; }

void water::timeSFX()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void water::animSFX()
 {
 	if(!active) return;
  //if(X<=0) alpha=-X;
  //if(X<=-(water_bmp->w-550)) alpha=180+(X+water_bmp->w-550);
  //if(alpha>180) alpha=180; if(alpha<0) alpha=0;
 }

void water::drawSFX(int sx,int sy)
 {
  if(!active) return;
  X=-sx+pos_x; Y=-sy+pos_y;
  if(inScreen())
   {
  	if(ti[0]->isOn())
  	 {
  	  blit(water_bmp,aux_bmp,0,0,0,0,1,40);
  	  blit(water_bmp,water_bmp,1,0,0,0,water_bmp->w-1,40);
  	  blit(aux_bmp,water_bmp,0,0,water_bmp->w-1,0,1,40);
  	 }
  	if(waveon)
  	 {
  	  if(lev>3) levdx=-1; if(lev<-1) levdx=1;
  	  if(ti[1]->isOn()) lev+=levdx;
  	 }
  	else lev=1;
  	drawing_mode(DRAW_MODE_TRANS,out,0,0);
  	set_trans_blender(0,0,0,alpha);
  	for(int i=0;i<wlen;i++) draw_trans_sprite(out,water_bmp,(X+i*192)-128,Y+lev);
  	set_trans_blender(0,0,0,255);
  	drawing_mode(DRAW_MODE_SOLID,out,0,0);
   }
 }

void water::platformSFX()
 {
 	if(!active) return;
 	/*if(hitted || explode) return;
 	plat_wall=0;
 	if(!jump_down && !jump_up && !rise && !unrise && !hitted && !explode)
 	 {
 	  if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
 	 }
 	plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	if(!unrise) plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
 	plat_stair=0;//plat_map->getActStairEnemies(1);*/
 }

//BITMAP *water::getActFrameColP() { return frame_col[act_frame]; }

bool water::inScreen()
 {
  if(alpha>0) return true;
  else return false;
 }

bool water::isOn() { return ison; }

int water::getPlatYPos() { return plat_y; }

int water::getX() { return X; }

int water::getY() { return Y; }

void water::setStartX(int sx) { start_x=sx; }

void water::setStartY(int sy) { start_y=sy; }

void water::setCtrlXP(int *p) { ctrl_x=p; }

void water::setCtrlYP(int *p) { ctrl_y=p; }

int *water::getPosXP() { return &pos_x; }

int *water::getPosYP() { return &pos_y; }

void water::setScrollSpeed(int s) { ti[0]->setTick(s); }

void water::setAlpha(int a) { alpha=a; }

void water::setWaveSpeed(int s)   { if(s==-1) waveon=false; else ti[1]->setTick(s); }

void water::setWaterLen(int l)    { wlen=l; }

void water::setPlatformMap(map *p) { plat_map=p; }

void water::setHightMap(map *p) { hight_map=p; }

void water::setBackgroundMap(map *p) { back_map=p; }

void water::resetSFX(bool act)
 {
 	active=act;
 	act_x=pos_x=start_x; act_y=pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
 	dx=0; dy=0; plat_y=start_y;
 	ison=0; stop=1; lev=-2; levdx=1;
 	plat_wall=0;
 	alpha=128;
 	wlen=5;
 	count=r=g=b=0;
 	waveon=true;
 	act_frame=0; act_color=makecol32(r,g,b);
 }

void water::debug()
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
