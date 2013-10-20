class skeleton : public _sprite
 {
  private:
   bool have_vase;
   int pos_x,pos_y,scroll_x,scroll_y,jump_y;
   int plat_y,plat_stair,plat_wall;
   int dx,dy;
   int life,speed,slow;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool can_hit,hittable;
   bool stop,move,jump_up,jump_down,down,climb,fire1,fire2,fire_down,fire_up,fall,turn,rise,unrise,hitted,explode;
   timer *ti[5];
   SAMPLE *sound[4];
   bonus *vase;
   map *plat_map;
   map *hight_map;
   map *back_map;

  public:
   skeleton(char *,int,int,BITMAP *,int i=0,skeleton *p=NULL);
   ~skeleton();
   void drawSprite(int,int);
   void timeSprite();
   void platformSprite();
   void animSprite();
   void setPlatformMap(map *p);
   void setHightMap(map *p);
   void setBackgroundMap(map *p);
   void loadSample(char *,int);
   void assignSample(int,skeleton *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getPlatYPos();
   void attachVase(bonus *);
   void setHaveVase(bool);
   bool getHaveVase(); 	 
   int getScore();
   int *getPosXP();
   int *getPosYP();
   void setHitted(bool);
   bool canHit();
   bool isHittable();
   void resetObj(bool);
   void debug();
 };

skeleton::skeleton(char *filename, int sx, int sy, BITMAP *bmp, int i, skeleton *p) : _sprite(filename,bmp,sx,sy,i,p)
 {
  resetObj(true);
  ti[0]=new timer(3); // Move time
  ti[1]=new timer(8); // Rise/Unrise time
  ti[2]=new timer(1); // Explode time
  ti[3]=new timer(1); // Speed time
  ti[4]=new timer(0);
 }

skeleton::~skeleton()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(index==0)
   {
    for(int i=0;i<2;i++)    destroy_sample(sound[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames[i]);
    for(int i=0;i<columns*rows;i++) destroy_bitmap(frames_collision[i]);
   }
 }

void skeleton::loadSample(char *filename, int n)
 {
  sound[n]=load_sample(filename);
 }

void skeleton::assignSample(int n,skeleton *p)
 {
 	sound[n]=p->sound[n];
 }

void skeleton::timeSprite()
 {
 	for(int i=0;i<5;i++) ti[i]->time();
 }

void skeleton::animSprite()
 {
 	if(!enabled) return;
 	if(explode) 
 	 {
 	 	if(ti[2]->isOn()) act_frame++;
 	 	if(act_frame==17)
 	 	 {
 	 	  act_frame=0;
 	 	  explode=0;
 	 	  rise=0; unrise=0; move=0; dy=0;
 	 	  pos_x=random(2300);
 	 	  side=direction=random(1);
 	 	  slow=random(1);
 	 	  speed=random(1)+1;
 	 	  life=random(300)+300;
 	 	 }
 	 	return;
 	 }
 	if(hitted) { play_sample(sound[1],120,150,1000,0); dy=0; hitted=0; hittable=false; explode=1; move=0; rise=0; fall=0; unrise=0; act_frame=10; return; }
 	if(fall && !unrise) { act_frame=7; }
 	if(rise) 
 	 {
 	 	pos_y=plat_map->getActPlatformEnemies(pos_x,0);
 	 	if(ti[1]->isOn()) act_frame++;
 	 	if(act_frame==8) { rise=0; move=1; }
 	 }
 	if(unrise) 
 	 {
 	 	move=0; fall=0; rise=0;
 	 	if(ti[1]->isOn()) act_frame--;
 	 	if(act_frame==0)
 	 	 {
 	 	 	rise=1; unrise=0;
 	 	 	pos_x=random(2300);
 	 	 	side=direction=random(1);
 	 	 	slow=random(1);
 	 	 	speed=random(1)+1;
 	 	 	life=random(300)+300;
 	 	 }
 	 }
 	if(move)
 	 {
 	  //if(slow) { if(ti[3]->isOn()) 
 	  pos_x+=dx; //} else pos_x+=dx;
 	  if(ti[0]->isOn()) { act_frame++; if(act_frame>9) act_frame=8; }
 	 }
 	if(side) dx=-speed; else dx=speed;
  pos_y+=dy;
  life--;
 	if(act_frame==1 && rise && inScreen() && ti[1]->isOn()) play_sample(sound[0],120,150,1000,0);
 	if(!life || plat_wall) { unrise=1; move=0; rise=0; act_frame=7; }
 	if(!rise && !move && !unrise && !hitted && !explode) { rise=1; act_frame=0; }

 	/*if(turn) { if(plat_wall) dx=0; side=!side; turn=0; }
  if(stop) act_frame=0;
  if(climb)
   {
   	if(act_frame<18) act_frame=18;
   	if(key[KEY_UP] || key[KEY_DOWN]) if(ti[1]->isOn()) { act_frame++; if(act_frame>21) act_frame=18; }
   	jump_up=0; jump_down=0; fire1=0; fire2=0; fire_down=0; fire_up=0;
   	return;
   }
  if(move && !jump_up && !jump_down)
   {
   	if(!plat_wall) pos_x+=dx;
   	if(timer[0]) { act_frame++; if(act_frame>6) act_frame=1; }
   }
 	if(jump_down)
 	 {
 	 	if(!fire1 && !fire2 && !fire_up && !fire_down)
 	 	 if(move && !fire1 && !fire2 && !fire_up && !fire_down) act_frame=8; else act_frame=9;
 	 	if(move) if(!plat_wall) pos_x+=dx;
 	 	if(pos_y>=plat_y) { jump_up=0; jump_down=0; move=0; dy=0; pos_y=plat_y; play_sample(sound[1],120,150,1000,0); return; }
 	 	if(pos_y%5==0) dy++; // Gravità
 	 }
 	if(jump_up)
 	 {
 	 	if(!fire1 && !fire2 && !fire_up && !fire_down)
 	 	 if(move) act_frame=8; else act_frame=9;
 	 	if(move) if(!plat_wall) pos_x+=dx;
 	 	jump_y++; if(jump_y==21) { jump_up=0; jump_down=1; dy=2; }
 	 	if(jump_y%5==0) dy++; // Gravità
 	 }
  if(down) act_frame=7;
  //if(fall && !fire1 && !fire2 && !fire_up && !fire_down) { act_frame=6; }
  if(fire1)     { if(timer[1]) { act_frame++; if(act_frame>11) { act_frame=11; fire1=0;     } } }
  if(fire2)     { if(timer[1]) { act_frame++; if(act_frame>13) { act_frame=13; fire2=0;     } } }
  if(fire_up)   { if(timer[1]) { act_frame++; if(act_frame>15) { act_frame=15; fire_up=0;   } } }
  if(fire_down) { if(timer[1]) { act_frame++; if(act_frame>17) { act_frame=17; fire_down=0; } } }
  //if(plat_wall) { pos_x-=dx; }
  if(pos_x<22) pos_x=22; if(pos_x>366) pos_x=366;*/
 }

void skeleton::drawSprite(int sx,int sy)
 {
 	if(!enabled) return;
 	X=-sx+pos_x-23; Y=-sy+pos_y-12;
 	if(act_frame>6 && act_frame<10) can_hit=true; else can_hit=false;
  if(act_frame>3 && act_frame<10) hittable=true; else hittable=false;
 	if(inScreen())
 	 {
    if(side) draw_sprite_h_flip(screen,frames[act_frame],X,Y);
    else     draw_sprite       (screen,frames[act_frame],X,Y);
    // Extra animation
    if(side) draw_sprite_h_flip(screen,frames[act_frame+columns],X+15,Y+8);
    else     draw_sprite       (screen,frames[act_frame+columns],X-15,Y+8);
   }
 }

void skeleton::platformSprite()
 {
  if(!enabled) return;
  if(hitted || explode) return;
  plat_wall=0;
  if(!jump_down && !jump_up && !climb && !rise && !unrise && !hitted && !explode)
   {
    if(pos_y>plat_y) if(pos_y-plat_y<=8) pos_y=plat_y; 
    if(pos_y<plat_y) if(plat_y-pos_y<=8) pos_y=plat_y; 
    if(pos_y<plat_y) { fall=1; dy++; dx=0; } else { fall=0; dy=0; }
   }
  plat_y=plat_map->getActPlatformEnemies(pos_x,pos_y);
  if(!unrise) plat_wall=plat_map->getActPlatformEnemies(pos_x,pos_y,2,direction);
  plat_stair=0;//plat_map->getActStairEnemies(1);
 }

BITMAP *skeleton::getActFrameColP() { return frames_collision[act_frame]; }

bool skeleton::inScreen()
 {
  if(X>=-32 && X<=384) return true;
  else return false;
 }

int skeleton::getPlatYPos() { return plat_y; }

int *skeleton::getPosXP() { return &pos_x; }

int *skeleton::getPosYP() { return &pos_y; }

int skeleton::getScore() { if(have_vase) { vase->setAttached(false); have_vase=0; } return 100; }

void skeleton::setPlatformMap(map *p) { plat_map=p; }

void skeleton::setHightMap(map *p) { hight_map=p; }

void skeleton::setBackgroundMap(map *p) { back_map=p; }

void skeleton::setHitted(bool b) { hitted=b; }

bool skeleton::canHit() { return can_hit; }

bool skeleton::isHittable() { return hittable; }

void skeleton::setHaveVase(bool b) { have_vase=b; }

bool skeleton::getHaveVase() { return have_vase; }

void skeleton::attachVase(bonus *p)
 {
  if(random(200)==0 && isEnabled() && !getHaveVase() && !inScreen() && !rise)
   {
    vase=p;
    vase->resetObj(true,11);
    vase->setVaseX(getPosXP()); vase->setVaseY(getPosYP());
    vase->setVasepx(0); vase->setVasepy(2);
    vase->setAttached(true);
    setHaveVase(true);
   }
 }

void skeleton::resetObj(bool enb)
 {
  enabled=enb;
  have_vase=false; // No vase at start
  pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
  dx=0; dy=0; plat_y=start_y;
  act_frame=0;
  life=random(300)+300; slow=random(1); speed=random(1)+1; pos_x=random(2300);
  stop=1; move=0; jump_up=0; jump_down=0; down=0; climb=0; fire1=0; fire2=0; fire_down=0; fire_up=0; fall=0; turn=0; rise=0; unrise=0; hitted=0; explode=0;
  side=0; direction=0;
  plat_wall=0;
 }

void skeleton::debug()
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
