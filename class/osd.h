class osd
 {
 	private:
 	 enum { _COL=33, _ROW=3, _XPIX=32, _YPIX=30 };
 	 int X,Y,_i;
 	 int *lives,*score,*gauge,*min,*sec;
 	 int weapon_frame;
 	 int alpha;
 	 bool powerup,*magicpw;
 	 timer *ti[5];
 	 BITMAP *frame[_COL*_ROW];
 	 BITMAP *out;

 	 void loadSpriteFrames(char *);
 	 void assignSpriteFrame(osd *);
 	 void setOutBitmap(BITMAP *);
 	 void energyBar();
 	
 	public:
 	 osd(char *,int,int,BITMAP *,int i=0,osd *p=NULL);
 	 ~osd();
 	 void drawOSD(int,int);
 	 void timeOSD();
 	 void animOSD();
 	 bool inScreen();
 	 void setActWeapon(int,bool);
 	 void setMagicpwP(bool *);
 	 void setMagicGaugeP(int *);
 	 void setLivesP(int *);
 	 void setScoreP(int *);
 	 void setMinSecP(int *,int *);
 	 void debug();
 };

osd::osd(char *filename, int start_x, int start_y, BITMAP *out, int i, osd *p)
 {
  _i=i;
  alpha=0;
  powerup=0;
  if(!_i) loadSpriteFrames(filename); else assignSpriteFrame(p);
  setOutBitmap(out);
  weapon_frame=6;
  //pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  ti[0]=new timer(1);  // Anim time
  ti[1]=new timer(0);  
  ti[2]=new timer(0);  
  ti[3]=new timer(0); 
  ti[4]=new timer(0);
 }

osd::~osd()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(_i==0)
   {
    //for(int i=0;i<2;i++)         destroy_sample(sound[i]);
 	for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame[i]);
 	//for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame_col[i]);
   }
 }

void osd::loadSpriteFrames(char *filename)
 {
  BITMAP *aux;

  aux=load_bmp(filename,NULL);
  //for(int i=0;i<24;i++) frame[i]=create_sub_bitmap(aux,_XPIX*i+i,0,_XPIX,_YPIX);
  for(int i=0;i<_ROW;i++)
   for(int j=0;j<_COL;j++)
    {
     //frame[j+_COL*i]=create_sub_bitmap(aux,_XPIX*j+j,_YPIX*i+i,_XPIX,_YPIX);
     frame[j+_COL*i]=create_bitmap(_XPIX,_YPIX);
     blit(aux,frame[j+_COL*i],_XPIX*j+j,_YPIX*i+i,0,0,frame[j+_COL*i]->w,frame[j+_COL*i]->h);
    }
  destroy_bitmap(aux);
 }

void osd::assignSpriteFrame(osd *p)
 {
 	for(int i=0;i<_ROW*_COL;i++)
 	 {
 	  frame[i]=p->frame[i];
 	 }
 }

void osd::setOutBitmap(BITMAP *bmp) { out=bmp; }

void osd::timeOSD()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void osd::animOSD()
 {
 	/*if(dead) return;
 	if(explode) 
 	 {
 	 	if(ti[1]->isOn()) act_frame++;
 	 	if(act_frame==35)
 	 	 {
 	 	  act_frame=40;
 	 	  explode=0;
 	 	  dead=1;
 	 	 }
 	 	return;
 	 }
 	if(!fly) if(random(100)==1) fly=1;
 	if(fly)  { if(side) dx=-2; else dx=2; if(ti[3]->isOn()) { act_frame++; if(act_frame>2) act_frame=1; } }
 	if(hitted) { play_sample(sound[0],120,150,1000,0); hitted=0; act_frame=19; stop=1; energy--; if(energy==0) { explode=1; fly=0; stop=0; play_sample(sound[1],120,150,1000,0); } else explode=0; ti[2]->reset(); return; }
 	if(stop) { dx=0; act_frame=19; if(ti[2]->isOn()) stop=0; }
 	pos_x+=dx;
 	pos_y+=dy;*/
 	/*if(climb) // Sulla scala
   }*/
 }

