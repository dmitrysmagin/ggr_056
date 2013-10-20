class bonus
 {
 	private:
 	 enum { _COL=25, _ROW=3, _XPIX=46, _YPIX=46 };
 	 int *vasex,*vasey,vase_px,vase_py;
 	 int open,taked,takeble,type,start_x,start_y,afr,pos_x,pos_y,scroll_x,scroll_y,X,Y,_i;
 	 int plat_y,plat_stair,plat_wall;
 	 int dx,dy;
 	 int act_frame,min_frame,max_frame;
 	 bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
 	 bool active,stop,move,jump,fire,fall,attached;
 	 timer *ti[5];
 	 SAMPLE *sound[4];
 	 BITMAP *frame[_COL*_ROW];
 	 BITMAP *frame_col[_COL*_ROW];
 	 BITMAP *out;
 	 map *plat_map;
 	 map *hight_map;
 	 map *back_map;
 	 bonus *invase;
 	 
 	 void loadSpriteFrames(char *);
 	 void assignSpriteFrame(bonus *);
 	 void setOutBitmap(BITMAP *);
 	
 	public:
 	 bonus(char *,int,int,BITMAP *,int fr=0,int i=0,bonus *p=NULL);
 	 ~bonus();
 	 void drawSprite(int,int);
 	 void timeSprite();
 	 void platformSprite();
 	 void animSprite();
 	 void setStartX(int);
 	 void setStartY(int);
 	 void setVaseX(int *);
 	 void setVaseY(int *);
     void setVasepx(int);
     void setVasepy(int);
 	 void setTaked(bool);
 	 void setAttached(bool);
 	 void setInvaseP(bonus *);
 	 bool isTaked();
 	 bool isTakeble();
 	 bool isActive();
 	 void setPlatformMap(map *p);
 	 void setHightMap(map *p);
 	 void setBackgroundMap(map *p);
 	 void loadSample(char *,int);
 	 void assignSample(int,bonus *);
 	 BITMAP *getActFrameColP();
 	 bool inScreen();
 	 int getPlatYPos();
 	 int getX();
 	 int getY();
 	 int *getPosXP();
 	 int *getPosYP();
 	 int getExtra();
 	 int getType();
 	 void resetObj(bool,int typ=0);
 	 void debug();
 };

bonus::bonus(char *filename, int sx, int sy, BITMAP *out, int fr, int i, bonus *p)
 {
  _i=i;
  if(!_i) loadSpriteFrames(filename); else assignSpriteFrame(p);
  setOutBitmap(out);
  start_x=sx; start_y=sy; afr=fr;
  resetObj(true);
  takeble=false;
  ti[0]=new timer(2);   // Anim time
  ti[1]=new timer(150); // Bonus taked time
  ti[2]=new timer(0);
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

bonus::~bonus()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(_i==0)
   {
    for(int i=0;i<1;i++)         destroy_sample(sound[i]);
    for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame[i]);
    for(int i=0;i<_COL*_ROW;i++) destroy_bitmap(frame_col[i]);
   }
 }

void bonus::loadSpriteFrames(char *filename)
 {
  BITMAP *aux;

  aux=load_bmp(filename,NULL);
  //for(int i=0;i<24;i++) frame[i]=create_sub_bitmap(aux,46*i+i,0,46,46);
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

void bonus::assignSpriteFrame(bonus *p)
 {
  for(int i=0;i<_ROW*_COL;i++)
   {
    frame[i]=p->frame[i];
    frame_col[i]=p->frame_col[i];
   }
 }

void bonus::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void bonus::assignSample(int n,bonus *p)
 {
  sound[n]=p->sound[n];
 }

void bonus::setOutBitmap(BITMAP *bmp) { out=bmp; }

void bonus::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void bonus::animSprite()
 {
  if(!active) return;
  if(type==11)
   {
    if(attached) { pos_x=*vasex+vase_px; pos_y=*vasey+vase_py; dy=0; return; } // Vase (attached to an enemies)
    dy++; if(dy>7) dy=7;
    if(fall==0) open=1;
    if(open)
     if(ti[0]->isOn())
      {
       act_frame++;
       if(act_frame==min_frame+1) // Activate bonus inside the vase
        {
         invase->setStartX(*getPosXP());
   	     invase->setStartY(*getPosYP()-7);
   	     invase->resetObj(true,random(1)+3); // Can be weapon or bonus points      	
 	    }
 	   if(act_frame>max_frame) active=false;
 	  }
 	 pos_y+=dy;
 	 return;
   }
  if(type==4 && taked) { if(ti[1]->isOn()) active=false; }
  if(!stop) { jump=1; dy=-8; stop=1; }
  if(jump)
   {
    dy++; // Gravità
   	if(pos_y>start_y) { jump=0; fall=1; dy=1; }
   }
  pos_y+=dy;
 }

void bonus::drawSprite(int sx,int sy)
 {
  if(!active) return;
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(inScreen())
   {
  	if(type!=11) if(ti[0]->isOn()) { act_frame++; if(act_frame>max_frame) act_frame=min_frame; }
    if(side) draw_sprite_h_flip(out,frame[act_frame],X,Y);
    else     draw_sprite       (out,frame[act_frame],X,Y);
   }
 }

void bonus::platformSprite()
 {
  if(!active) return;
  plat_wall=0;
  if(!jump)
   {
    if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
    if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
    if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; takeble=true; }
   }
  plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
  plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
  plat_stair=0;//plat_map->getActStairEnemies(1);
 }
 
