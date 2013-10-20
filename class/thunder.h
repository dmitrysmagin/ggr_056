class thunder
 {
  private:
 	 enum { _NONE=0, _FLASH=1, _FADE=2 };
 	 int action,start_x,start_y,pos_x,pos_y,X,Y,_i;
 	 int *ctrl_x,*ctrl_y;
 	 bool active;
 	 int alpha,act_frame,c0,c1,c2,start_deep;
 	 timer *ti[5];
 	 SAMPLE *sound[4];
 	 BITMAP *out,*aux_bmp;

 	 void setOutBitmap(BITMAP *);
 	
 	public:
 	 thunder(int,int,BITMAP *,int i=0,thunder *p=NULL);
 	 ~thunder();
 	 void drawSFX(int,int);
 	 void timeSFX();
 	 void animSFX();
 	 void drawThunder(int,int,int,int,int,int,int,int);
 	 void setPlatformMap(map *p);
 	 void setHightMap(map *p);
 	 void setBackgroundMap(map *p);
   void setScrollSpeed(int);
   void setDeep(int);
   void setWaterLen(int);
   void setWaveSpeed(int);
 	 void loadSample(char *,int);
 	 void assignSample(int,thunder *);
 	 bool inScreen();
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

thunder::thunder(int sx, int sy, BITMAP *out, int i, thunder *p)
 {
 	_i=i;
 	start_x=sx; start_y=sy;
 	c0=makecol(255,255,255); c1=makecol(00,0xd3,0xf7); c2=makecol(000,0x92,0xc6);
 	setOutBitmap(out);
 	resetSFX(true);
 	// Crea thunder
 	aux_bmp=create_bitmap(180,180); clear_to_color(aux_bmp,makecol(255,0,255));
 	ti[0]=new timer(300+random(50)); // Scroll thunder time
 	ti[1]=new timer(0);
 	ti[2]=new timer(0);
 	ti[3]=new timer(0);
 	ti[4]=new timer(0);
 }

thunder::~thunder()
 {
 	for(int i=0;i<5;i++) delete(ti[i]);
 	destroy_bitmap(aux_bmp);
 	if(_i==0)
 	 {
    for(int i=0;i<0;i++) destroy_sample(sound[i]);
 	 }
 }

void thunder::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void thunder::setOutBitmap(BITMAP *bmp) { out=bmp; }

void thunder::timeSFX()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void thunder::drawThunder(int sx,int sy,int deep,int ofs,int step,int stop,int p1, int p2)
 {
 	int x,y,oldx,w,h;

  if(deep==0) return;
 	w=aux_bmp->w; h=aux_bmp->h;
 	x=oldx=sx; //w>>1;
 	for(y=sy;y<h-stop;y+=step)
 	 {
 	 	oldx=x; x=sx+ofs+random(10);
 	 	if(deep>2) if(y<p1-stop) { line(aux_bmp,oldx+2,y,x+2,y+step,c2); line(aux_bmp,oldx-2,y,x-2,y+step,c2); }
 	 	if(deep>1) if(y<p2-stop) { line(aux_bmp,oldx+1,y,x+1,y+step,c1); line(aux_bmp,oldx-1,y,x-1,y+step,c1); }
 	  line(aux_bmp,oldx,y,x,y+step,c0);
 	  if(y==0) drawThunder(x,y+step,deep-1,30,10,20,p1,p2);
 	  if(y==20) drawThunder(x,y+step,deep-1,-20,10,50,p1,p2);
 	  if(y==60) drawThunder(x,y+step,deep-1,-20,10,70,p1,p2);
 	 }
 }

void thunder::animSFX()
 {
 	if(!active) return;
 	if(action==_NONE) if(ti[0]->isOn()) { clear_to_color(aux_bmp,makecol(255,0,255)); alpha=250; action=_FLASH; }
 	if(action==_FLASH)
 	 {
 	 	drawThunder(aux_bmp->w>>1,0,start_deep,0,20,0,aux_bmp->h>>1,(aux_bmp->h>>1)+(aux_bmp->h>>2));
    action=_FADE;
 	 }
 	if(action==_FADE)
 	 {
 	 	alpha-=25;
 	  if(alpha==0) { ti[0]->reset(); clear_to_color(aux_bmp,makecol(255,0,255)); action=_NONE; }
 	 }
 }

void thunder::drawSFX(int sx,int sy)
 {
  if(!active) return;
  X=-sx+pos_x; Y=-sy+pos_y;
  if(inScreen())
   {
    drawing_mode(DRAW_MODE_TRANS,out,0,0);
    set_trans_blender(0,0,0,alpha);
    draw_trans_sprite(out,aux_bmp,X,Y);
    set_trans_blender(0,0,0,255);
    drawing_mode(DRAW_MODE_SOLID,out,0,0);
   }
 }

bool thunder::inScreen()
 {
  if(alpha>0) return true;
  else return false;
 }

int thunder::getX() { return X; }

int thunder::getY() { return Y; }

void thunder::setStartX(int sx) { start_x=sx; }

void thunder::setStartY(int sy) { start_y=sy; }

void thunder::setCtrlXP(int *p) { ctrl_x=p; }

void thunder::setCtrlYP(int *p) { ctrl_y=p; }

int *thunder::getPosXP() { return &pos_x; }

int *thunder::getPosYP() { return &pos_y; }

void thunder::setScrollSpeed(int s) { ti[0]->setTick(s); }

void thunder::setDeep(int d) { start_deep=d; }

void thunder::resetSFX(bool act)
 {
  active=act;
 	action=_NONE;
 	start_deep=3;
 	pos_x=start_x>>1; pos_y=start_y>>1;
 	alpha=255;
 	act_frame=0;
 }

void thunder::debug()
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
