class fire
 {
  private:
   int start_x,start_y,pos_x,pos_y,scroll_x,scroll_y,act_x,act_y,jump_y,X,Y,_i;
   int plat_y,plat_stair,plat_wall;
   int *ctrl_x,*ctrl_y;
   int dx,dy,alpha,power;
   int r,g,b,count;
   int speed,energy;
   bool active,stop,ison,fade;
   int act_frame,act_color;
   timer *ti[5];
   SAMPLE *sound[4];
   //BITMAP *frame[100];
   //BITMAP *frame_col[100];
   BITMAP *out,*fire_bmp;
   int PAL[255];
   unsigned char fire_map[220][80];
   map *plat_map;
   map *hight_map;
   map *back_map;

   void setOutBitmap(BITMAP *);
 	
  public:
   fire(int,int,BITMAP *,int i=0,fire *p=NULL);
   ~fire();
   void drawSFX(int,int);
   void timeSFX();
   void platformSFX();
   void animSFX();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,fire *);
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
   void setFade(bool);
   void setPower(int);
   int *getPosXP();
   int *getPosYP();
   void resetSFX(bool);
   void debug();
 };

fire::fire(int sx, int sy, BITMAP *out, int i, fire *p)
 {
  _i=i;
 	start_x=sx; start_y=sy;
 	setOutBitmap(out);
 	resetSFX(true);
 	// Crea fire
 	fire_bmp=create_bitmap(220,80);
 	ti[0]=new timer(0); // Anim time
 	ti[1]=new timer(0);
 	ti[2]=new timer(0);
 	ti[3]=new timer(0);
 	ti[4]=new timer(0);
 }

fire::~fire()
 {
 	for(int i=0;i<5;i++) delete(ti[i]);
 	destroy_bitmap(fire_bmp);
 	if(_i==0)
 	 {
    for(int i=0;i<0;i++)    destroy_sample(sound[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame_col[i]);
 	 }
 }

void fire::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void fire::setOutBitmap(BITMAP *bmp) { out=bmp; }

void fire::timeSFX()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void fire::animSFX()
 {
 	if(!active) return;
 	alpha=0;
  if(X<=0) alpha=-X;
  if(X<=-(fire_bmp->w-550)) alpha=180+(X+fire_bmp->w-550);
  if(alpha>180) alpha=180; if(alpha<0) alpha=0;
 }

void fire::drawSFX(int sx,int sy)
 {
  if(!active) return;
  X=-sx+pos_x; Y=-sy+pos_y;
  if(ti[0]->isOn())
   {
  	unsigned char c,c1,c2,c3;
  	int m;

    for(int x=0;x<fire_bmp->w;x++)
     {
     	if(fade) m=x>>4; else m=0;
     	fire_map[x][fire_bmp->h-1]=random(power-m);
     	putpixel(fire_bmp,x,fire_bmp->h-1,PAL[fire_map[x][fire_bmp->h-1]]);
     }
    for(int x=2;x<fire_bmp->w-2;x++)
     {
      for(int y=fire_bmp->h-2;y>=0;y-=2)
       {
        c1=fire_map[x-1][y+1];
        c2=fire_map[x][y+1];
        c3=fire_map[x+1][y+1];
        c=(c1+(c2<<1)+c3)>>2; if(c>1) c-=(random(1)+1);
        fire_map[x][y]=c; fire_map[x][y-1]=c;
        putpixel(fire_bmp,x,y,PAL[c]); putpixel(fire_bmp,x,y-1,PAL[c]);
       }
     }
   }
  masked_blit(fire_bmp,out,2,0,X+637,Y+12,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+422,Y+6,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+212,Y+5,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X,Y,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+1000,Y+180,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+1215,Y+180,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+1430,Y+180,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+1645,Y+180,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+1860,Y+180,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+2075,Y+180,fire_bmp->w-4,fire_bmp->h);
  masked_blit(fire_bmp,out,2,0,X+2200,Y+180,fire_bmp->w-4,fire_bmp->h);
 }

void fire::platformSFX()
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

//BITMAP *fire::getActFrameColP() { return frame_col[act_frame]; }

bool fire::inScreen()
 {
  if(X>=-32 && X<=384) return true;
  else return false;
 }

bool fire::isOn() { return ison; }

int fire::getPlatYPos() { return plat_y; }

int fire::getX() { return X; }

int fire::getY() { return Y; }

void fire::setStartX(int sx) { start_x=sx; }

void fire::setStartY(int sy) { start_y=sy; }

void fire::setCtrlXP(int *p) { ctrl_x=p; }

void fire::setCtrlYP(int *p) { ctrl_y=p; }

void fire::setFade(bool b) { fade=b; }

void fire::setPower(int pw) { power=pw; }

int *fire::getPosXP() { return &pos_x; }

int *fire::getPosYP() { return &pos_y; }

void fire::setPlatformMap(map *p) { plat_map=p; }

void fire::setHightMap(map *p) { hight_map=p; }

void fire::setBackgroundMap(map *p) { back_map=p; }

void fire::resetSFX(bool act)
 {
 	power=132; fade=1;
 	active=act;
 	act_x=pos_x=start_x; act_y=pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
 	dx=0; dy=0; plat_y=start_y;
 	ison=0; stop=1;
 	plat_wall=0;
 	alpha=0;
 	count=r=g=b=0;
 	act_frame=0; act_color=makecol32(r,g,b);
 	// Generate palette
 	PAL[0]=16711935; // Transparent color (in truecolor mode)	
 	for(int i=1;  i<128;i++) { PAL[i]=makecol32(128+i/2,i*2,0); }
 	for(int i=128;i<255;i++) { PAL[i]=makecol32(128+i/2,255,0); }
 }

void fire::debug()
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