void osd::drawOSD(int sx,int sy)
 {
  //pivot_sprite(rend,car,rend_x,rend_y,piv_cx,piv_cy,itofix(ang1));
  timeOSD();
  X=0; Y=0; alpha=180;
  drawing_mode(DRAW_MODE_TRANS,out,0,0);
  set_trans_blender(0,0,0,alpha);
 	 	
  draw_trans_sprite(out,frame[4],X+7,Y+28); draw_trans_sprite(out,frame[5],X+7+32,Y+28); // Time hud
  textprintf_ex(out,font,X+16,Y+24,makecol32(255,0,0),-1,"Time"); // "Time"
  // Time
  if(*sec<10) textprintf_ex(out,font,X+16,Y+35,makecol32(200,200,200),-1,"%d:0%d",*min,*sec);
  else        textprintf_ex(out,font,X+16,Y+35,makecol32(200,200,200),-1,"%d:%d",*min,*sec);
  draw_trans_sprite(out,frame[3],X+170,Y+194); // Weapon hud
  if(powerup) draw_trans_sprite(out,frame[weapon_frame+33],X+178,Y+201);  // Weapon (with powerup)
  else        draw_trans_sprite(out,frame[weapon_frame],X+178,Y+201);     // Weapon 
  textprintf_ex(out,font,X+8,Y,makecol32(255,0,0),-1,"Player 1"); // Player 1
  textprintf_ex(out,font,X+16,Y+12,makecol32(200,200,200),-1,"%d",*score); // Score
  for(int i=0;i<*lives-1;i++) draw_trans_sprite(out,frame[1],X+363-i*18,Y+210); // Lives
  draw_trans_sprite(out,frame[66],X+207,Y+190); // Shild
  draw_trans_sprite(out,frame[67],X+205,Y+210); // Power
  energyBar(); // Energybar
   
  set_trans_blender(0,0,0,255);
  drawing_mode(DRAW_MODE_SOLID,out,0,0);
 }

bool osd::inScreen()
 {
  if(X>=-46 && X<=384) return true;
  else return false;
 }

void osd::setActWeapon(int w,bool pow)
 {
  powerup=pow;
  if(w==LANCE) weapon_frame=6;
  if(w==KNIFE) weapon_frame=7;
  if(w==AXE)   weapon_frame=8;
  if(w==SWORD) weapon_frame=9;
  if(w==ARROW) weapon_frame=10;
  if(w==FLAME) weapon_frame=11;
 }

void osd::energyBar()
 {
  if(*magicpw)
   {
    for(int i=23;i<=27;i++) draw_trans_sprite(out,frame[i],X+15+(i-23)*32,Y+204);
   	if(*gauge>0)  draw_trans_sprite(out,frame[54],X+15,Y+204);
   	if(*gauge>10) draw_trans_sprite(out,frame[55],X+15+32,Y+204);
   	if(*gauge>20) draw_trans_sprite(out,frame[56],X+15+32,Y+204);
   	if(*gauge>30) draw_trans_sprite(out,frame[57],X+15+32,Y+204);
   	if(*gauge>40) { draw_trans_sprite(out,frame[58],X+15+32,Y+204); draw_trans_sprite(out,frame[59],X+15+64,Y+204); }
   	if(*gauge>49) draw_trans_sprite(out,frame[60],X+15+64,Y+204);
   	if(*gauge>57) { draw_trans_sprite(out,frame[61],X+15+64,Y+204); draw_trans_sprite(out,frame[62],X+15+96,Y+204); }
   	if(*gauge>64) draw_trans_sprite(out,frame[29],X+15+96,Y+204);
   	if(*gauge>69) { draw_trans_sprite(out,frame[63],X+15+96,Y+204); draw_trans_sprite(out,frame[31],X+15+128,Y+204); }
   	if(*gauge>73) { draw_trans_sprite(out,frame[64],X+15+128,Y+204); draw_trans_sprite(out,frame[65],X+15+160,Y+204); }
   	if(*gauge==76)
   	 {
   	  if(ti[0]->isOn()) draw_trans_sprite(out,frame[33+2],X+176,Y+201); else draw_trans_sprite(out,frame[2],X+176,Y+201);
   	  draw_trans_sprite(out,frame[weapon_frame+66],X+178,Y+201);
   	 }
   }
 }

void osd::setMagicpwP(bool *p) { magicpw=p; }

void osd::setMagicGaugeP(int *p) { gauge=p; }

void osd::setLivesP(int *p) { lives=p; }

void osd::setScoreP(int *p) { score=p; }

void osd::setMinSecP(int *m, int *s) { min=m; sec=s; }

void osd::debug() { }
