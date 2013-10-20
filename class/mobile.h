class mobile
 {
  private:
   int bonus_seq,start_x,start_y,pos_x,pos_y,scroll_x,scroll_y,jump_y,X,Y,_i;
   int *glob_bonus_seq;
   int plat_y,plat_stair,plat_wall;
   int dis_x,dis_y; // Distanza di apparizione (solo per magic box)
   int dx,dy,max_y,min_y;
   int *ctrl_x,*ctrl_y;
   int bak_frame,act_frame,start_frame,end_frame;
   int energy,life,speed,type;
   int *difficulty;
   bool side,direction; // posizione speculare e direzione di moto 0 = destra, 1 = sinistra
   bool can_hit,hittable,fground;
   bool dead,active,hide,move,down,fire1,fire2,fall,rise,unrise,hitted,explode;
   timer *ti[5];
   BITMAP *frame[100];
   BITMAP *frame_col[100];
   BITMAP *out;
   int mobiledata[100][6+48]; // 0 dis. muro da sinistra
                              // 1 dist. muro da destra
                              // 2 altezza muro sin.
                              // 3 altezza muro des.
                              // 4 pixel per hit orizzontale 
                              // 5 pixel per hit verticale
                              // 6 altezza piattaforma (o altre 47 altezze per altezza variabile (TODO) ???)
   void loadSpriteFrames(char *);
   void loadMobile(char *dat_mobile_filename);
   void assignSpriteFrame(mobile *);
   void setOutBitmap(BITMAP *);

  public:
   mobile(char *,char *,int,int,int,int,int,BITMAP *,int i=0,mobile *p=NULL);
   ~mobile();
   void setStartX(int);
   void setStartY(int);
   void setMaxY(int);
   void setMinY(int);
   void setDisX(int);
   void setDisY(int); 
   void setBonusSeqP(int *);
   void setDifficultyP(int *);
   void setFGround(bool);  
   bool isActive();
   void drawSprite(int,int);
   void timeSprite();
   void animSprite();
   void loadSample(char *,int);
   void assignSample(int,mobile *);
   BITMAP *getActFrameColP();
   bool inScreen();
   int getBonusSeq(int,bool,bool);
   int getPlatYPos();
   int getMobileH();
   int getWallLeft();
   int getWallHLeft();
   int getWallRight();
   int getWallHRight();
   int getX();
   int getY();
   int getEnergy();
   int getHHit();
   int getVHit();
   bool getFGround();
   int *getPosXP();
   int *getPosYP();
   void setCtrlXP(int *);
   void setCtrlYP(int *);
   void setStartFrame(int);
   void setEndFrame(int);
   void setType(int);
   void setHitted(bool);
   bool canHit();
   bool isHittable();
   void resetObj(bool);
   void debug();
 };

mobile::mobile(char *filename, char *dat_filename, int sx, int sy, int fra, int end_f, int typ, BITMAP *out, int i, mobile *p)
 {
 	_i=i;
 	if(!_i) loadSpriteFrames(filename); else assignSpriteFrame(p);
 	loadMobile(dat_filename);
 	setOutBitmap(out);
 	type=typ;
 	start_x=sx; start_y=sy; max_y=min_y=0;
 	if(type==2)  { dis_x=fra; dis_y=end_f; bak_frame=44; end_frame=49; } // Magic box
 	else         { bak_frame=fra; end_frame=end_f; }
 	resetObj(true);
 	ti[0]=new timer(4); // Anim time
 	ti[1]=new timer(1); // Explode time
 	ti[2]=new timer(0);
 	ti[3]=new timer(0);
 	ti[4]=new timer(0);
 }

mobile::~mobile()
 {
  for(int i=0;i<5;i++) delete(ti[i]);
  if(_i==0)
   {
    for(int i=0;i<22*3;i++) destroy_bitmap(frame[i]);
    for(int i=0;i<22*3;i++) destroy_bitmap(frame_col[i]);
   } 	
 }

void mobile::loadMobile(char *dat_mobile_filename)
 {
 	int v;
 	FILE *fd;

  // Legge il file .dat con le definizioni delle singole piattaforme mobili
  int n;
  fd=fopen(dat_mobile_filename,"r");
  fscanf(fd,"%d",&n); // Numero di piattaforme mobili definite (righe nel file)
 	//data=(int **)malloc(sizeof(int *)*tilemax_x);
 	//for(int x=0;x<tilemax_x;x++) data[x]=(int *)malloc(sizeof(int)*tilemax_y);
 	for(int x=0;x<n;x++)
 	 for(int y=0;y<7;y++) // 7 oppure 6+48=54 se voglio usare le piattaforme variabili in altezza (pendii e salite)
 	  {
 	   fscanf(fd,"%d",&v);
 	   mobiledata[x][y]=v;
 	  }
 	fclose(fd);
 }

