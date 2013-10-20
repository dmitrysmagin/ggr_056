class rain
 {
 	private:
 	 int start_x,start_y,pos_x,pos_y,scroll_x,scroll_y,act_x,act_y,jump_y,X,Y,_i;
 	 int plat_y,plat_stair,plat_wall;
 	 int *ctrl_x,*ctrl_y;
 	 int dx,dy,alpha;
 	 int r,g,b,count;
 	 int *RX,*RY,*RS,num_drops;
 	 bool active,stop,ison,type,raining;
 	 int max_drops;
 	 int act_frame,act_color;
 	 timer *ti[5];
 	 SAMPLE *sound[4];
 	 //BITMAP *frame[100];
 	 //BITMAP *frame_col[100];
 	 BITMAP *out,*rain_bmp;
 	 int TRA,COL1,COL2,COL3;
 	 unsigned char rain_map[640][50];
 	 map *plat_map;
 	 map *hight_map;
 	 map *back_map;

 	 void setOutBitmap(BITMAP *);
 	
 	public:
 	 rain(int,int,BITMAP *,int i=0,rain *p=NULL);
 	 ~rain();
 	 void drawSFX(int,int);
 	 void timeSFX();
 	 void platformSFX();
 	 void animSFX();
 	 void setPlatformMap(map *p);
 	 void setHightMap(map *p);
 	 void setBackgroundMap(map *p);
 	 void loadSample(char *,int);
 	 void assignSample(int,rain *);
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
 	 void resetSFX(bool,int);
 	 void debug();
 };

rain::rain(int sx, int sy, BITMAP *out, int i, rain *p)
 {
 	_i=i;
 	start_x=sx; start_y=sy;
 	num_drops=sy;
 	RX=(int *)malloc(sizeof(int)*num_drops);
 	RY=(int *)malloc(sizeof(int)*num_drops);
 	RS=(int *)malloc(sizeof(int)*num_drops);
 	setOutBitmap(out);
 	resetSFX(true,0);
 	// Crea rain
 	rain_bmp=create_bitmap(384,224);
 	ti[0]=new timer(2); // Drop time
 	ti[1]=new timer(0);
 	ti[2]=new timer(0);
 	ti[3]=new timer(0);
 	ti[4]=new timer(0);
 }

rain::~rain()
 {
 	for(int i=0;i<5;i++) delete(ti[i]);
 	destroy_bitmap(rain_bmp);
 	if(_i==0)
 	 {
    for(int i=0;i<0;i++)    destroy_sample(sound[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame_col[i]);
 	 }
 }

void rain::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void rain::setOutBitmap(BITMAP *bmp) { out=bmp; }

void rain::timeSFX()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void rain::animSFX()
 {
  if(!active) return;
  alpha=0;
  if(X<=0) alpha=-X;
  if(X<=-(rain_bmp->w-550)) alpha=180+(X+rain_bmp->w-550);
  if(alpha>180) alpha=180; if(alpha<0) alpha=0;
 }

void rain::drawSFX(int sx,int sy)
 {
 	if(!active) return;
 	if(*ctrl_x>=start_x) raining=1;
 	X=-sx+pos_x; Y=-sy+pos_y;
 	if(raining)
 	 {
 	  clear_to_color(rain_bmp,TRA);
    if(type) // Pioggia in primo piano
     {
      for(int i=0;i<max_drops;i++)
       {
     	  line(rain_bmp,RX[i],RY[i],RX[i]-4,RY[i]+11,COL1);
     	  line(rain_bmp,RX[i]-4,RY[i]+11,RX[i]-8,RY[i]+22,COL2);
     	  putpixel(rain_bmp,RX[i]-8,RY[i]+23,COL3);
     	  RY[i]+=RS[i];
     	  if(RY[i]>224) { RY[i]=-random(224); RX[i]=random(384*2); }
     	  RX[i]-=3;
       }
     }
    else // Pioggia in secondo piano
     {
      for(int i=0;i<max_drops;i++)
       {
     	  line(rain_bmp,RX[i],RY[i],RX[i]-8,RY[i]+16,COL1);
     	  RY[i]+=RS[i];
     	  if(RY[i]>224) { RY[i]=-random(224); RX[i]=random(384*2); }
     	  RX[i]-=4;
       }
     }
    if(max_drops<num_drops) if(ti[0]->isOn()) max_drops++;
 	 	masked_blit(rain_bmp,out,2,0,0,0,rain_bmp->w-4,rain_bmp->h);
   }
 }

void rain::platformSFX()
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

//BITMAP *rain::getActFrameColP() { return frame_col[act_frame]; }

bool rain::inScreen()
 {
 	if(alpha>0) return true;
 	else return false;
 }

bool rain::isOn() { return ison; }

int rain::getPlatYPos() { return plat_y; }

int rain::getX() { return X; }

int rain::getY() { return Y; }

void rain::setStartX(int sx) { start_x=sx; }

void rain::setStartY(int sy) { start_y=sy; }

void rain::setCtrlXP(int *p) { ctrl_x=p; }

void rain::setCtrlYP(int *p) { ctrl_y=p; }

int *rain::getPosXP() { return &pos_x; }

int *rain::getPosYP() { return &pos_y; }

void rain::setPlatformMap(map *p) { plat_map=p; }

void rain::setHightMap(map *p) { hight_map=p; }

void rain::setBackgroundMap(map *p) { back_map=p; }

void rain::resetSFX(bool act,int ty)
 {
  type=ty;
  active=act;
 	act_x=pos_x=start_x; act_y=pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
 	dx=0; dy=0; plat_y=start_y;
 	ison=0; stop=1;
 	raining=0;
 	max_drops=0;
 	plat_wall=0;
 	alpha=0;
 	count=r=g=b=0;
 	act_frame=0; act_color=makecol32(r,g,b);
 	// Generate palette
 	TRA=16711935; // Transparent color (in truecolor mode)
 	COL1=makecol32(0x00,0x51,0xa5);
 	COL2=makecol32(0x00,0x71,0xc6);
 	COL3=makecol32(0x00,0x82,0xd6);
 	if(type) // Pioggia in primo piano
 	 {
 	  for(int i=0;i<num_drops;i++) { RX[i]=random(384*2); RY[i]=-random(224)-32; RS[i]=random(8)+4; }
 	 }  
  else // Pioggia in secondo piano
 	 {
 	  for(int i=0;i<num_drops;i++) { RX[i]=random(384*2); RY[i]=-random(224)-32; RS[i]=random(10)+5; }
 	 }  
 }

void rain::debug()
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
