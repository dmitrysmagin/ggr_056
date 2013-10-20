class bullet
 {
  private:
   int pos_x,pos_y,scroll_x,scroll_y,jump_y,X,Y,hit_x,hit_y,_i;
   int plat_y,plat_stair,plat_wall;
   int dx,dy;
   int start_frame,end_frame,typ;
   int act_frame,hit_frame;
   int life,speed;
   int angle,dist; // Used for rotating bullets
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool can_hit;
   bool stop,move,jump_up,jump_down,down,climb,fire,fall,turn,rise,unrise;
   timer *ti[5];
   SAMPLE *sound[4];
   BITMAP *frame[75];
   BITMAP *frame_col[75];
   BITMAP *out;
   map *plat_map;
   map *hight_map;
   map *back_map;

   void setOutBitmap(BITMAP *);

  public:
   bullet(char *,BITMAP *,int i=0,int sta_fr=50,int end_fr=53,int ty=0);
   ~bullet();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,bullet *);
   BITMAP *getActFrameColP();
   void setAngle(int);
   void setFrameP(int,BITMAP *);
   void setFrameColP(int,BITMAP *);
   bool inScreen();
   int getPlatYPos();
   int getX();
   int getY();
   int *getPosXP();
   int *getPosYP();
   void shoot(int,int,int,int,bool);
   void hit();
   bool isFired();
   bool canHit();
   void resetObj();
   void debug();
 };

bullet::bullet(char *filename, BITMAP *out, int i, int sta_fr,int end_fr,int ty)
 {
  _i=i;
  start_frame=sta_fr;
  end_frame=end_fr;
  typ=ty;
  setOutBitmap(out);
  resetObj();
  ti[0]=new timer(3); // Anim time
  ti[1]=new timer(1); // Rotating time (aumento del raggio di rotazione)
  ti[2]=new timer(0);
  ti[3]=new timer(0);
  ti[4]=new timer(0);
 }

bullet::~bullet()
 {
 	for(int i=0;i<5;i++) delete(ti[i]);
 	//if(_i==0) for(int i=0;i<1;i++) destroy_sample(sound[i]);
 }

void bullet::loadSample(char *filename, int n)
 {
 	sound[n]=load_sample(filename);
 }

void bullet::assignSample(int n,bullet *p)
 {
 	sound[n]=p->sound[n];
 }

void bullet::setOutBitmap(BITMAP *bmp) { out=bmp; }

void bullet::timeSprite()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void bullet::animSprite()
 {
 	if(!fire) return;
 	//if(fire) { if(side) dx=-speed; else dx=speed; dy=1; life--; }
 	if(fire) life--;
 	//if(life<=0 || !inScreen()) fire=0;
 	if(!inScreen()) fire=0;
 	pos_x+=dx; pos_y+=dy;
 	if(typ==2) // Rotating bullets
 	 {
 	  angle+=12; if(angle==255) angle=0;
 	  if(ti[1]->isOn()) if(dist<50) dist++;
 	 }
 	if(typ==3) // Up and down bullets
 	 {
 	 	
 	  if(ti[1]->isOn()) dy++; //if(dist<50) dist++;
 	 }
 }

void bullet::drawSprite(int sx, int sy)
 {
  X=-sx+pos_x-23; Y=-sy+pos_y-12;
  if(typ==2) // Rotating bullets
   {
    X+=fixtoi(fixmul(itofix(dist),fixsin(itofix(angle))));
    Y+=fixtoi(fixmul(itofix(dist),fixcos(itofix(angle))));
   } 	
  if(fire)
   {
   	if(ti[0]->isOn()) act_frame++; if(act_frame>end_frame) act_frame=start_frame;
    if(side) draw_sprite_h_flip(out,frame[act_frame],X,Y);
    else     draw_sprite       (out,frame[act_frame],X,Y);
   }
 }

void bullet::platformSprite()
 {
 	plat_wall=0;
 	if(!jump_down && !jump_up && !climb && !rise && !unrise)
 	 {
 	  if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
 	  if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
 	 }
 	plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
 	plat_stair=0;//plat_map->getActPlatform(1);
 	if(!unrise) plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
 }

BITMAP *bullet::getActFrameColP() { return frame_col[act_frame]; }

void bullet::setAngle(int a) { angle=a; }

void bullet::setFrameP(int i, BITMAP *p)    { frame[i]=p; }

void bullet::setFrameColP(int i, BITMAP *p) { frame_col[i]=p; }

bool bullet::inScreen()
 {
  if(X>=-32 && X<=384 && Y>-40 && Y<224) return true;
  else return false;
 }

int bullet::getPlatYPos() { return plat_y; }

int bullet::getX() { return X; }

int bullet::getY() { return Y; }

int *bullet::getPosXP() { return &pos_x; }

int *bullet::getPosYP() { return &pos_y; }

void bullet::setPlatformMap(map *p) { plat_map=p; }

void bullet::setHightMap(map *p) { hight_map=p; }

void bullet::setBackgroundMap(map *p) { back_map=p; }

bool bullet::canHit() { if(life<160) return can_hit; else return false; } // Controllo life per aggirere il bug di riposizionamento arma

void bullet::resetObj()
 {
  pos_x=100; pos_y=100; scroll_x=0; scroll_y=0; jump_y=0; hit_x=0; hit_y=0;
  dx=0; dy=0; //plat_y=start_y;
  act_frame=start_frame; hit_frame=0;
  life=160; speed=3; pos_x=0;
  angle=dist=0; // Used for rotating bullets
  stop=1; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire=0; fall=0,turn=0,rise=0,unrise=0;
  side=0; direction=0; can_hit=1;
  plat_wall=0; 	
 }

void bullet::shoot(int x0,int y0,int x1,int y1,bool s)
 {
 	if(!fire)
 	 {
 	 	int ang;

 	 	act_frame=start_frame;
 	 	side=s;
	  fire=1;
	  can_hit=1;
	  pos_x=x0; pos_y=y0;
	  life=160;
  	ang=angle2P(x0,y0,x1,y1);
   	dx=fixtoi(fixmul(itofix(speed),(fixcos(itofix(ang)))));
   	dy=-fixtoi(fixmul(itofix(speed),(fixsin(itofix(ang)))));
   	if(typ==1) { // Unidirectional side bullets
   		          if(s) { if(dx>0) dx*=-1; if(dx>=-1) dx=-2; }
   		          else  { if(dx<0) dx*=-1; if(dx<=1)  dx=2;  }
   		         }
   	if(typ==3) { // Up and down bullets
   		          if(_i==0) dx=0; if(_i==1) dx=2; if(_i==2) dx=-2;
   		          dy=-9;
   		         }
	 }
 }

void bullet::hit()
 {
 	fire=0;
 	life=0;
 	can_hit=0;
 	pos_x=0; pos_y=0;
 	act_frame=0;
 }

bool bullet::isFired() { return fire; }

void bullet::debug()
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