void mobile::loadSpriteFrames(char *filename)
 {
  BITMAP *aux;

  aux=load_bmp(filename,NULL);
  //for(int i=0;i<24;i++) frame[i]=create_sub_bitmap(aux,48*i+i,0,48,54);
  for(int i=0;i<3;i++)
   for(int j=0;j<22;j++)
    {
     //frame[j+22*i]=create_sub_bitmap(aux,48*j+j,54*i+i,48,54);
     frame[j+22*i]=create_bitmap(48,54);
     blit(aux,frame[j+22*i],48*j+j,54*i+i,0,0,frame[j+22*i]->w,frame[j+22*i]->h);
     frame_col[j+22*i]=create_bitmap_ex(8,48,54);
     clear_to_color(frame_col[j+22*i],0);
     for(int h=0;h<frame[j+22*i]->h;h++)
      for(int w=0;w<frame[j+22*i]->w*4;w+=4)
       if(makecol32(frame[j+22*i]->line[h][w+2],
                    frame[j+22*i]->line[h][w+1],
                    frame[j+22*i]->line[h][w])!=16711935)
        frame_col[j+22*i]->line[h][w>>2]=1;
    }
  destroy_bitmap(aux);
 }

void mobile::assignSpriteFrame(mobile *p)
 {
  for(int i=0;i<3*22;i++)
   {
    frame[i]=p->frame[i];
    frame_col[i]=p->frame_col[i];
   }
 }

void mobile::setOutBitmap(BITMAP *bmp) { out=bmp; }

void mobile::timeSprite()
 {
  for(int i=0;i<5;i++) ti[i]->time();
 }

void mobile::animSprite()
 {
  if(!active) return;
  if(type==0) { return; } // Static
  if(type==1) { if(ti[0]->isOn()) { act_frame++; if(act_frame>end_frame) act_frame=start_frame; } return; } // Static (animated)
  if(type==2) // Magic box
   {
   	if(dead) return;
    if(act_frame>55) { /*allegro_message("BUG DA SISTEMARE MEGLIO!");*/ dead=0; explode=1; rise=0; hide=0; act_frame=end_frame; hitted=true; energy=1; } // Fix magic box bug
	if(explode) 
  	 {
      if(ti[1]->isOn()) act_frame++;
 	  if(act_frame==55)
 	   {
 	    act_frame=start_frame;
 	 	dead=1;
 	 	explode=0;
  	   }
  	  return;
 	 }
 	if(hide)
 	 {
      if(dis_x<=0) if(*ctrl_x<pos_x+dis_x && *ctrl_y<pos_y+dis_y && inScreen()) { bonus_seq=*glob_bonus_seq; (*glob_bonus_seq)++; hide=0; rise=1; }
 	  if(dis_x>0)  if(*ctrl_x>pos_x+dis_x && *ctrl_y<pos_y+dis_y && inScreen()) { bonus_seq=*glob_bonus_seq; (*glob_bonus_seq)++; hide=0; rise=1; }
      if(*glob_bonus_seq>=8) *glob_bonus_seq=0;
     }
    if(rise) if(ti[0]->isOn()) { act_frame++; if(act_frame==end_frame) rise=0; }
    if(hitted) { hitted=0; energy--; if(energy==1) act_frame=50; if(energy==0) { explode=1; } else explode=0; ti[1]->reset(); return; }
    return;
   }
  if(type==3) { if(ti[0]->isOn()) { act_frame++; if(act_frame>end_frame) act_frame=start_frame; } return; } // Moving (up/down) (animated)
  if(type==4) { return; } // Moving (up/down)
 }

void mobile::drawSprite(int sx,int sy)
 {
  if(!active) return;
  X=-sx+pos_x-24; Y=-sy+pos_y-20;
  //if(act_frame>6 && act_frame<14) can_hit=true; else can_hit=false;
  if(type==2) if(act_frame>=48 && act_frame<=50) hittable=true; else hittable=false;
  else        if(act_frame>0) hittable=true; else hittable=false;
  if(type==3 || type==4) { pos_y+=dy; if(pos_y<min_y) dy=1; if(pos_y>max_y) dy=-1; } // Moving (up/down)
  if(inScreen())
   {
    draw_sprite(out,frame[act_frame],X,Y);
   }
 }

BITMAP *mobile::getActFrameColP() { return frame_col[act_frame]; }

bool mobile::inScreen()
 {
  //if(X>=-48 && X<=384) return true;
  //else return false;
  if(X>=-48 && X<=384)
   if(Y>=-48 && Y<=224) return true;
  return false;
 }

int mobile::getPlatYPos() { return plat_y; }

int mobile::getMobileH()  { return mobiledata[act_frame][6]; }

int mobile::getWallLeft()  { return mobiledata[act_frame][0]; }

