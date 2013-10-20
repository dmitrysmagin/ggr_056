class fog
 {
 	private:
 	 int start_x,start_y,pos_x,pos_y,scroll_x,scroll_y,act_x,act_y,jump_y,X,Y,_i;
 	 int plat_y,plat_stair,plat_wall;
 	 int *ctrl_x,*ctrl_y;
 	 int dx,dy,alpha;
 	 int r,g,b,count;
 	 int speed,energy;
 	 bool active,stop,ison;
 	 int act_frame,act_color;
 	 timer *ti[5];
 	 SAMPLE *sound[4];
 	 //BITMAP *frame[100];
 	 //BITMAP *frame_col[100];
 	 BITMAP *out,*fog_bmp,*aux_bmp;
 	 map *plat_map;
 	 map *hight_map;
 	 map *back_map;

 	 void setOutBitmap(BITMAP *);
 	
 	public:
 	 fog(int,int,BITMAP *,int i=0,fog *p=NULL);
 	 ~fog();
 	 void drawSFX(int,int);
 	 void timeSFX();
 	 void platformSFX();
 	 void animSFX();
 	 void setPlatformMap(map *p);
 	 void setHightMap(map *p);
 	 void setBackgroundMap(map *p);
 	 void loadSample(char *,int);
 	 void assignSample(int,fog *);
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

fog::fog(int sx, int sy, BITMAP *out, int i, fog *p)
 {
 	_i=i;
 	start_x=sx; start_y=sy;
 	setOutBitmap(out);
 	resetSFX(true);
 	// Crea fog
 	fog_bmp=load_bitmap("data/sfx/fog.bmp",NULL);
 	aux_bmp=create_bitmap(1,224);
 	ti[0]=new timer(3); // Anim time
 	ti[1]=new timer(200); // Anim pause time
 	ti[2]=new timer(0);
 	ti[3]=new timer(0);
 	ti[4]=new timer(0);
 }

fog::~fog()
 {
 	for(int i=0;i<5;i++) delete(ti[i]);
 	destroy_bitmap(fog_bmp);
 	destroy_bitmap(aux_bmp);
 	if(_i==0)
 	 {
    for(int i=0;i<0;i++)    destroy_sample(sound[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame[i]);
 	  //for(int i=0;i<25*3;i++) destroy_bitmap(frame_col[i]);
 	 }
 }

void fog::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void fog::setOutBitmap(BITMAP *bmp) { out=bmp; }

void fog::timeSFX()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void fog::animSFX()
 {
 	if(!active) return;
 	alpha=0;
  if(X<=0) alpha=-X;
  if(X<=-(fog_bmp->w-550)) alpha=180+(X+fog_bmp->w-550);
  if(alpha>180) alpha=180; if(alpha<0) alpha=0;
 }

void fog::drawSFX(int sx,int sy)
 {
  if(!active) return;
  X=-sx+pos_x; Y=-sy+pos_y;
  if(inScreen())
   {
    blit(fog_bmp,aux_bmp,0,0,0,0,1,224);
    blit(fog_bmp,fog_bmp,1,0,0,0,fog_bmp->w-1,224);
    blit(aux_bmp,fog_bmp,0,0,fog_bmp->w-1,0,1,224);
    drawing_mode(DRAW_MODE_TRANS,out,0,0);
    set_trans_blender(0,0,0,alpha);
    draw_trans_sprite(out,fog_bmp,X,Y);
    set_trans_blender(0,0,0,255);
    drawing_mode(DRAW_MODE_SOLID,out,0,0);
   }
 }

void fog::platformSFX()
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

//BITMAP *fog::getActFrameColP() { return frame_col[act_frame]; }

bool fog::inScreen()
 {
 	if(alpha>0) return true;
 	else return false;
 }

bool fog::isOn() { return ison; }

int fog::getPlatYPos() { return plat_y; }

int fog::getX() { return X; }

int fog::getY() { return Y; }

void fog::setStartX(int sx) { start_x=sx; }

void fog::setStartY(int sy) { start_y=sy; }

void fog::setCtrlXP(int *p) { ctrl_x=p; }

void fog::setCtrlYP(int *p) { ctrl_y=p; }

int *fog::getPosXP() { return &pos_x; }

int *fog::getPosYP() { return &pos_y; }

void fog::setPlatformMap(map *p) { plat_map=p; }

void fog::setHightMap(map *p) { hight_map=p; }

void fog::setBackgroundMap(map *p) { back_map=p; }

void fog::resetSFX(bool act)
 {
 	active=act;
 	act_x=pos_x=start_x; act_y=pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
 	dx=0; dy=0; plat_y=start_y;
 	ison=0; stop=1;
 	plat_wall=0;
 	alpha=0;
 	count=r=g=b=0;
 	act_frame=0; act_color=makecol32(r,g,b);
 }

void fog::debug()
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