BITMAP *bonus::getActFrameColP() { return frame_col[act_frame]; }

bool bonus::inScreen()
 {
  if(X>=-32 && X<=384) return true;
  else return false;
 }

bool bonus::isActive() { return active; }

int bonus::getPlatYPos() { return plat_y; }

int bonus::getX() { return X; }

int bonus::getY() { return Y; }

int *bonus::getPosXP() { return &pos_x; }

int *bonus::getPosYP() { return &pos_y; }

int bonus::getType() { return type; }

int bonus::getExtra()
 {
 	if(type==3) return (min_frame-50)/2; // Weapon
 	if(type==4) return (min_frame-25)/4; // Bonus
 }

void bonus::setInvaseP(bonus *obj) { invase=obj; }

void bonus::setPlatformMap(map *p) { plat_map=p; }

void bonus::setHightMap(map *p) { hight_map=p; }

void bonus::setBackgroundMap(map *p) { back_map=p; }

void bonus::setAttached(bool b) { attached=b; }

void bonus::setStartX(int sx) { start_x=sx; }

void bonus::setStartY(int sy) { start_y=sy; }

void bonus::setVaseX(int *vx) { vasex=vx; }

void bonus::setVaseY(int *vy) { vasey=vy; }

void bonus::setVasepx(int px) { vase_px=px; }

void bonus::setVasepy(int py) { vase_py=py; }

void bonus::setTaked(bool b) { if(!taked) { taked=b; act_frame+=2; min_frame+=2; max_frame+=2; ti[1]->reset(); } }

bool bonus::isTaked() { return taked; }

bool bonus::isTakeble() { return takeble; }

void bonus::resetObj(bool act,int typ)
 {
  taked=0;
  open=0;
  active=act;
  type=typ;
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; vase_px=vase_py=0;
  dx=0; dy=0;  plat_y=start_y; 
  if(type==0)  { act_frame=0; max_frame=0; min_frame=0; takeble=true; } // Magician
  if(type==1)  { act_frame=1; max_frame=1; min_frame=1; takeble=true; } // Normal
  if(type==2)  { act_frame=2; max_frame=2; min_frame=2; takeble=true; } // Gold
  if(type==3)  { int w=random(5); act_frame=50+w*2; max_frame=51+w*2; min_frame=50+w*2; takeble=true;  } // Weapon
  if(type==4)  { int b=random(4); act_frame=25+b*4; max_frame=26+b*4; min_frame=25+b*4; takeble=false; } // Bonus
  if(type==5)  { act_frame=3; max_frame=3; min_frame=3; takeble=true; } // Scudo
  if(type==6)  { act_frame=4; max_frame=4; min_frame=4; takeble=true; } // Tagliola
  if(type==7)  { act_frame=6; max_frame=6; min_frame=6; takeble=true; } // Power up armi
  if(type==10) { act_frame=5; max_frame=5; min_frame=5; takeble=true; } // Key (next level)
  if(type==11) { act_frame=7; max_frame=11; min_frame=7; takeble=false; attached=0; } // Vase
  stop=0; move=0; jump=0; fire=0; fall=0;
  side=0; direction=0;
 }

void bonus::debug()
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