int mobile::getWallHLeft()  { return mobiledata[act_frame][2]; }

int mobile::getWallRight() { return mobiledata[act_frame][1]; }

int mobile::getWallHRight()  { return mobiledata[act_frame][3]; }

void mobile::setCtrlXP(int *p) { ctrl_x=p; }

void mobile::setCtrlYP(int *p) { ctrl_y=p; }

int mobile::getX() { return X; }

int mobile::getY() { return Y; }

int mobile::getEnergy() { return energy; }

int *mobile::getPosXP() { return &pos_x; }

int *mobile::getPosYP() { return &pos_y; }

int mobile::getHHit() { return mobiledata[act_frame][4]; }

int mobile::getVHit() { return mobiledata[act_frame][5]; }

bool mobile::getFGround() { return fground; }

int mobile::getBonusSeq(int dress, bool shild, bool powerup)
 {
  if(bonus_seq==0) return 0; // Mago
  if(bonus_seq==1)
   { 
   	if(dress==0) return 2; // Gold se normal
   	if(*difficulty==1) if(dress==1) return 1; // Normal se nudo e difficulty = 1 (Easy)
   	else               if(dress==1) return 0; // Mago se nudo e difficulty != 1  (Normal or Hard)   	
   	if(dress==2) return 3; // Arma se gold
   }
  if(bonus_seq==2) return 3; // Weapon
  if(bonus_seq==3)
   {
   	if(dress==0) return 2; // Gold se normal
   	if(dress==1) return 1; // Normal se nudo
   	if(dress==2) return 3; // Arma se gold 	 	
   }
  if(bonus_seq==4) return 4; // Bonus
  if(bonus_seq==5)
   {
   	if(dress==0) return 2; // Gold se normal
   	if(dress==1) return 6; // Tagliola se nudo
//  if(dress==2 && shild && powerup) return 4; // Bonus se scudo, gold e power up
//  if(dress==2 && shild) return 7;            // Power up armi se ha gia scudo e gold
 	if(dress==2) return 5; // Scudo se gold
   }
  if(bonus_seq==6) return 4; // Bonus
  if(bonus_seq==7)
   {
   	if(dress==0) return 2; // Gold se normal
   	if(dress==1) return 6; // Tagliola se nudo
// 	if(dress==2 && shild && powerup) return 4; // Bonus se scudo, gold e power up
//  if(dress==2 && shild) return 7;            // Power up armi se ha gia scudo e gold
    if(dress==2) return 7; // Powerup se gold
   }
  return 0; // altri casi da fare
 }

void mobile::setHitted(bool b) { hitted=b; }

bool mobile::canHit() { return can_hit; }

bool mobile::isHittable() { return hittable; }

void mobile::setStartX(int sx) { start_x=sx; }

void mobile::setStartY(int sy) { start_y=sy; }

void mobile::setMaxY(int my) { max_y=my; }

void mobile::setMinY(int my) { min_y=my; }

void mobile::setDisX(int ddx) { dis_x=ddx; }

void mobile::setDisY(int ddy) { dis_y=ddy; }

void mobile::setFGround(bool b) { fground=b; }

void mobile::setBonusSeqP(int *p) { glob_bonus_seq=p; }

void mobile::setDifficultyP(int *p) { difficulty=p; }

void mobile::setStartFrame(int sf)  { bak_frame=sf; }

void mobile::setEndFrame(int ef)    { end_frame=ef; }

void mobile::setType(int ty)    { type=ty; }

bool mobile::isActive() { return active; }

void mobile::resetObj(bool act)
 {
 	active=act;
 	bonus_seq=0;
 	act_frame=start_frame=bak_frame;
 	pos_x=start_x; pos_y=start_y; scroll_x=0; scroll_y=0; jump_y=0;
 	dx=0; dy=1; plat_y=start_y;
 	if(type==2) energy=3; else energy=0;
 	life=random(300)+300; speed=1;
 	hide=1; dead=0; down=0; fire1=0; fire2=0; fall=0; rise=0; unrise=0; hitted=0; explode=0;
 	side=0; direction=0;
 	plat_wall=0;
 }

void mobile::debug()
 {
  if(act_frame==55) draw_sprite(out,frame[act_frame],X,Y);
  if(active)
   {
    line(out,X,Y+54-1-mobiledata[act_frame][6],X+48,Y+54-1-mobiledata[act_frame][6],makecol(255,255,0)); 
    line(out,X+mobiledata[act_frame][0],Y+54,X+mobiledata[act_frame][0],Y+54-2-mobiledata[act_frame][2],makecol(255,0,0));
    line(out,X+48-mobiledata[act_frame][1],Y+54,X+48-mobiledata[act_frame][1],Y+54-2-mobiledata[act_frame][3],makecol(255,0,0));
   }
 }
