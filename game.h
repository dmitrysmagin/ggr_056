#define VER "0.56"
#define CFGFILE "config.cfg"
#define FADETIME 18

#define N_STAGE 16
#define N_PLAT 32
#define N_SK 8
#define N_ZO 10
#define N_VO 1
#define N_BL 7
#define N_RF 20
#define N_RA 3
#define N_RK 1
#define N_P2 7
#define N_ST 5
#define N_GO 12
#define N_BA 8
#define N_GI 6
#define N_FW 10
#define N_IN 5
#define N_FG 6
#define N_CL 8
#define N_TU 4
#define N_DE 3
#define N_LD 6
#define N_BC 2
#define N_PO 4
#define N_MA 12
#define N_FL 3
#define N_IMMOBILE 20
#define N_PARFX 240
#define N_WASFX 2
#define N_THSFX 4
#define N_BBOX 12
#define N_BVASE 8

bool L_B=1,L_0=1,L_1=1,L_S=1,L_H=1,L_OSD=1,DBUG=0,VSYNC=1;
int MS=0,OMS=0,FPS=0,fps=0;
int SCREENRES,SCANLINES,SCREENX,SCREENY,SCREEND,G_RESX,G_RESY,MFPS;
int MUSIC_VOLUME,SOUND_VOLUME;
int KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYJUMP,KEYSHOOT; // Move player keyboard config
int RESTART_X,RESTART_Y;
bool FSCREEN,DEBUG,STRETCH;

int STAGE,LIVES=3,DIFFICULTY=2,READY_GO,SCORE,GAME_OVER,PROLOGUE,INTERLUDE,EPILOGUE,BONUS_SEQ,MIN,SEC,TIME_MS,CKPOINT;
int MENU_LIVES = LIVES;
int MAX_STAGE;
int READY_GO_TIME=80;
bool TIMEOVER,RETIME;

BITMAP *VSCR;
MIDI *music[N_STAGE];

player           *PL;
osd              *OSD;
skeleton         *SK[N_SK];
zombie           *ZO[N_ZO];
blade            *BL[N_BL];
rockfall         *RF[N_RF];
volture          *VO[N_VO];
red_arremer      *RA[N_RA];
red_arremer_king *RK[N_RK];
plant_mk2        *P2[N_P2];
stalagmite       *ST[N_ST];
ghost            *GO[N_GO];
bat              *BA[N_BA];
giant            *GI[N_GI];
firewall         *FW[N_FW];
insect           *IN[N_IN];
fire_grizzly     *FG[N_FG];
clam             *CL[N_CL];
turtle           *TU[N_TU];
demon            *DE[N_DE];
little_devil     *LD[N_LD];
big_crab         *BC[N_BC];
pork             *PO[N_PO];
magician         *MA[N_MA];
flower           *FL[N_FL];
immobile         *IMMOBILE[N_IMMOBILE];
map              *MAP0[N_STAGE],*MAP1[N_STAGE],*MAPH[N_STAGE];
mobile           **MOBILE;
particle         *PA_SFX[N_PARFX];
flash            *FL_SFX;
fog              *FO_SFX;
fire             *FI_SFX;
water            *WA_SFX[N_WASFX];
rain             *RA_SFX[2];
snow             *SN_SFX[2];
thunder          *TH_SFX[N_THSFX];
bonus            *BONUSB[N_BBOX];
bonus            *BVASE[N_BVASE];
bonus            *KEY,*SHILD;

void init();
void initEnv();
void initObjects();
void initGame(int,int,int);
void restartStage(int);
void resetObj();
void checkGame();
void readConf(int *,int *,bool *,bool *,bool *,int *,int *,int *,int *,int *,int *,int *,int *);
void writeConf(int,int,bool,bool,bool,int,int,int,int,int,int,int,int);
void readyGo();
void checkTimer();
void gameOver();
void Intro();
void Title();
void stageMap();
void layer_back();
void layer_0();
void layer_1();
void layer_s();
void layer_h();
void layer_OSD(int);
void Player();
void Enemies();
void Scroll();
void Collision();
void collisionPlayerBonus();
void collisionShildBullets();
void collisionPlayerEnemies();
void collisionWeaponEnemies();
void collisionMagicEnemies();
void collisionPlayerBullets();
void rePaint(int);
void debug(BITMAP *,int);

void init()
 {
  initEnv();
  allegro_init();
  install_keyboard();
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL);
  set_volume(SOUND_VOLUME,MUSIC_VOLUME); 
  VSCR=create_bitmap_ex(SCREEND,G_RESX,G_RESY);
  set_color_depth(SCREEND);

  // Set the screen
  // Fullscreen patch for MacOS (Thanks to Jérôme Rabas)
  #ifdef ALLEGRO_MACOSX
   // Source code for MacOS
   if(FSCREEN)
    { 
     GFX_MODE_LIST *gfx_mode_list=get_gfx_mode_list(GFX_QUARTZ_FULLSCREEN);  
     if (!gfx_mode_list ) { exit(0); }
     int compteur=0;
     GFX_MODE gfx_mode;
     do
      {
       gfx_mode=gfx_mode_list->mode[compteur++];
      }
     while(!(SCREENX<=gfx_mode.width && SCREENY<=gfx_mode.height) && gfx_mode.width!=0);
     if(set_gfx_mode(GFX_QUARTZ_FULLSCREEN,gfx_mode.width,gfx_mode.height,0,0)==-1) { writeConf(1,0,true,true,false,KEY_LEFT,KEY_RIGHT,KEY_UP,KEY_DOWN,KEY_ALT,KEY_LCONTROL,128,200); exit(0); }
    }
   else { if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,SCREENX,SCREENY,0,0)==-1) { writeConf(1,0,true,true,false,KEY_LEFT,KEY_RIGHT,KEY_UP,KEY_DOWN,KEY_ALT,KEY_LCONTROL,128,200); exit(0); } }
  #else
   // Original source code
   if(FSCREEN) { if(set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,SCREENX,SCREENY,0,0) == -1) { writeConf(1,0,true,true,false,KEY_LEFT,KEY_RIGHT,KEY_UP,KEY_DOWN,KEY_ALT,KEY_LCONTROL,128,200); exit(0); } }
   else        { if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,SCREENX,SCREENY,0,0)   == -1) { writeConf(1,0,true,true,false,KEY_LEFT,KEY_RIGHT,KEY_UP,KEY_DOWN,KEY_ALT,KEY_LCONTROL,128,200); exit(0); } }
  #endif

  LOCK_FUNCTION(count);
  LOCK_VARIABLE(counter);  
  install_int_ex(count,MSEC_TO_TIMER(1));
  STAGE=MAX_STAGE=0;
  initObjects();
 }

void initEnv()
 {
  MFPS = 60;
  G_RESX = 384; G_RESY = 224;
  SCREENRES = 1;
  SCANLINES = 0;
  FSCREEN   = true;
  STRETCH   = true;
  DEBUG     = false;
  KEYLEFT  = KEY_LEFT;
  KEYRIGHT = KEY_RIGHT;
  KEYUP    = KEY_UP;
  KEYDOWN  = KEY_DOWN;
  KEYJUMP  = KEY_ALT;
  KEYSHOOT = KEY_LCONTROL;
  MUSIC_VOLUME = 128;
  SOUND_VOLUME = 200;

  readConf(&SCREENRES,&SCANLINES,&FSCREEN,&STRETCH,&DEBUG,&KEYLEFT,&KEYRIGHT,&KEYUP,&KEYDOWN,&KEYJUMP,&KEYSHOOT,&MUSIC_VOLUME,&SOUND_VOLUME);
  if(SCREENRES==0) { SCREENX=320; SCREENY=200; SCREEND=32; }
  if(SCREENRES==1) { SCREENX=320; SCREENY=240; SCREEND=32; }
  if(SCREENRES==2) { SCREENX=384; SCREENY=224; SCREEND=32; }
  if(SCREENRES==3) { SCREENX=400; SCREENY=300; SCREEND=32; }
  if(SCREENRES==4) { SCREENX=512; SCREENY=384; SCREEND=32; }
  if(SCREENRES==5) { SCREENX=640; SCREENY=480; SCREEND=32; }
  if(SCREENRES==6) { SCREENX=768; SCREENY=448; SCREEND=32; }
 }

void initGame(int st,int li, int di)
 {
  // New game start
  STAGE = st; LIVES = li; DIFFICULTY = di;
  READY_GO = READY_GO_TIME; SCORE = 0; GAME_OVER = 0;
  BONUS_SEQ = 0; CKPOINT = 0; INTERLUDE = 0; EPILOGUE = 0;
  if(STAGE==0) PROLOGUE = 1; else PROLOGUE = 0;
  // Resettare posizione giocatore ed armi per inizio partita
  resetObj();
  stageMap();
 }

void resetObj()
 {
  // Reset to false all objects
  int tot_mobile = 0;

  for(int i=0;i<N_BA;i++) BA[i]->resetObj(false); // Bat
  for(int i=0;i<N_BC;i++) BC[i]->resetObj(false); // Big crab
  for(int i=0;i<N_BL;i++) BL[i]->resetObj(false); // Blade
  for(int i=0;i<N_CL;i++) CL[i]->resetObj(false); // Clam
  for(int i=0;i<N_DE;i++) DE[i]->resetObj(false); // Demon
  for(int i=0;i<N_FG;i++) FG[i]->resetObj(false); // Fire grizzly
  for(int i=0;i<N_FW;i++) FW[i]->resetObj(false); // Firewall
  for(int i=0;i<N_FL;i++) FL[i]->resetObj(false); // Flower
  for(int i=0;i<N_GI;i++) GI[i]->resetObj(false); // Giant
  for(int i=0;i<N_GO;i++) GO[i]->resetObj(false); // Ghost
  for(int i=0;i<N_IN;i++) IN[i]->resetObj(false); // Insect
  for(int i=0;i<N_LD;i++) LD[i]->resetObj(false); // Little devil
  for(int i=0;i<N_P2;i++) P2[i]->resetObj(false); // Plant mk2
  for(int i=0;i<N_PO;i++) PO[i]->resetObj(false); // Pork
  for(int i=0;i<N_RA;i++) RA[i]->resetObj(false); // Red arremer
  for(int i=0;i<N_RF;i++) RF[i]->resetObj(false); // Rockfall
  for(int i=0;i<N_SK;i++) SK[i]->resetObj(false); // Skeleton
  for(int i=0;i<N_ST;i++) ST[i]->resetObj(false); // Stalagmite
  for(int i=0;i<N_TU;i++) TU[i]->resetObj(false); // Turtle
  for(int i=0;i<N_VO;i++) VO[i]->resetObj(false); // Volture
  for(int i=0;i<N_ZO;i++) ZO[i]->resetObj(false); // Zombie

  // Immobile enemies
  for(int i=0;i<N_IMMOBILE;i++) IMMOBILE[i]->resetObj(false);
  // Vase (with bonus)
  for(int i=0;i<N_BVASE;i++) BVASE[i]->resetObj(false);
  for(int i=0;i<N_BBOX;i++)  BONUSB[i]->resetObj(false);
  // Mobile (da 0 a 11 Magic box)
  for(int i=0;i<N_PLAT;i++)  MOBILE[i]->resetObj(false);
  // Other
  for(int i=0;i<N_PARFX;i++) PA_SFX[i]->resetObj(false);   // Particle
  for(int i=0;i<N_WASFX;i++) WA_SFX[i]->resetSFX(false);   // Water
  for(int i=0;i<2;      i++) RA_SFX[i]->resetSFX(false,i); // Rain
  for(int i=0;i<2;      i++) SN_SFX[i]->resetSFX(false,i); // Snow
  for(int i=0;i<N_THSFX;i++) TH_SFX[i]->resetSFX(false);   // Thunder
  FI_SFX->resetSFX(false);                                 // Fire
  FO_SFX->resetSFX(false);                                 // Fog
  FL_SFX->resetSFX(false);                                 // Flash

  BONUS_SEQ = 0;
  TIMEOVER  = false;
  PL->setPlatformMap(MAP1[STAGE]);
  PL->setHightMap(MAPH[STAGE]);
  PL->setBackgroundMap(MAP0[STAGE]);
  PL->calcMax();
  PL->setShildP(SHILD);
  KEY->resetObj(false); SHILD->resetObj(false);
  for(int i=0;i<N_MA;i++) MA[i]->resetObj(true); // Magician (always on for all stages)

  // Set platform map to the objects that need it
  for(int i=0;i<N_ZO;i++) ZO[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_SK;i++) SK[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_GI;i++) GI[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_FG;i++) FG[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_TU;i++) TU[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_DE;i++) DE[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_LD;i++) LD[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_BC;i++) BC[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_PO;i++) PO[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_FL;i++) FL[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_BBOX;i++)  BONUSB[i]->setPlatformMap(MAP1[STAGE]);
  for(int i=0;i<N_BVASE;i++) BVASE[i]->setPlatformMap(MAP1[STAGE]);
  KEY->setPlatformMap(MAP1[STAGE]); SHILD->setPlatformMap(MAP1[STAGE]);

  if(STAGE==0 || STAGE==14)
   {
    // Player position
    if(STAGE==0)  {
                   if(!CKPOINT) { RESTART_X=130;  RESTART_Y=607; MIN=2; SEC=30; RETIME=true;  }
                   else         { RESTART_X=4100; RESTART_Y=597; MIN=2; SEC=30; RETIME=false; }
			      }
    if(STAGE==14) {
                   if(!CKPOINT) { RESTART_X=6070; RESTART_Y=601; MIN=2; SEC=30; RETIME=true;  }
  	               else         { RESTART_X=3700; RESTART_Y=605; MIN=2; SEC=30; RETIME=false; }
			      }
    PL->resetPlayer(RESTART_X,RESTART_Y); if(STAGE==14) PL->setSide(1);
 	// Blade
 	BL[0]->setStartX(1895); BL[0]->setStartY(108+448); BL[0]->setMaxY(158+448); BL[0]->setCount(5);
 	BL[1]->setStartX(2087); BL[1]->setStartY(121+448); BL[1]->setMaxY(160+448); BL[1]->setCount(2);
    BL[2]->setStartX(2247); BL[2]->setStartY(121+448); BL[2]->setMaxY(160+448); BL[2]->setCount(7);
    BL[3]->setStartX(1863); BL[3]->setStartY(9+448);   BL[3]->setMaxY(60+448);  BL[3]->setCount(4);
    BL[4]->setStartX(2071); BL[4]->setStartY(-4+448);  BL[4]->setMaxY(48+448);  BL[4]->setCount(8);
    BL[5]->setStartX(2503); BL[5]->setStartY(121+448); BL[5]->setMaxY(160+448); BL[5]->setCount(8);
    BL[6]->setStartX(2663); BL[6]->setStartY(46+448);  BL[6]->setMaxY(126+448); BL[6]->setCount(3);
    for(int i=0;i<N_BL;i++) BL[i]->resetObj(true);
 	// Zombie
 	for(int i=0;i<N_ZO;i++) { ZO[i]->setStartX(194); ZO[i]->setStartY(0); ZO[i]->resetObj(true); }
 	// Skeleton
 	for(int i=0;i<N_SK;i++) { SK[i]->setStartX(194); SK[i]->setStartY(152); SK[i]->resetObj(true); }
 	// Plant mk2
 	P2[0]->setStartX(1364); P2[0]->setStartY(554);
 	P2[1]->setStartX(1698); P2[1]->setStartY(487);
 	P2[2]->setStartX(2202); P2[2]->setStartY(608);
 	P2[3]->setStartX(2940); P2[3]->setStartY(609);
 	P2[4]->setStartX(3852); P2[4]->setStartY(609);
 	P2[5]->setStartX(4498); P2[5]->setStartY(448);
    P2[6]->setStartX(5374); P2[6]->setStartY(189);
 	for(int i=0;i<N_P2;i++) P2[i]->resetObj(true);
 	// Volture
 	VO[0]->setStartX(940);  VO[0]->setStartY(550);
 	for(int i=0;i<N_VO;i++) VO[i]->resetObj(true);
 	// Red arremer
 	RA[0]->setStartX(4208); RA[0]->setStartY(580);
 	RA[1]->setStartX(5208); RA[1]->setStartY(420);
 	RA[2]->setStartX(912);  RA[2]->setStartY(513);
 	for(int i=0;i<N_RA;i++) RA[i]->resetObj(true);
 	// Red arremer King
 	if(STAGE==0)  { RK[0]->setStartX(6042); RK[0]->setStartY(606); }
 	if(STAGE==14) { RK[0]->setStartX(174);  RK[0]->setStartY(614); }
 	RK[0]->setExitKeyP(KEY);	 	
 	for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
 	// Ghost
 	for(int i=0;i<N_GO;i++) { GO[i]->setStartX(2550); GO[i]->setStartY(1300); GO[i]->resetObj(true); }
 	// Pork
 	for(int i=0;i<2;i++) PO[i]->resetObj(true);
 	PO[0]->setMinX(4962); PO[0]->setMaxX(5406); *PO[0]->getPosXP()=5130; *PO[0]->getPosYP()=224;
 	PO[1]->setMinX(4590); PO[1]->setMaxX(5330); *PO[1]->getPosXP()=5000; *PO[1]->getPosYP()=350;
 	// Flower
 	FL[0]->setStartX(288);  FL[0]->setStartY(607); FL[0]->setMinX(120);  FL[0]->setMaxX(600);
 	FL[1]->setStartX(4596); FL[1]->setStartY(558); FL[1]->setMinX(4400); FL[1]->setMaxX(5244);
 	FL[2]->setStartX(4730); FL[2]->setStartY(525); FL[2]->setMinX(4400); FL[2]->setMaxX(5244);
 	for(int i=0;i<N_FL;i++) FL[i]->resetObj(true);

    // Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(100);  MOBILE[0]->setStartY(607); MOBILE[0]->setDisX(-70); MOBILE[0]->setDisY(-36);  MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(778);  MOBILE[1]->setStartY(576); MOBILE[1]->setDisX(80);  MOBILE[1]->setDisY(50);   MOBILE[1]->setFGround(true);
    MOBILE[2]->setStartX(1496); MOBILE[2]->setStartY(587); MOBILE[2]->setDisX(-1);  MOBILE[2]->setDisY(-35);  MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(2218); MOBILE[3]->setStartY(608); MOBILE[3]->setDisX(-1);  MOBILE[3]->setDisY(-134); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(3100); MOBILE[4]->setStartY(607); MOBILE[4]->setDisX(10);  MOBILE[4]->setDisY(-72);  MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(3800); MOBILE[5]->setStartY(608); MOBILE[5]->setDisX(96);  MOBILE[5]->setDisY(50);   MOBILE[5]->setFGround(true);
    MOBILE[6]->setStartX(4366); MOBILE[6]->setStartY(607); MOBILE[6]->setDisX(80);  MOBILE[6]->setDisY(-22);  MOBILE[6]->setFGround(true);
    MOBILE[7]->setStartX(5228); MOBILE[7]->setStartY(505); MOBILE[7]->setDisX(-1);  MOBILE[7]->setDisY(-22);  MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(4560); MOBILE[8]->setStartY(450); MOBILE[8]->setDisX(-50); MOBILE[8]->setDisY(-36);  MOBILE[8]->setFGround(true);
    MOBILE[9]->setStartX(5042); MOBILE[9]->setStartY(230); MOBILE[9]->setDisX(-50); MOBILE[9]->setDisY(-36);  MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(0);   MOBILE[10]->setStartY(0);  MOBILE[10]->setDisX(0);  MOBILE[10]->setDisY(0);   MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(0);   MOBILE[11]->setStartY(0);  MOBILE[11]->setDisX(0);  MOBILE[11]->setDisY(0);   MOBILE[11]->setFGround(true);

    MOBILE[12]->setStartX(360);  MOBILE[12]->setStartY(608); MOBILE[12]->setStartFrame(01); MOBILE[12]->setEndFrame(01); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
    MOBILE[13]->setStartX(580);  MOBILE[13]->setStartY(613); MOBILE[13]->setStartFrame(01); MOBILE[13]->setEndFrame(01); MOBILE[13]->setType(0); MOBILE[13]->setFGround(true);
    MOBILE[14]->setStartX(1160); MOBILE[14]->setStartY(606); MOBILE[14]->setStartFrame(02); MOBILE[14]->setEndFrame(02); MOBILE[14]->setType(0); MOBILE[14]->setFGround(true);
    MOBILE[15]->setStartX(1634); MOBILE[15]->setStartY(607); MOBILE[15]->setStartFrame(01); MOBILE[15]->setEndFrame(01); MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
    MOBILE[16]->setStartX(2976); MOBILE[16]->setStartY(607); MOBILE[16]->setStartFrame(03); MOBILE[16]->setEndFrame(04); MOBILE[16]->setType(1); MOBILE[16]->setFGround(true);
    MOBILE[17]->setStartX(3260); MOBILE[17]->setStartY(624); MOBILE[17]->setStartFrame(03); MOBILE[17]->setEndFrame(04); MOBILE[17]->setType(1); MOBILE[17]->setFGround(true);
    MOBILE[18]->setStartX(3350); MOBILE[18]->setStartY(626); MOBILE[18]->setStartFrame(03); MOBILE[18]->setEndFrame(04); MOBILE[18]->setType(1); MOBILE[18]->setFGround(true);
    MOBILE[19]->setStartX(3538); MOBILE[19]->setStartY(624); MOBILE[19]->setStartFrame(05); MOBILE[19]->setEndFrame(05); MOBILE[19]->setType(0); MOBILE[19]->setFGround(true);
    MOBILE[20]->setStartX(3586); MOBILE[20]->setStartY(624); MOBILE[20]->setStartFrame(05); MOBILE[20]->setEndFrame(05); MOBILE[20]->setType(0); MOBILE[20]->setFGround(true);
    MOBILE[21]->setStartX(3634); MOBILE[21]->setStartY(624); MOBILE[21]->setStartFrame(05); MOBILE[21]->setEndFrame(05); MOBILE[21]->setType(0); MOBILE[21]->setFGround(true);
    MOBILE[22]->setStartX(4024); MOBILE[22]->setStartY(598); MOBILE[22]->setStartFrame(23); MOBILE[22]->setEndFrame(23); MOBILE[22]->setType(0); MOBILE[22]->setFGround(true);
    MOBILE[23]->setStartX(4788); MOBILE[23]->setStartY(378); MOBILE[23]->setStartFrame(23); MOBILE[23]->setEndFrame(23); MOBILE[23]->setType(0); MOBILE[23]->setFGround(true);
    tot_mobile = 23;
    if(STAGE==14)
     {
	  MOBILE[24]->setStartX(5628); MOBILE[24]->setStartY(606); MOBILE[24]->setStartFrame(24); MOBILE[24]->setEndFrame(24); MOBILE[24]->setType(0); MOBILE[24]->setFGround(true);
	  MOBILE[25]->setStartX(5580); MOBILE[25]->setStartY(400); MOBILE[25]->setMinY(350); MOBILE[25]->setMaxY(510); MOBILE[25]->setStartFrame(43); MOBILE[25]->setEndFrame(43); MOBILE[25]->setType(4); MOBILE[25]->setFGround(false);
	  MOBILE[26]->setStartX(5508); MOBILE[26]->setStartY(300); MOBILE[26]->setMinY(250); MOBILE[26]->setMaxY(354); MOBILE[26]->setStartFrame(43); MOBILE[26]->setEndFrame(43); MOBILE[26]->setType(4); MOBILE[26]->setFGround(false);
	  tot_mobile = 26;
     }

    for(int i=0; i<=11;i++)         MOBILE[i]->resetObj(true);
    for(int i=12;i<=tot_mobile;i++) MOBILE[i]->resetObj(true);

 	// Other
 	FL_SFX->setStartX(4802+768); FL_SFX->setStartY(520); FL_SFX->resetSFX(true);
 	FO_SFX->setStartX(2300);     FO_SFX->setStartY(448); FO_SFX->resetSFX(true);
 	WA_SFX[0]->setStartX(2864); WA_SFX[0]->setStartY(655); WA_SFX[0]->resetSFX(true); WA_SFX[0]->setScrollSpeed(0); WA_SFX[0]->setWaveSpeed(-1); WA_SFX[0]->setWaterLen(18); WA_SFX[0]->setAlpha(200);
 	RA_SFX[0]->setStartX(4001); RA_SFX[0]->setStartY(516); RA_SFX[0]->resetSFX(true,0);
    RA_SFX[1]->setStartX(4200); RA_SFX[1]->setStartY(516); RA_SFX[1]->resetSFX(true,1);
 	TH_SFX[0]->setStartX(100);  TH_SFX[0]->setStartY(448); TH_SFX[0]->resetSFX(true); TH_SFX[0]->setDeep(2); // 3 = lampo ravvicinato
 	TH_SFX[1]->setStartX(800);  TH_SFX[1]->setStartY(448); TH_SFX[1]->resetSFX(true); TH_SFX[1]->setDeep(2);
 	TH_SFX[2]->setStartX(1500); TH_SFX[2]->setStartY(448); TH_SFX[2]->resetSFX(true); TH_SFX[2]->setDeep(2);
 	TH_SFX[3]->setStartX(2200); TH_SFX[3]->setStartY(448); TH_SFX[3]->resetSFX(true); TH_SFX[3]->setDeep(2);
 	
 	// Set difficulty (use a cascade switch)
 	switch(DIFFICULTY)
 	 {
 	  case 1: {
 	           if(STAGE==0) {
	                         SK[1]->resetObj(false); VO[0]->resetObj(false);
	                         ZO[1]->resetObj(false); GO[1]->resetObj(false);
	                         GO[2]->resetObj(false); P2[5]->resetObj(false);
	                        }
              }
	  case 2: {
	           if(STAGE==0) {
	                         FL[0]->resetObj(false); RA[2]->resetObj(false);
	                         SK[0]->resetObj(false); ZO[0]->resetObj(false);
	                         GO[0]->resetObj(false);
	                        }
	          }
	 }
   }
  if(STAGE==1 || STAGE==13)
   {
    // Player position
    if(STAGE==1)  {
                   if(!CKPOINT) { RESTART_X=130;  RESTART_Y=607; MIN=3; SEC=00; RETIME=true;  }
                   else         { RESTART_X=3600; RESTART_Y=348; MIN=3; SEC=00; RETIME=false; }
			      }
    if(STAGE==13) {
                   if(!CKPOINT) { RESTART_X=6862; RESTART_Y=421; MIN=3; SEC=00; RETIME=true;  }
  	               else         { RESTART_X=3900; RESTART_Y=348; MIN=3; SEC=00; RETIME=false; }
			      }
    PL->resetPlayer(RESTART_X,RESTART_Y); if(STAGE==13) PL->setSide(1);
  	// Plant mk2
  	P2[0]->setStartX(570);  P2[0]->setStartY(605);
  	P2[1]->setStartX(1266); P2[1]->setStartY(516);
  	P2[2]->setStartX(1788); P2[2]->setStartY(446);
  	P2[3]->setStartX(2244); P2[3]->setStartY(407);
  	P2[4]->setStartX(3052); P2[4]->setStartY(325);
  	P2[5]->setStartX(3520); P2[5]->setStartY(348);
    P2[6]->setStartX(5444); P2[6]->setStartY(455);
  	for(int i=0;i<N_P2;i++) P2[i]->resetObj(true);
  	// Red arremer King
  	if(STAGE==1)  { RK[0]->setStartX(6830); RK[0]->setStartY(390); }
  	if(STAGE==13) { RK[0]->setStartX(28);   RK[0]->setStartY(614); }
  	RK[0]->setExitKeyP(KEY);
  	for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
  	// Firewall
  	FW[0]->setStartX(4300); FW[0]->setStartY(6+224*2);   FW[0]->setMaxY(100); FW[0]->setCount(5);  FW[0]->setMaxCount(5);
  	FW[1]->setStartX(4450); FW[1]->setStartY(6+224*2);   FW[1]->setMaxY(100); FW[1]->setCount(16); FW[1]->setMaxCount(18);
  	FW[2]->setStartX(4700); FW[2]->setStartY(22+224*2);  FW[2]->setMaxY(100); FW[2]->setCount(1);  FW[2]->setMaxCount(2);
  	FW[3]->setStartX(4772); FW[3]->setStartY(22+224*2);  FW[3]->setMaxY(100); FW[3]->setCount(6);  FW[2]->setMaxCount(12);
  	FW[4]->setStartX(5725); FW[4]->setStartY(190+224*2); FW[4]->setMaxY(100); FW[4]->setCount(10); FW[4]->setMaxCount(15);
  	FW[5]->setStartX(6000); FW[5]->setStartY(190+224*2); FW[5]->setMaxY(100); FW[5]->setCount(12); FW[5]->setMaxCount(18);
  	FW[6]->setStartX(5967); FW[6]->setStartY(190+224*2); FW[6]->setMaxY(100); FW[6]->setCount(5);  FW[6]->setMaxCount(20);
  	FW[7]->setStartX(6200); FW[7]->setStartY(190+224*2); FW[7]->setMaxY(100); FW[7]->setCount(2);  FW[7]->setMaxCount(10);
  	FW[8]->setStartX(6200); FW[8]->setStartY(210+224*2); FW[8]->setMaxY(100); FW[8]->setCount(13); FW[8]->setMaxCount(20);
  	FW[9]->setStartX(6258); FW[9]->setStartY(190+224*2); FW[9]->setMaxY(100); FW[9]->setCount(8);  FW[9]->setMaxCount(16);
  	for(int i=0;i<N_FW;i++) FW[i]->resetObj(true);
  	// Insect
  	for(int i=0;i<N_IN;i++) { IN[i]->setStartX(1900); IN[i]->setStartY(1200); IN[i]->setType(0); IN[i]->resetObj(true); }
  	// Fire grizzly
  	FG[0]->setStartX(4400); FG[0]->setStartY(607); FG[0]->setMinX(4280); FG[0]->setMaxX(4500);
  	FG[1]->setStartX(5000); FG[1]->setStartY(607); FG[1]->setMinX(4850); FG[1]->setMaxX(5300);
  	FG[2]->setStartX(5120); FG[2]->setStartY(607); FG[2]->setMinX(4850); FG[2]->setMaxX(5310);
  	FG[3]->setStartX(6300); FG[3]->setStartY(607); FG[3]->setMinX(6030); FG[3]->setMaxX(6270);
  	for(int i=0;i<4;i++) FG[i]->resetObj(true);
  	// Turtle
  	for(int i=0;i<N_TU;i++) { TU[i]->setStartX(194); TU[i]->setStartY(152); TU[i]->resetObj(true); }
  	// Little devil
  	for(int i=0;i<N_LD;i++) { LD[i]->setStartX(194); LD[i]->setStartY(152); LD[i]->resetObj(true); }
  	// Big crab
  	BC[0]->setStartX(2225); BC[0]->setStartY(506);
  	BC[1]->setStartX(2785); BC[1]->setStartY(450);
  	for(int i=0;i<N_BC;i++) BC[i]->resetObj(true);

 	// Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(150);  MOBILE[0]->setStartY(607); MOBILE[0]->setDisX(0);   MOBILE[0]->setDisY(-50); MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(1330); MOBILE[1]->setStartY(527); MOBILE[1]->setDisX(100); MOBILE[1]->setDisY(0);   MOBILE[1]->setFGround(true);
    MOBILE[2]->setStartX(1650); MOBILE[2]->setStartY(471); MOBILE[2]->setDisX(80);  MOBILE[2]->setDisY(-50); MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(1950); MOBILE[3]->setStartY(368); MOBILE[3]->setDisX(60);  MOBILE[3]->setDisY(-40); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(776);  MOBILE[4]->setStartY(590); MOBILE[4]->setDisX(80);  MOBILE[4]->setDisY(0);   MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(3168); MOBILE[5]->setStartY(342); MOBILE[5]->setDisX(0);   MOBILE[5]->setDisY(-62); MOBILE[5]->setFGround(true);
    MOBILE[6]->setStartX(4450); MOBILE[6]->setStartY(366); MOBILE[6]->setDisX(50);  MOBILE[6]->setDisY(-65); MOBILE[6]->setFGround(true);
    MOBILE[7]->setStartX(5160); MOBILE[7]->setStartY(400); MOBILE[7]->setDisX(0);   MOBILE[7]->setDisY(-50); MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(6324); MOBILE[8]->setStartY(519); MOBILE[8]->setDisX(0);  MOBILE[8]->setDisY(-40);  MOBILE[8]->setFGround(true);

    MOBILE[9]->setStartX(0);    MOBILE[9]->setStartY(0);   MOBILE[9]->setDisX(0);  MOBILE[9]->setDisY(0);    MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(0);   MOBILE[10]->setStartY(0);  MOBILE[10]->setDisX(0); MOBILE[10]->setDisY(0);   MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(0);   MOBILE[11]->setStartY(0);  MOBILE[11]->setDisX(0); MOBILE[11]->setDisY(0);   MOBILE[11]->setFGround(true);

 	MOBILE[12]->setStartX(74);   MOBILE[12]->setStartY(625); MOBILE[12]->setStartFrame(24); MOBILE[12]->setEndFrame(24); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
    MOBILE[13]->setStartX(610);  MOBILE[13]->setStartY(608); MOBILE[13]->setStartFrame(22); MOBILE[13]->setEndFrame(22); MOBILE[13]->setType(0); MOBILE[13]->setFGround(true);
    MOBILE[14]->setStartX(1160); MOBILE[14]->setStartY(506); MOBILE[14]->setStartFrame(22); MOBILE[14]->setEndFrame(22); MOBILE[14]->setType(0); MOBILE[14]->setFGround(true);
    tot_mobile = 14;
    if(STAGE==13)
     {
      MOBILE[15]->setStartX(5526); MOBILE[15]->setStartY(520); MOBILE[15]->setStartFrame(03); MOBILE[15]->setEndFrame(04); MOBILE[15]->setType(1); MOBILE[15]->setFGround(true);
      MOBILE[16]->setStartX(5400); MOBILE[16]->setStartY(455); MOBILE[16]->setStartFrame(03); MOBILE[16]->setEndFrame(04); MOBILE[16]->setType(1); MOBILE[16]->setFGround(true);
      MOBILE[17]->setStartX(4714); MOBILE[17]->setStartY(405); MOBILE[17]->setStartFrame(03); MOBILE[17]->setEndFrame(04); MOBILE[17]->setType(1); MOBILE[17]->setFGround(false);
      tot_mobile = 17;
	 }

    for(int i=0;i<=8;i++)           MOBILE[i]->resetObj(true);
    for(int i=12;i<=tot_mobile;i++) MOBILE[i]->resetObj(true);

 	// Other
 	for(int i=0;i<80;i++)        { PA_SFX[i]->setStartX(875);  PA_SFX[i]->setStartY(539); PA_SFX[i]->resetObj(true); } // Particle
 	for(int i=80;i<160;i++)      { PA_SFX[i]->setStartX(800);  PA_SFX[i]->setStartY(485); PA_SFX[i]->resetObj(true); }
 	for(int i=160;i<N_PARFX;i++) { PA_SFX[i]->setStartX(3370); PA_SFX[i]->setStartY(315); PA_SFX[i]->resetObj(true); }
 	FI_SFX->setStartX(3996); FI_SFX->setStartY(415); FI_SFX->resetSFX(true); FI_SFX->setPower(130); FI_SFX->setFade(true);

 	// Set difficulty (use a cascade switch)
 	switch(DIFFICULTY)
 	 {
 	  case 1: {
 	           if(STAGE==1) {
	                         TU[1]->resetObj(false); FW[3]->resetObj(false);
	                         FW[5]->resetObj(false); LD[2]->resetObj(false);
	                         IN[1]->resetObj(false);
	                        }
              }
	  case 2: {
	           if(STAGE==1) {
	                         TU[0]->resetObj(false); FW[1]->resetObj(false);
	                         FW[8]->resetObj(false); LD[0]->resetObj(false);
	                         LD[1]->resetObj(false); IN[0]->resetObj(false);
	                        }
	          }
	 }
   }
  if(STAGE==2 || STAGE==12)
   {
    // Player position
    if(STAGE==2)  {
                   if(!CKPOINT) { RESTART_X=130;  RESTART_Y=591; MIN=3; SEC=30; RETIME=true;  }
                   else         { RESTART_X=3250; RESTART_Y=579; MIN=2; SEC=30; RETIME=false; }
			      }
    if(STAGE==12) {
                   if(!CKPOINT) { RESTART_X=5266; RESTART_Y=565; MIN=2; SEC=30; RETIME=true;  }
  	               else         { RESTART_X=3100; RESTART_Y=576; MIN=3; SEC=30; RETIME=false; }
			      }
    PL->resetPlayer(RESTART_X,RESTART_Y); if(STAGE==12) PL->setSide(1);
 	// Plant mk2
 	P2[0]->setStartX(850);  P2[0]->setStartY(591);
 	P2[1]->setStartX(1320); P2[1]->setStartY(578);
 	P2[2]->setStartX(2826); P2[2]->setStartY(449);
 	P2[3]->setStartX(3694); P2[3]->setStartY(548);
 	P2[4]->setStartX(4490); P2[4]->setStartY(342);
 	P2[5]->setStartX(4228); P2[5]->setStartY(316);
 	P2[6]->setStartX(4760); P2[6]->setStartY(342);
 	for(int i=0;i<N_P2;i++) P2[i]->resetObj(true);
 	// Volture
 	VO[0]->setStartX(2240); VO[0]->setStartY(568);
 	for(int i=0;i<N_VO;i++) VO[i]->resetObj(true); 	 	
 	// Red arremer
 	RA[0]->setStartX(500);  RA[0]->setStartY(591); RA[0]->resetObj(true);
 	RA[1]->setStartX(2600); RA[1]->setStartY(449); RA[1]->resetObj(true);
 	// Red arremer King
 	if(STAGE==2)  { RK[0]->setStartX(5220); RK[0]->setStartY(520); }
 	if(STAGE==12) { RK[0]->setStartX(90);   RK[0]->setStartY(600); }
 	RK[0]->setExitKeyP(KEY);
 	for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
 	// Bat
 	for(int i=0;i<N_BA;i++) { BA[i]->setStartX(0); BA[i]->setStartY(6000); BA[i]->resetObj(true); }
 	// Giant
 	GI[0]->setStartX(1358); GI[0]->setStartY(576); GI[0]->setMinX(850);  GI[0]->setMaxX(1500);
 	GI[1]->setStartX(2100); GI[1]->setStartY(449); GI[1]->setMinX(1620); GI[1]->setMaxX(2050);
 	GI[2]->setStartX(3200); GI[2]->setStartY(449); GI[2]->setMinX(1950); GI[2]->setMaxX(2700);
 	GI[3]->setStartX(2600); GI[3]->setStartY(576); GI[3]->setMinX(1600); GI[3]->setMaxX(2600);
 	GI[4]->setStartX(3200); GI[4]->setStartY(322); GI[4]->setMinX(1900); GI[4]->setMaxX(2750);
 	GI[5]->setStartX(2800); GI[5]->setStartY(322); GI[5]->setMinX(1700); GI[5]->setMaxX(2750);
 	for(int i=0;i<N_GI;i++) GI[i]->resetObj(true);
 	// Clam
 	CL[0]->setStartX(3898); CL[0]->setStartY(578); CL[0]->setSide(0);
 	CL[1]->setStartX(4152); CL[1]->setStartY(500); CL[1]->setSide(1);
 	CL[2]->setStartX(4160); CL[2]->setStartY(548); CL[2]->setSide(1);
 	CL[3]->setStartX(4172); CL[3]->setStartY(387); CL[3]->setSide(0);
 	CL[4]->setStartX(4120); CL[4]->setStartY(226); CL[4]->setSide(1);
 	CL[5]->setStartX(4684); CL[5]->setStartY(300); CL[5]->setSide(1);
    CL[6]->setStartX(4712); CL[6]->setStartY(500); CL[6]->setSide(1);
    CL[7]->setStartX(4862); CL[7]->setStartY(566); CL[7]->setSide(0);
 	for(int i=0;i<N_CL;i++) CL[i]->resetObj(true);

 	// Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(740);  MOBILE[0]->setStartY(591); MOBILE[0]->setDisX(0);   MOBILE[0]->setDisY(-42); MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(1100); MOBILE[1]->setStartY(591); MOBILE[1]->setDisX(80);  MOBILE[1]->setDisY(-48); MOBILE[1]->setFGround(true);
    MOBILE[2]->setStartX(1728); MOBILE[2]->setStartY(195); MOBILE[2]->setDisX(-99); MOBILE[2]->setDisY(-36); MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(2990); MOBILE[3]->setStartY(195); MOBILE[3]->setDisX(-99); MOBILE[3]->setDisY(-36); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(3248); MOBILE[4]->setStartY(579); MOBILE[4]->setDisX(80);  MOBILE[4]->setDisY(-36); MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(4000); MOBILE[5]->setStartY(334); MOBILE[5]->setDisX(-60); MOBILE[5]->setDisY(-36); MOBILE[5]->setFGround(true);
    MOBILE[6]->setStartX(4576); MOBILE[6]->setStartY(342); MOBILE[6]->setDisX(150); MOBILE[6]->setDisY(-36); MOBILE[6]->setFGround(true);

    MOBILE[7]->setStartX(0);    MOBILE[7]->setStartY(0);   MOBILE[7]->setDisX(0);   MOBILE[7]->setDisY(0);  MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(0);    MOBILE[8]->setStartY(0);   MOBILE[8]->setDisX(0);   MOBILE[8]->setDisY(0);  MOBILE[8]->setFGround(true);
    MOBILE[9]->setStartX(0);    MOBILE[9]->setStartY(0);   MOBILE[9]->setDisX(0);   MOBILE[9]->setDisY(0);  MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(0);   MOBILE[10]->setStartY(0);  MOBILE[10]->setDisX(0);  MOBILE[10]->setDisY(0); MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(0);   MOBILE[11]->setStartY(0);  MOBILE[11]->setDisX(0);  MOBILE[11]->setDisY(0); MOBILE[11]->setFGround(true);

 	MOBILE[12]->setStartX(2000); MOBILE[12]->setStartY(575); MOBILE[12]->setStartFrame(25); MOBILE[12]->setEndFrame(25); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
    MOBILE[13]->setStartX(2000); MOBILE[13]->setStartY(527); MOBILE[13]->setStartFrame(25); MOBILE[13]->setEndFrame(25); MOBILE[13]->setType(0); MOBILE[13]->setFGround(true);
    MOBILE[14]->setStartX(2000); MOBILE[14]->setStartY(479); MOBILE[14]->setStartFrame(26); MOBILE[14]->setEndFrame(26); MOBILE[14]->setType(0); MOBILE[14]->setFGround(true);
 	MOBILE[15]->setStartX(2672); MOBILE[15]->setStartY(575); MOBILE[15]->setStartFrame(25); MOBILE[15]->setEndFrame(25); MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
    MOBILE[16]->setStartX(2672); MOBILE[16]->setStartY(527); MOBILE[16]->setStartFrame(25); MOBILE[16]->setEndFrame(25); MOBILE[16]->setType(0); MOBILE[16]->setFGround(true);
    MOBILE[17]->setStartX(2672); MOBILE[17]->setStartY(479); MOBILE[17]->setStartFrame(26); MOBILE[17]->setEndFrame(26); MOBILE[17]->setType(0); MOBILE[17]->setFGround(true);
 	MOBILE[18]->setStartX(1621); MOBILE[18]->setStartY(321); MOBILE[18]->setStartFrame(25); MOBILE[18]->setEndFrame(25); MOBILE[18]->setType(0); MOBILE[18]->setFGround(true);
    MOBILE[19]->setStartX(1621); MOBILE[19]->setStartY(273); MOBILE[19]->setStartFrame(25); MOBILE[19]->setEndFrame(25); MOBILE[19]->setType(0); MOBILE[19]->setFGround(true);
    MOBILE[20]->setStartX(1621); MOBILE[20]->setStartY(225); MOBILE[20]->setStartFrame(26); MOBILE[20]->setEndFrame(26); MOBILE[20]->setType(0); MOBILE[20]->setFGround(true);
 	MOBILE[21]->setStartX(2224); MOBILE[21]->setStartY(321); MOBILE[21]->setStartFrame(25); MOBILE[21]->setEndFrame(25); MOBILE[21]->setType(0); MOBILE[21]->setFGround(true);
    MOBILE[22]->setStartX(2224); MOBILE[22]->setStartY(273); MOBILE[22]->setStartFrame(25); MOBILE[22]->setEndFrame(25); MOBILE[22]->setType(0); MOBILE[22]->setFGround(true);
    MOBILE[23]->setStartX(2224); MOBILE[23]->setStartY(225); MOBILE[23]->setStartFrame(26); MOBILE[23]->setEndFrame(26); MOBILE[23]->setType(0); MOBILE[23]->setFGround(true);

 	MOBILE[24]->setStartX(2480); MOBILE[24]->setStartY(321); MOBILE[24]->setStartFrame(25); MOBILE[24]->setEndFrame(25); MOBILE[24]->setType(0); MOBILE[24]->setFGround(true);
    MOBILE[25]->setStartX(2480); MOBILE[25]->setStartY(273); MOBILE[25]->setStartFrame(25); MOBILE[25]->setEndFrame(25); MOBILE[25]->setType(0); MOBILE[25]->setFGround(true);
    MOBILE[26]->setStartX(2480); MOBILE[26]->setStartY(225); MOBILE[26]->setStartFrame(26); MOBILE[26]->setEndFrame(26); MOBILE[26]->setType(0); MOBILE[26]->setFGround(true);

    MOBILE[27]->setStartX(1632); MOBILE[27]->setStartY(481); MOBILE[27]->setStartFrame(27); MOBILE[27]->setEndFrame(27); MOBILE[27]->setType(0); MOBILE[27]->setFGround(true);
    MOBILE[28]->setStartX(1680); MOBILE[28]->setStartY(481); MOBILE[28]->setStartFrame(28); MOBILE[28]->setEndFrame(28); MOBILE[28]->setType(0); MOBILE[28]->setFGround(true);
    MOBILE[29]->setStartX(1728); MOBILE[29]->setStartY(481); MOBILE[29]->setStartFrame(29); MOBILE[29]->setEndFrame(29); MOBILE[29]->setType(0); MOBILE[29]->setFGround(true);
    tot_mobile = 29;
    if(STAGE==12)
     {
	  MOBILE[30]->setStartX(4936); MOBILE[30]->setStartY(570); MOBILE[30]->setStartFrame(24); MOBILE[30]->setEndFrame(24); MOBILE[30]->setType(0); MOBILE[30]->setFGround(true);
	  tot_mobile = 30;
	 }

    for(int i=0;i<=6;i++)           MOBILE[i]->resetObj(true);
    for(int i=12;i<=tot_mobile;i++) MOBILE[i]->resetObj(true);

 	// Other
 	WA_SFX[0]->setStartX(0);    WA_SFX[0]->setStartY(647); WA_SFX[0]->resetSFX(true); WA_SFX[0]->setScrollSpeed(6); WA_SFX[0]->setWaveSpeed(50);
 	WA_SFX[1]->setStartX(3265); WA_SFX[1]->setStartY(637); WA_SFX[1]->resetSFX(true); WA_SFX[1]->setScrollSpeed(6); WA_SFX[1]->setWaveSpeed(8);  WA_SFX[1]->setWaterLen(12); WA_SFX[1]->setAlpha(200);
 	RA_SFX[0]->setStartX(3100); RA_SFX[0]->setStartY(516); RA_SFX[0]->resetSFX(true,0);
 	RA_SFX[1]->setStartX(3050); RA_SFX[1]->setStartY(516); RA_SFX[1]->resetSFX(true,1);

 	// Set difficulty (use a cascade switch)
 	switch(DIFFICULTY)
 	 {
 	  case 1: {
 	           if(STAGE==2) {
	                         P2[0]->resetObj(false); GI[0]->resetObj(false);
	                         CL[0]->resetObj(false);
	                        }
              }
	  case 2: {
	           if(STAGE==2) {
				             GI[2]->resetObj(false); CL[1]->resetObj(false);
	                        }
	          }
	 }
   }
  if(STAGE==3 || STAGE==11)
   {
    // Player position
    if(STAGE==3)  {
                   if(!CKPOINT) { RESTART_X=130;  RESTART_Y=598; MIN=2; SEC=30; RETIME=true;  }
                   else         { RESTART_X=3700; RESTART_Y=625; MIN=2; SEC=30; RETIME=false; }
			      }
    if(STAGE==11) {
                   if(!CKPOINT) { RESTART_X=6080; RESTART_Y=593; MIN=2; SEC=30; RETIME=true;  }
  	               else         { RESTART_X=3200; RESTART_Y=544; MIN=2; SEC=30; RETIME=false; }
			      }
    PL->resetPlayer(RESTART_X,RESTART_Y); if(STAGE==11) PL->setSide(1);
    // Plant mk2
    P2[0]->setStartX(850);  P2[0]->setStartY(591);
    P2[1]->setStartX(2850); P2[1]->setStartY(591);
    P2[2]->setStartX(4715); P2[2]->setStartY(290);
    P2[3]->setStartX(5021); P2[3]->setStartY(394);
    P2[4]->setStartX(5682); P2[4]->setStartY(594);
    for(int i=0;i<5;i++) P2[i]->resetObj(true);
    // Volture
    VO[0]->setStartX(2240); VO[0]->setStartY(568);
    for(int i=0;i<N_VO;i++) VO[i]->resetObj(true); 	 	
    // Red arremer
    RA[0]->setStartX(500);  RA[0]->setStartY(591); RA[0]->resetObj(true);
    RA[1]->setStartX(2600); RA[1]->setStartY(449); RA[1]->resetObj(true);
    // Red arremer King
    if(STAGE==3)  { RK[0]->setStartX(6050); RK[0]->setStartY(600); }
    if(STAGE==11) { RK[0]->setStartX(88);   RK[0]->setStartY(602); }
    for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
    RK[0]->setExitKeyP(KEY);
 	// Bat
 	for(int i=0;i<N_BA;i++) { BA[i]->setStartX(0); BA[i]->setStartY(6000); BA[i]->resetObj(true); }
 	// Giant
 	GI[0]->setStartX(1358); GI[0]->setStartY(576);
 	GI[1]->setStartX(2100); GI[1]->setStartY(449);
 	GI[2]->setStartX(3200); GI[2]->setStartY(449);
 	GI[3]->setStartX(2600); GI[3]->setStartY(576);
 	GI[4]->setStartX(3200); GI[4]->setStartY(322);
 	GI[5]->setStartX(2800); GI[5]->setStartY(322);
 	for(int i=0;i<N_GI;i++) GI[i]->resetObj(true);

 	// Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(30);   MOBILE[0]->setStartY(598);  MOBILE[0]->setDisX(130); MOBILE[0]->setDisY(0);   MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(846);  MOBILE[1]->setStartY(592);  MOBILE[1]->setDisX(100); MOBILE[1]->setDisY(0);   MOBILE[1]->setFGround(true);
    MOBILE[2]->setStartX(1230); MOBILE[2]->setStartY(592);  MOBILE[2]->setDisX(-80); MOBILE[2]->setDisY(0);   MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(1336); MOBILE[3]->setStartY(610);  MOBILE[3]->setDisX(-90); MOBILE[3]->setDisY(-82); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(2680); MOBILE[4]->setStartY(571);  MOBILE[4]->setDisX(140); MOBILE[4]->setDisY(-99); MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(3940); MOBILE[5]->setStartY(515);  MOBILE[5]->setDisX(-90); MOBILE[5]->setDisY(-66); MOBILE[5]->setFGround(true);
    
    //MOBILE[6]->setStartX(340);  MOBILE[6]->setStartY(580);  MOBILE[6]->setDisX(0);  MOBILE[6]->setDisY(0);  MOBILE[6]->setFGround(true);
    //MOBILE[7]->setStartX(380);  MOBILE[7]->setStartY(580);  MOBILE[7]->setDisX(0);  MOBILE[7]->setDisY(0);  MOBILE[7]->setFGround(true);
    //MOBILE[8]->setStartX(420);  MOBILE[8]->setStartY(580);  MOBILE[8]->setDisX(0);  MOBILE[8]->setDisY(0);  MOBILE[8]->setFGround(true);
    //MOBILE[9]->setStartX(460);  MOBILE[9]->setStartY(580);  MOBILE[9]->setDisX(0);  MOBILE[9]->setDisY(0);  MOBILE[9]->setFGround(true);
    //MOBILE[10]->setStartX(20);  MOBILE[10]->setStartY(580); MOBILE[10]->setDisX(0); MOBILE[10]->setDisY(0); MOBILE[10]->setFGround(true);
    //MOBILE[11]->setStartX(60);  MOBILE[11]->setStartY(580); MOBILE[11]->setDisX(0); MOBILE[11]->setDisY(0); MOBILE[11]->setFGround(true);

 	MOBILE[12]->setStartX(538);  MOBILE[12]->setStartY(642); MOBILE[12]->setStartFrame(32); MOBILE[12]->setEndFrame(37); MOBILE[12]->setType(1); MOBILE[12]->setFGround(false);
    MOBILE[13]->setStartX(638);  MOBILE[13]->setStartY(642); MOBILE[13]->setStartFrame(32); MOBILE[13]->setEndFrame(37); MOBILE[13]->setType(1); MOBILE[13]->setFGround(false);
    MOBILE[14]->setStartX(860+35);  MOBILE[14]->setStartY(614); MOBILE[14]->setStartFrame(5);  MOBILE[14]->setEndFrame(5);  MOBILE[14]->setType(0); MOBILE[14]->setFGround(true);
    MOBILE[15]->setStartX(908+35);  MOBILE[15]->setStartY(614); MOBILE[15]->setStartFrame(5);  MOBILE[15]->setEndFrame(5);  MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
    MOBILE[16]->setStartX(956+35);  MOBILE[16]->setStartY(614); MOBILE[16]->setStartFrame(5);  MOBILE[16]->setEndFrame(5);  MOBILE[16]->setType(0); MOBILE[16]->setFGround(true);
    MOBILE[17]->setStartX(1004+35); MOBILE[17]->setStartY(614); MOBILE[17]->setStartFrame(5);  MOBILE[17]->setEndFrame(5);  MOBILE[17]->setType(0); MOBILE[17]->setFGround(true);

    MOBILE[18]->setStartX(1770); MOBILE[18]->setStartY(600); MOBILE[18]->setMinY(500); MOBILE[18]->setMaxY(640); MOBILE[18]->setStartFrame(5); MOBILE[18]->setEndFrame(5); MOBILE[18]->setType(4); MOBILE[18]->setFGround(true);
    MOBILE[19]->setStartX(2416); MOBILE[19]->setStartY(600); MOBILE[19]->setMinY(500); MOBILE[19]->setMaxY(640); MOBILE[19]->setStartFrame(5); MOBILE[19]->setEndFrame(5); MOBILE[19]->setType(4); MOBILE[19]->setFGround(true);
    MOBILE[20]->setStartX(2518); MOBILE[20]->setStartY(450); MOBILE[20]->setMinY(500); MOBILE[20]->setMaxY(640); MOBILE[20]->setStartFrame(5); MOBILE[20]->setEndFrame(5); MOBILE[20]->setType(4); MOBILE[20]->setFGround(true);
    MOBILE[21]->setStartX(2800); MOBILE[21]->setStartY(480); MOBILE[21]->setMinY(500); MOBILE[21]->setMaxY(640); MOBILE[21]->setStartFrame(5); MOBILE[21]->setEndFrame(5); MOBILE[21]->setType(4); MOBILE[21]->setFGround(true);
    MOBILE[22]->setStartX(2848); MOBILE[22]->setStartY(480); MOBILE[22]->setMinY(500); MOBILE[22]->setMaxY(640); MOBILE[22]->setStartFrame(5); MOBILE[22]->setEndFrame(5); MOBILE[22]->setType(4); MOBILE[22]->setFGround(true);
    MOBILE[23]->setStartX(3800); MOBILE[23]->setStartY(480); MOBILE[23]->setMinY(400); MOBILE[23]->setMaxY(640); MOBILE[23]->setStartFrame(38); MOBILE[23]->setEndFrame(38); MOBILE[23]->setType(4); MOBILE[23]->setFGround(false);
    MOBILE[24]->setStartX(4600); MOBILE[24]->setStartY(480); MOBILE[24]->setMinY(350); MOBILE[24]->setMaxY(560); MOBILE[24]->setStartFrame(38); MOBILE[24]->setEndFrame(38); MOBILE[24]->setType(4); MOBILE[24]->setFGround(false);
    MOBILE[25]->setStartX(4643); MOBILE[25]->setStartY(480); MOBILE[25]->setMinY(350); MOBILE[25]->setMaxY(560); MOBILE[25]->setStartFrame(38); MOBILE[25]->setEndFrame(38); MOBILE[25]->setType(4); MOBILE[25]->setFGround(false);
    MOBILE[26]->setStartX(5368); MOBILE[26]->setStartY(480); MOBILE[26]->setMinY(300); MOBILE[26]->setMaxY(580); MOBILE[26]->setStartFrame(38); MOBILE[26]->setEndFrame(38); MOBILE[26]->setType(4); MOBILE[26]->setFGround(false);
    MOBILE[27]->setStartX(5411); MOBILE[27]->setStartY(480); MOBILE[27]->setMinY(300); MOBILE[27]->setMaxY(580); MOBILE[27]->setStartFrame(38); MOBILE[27]->setEndFrame(38); MOBILE[27]->setType(4); MOBILE[27]->setFGround(false);

    MOBILE[28]->setStartX(4552); MOBILE[28]->setStartY(290); MOBILE[28]->setStartFrame(39); MOBILE[28]->setEndFrame(39); MOBILE[28]->setType(0); MOBILE[28]->setFGround(true);
    tot_mobile = 28;

    for(int i=0;i<=5;i++)           MOBILE[i]->resetObj(true);
    for(int i=12;i<=tot_mobile;i++) MOBILE[i]->resetObj(true);

 	// Other
 	WA_SFX[0]->setStartX(590);  WA_SFX[0]->setStartY(651); WA_SFX[0]->resetSFX(true); WA_SFX[0]->setScrollSpeed(6); WA_SFX[0]->setWaveSpeed(50); WA_SFX[0]->setWaterLen(5); WA_SFX[0]->setAlpha(200);
 	WA_SFX[1]->setStartX(1800); WA_SFX[1]->setStartY(651); WA_SFX[1]->resetSFX(true); WA_SFX[1]->setScrollSpeed(6); WA_SFX[1]->setWaveSpeed(50); WA_SFX[1]->setWaterLen(9); WA_SFX[1]->setAlpha(200);
 	SN_SFX[0]->setStartX(3200); SN_SFX[0]->setStartY(516); SN_SFX[0]->resetSFX(true,0);
 	SN_SFX[1]->setStartX(3250); SN_SFX[1]->setStartY(516); SN_SFX[1]->resetSFX(true,1);
   }
  if(STAGE==4 || STAGE==10)
   {
    // Player position
    if(STAGE==4)  {
                   if(!CKPOINT) { RESTART_X=130;  RESTART_Y=222; MIN=3; SEC=00; RETIME=true;  }
                   else         { RESTART_X=2390; RESTART_Y=574; MIN=4; SEC=00; RETIME=false; }
			      }
    if(STAGE==10) {
                   if(!CKPOINT) { RESTART_X=4498; RESTART_Y=2175; MIN=4; SEC=00; RETIME=true;  }
  	               else         { RESTART_X=2300; RESTART_Y=584;  MIN=3; SEC=00; RETIME=false; }
			      }
    PL->resetPlayer(RESTART_X,RESTART_Y); if(STAGE==10) PL->setSide(1);
 	// Rockfall
 	RF[0]->setStartX(1800);  RF[0]->setStartY(75);  RF[0]->setMaxY(230);  RF[0]->resetObj(true);
 	RF[1]->setStartX(2000);  RF[1]->setStartY(70);  RF[1]->setMaxY(230);  RF[1]->resetObj(true);
 	RF[2]->setStartX(2100);  RF[2]->setStartY(70);  RF[2]->setMaxY(230);  RF[2]->resetObj(true);
 	RF[3]->setStartX(2300);  RF[3]->setStartY(70);  RF[3]->setMaxY(230);  RF[3]->resetObj(true);
 	RF[4]->setStartX(2400);  RF[4]->setStartY(70);  RF[4]->setMaxY(412);  RF[4]->resetObj(true);
 	RF[5]->setStartX(2600);  RF[5]->setStartY(70);  RF[5]->setMaxY(230);  RF[5]->resetObj(true);
 	RF[6]->setStartX(2900);  RF[6]->setStartY(70);  RF[6]->setMaxY(230);  RF[6]->resetObj(true);
 	RF[7]->setStartX(3000);  RF[7]->setStartY(70);  RF[7]->setMaxY(230);  RF[7]->resetObj(true);
 	RF[8]->setStartX(3100);  RF[8]->setStartY(70);  RF[8]->setMaxY(230);  RF[8]->resetObj(true);
 	RF[9]->setStartX(3250);  RF[9]->setStartY(70);  RF[9]->setMaxY(230);  RF[9]->resetObj(true);
    RF[10]->setStartX(1900); RF[10]->setStartY(70); RF[10]->setMaxY(230); RF[10]->resetObj(true);
    RF[11]->setStartX(3200); RF[11]->setStartY(70); RF[11]->setMaxY(230); RF[11]->resetObj(true);
    RF[12]->setStartX(3350); RF[12]->setStartY(70); RF[12]->setMaxY(354); RF[12]->resetObj(true);
    RF[13]->setStartX(3450); RF[13]->setStartY(70); RF[13]->setMaxY(230); RF[13]->resetObj(true);
    RF[14]->setStartX(3500); RF[14]->setStartY(70); RF[14]->setMaxY(230); RF[14]->resetObj(true);
    RF[15]->setStartX(3600); RF[15]->setStartY(70); RF[15]->setMaxY(230); RF[15]->resetObj(true);
    RF[16]->setStartX(3700); RF[16]->setStartY(70); RF[16]->setMaxY(230); RF[16]->resetObj(true);
    RF[17]->setStartX(3800); RF[17]->setStartY(70); RF[17]->setMaxY(230); RF[17]->resetObj(true);
    RF[18]->setStartX(3400); RF[18]->setStartY(70); RF[18]->setMaxY(230); RF[18]->resetObj(true);
    RF[19]->setStartX(3650); RF[19]->setStartY(70); RF[19]->setMaxY(230); RF[19]->resetObj(true);
    // Plant mk2
    P2[0]->setStartX(850);  P2[0]->setStartY(591);
    P2[1]->setStartX(2850); P2[1]->setStartY(219);
    P2[2]->setStartX(4715); P2[2]->setStartY(290);
    P2[3]->setStartX(5021); P2[3]->setStartY(394);
    for(int i=0;i<4;i++)    P2[i]->resetObj(true);
    // Stalagmite
    ST[0]->setStartX(815);  ST[0]->setStartY(220);
    ST[1]->setStartX(1319); ST[1]->setStartY(220);
    ST[2]->setStartX(3999); ST[2]->setStartY(220);
    for(int i=0;i<N_ST;i++) ST[i]->resetObj(true);
    // Volture
    VO[0]->setStartX(2240); VO[0]->setStartY(568);
    for(int i=0;i<N_VO;i++) VO[i]->resetObj(true); 	 	
    // Red arremer
    RA[0]->setStartX(3000); RA[0]->setStartY(218); RA[0]->resetObj(true);
    RA[1]->setStartX(2600); RA[1]->setStartY(218); RA[1]->resetObj(true);
    RA[2]->setStartX(4044); RA[2]->setStartY(218); RA[2]->resetObj(true);
    // Red arremer King
    if(STAGE==4)  { RK[0]->setStartX(4450); RK[0]->setStartY(2170); }
    if(STAGE==10) { RK[0]->setStartX(84);   RK[0]->setStartY(228);  }
    RK[0]->setExitKeyP(KEY);
    for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
    // Bat
    for(int i=0;i<N_BA;i++) { BA[i]->setStartX(0); BA[i]->setStartY(6000); BA[i]->resetObj(true); }
    // Giant
    GI[0]->setStartX(1358); GI[0]->setStartY(576);
    GI[1]->setStartX(2100); GI[1]->setStartY(449);
    GI[2]->setStartX(3200); GI[2]->setStartY(449);
    GI[3]->setStartX(2600); GI[3]->setStartY(576);
    GI[4]->setStartX(3200); GI[4]->setStartY(322);
    GI[5]->setStartX(2800); GI[5]->setStartY(322);
    for(int i=0;i<N_GI;i++) GI[i]->resetObj(true);

	// Immobile enemies
 	IMMOBILE[0]->setStartXYFrame ( 456,  64, 0); IMMOBILE[0]->resetObj(true);
 	IMMOBILE[1]->setStartXYFrame ( 424,   0,18); IMMOBILE[1]->resetObj(true);
 	IMMOBILE[2]->setStartXYFrame ( 488,   0,36); IMMOBILE[2]->resetObj(true);
 	IMMOBILE[3]->setStartXYFrame ( 838,  30, 0); IMMOBILE[3]->resetObj(true);
 	IMMOBILE[4]->setStartXYFrame ( 806, -30,18); IMMOBILE[4]->resetObj(true);
 	IMMOBILE[5]->setStartXYFrame ( 870, -30,36); IMMOBILE[5]->resetObj(true);
 	IMMOBILE[6]->setStartXYFrame (1248, 132, 0); IMMOBILE[6]->resetObj(true);
 	IMMOBILE[7]->setStartXYFrame (1216,  68,18); IMMOBILE[7]->resetObj(true);
 	IMMOBILE[8]->setStartXYFrame (1280,  68,36); IMMOBILE[8]->resetObj(true);
	IMMOBILE[9]->setStartXYFrame (2250, 330,54); IMMOBILE[9]->resetObj(true);
	IMMOBILE[10]->setStartXYFrame(3890,1212,54); IMMOBILE[10]->resetObj(true);
	IMMOBILE[11]->setStartXYFrame(3524,1304,54); IMMOBILE[11]->resetObj(true);
	IMMOBILE[12]->setStartXYFrame(3686,2030,54); IMMOBILE[12]->resetObj(true);
	IMMOBILE[13]->setStartXYFrame(1840, 370,41); IMMOBILE[13]->resetObj(true);
	IMMOBILE[14]->setStartXYFrame(4192,1066,54); IMMOBILE[14]->resetObj(true);
	IMMOBILE[15]->setStartXYFrame(1996, 330,54); IMMOBILE[15]->resetObj(true);
	IMMOBILE[16]->setStartXYFrame(3220, 862,54); IMMOBILE[16]->resetObj(true);
	IMMOBILE[17]->setStartXYFrame(3250, 870,54); IMMOBILE[17]->resetObj(true);
	IMMOBILE[18]->setStartXYFrame(3354, 832,54); IMMOBILE[18]->resetObj(true);
	IMMOBILE[19]->setStartXYFrame(3388, 840,54); IMMOBILE[19]->resetObj(true);

    // Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(252);  MOBILE[0]->setStartY(222);  MOBILE[0]->setDisX(0);  MOBILE[0]->setDisY(0);   MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(870);  MOBILE[1]->setStartY(106);  MOBILE[1]->setDisX(0);  MOBILE[1]->setDisY(-30); MOBILE[1]->setFGround(true);
    MOBILE[2]->setStartX(1470); MOBILE[2]->setStartY(222);  MOBILE[2]->setDisX(0);  MOBILE[2]->setDisY(-70); MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(4080); MOBILE[3]->setStartY(222);  MOBILE[3]->setDisX(0);  MOBILE[3]->setDisY(-70); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(4562); MOBILE[4]->setStartY(272);  MOBILE[4]->setDisX(0);  MOBILE[4]->setDisY(-80); MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(1996); MOBILE[5]->setStartY(414);  MOBILE[5]->setDisX(0);  MOBILE[5]->setDisY(-30); MOBILE[5]->setFGround(true);

    MOBILE[6]->setStartX(340);  MOBILE[6]->setStartY(1222);  MOBILE[6]->setDisX(0);  MOBILE[6]->setDisY(0);  MOBILE[6]->setFGround(true);
    MOBILE[7]->setStartX(380);  MOBILE[7]->setStartY(1222);  MOBILE[7]->setDisX(0);  MOBILE[7]->setDisY(0);  MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(420);  MOBILE[8]->setStartY(1222);  MOBILE[8]->setDisX(0);  MOBILE[8]->setDisY(0);  MOBILE[8]->setFGround(true);
    MOBILE[9]->setStartX(460);  MOBILE[9]->setStartY(1222);  MOBILE[9]->setDisX(0);  MOBILE[9]->setDisY(0);  MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(20);  MOBILE[10]->setStartY(1222); MOBILE[10]->setDisX(0); MOBILE[10]->setDisY(0); MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(60);  MOBILE[11]->setStartY(1222); MOBILE[11]->setDisX(0); MOBILE[11]->setDisY(0); MOBILE[11]->setFGround(true);

    MOBILE[12]->setStartX(200);  MOBILE[12]->setStartY(226); MOBILE[12]->setStartFrame(40); MOBILE[12]->setEndFrame(40); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
    MOBILE[13]->setStartX(816);  MOBILE[13]->setStartY(226); MOBILE[13]->setStartFrame(40); MOBILE[13]->setEndFrame(40); MOBILE[13]->setType(0); MOBILE[13]->setFGround(true);
    MOBILE[14]->setStartX(1320); MOBILE[14]->setStartY(226); MOBILE[14]->setStartFrame(40); MOBILE[14]->setEndFrame(40); MOBILE[14]->setType(0); MOBILE[14]->setFGround(true);
    MOBILE[15]->setStartX(4000); MOBILE[15]->setStartY(226); MOBILE[15]->setStartFrame(40); MOBILE[15]->setEndFrame(40); MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
    tot_mobile = 15;
    if(STAGE==10) {
		           MOBILE[16]->setStartX(3710); MOBILE[16]->setStartY(2100); MOBILE[16]->setMinY(2080); MOBILE[16]->setMaxY(2180); MOBILE[16]->setStartFrame(42); MOBILE[16]->setEndFrame(42); MOBILE[16]->setType(4); MOBILE[16]->setFGround(false);
		           MOBILE[17]->setStartX(3354); MOBILE[17]->setStartY(1800); MOBILE[17]->setMinY(1710); MOBILE[17]->setMaxY(1970); MOBILE[17]->setStartFrame(42); MOBILE[17]->setEndFrame(42); MOBILE[17]->setType(4); MOBILE[17]->setFGround(false);
		           MOBILE[18]->setStartX(3934); MOBILE[18]->setStartY(815);  MOBILE[18]->setStartFrame(22); MOBILE[18]->setEndFrame(22); MOBILE[18]->setType(0); MOBILE[18]->setFGround(true);
		           MOBILE[19]->setStartX(2540); MOBILE[19]->setStartY(720);  MOBILE[19]->setMinY(680);  MOBILE[19]->setMaxY(836);  MOBILE[19]->setStartFrame(42); MOBILE[19]->setEndFrame(42); MOBILE[19]->setType(4); MOBILE[19]->setFGround(false);
		           MOBILE[20]->setStartX(2470); MOBILE[20]->setStartY(620);  MOBILE[20]->setMinY(600);  MOBILE[20]->setMaxY(760);  MOBILE[20]->setStartFrame(42); MOBILE[20]->setEndFrame(42); MOBILE[20]->setType(4); MOBILE[20]->setFGround(false);
		           tot_mobile = 20;
			      }

    for(int i=0;i<=5;i++)           MOBILE[i]->resetObj(true);
    for(int i=12;i<=tot_mobile;i++) MOBILE[i]->resetObj(true);

    // Other
    WA_SFX[0]->setStartX(590);  WA_SFX[0]->setStartY(651); WA_SFX[0]->resetSFX(true); WA_SFX[0]->setScrollSpeed(6); WA_SFX[0]->setWaveSpeed(50); WA_SFX[0]->setWaterLen(5); WA_SFX[0]->setAlpha(200); WA_SFX[0]->resetSFX(false);
    WA_SFX[1]->setStartX(1800); WA_SFX[1]->setStartY(651); WA_SFX[1]->resetSFX(true); WA_SFX[1]->setScrollSpeed(6); WA_SFX[1]->setWaveSpeed(50); WA_SFX[1]->setWaterLen(9); WA_SFX[1]->setAlpha(200); WA_SFX[1]->resetSFX(false);
   }

  if(STAGE==5 || STAGE==9)
   {
    // Player position
    if(STAGE==5) {
                  if(!CKPOINT) { RESTART_X=130;  RESTART_Y=2176; MIN=3; SEC=00; RETIME=true;  }
  	              else         { RESTART_X=1632; RESTART_Y=167;  MIN=2; SEC=00; RETIME=false; }
			     }
    if(STAGE==9) {
                  if(!CKPOINT) { RESTART_X=2602; RESTART_Y=228; MIN=2; SEC=00; RETIME=true;  }
  	              else         { RESTART_X=1484; RESTART_Y=129; MIN=3; SEC=00; RETIME=false; }
			     }
    PL->resetPlayer(RESTART_X,RESTART_Y); if(STAGE==9) PL->setSide(1);
 	// Plant mk2
 	P2[0]->setStartX(1350); P2[0]->setStartY(2092);
 	P2[1]->setStartX(1232); P2[1]->setStartY(641);

 	P2[2]->setStartX(2826); P2[2]->setStartY(449);
 	P2[3]->setStartX(3694); P2[3]->setStartY(548);
 	P2[4]->setStartX(4490); P2[4]->setStartY(342);
 	P2[5]->setStartX(4228); P2[5]->setStartY(316);
 	P2[6]->setStartX(4760); P2[6]->setStartY(342);
 	for(int i=0;i<N_P2;i++) P2[i]->resetObj(true);
    // Stalagmite
    ST[0]->setStartX(993); ST[0]->setStartY(350); ST[0]->resetObj(true);
 	// Volture
 	VO[0]->setStartX(2240); VO[0]->setStartY(568);
 	for(int i=0;i<N_VO;i++) VO[i]->resetObj(true); 	 	
 	// Red arremer
 	RA[0]->setStartX(1400); RA[0]->setStartY(1660); RA[0]->resetObj(true);
 	RA[1]->setStartX(2600); RA[1]->setStartY(449);  RA[1]->resetObj(true);
 	// Red arremer King
 	if(STAGE==5) { RK[0]->setStartX(2650); RK[0]->setStartY(233); }
 	if(STAGE==9) { RK[0]->setStartX(80);   RK[0]->setStartY(2180); }
 	RK[0]->setExitKeyP(KEY);
 	for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
 	// Bat
 	for(int i=0;i<N_BA;i++) { BA[i]->setStartX(0); BA[i]->setStartY(6000); BA[i]->resetObj(true); }
 	// Giant
 	GI[0]->setStartX(1358); GI[0]->setStartY(576); GI[0]->setMinX(850);  GI[0]->setMaxX(1500);
 	GI[1]->setStartX(2100); GI[1]->setStartY(449); GI[1]->setMinX(1620); GI[1]->setMaxX(2050);
 	GI[2]->setStartX(3200); GI[2]->setStartY(449); GI[2]->setMinX(1950); GI[2]->setMaxX(2700);
 	GI[3]->setStartX(2600); GI[3]->setStartY(576); GI[3]->setMinX(1600); GI[3]->setMaxX(2600);
 	GI[4]->setStartX(3200); GI[4]->setStartY(322); GI[4]->setMinX(1900); GI[4]->setMaxX(2750);
 	GI[5]->setStartX(2800); GI[5]->setStartY(322); GI[5]->setMinX(1700); GI[5]->setMaxX(2750);
 	for(int i=0;i<N_GI;i++) GI[i]->resetObj(true);
 	// Insect
 	for(int i=0;i<1;i++) { IN[i]->setStartX(850); IN[i]->setStartY(750); IN[i]->setType(1); IN[i]->resetObj(true); }
 	// Clam
 	CL[0]->setStartX(3898); CL[0]->setStartY(578); CL[0]->setSide(0);
 	CL[1]->setStartX(4152); CL[1]->setStartY(500); CL[1]->setSide(1);
 	CL[2]->setStartX(4160); CL[2]->setStartY(548); CL[2]->setSide(1);
 	CL[3]->setStartX(4172); CL[3]->setStartY(387); CL[3]->setSide(0);
 	CL[4]->setStartX(4120); CL[4]->setStartY(226); CL[4]->setSide(1);
 	CL[5]->setStartX(4684); CL[5]->setStartY(300); CL[5]->setSide(1);
    CL[6]->setStartX(4712); CL[6]->setStartY(500); CL[6]->setSide(1);
    CL[7]->setStartX(4862); CL[7]->setStartY(566); CL[7]->setSide(0);
 	for(int i=0;i<N_CL;i++) CL[i]->resetObj(true);
 	// Demon
 	DE[0]->setStartX(580); DE[0]->setStartY(2176); DE[0]->resetObj(true);
 	DE[1]->setStartX(860); DE[1]->setStartY(2176); DE[1]->resetObj(true);

	// Immobile enemies
 	IMMOBILE[0]->setStartXYFrame(1720,120,72); IMMOBILE[0]->resetObj(true);
 	IMMOBILE[1]->setStartXYFrame(1970,155,72); IMMOBILE[1]->resetObj(true);

 	// Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(300);   MOBILE[0]->setStartY(2176); MOBILE[0]->setDisX(0);   MOBILE[0]->setDisY(-42); MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(1550);  MOBILE[1]->setStartY(2178); MOBILE[1]->setDisX(0);   MOBILE[1]->setDisY(-42); MOBILE[1]->setFGround(true);

    MOBILE[2]->setStartX(1728); MOBILE[2]->setStartY(195); MOBILE[2]->setDisX(-99); MOBILE[2]->setDisY(-36); MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(2990); MOBILE[3]->setStartY(195); MOBILE[3]->setDisX(-99); MOBILE[3]->setDisY(-36); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(3248); MOBILE[4]->setStartY(579); MOBILE[4]->setDisX(80);  MOBILE[4]->setDisY(-36); MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(4000); MOBILE[5]->setStartY(334); MOBILE[5]->setDisX(-60); MOBILE[5]->setDisY(-36); MOBILE[5]->setFGround(true);
    MOBILE[6]->setStartX(4576); MOBILE[6]->setStartY(342); MOBILE[6]->setDisX(150); MOBILE[6]->setDisY(-36); MOBILE[6]->setFGround(true);
    MOBILE[7]->setStartX(0);    MOBILE[7]->setStartY(0);   MOBILE[7]->setDisX(0);   MOBILE[7]->setDisY(0);  MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(0);    MOBILE[8]->setStartY(0);   MOBILE[8]->setDisX(0);   MOBILE[8]->setDisY(0);  MOBILE[8]->setFGround(true);
    MOBILE[9]->setStartX(0);    MOBILE[9]->setStartY(0);   MOBILE[9]->setDisX(0);   MOBILE[9]->setDisY(0);  MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(0);   MOBILE[10]->setStartY(0);  MOBILE[10]->setDisX(0);  MOBILE[10]->setDisY(0); MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(0);   MOBILE[11]->setStartY(0);  MOBILE[11]->setDisX(0);  MOBILE[11]->setDisY(0); MOBILE[11]->setFGround(true);

 	MOBILE[12]->setStartX(1460); MOBILE[12]->setStartY(2178); MOBILE[12]->setStartFrame(22); MOBILE[12]->setEndFrame(25); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
    MOBILE[13]->setStartX(1210); MOBILE[13]->setStartY(1800); MOBILE[13]->setMinY(1700);     MOBILE[13]->setMaxY(1930);   MOBILE[13]->setStartFrame(41); MOBILE[13]->setEndFrame(41); MOBILE[13]->setType(4); MOBILE[13]->setFGround(true);
    MOBILE[14]->setStartX(1040); MOBILE[14]->setStartY(1100); MOBILE[14]->setMinY(1010);     MOBILE[14]->setMaxY(1260);   MOBILE[14]->setStartFrame(41); MOBILE[14]->setEndFrame(41); MOBILE[14]->setType(4); MOBILE[14]->setFGround(true);
    MOBILE[15]->setStartX(994);  MOBILE[15]->setStartY(356);  MOBILE[15]->setStartFrame(40); MOBILE[15]->setEndFrame(40); MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
    MOBILE[16]->setStartX(1698); MOBILE[16]->setStartY(158);  MOBILE[16]->setStartFrame(22); MOBILE[16]->setEndFrame(40); MOBILE[16]->setType(0); MOBILE[16]->setFGround(true);
    MOBILE[17]->setStartX(1950); MOBILE[17]->setStartY(80);   MOBILE[17]->setMinY(60);       MOBILE[17]->setMaxY(140);    MOBILE[17]->setStartFrame(41); MOBILE[17]->setEndFrame(41); MOBILE[17]->setType(4); MOBILE[17]->setFGround(true);
    tot_mobile = 17;
    if(STAGE==9) {
		          MOBILE[18]->setStartX(2050); MOBILE[18]->setStartY(70); MOBILE[18]->setMinY(60); MOBILE[18]->setMaxY(176); MOBILE[18]->setStartFrame(41); MOBILE[18]->setEndFrame(41); MOBILE[18]->setType(4); MOBILE[18]->setFGround(true);
                  MOBILE[19]->setStartX(1600); MOBILE[19]->setStartY(90); MOBILE[19]->setMinY(80); MOBILE[19]->setMaxY(178); MOBILE[19]->setStartFrame(41); MOBILE[19]->setEndFrame(41); MOBILE[19]->setType(4); MOBILE[19]->setFGround(true);
                  tot_mobile = 19;
                 }

    for(int i=0;i<=1;i++)           MOBILE[i]->resetObj(true);
    for(int i=12;i<=tot_mobile;i++) MOBILE[i]->resetObj(true);

 	// Other
 	FO_SFX->setStartX(1330);   FO_SFX->setStartY(0); FO_SFX->resetSFX(true);
 	RA_SFX[0]->setStartX(3100); RA_SFX[0]->setStartY(516); RA_SFX[0]->resetSFX(true,0);
 	RA_SFX[1]->setStartX(3050); RA_SFX[1]->setStartY(516); RA_SFX[1]->resetSFX(true,1);
   }
  if(STAGE==6 || STAGE==8)
   {
    // Player position
    if(STAGE==6) {
                  if(!CKPOINT) { RESTART_X=130;  RESTART_Y=2166; MIN=3; SEC=00; RETIME=true;  }
  	              else         { RESTART_X=1060; RESTART_Y=1566; MIN=3; SEC=00; RETIME=false; }
			     }
    if(STAGE==8) {
                  if(!CKPOINT) { RESTART_X=3724; RESTART_Y=639;  MIN=3; SEC=00; RETIME=true;  }
  	              else         { RESTART_X=1230; RESTART_Y=1261; MIN=3; SEC=00; RETIME=false; }
			     }
    PL->resetPlayer(RESTART_X,RESTART_Y); if(STAGE==8) PL->setSide(1);
 	// Plant mk2
 	P2[0]->setStartX(1350); P2[0]->setStartY(2092);
 	P2[1]->setStartX(1232); P2[1]->setStartY(641);

 	P2[2]->setStartX(2826); P2[2]->setStartY(449);
 	P2[3]->setStartX(1840); P2[3]->setStartY(1022);
 	P2[4]->setStartX(4490); P2[4]->setStartY(342);
 	P2[5]->setStartX(4228); P2[5]->setStartY(316);
 	P2[6]->setStartX(4760); P2[6]->setStartY(342);
 	for(int i=0;i<N_P2;i++) P2[i]->resetObj(true);
    // Stalagmite
    ST[0]->setStartX(993); ST[0]->setStartY(350); ST[0]->resetObj(true);
    if(STAGE==8) {
		          ST[1]->setStartX(3227); ST[1]->setStartY(634);  ST[1]->resetObj(true);
                  ST[2]->setStartX(891);  ST[2]->setStartY(1564); ST[2]->resetObj(true);
                  ST[3]->setStartX(525);  ST[3]->setStartY(2163); ST[3]->resetObj(true);
                  ST[4]->setStartX(437);  ST[4]->setStartY(2107); ST[4]->resetObj(true);
                 }

 	// Volture
 	VO[0]->setStartX(2240); VO[0]->setStartY(568);
 	for(int i=0;i<N_VO;i++) VO[i]->resetObj(true); 	 	
 	// Red arremer
 	RA[0]->setStartX(470);  RA[0]->setStartY(2106); RA[0]->resetObj(true);
 	RA[1]->setStartX(2600); RA[1]->setStartY(449);  RA[1]->resetObj(true);
 	RA[2]->setStartX(2600); RA[2]->setStartY(449);  RA[2]->resetObj(true);
 	// Red arremer King
 	if(STAGE==6) { RK[0]->setStartX(3630); RK[0]->setStartY(642); }
 	if(STAGE==8) { RK[0]->setStartX(98);   RK[0]->setStartY(2172); }
 	RK[0]->setExitKeyP(KEY);
 	for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
 	// Bat
 	for(int i=0;i<N_BA;i++) { BA[i]->setStartX(0); BA[i]->setStartY(6000); BA[i]->resetObj(true); }
 	// Giant
 	GI[0]->setStartX(1358); GI[0]->setStartY(576); GI[0]->setMinX(850);  GI[0]->setMaxX(1500);
 	GI[1]->setStartX(2100); GI[1]->setStartY(449); GI[1]->setMinX(1620); GI[1]->setMaxX(2050);
 	GI[2]->setStartX(3200); GI[2]->setStartY(449); GI[2]->setMinX(1950); GI[2]->setMaxX(2700);
 	GI[3]->setStartX(2600); GI[3]->setStartY(576); GI[3]->setMinX(1600); GI[3]->setMaxX(2600);
 	GI[4]->setStartX(3200); GI[4]->setStartY(322); GI[4]->setMinX(1900); GI[4]->setMaxX(2750);
 	GI[5]->setStartX(2800); GI[5]->setStartY(322); GI[5]->setMinX(1700); GI[5]->setMaxX(2750);
 	for(int i=0;i<N_GI;i++) GI[i]->resetObj(true);
 	// Insect
 	for(int i=0;i<1;i++) { IN[i]->setStartX(850); IN[i]->setStartY(750); IN[i]->setType(1); IN[i]->resetObj(true); }
 	// Clam
 	CL[0]->setStartX(3898); CL[0]->setStartY(578); CL[0]->setSide(0);
 	CL[1]->setStartX(4152); CL[1]->setStartY(500); CL[1]->setSide(1);
 	CL[2]->setStartX(4160); CL[2]->setStartY(548); CL[2]->setSide(1);
 	CL[3]->setStartX(4172); CL[3]->setStartY(387); CL[3]->setSide(0);
 	CL[4]->setStartX(4120); CL[4]->setStartY(226); CL[4]->setSide(1);
 	CL[5]->setStartX(4684); CL[5]->setStartY(300); CL[5]->setSide(1);
    CL[6]->setStartX(4712); CL[6]->setStartY(500); CL[6]->setSide(1);
    CL[7]->setStartX(4862); CL[7]->setStartY(566); CL[7]->setSide(0);
 	for(int i=0;i<N_CL;i++) CL[i]->resetObj(true);
 	// Demon
 	DE[0]->setStartX(580); DE[0]->setStartY(2176); DE[0]->resetObj(true);
 	DE[1]->setStartX(860); DE[1]->setStartY(2176); DE[1]->resetObj(true);

	// Immobile enemies
 	IMMOBILE[0]->setStartXYFrame(1720,120,72); IMMOBILE[0]->resetObj(true);
 	IMMOBILE[1]->setStartXYFrame(1850,140,72); IMMOBILE[1]->resetObj(true);

 	// Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(260);  MOBILE[0]->setStartY(2166); MOBILE[0]->setDisX(0);   MOBILE[0]->setDisY(-42); MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(958);  MOBILE[1]->setStartY(2192); MOBILE[1]->setDisX(0);   MOBILE[1]->setDisY(-42); MOBILE[1]->setFGround(true);

    MOBILE[2]->setStartX(1728); MOBILE[2]->setStartY(195); MOBILE[2]->setDisX(-99); MOBILE[2]->setDisY(-36); MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(2990); MOBILE[3]->setStartY(195); MOBILE[3]->setDisX(-99); MOBILE[3]->setDisY(-36); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(3248); MOBILE[4]->setStartY(579); MOBILE[4]->setDisX(80);  MOBILE[4]->setDisY(-36); MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(4000); MOBILE[5]->setStartY(334); MOBILE[5]->setDisX(-60); MOBILE[5]->setDisY(-36); MOBILE[5]->setFGround(true);
    MOBILE[6]->setStartX(4576); MOBILE[6]->setStartY(342); MOBILE[6]->setDisX(150); MOBILE[6]->setDisY(-36); MOBILE[6]->setFGround(true);
    MOBILE[7]->setStartX(0);    MOBILE[7]->setStartY(0);   MOBILE[7]->setDisX(0);   MOBILE[7]->setDisY(0);  MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(0);    MOBILE[8]->setStartY(0);   MOBILE[8]->setDisX(0);   MOBILE[8]->setDisY(0);  MOBILE[8]->setFGround(true);
    MOBILE[9]->setStartX(0);    MOBILE[9]->setStartY(0);   MOBILE[9]->setDisX(0);   MOBILE[9]->setDisY(0);  MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(0);   MOBILE[10]->setStartY(0);  MOBILE[10]->setDisX(0);  MOBILE[10]->setDisY(0); MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(0);   MOBILE[11]->setStartY(0);  MOBILE[11]->setDisX(0);  MOBILE[11]->setDisY(0); MOBILE[11]->setFGround(true);

    tot_mobile = 11;
    if(STAGE==8) {
		          MOBILE[12]->setStartX(3228); MOBILE[12]->setStartY(640);  MOBILE[12]->setStartFrame(40); MOBILE[12]->setEndFrame(40); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
                  MOBILE[13]->setStartX(892);  MOBILE[13]->setStartY(1570); MOBILE[13]->setStartFrame(40); MOBILE[13]->setEndFrame(40); MOBILE[13]->setType(0); MOBILE[13]->setFGround(true);
                  MOBILE[14]->setStartX(526);  MOBILE[14]->setStartY(2169); MOBILE[14]->setStartFrame(40); MOBILE[14]->setEndFrame(40); MOBILE[14]->setType(0); MOBILE[14]->setFGround(true);
                  MOBILE[15]->setStartX(438);  MOBILE[15]->setStartY(2113); MOBILE[15]->setStartFrame(40); MOBILE[15]->setEndFrame(40); MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
                  tot_mobile = 15;
                 }

    for(int i=0;i<=1;i++)           MOBILE[i]->resetObj(true);
    for(int i=12;i<=tot_mobile;i++) MOBILE[i]->resetObj(true);

 	// Other
 	FO_SFX->setStartX(1330);   FO_SFX->setStartY(0); FO_SFX->resetSFX(true);
   }
  if(STAGE==7)
   {
    // Player position
    RESTART_X=130; RESTART_Y=373; MIN=2; SEC=00; RETIME=true;
    PL->resetPlayer(RESTART_X,RESTART_Y);
 	// Plant mk2
 	P2[0]->setStartX(1350); P2[0]->setStartY(2092);
 	P2[1]->setStartX(1232); P2[1]->setStartY(641);

 	P2[2]->setStartX(2826); P2[2]->setStartY(449);
 	P2[3]->setStartX(3694); P2[3]->setStartY(548);
 	P2[4]->setStartX(4490); P2[4]->setStartY(342);
 	P2[5]->setStartX(4228); P2[5]->setStartY(316);
 	P2[6]->setStartX(4760); P2[6]->setStartY(342);
 	for(int i=0;i<N_P2;i++) P2[i]->resetObj(true);
    // Stalagmite
    ST[0]->setStartX(993); ST[0]->setStartY(350); ST[0]->resetObj(true);
 	// Volture
 	VO[0]->setStartX(2240); VO[0]->setStartY(568);
 	for(int i=0;i<N_VO;i++) VO[i]->resetObj(true); 	 	
 	// Red arremer
 	RA[0]->setStartX(1400); RA[0]->setStartY(1660); RA[0]->resetObj(true);
 	RA[1]->setStartX(2600); RA[1]->setStartY(449);  RA[1]->resetObj(true);
 	// Red arremer King
 	RK[0]->setStartX(1314); RK[0]->setStartY(352);
 	RK[0]->setExitKeyP(KEY);
 	for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
 	// Bat
 	for(int i=0;i<N_BA;i++) { BA[i]->setStartX(0); BA[i]->setStartY(6000); BA[i]->resetObj(true); }
 	// Giant
 	GI[0]->setStartX(1358); GI[0]->setStartY(576); GI[0]->setMinX(850);  GI[0]->setMaxX(1500);
 	GI[1]->setStartX(2100); GI[1]->setStartY(449); GI[1]->setMinX(1620); GI[1]->setMaxX(2050);
 	GI[2]->setStartX(3200); GI[2]->setStartY(449); GI[2]->setMinX(1950); GI[2]->setMaxX(2700);
 	GI[3]->setStartX(2600); GI[3]->setStartY(576); GI[3]->setMinX(1600); GI[3]->setMaxX(2600);
 	GI[4]->setStartX(3200); GI[4]->setStartY(322); GI[4]->setMinX(1900); GI[4]->setMaxX(2750);
 	GI[5]->setStartX(2800); GI[5]->setStartY(322); GI[5]->setMinX(1700); GI[5]->setMaxX(2750);
 	for(int i=0;i<N_GI;i++) GI[i]->resetObj(true);
 	// Insect
 	for(int i=0;i<1;i++) { IN[i]->setStartX(850); IN[i]->setStartY(750); IN[i]->setType(1); IN[i]->resetObj(true); }
 	// Clam
 	CL[0]->setStartX(3898); CL[0]->setStartY(578); CL[0]->setSide(0);
 	CL[1]->setStartX(4152); CL[1]->setStartY(500); CL[1]->setSide(1);
 	CL[2]->setStartX(4160); CL[2]->setStartY(548); CL[2]->setSide(1);
 	CL[3]->setStartX(4172); CL[3]->setStartY(387); CL[3]->setSide(0);
 	CL[4]->setStartX(4120); CL[4]->setStartY(226); CL[4]->setSide(1);
 	CL[5]->setStartX(4684); CL[5]->setStartY(300); CL[5]->setSide(1);
    CL[6]->setStartX(4712); CL[6]->setStartY(500); CL[6]->setSide(1);
    CL[7]->setStartX(4862); CL[7]->setStartY(566); CL[7]->setSide(0);
 	for(int i=0;i<N_CL;i++) CL[i]->resetObj(true);
 	// Demon
 	DE[0]->setStartX(580); DE[0]->setStartY(2176); DE[0]->resetObj(true);
 	DE[1]->setStartX(860); DE[1]->setStartY(2176); DE[1]->resetObj(true);

	// Immobile enemies
 	IMMOBILE[0]->setStartXYFrame(1720,120,72); IMMOBILE[0]->resetObj(true);
 	IMMOBILE[1]->setStartXYFrame(1850,140,72); IMMOBILE[1]->resetObj(true);

	// Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(260);  MOBILE[0]->setStartY(2166); MOBILE[0]->setDisX(0);   MOBILE[0]->setDisY(-42); MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(958);  MOBILE[1]->setStartY(2192); MOBILE[1]->setDisX(0);   MOBILE[1]->setDisY(-42); MOBILE[1]->setFGround(true);

    MOBILE[2]->setStartX(1728); MOBILE[2]->setStartY(195); MOBILE[2]->setDisX(-99); MOBILE[2]->setDisY(-36); MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(2990); MOBILE[3]->setStartY(195); MOBILE[3]->setDisX(-99); MOBILE[3]->setDisY(-36); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(3248); MOBILE[4]->setStartY(579); MOBILE[4]->setDisX(80);  MOBILE[4]->setDisY(-36); MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(4000); MOBILE[5]->setStartY(334); MOBILE[5]->setDisX(-60); MOBILE[5]->setDisY(-36); MOBILE[5]->setFGround(true);
    MOBILE[6]->setStartX(4576); MOBILE[6]->setStartY(342); MOBILE[6]->setDisX(150); MOBILE[6]->setDisY(-36); MOBILE[6]->setFGround(true);
    MOBILE[7]->setStartX(0);    MOBILE[7]->setStartY(0);   MOBILE[7]->setDisX(0);   MOBILE[7]->setDisY(0);  MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(0);    MOBILE[8]->setStartY(0);   MOBILE[8]->setDisX(0);   MOBILE[8]->setDisY(0);  MOBILE[8]->setFGround(true);
    MOBILE[9]->setStartX(0);    MOBILE[9]->setStartY(0);   MOBILE[9]->setDisX(0);   MOBILE[9]->setDisY(0);  MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(0);   MOBILE[10]->setStartY(0);  MOBILE[10]->setDisX(0);  MOBILE[10]->setDisY(0); MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(0);   MOBILE[11]->setStartY(0);  MOBILE[11]->setDisX(0);  MOBILE[11]->setDisY(0); MOBILE[11]->setFGround(true);

 	MOBILE[12]->setStartX(1460); MOBILE[12]->setStartY(2178); MOBILE[12]->setStartFrame(22); MOBILE[12]->setEndFrame(25); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
    MOBILE[13]->setStartX(1210); MOBILE[13]->setStartY(1800); MOBILE[13]->setMinY(1700);     MOBILE[13]->setMaxY(1930);   MOBILE[13]->setStartFrame(41); MOBILE[13]->setEndFrame(41); MOBILE[13]->setType(4); MOBILE[13]->setFGround(true);
    MOBILE[14]->setStartX(1040); MOBILE[14]->setStartY(1100); MOBILE[14]->setMinY(1010);     MOBILE[14]->setMaxY(1260);   MOBILE[14]->setStartFrame(41); MOBILE[14]->setEndFrame(41); MOBILE[14]->setType(4); MOBILE[14]->setFGround(true);
    MOBILE[15]->setStartX(994);  MOBILE[15]->setStartY(356);  MOBILE[15]->setStartFrame(40); MOBILE[15]->setEndFrame(40); MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
    MOBILE[16]->setStartX(1698); MOBILE[16]->setStartY(158);  MOBILE[16]->setStartFrame(22); MOBILE[16]->setEndFrame(40); MOBILE[16]->setType(0); MOBILE[16]->setFGround(true);
    MOBILE[17]->setStartX(1950); MOBILE[17]->setStartY(80);   MOBILE[17]->setMinY(60);       MOBILE[17]->setMaxY(140);    MOBILE[17]->setStartFrame(41); MOBILE[17]->setEndFrame(41); MOBILE[17]->setType(4); MOBILE[17]->setFGround(true);

 	MOBILE[18]->setStartX(1621); MOBILE[18]->setStartY(321); MOBILE[18]->setStartFrame(25); MOBILE[18]->setEndFrame(25); MOBILE[18]->setType(0); MOBILE[18]->setFGround(true);
    MOBILE[19]->setStartX(1621); MOBILE[19]->setStartY(273); MOBILE[19]->setStartFrame(25); MOBILE[19]->setEndFrame(25); MOBILE[19]->setType(0); MOBILE[19]->setFGround(true);
    MOBILE[20]->setStartX(1621); MOBILE[20]->setStartY(225); MOBILE[20]->setStartFrame(26); MOBILE[20]->setEndFrame(26); MOBILE[20]->setType(0); MOBILE[20]->setFGround(true);
 	MOBILE[21]->setStartX(2224); MOBILE[21]->setStartY(321); MOBILE[21]->setStartFrame(25); MOBILE[21]->setEndFrame(25); MOBILE[21]->setType(0); MOBILE[21]->setFGround(true);
    MOBILE[22]->setStartX(2224); MOBILE[22]->setStartY(273); MOBILE[22]->setStartFrame(25); MOBILE[22]->setEndFrame(25); MOBILE[22]->setType(0); MOBILE[22]->setFGround(true);
    MOBILE[23]->setStartX(2224); MOBILE[23]->setStartY(225); MOBILE[23]->setStartFrame(26); MOBILE[23]->setEndFrame(26); MOBILE[23]->setType(0); MOBILE[23]->setFGround(true);
 	MOBILE[24]->setStartX(2480); MOBILE[24]->setStartY(321); MOBILE[24]->setStartFrame(25); MOBILE[24]->setEndFrame(25); MOBILE[24]->setType(0); MOBILE[24]->setFGround(true);
    MOBILE[25]->setStartX(2480); MOBILE[25]->setStartY(273); MOBILE[25]->setStartFrame(25); MOBILE[25]->setEndFrame(25); MOBILE[25]->setType(0); MOBILE[25]->setFGround(true);
    MOBILE[26]->setStartX(2480); MOBILE[26]->setStartY(225); MOBILE[26]->setStartFrame(26); MOBILE[26]->setEndFrame(26); MOBILE[26]->setType(0); MOBILE[26]->setFGround(true);
    MOBILE[27]->setStartX(1632); MOBILE[27]->setStartY(481); MOBILE[27]->setStartFrame(27); MOBILE[27]->setEndFrame(27); MOBILE[27]->setType(0); MOBILE[27]->setFGround(true);
    MOBILE[28]->setStartX(1680); MOBILE[28]->setStartY(481); MOBILE[28]->setStartFrame(28); MOBILE[28]->setEndFrame(28); MOBILE[28]->setType(0); MOBILE[28]->setFGround(true);
    MOBILE[29]->setStartX(1728); MOBILE[29]->setStartY(481); MOBILE[29]->setStartFrame(29); MOBILE[29]->setEndFrame(29); MOBILE[29]->setType(0); MOBILE[29]->setFGround(true);

    for(int i=0;i<=1;i++)   MOBILE[i]->resetObj(true);
    for(int i=12;i<=17;i++) MOBILE[i]->resetObj(true);

 	// Other
 	FO_SFX->setStartX(1330);    FO_SFX->setStartY(0);      FO_SFX->resetSFX(true);
 	WA_SFX[0]->setStartX(0);    WA_SFX[0]->setStartY(426); WA_SFX[0]->resetSFX(true); WA_SFX[0]->setScrollSpeed(0); WA_SFX[0]->setWaveSpeed(-1); WA_SFX[0]->setWaterLen(18); WA_SFX[0]->setAlpha(200);
 	RA_SFX[0]->setStartX(3100); RA_SFX[0]->setStartY(516); RA_SFX[0]->resetSFX(true,0);
 	RA_SFX[1]->setStartX(3050); RA_SFX[1]->setStartY(516); RA_SFX[1]->resetSFX(true,1);
   }
  if(STAGE==15)
   {
    // Player position
    if(!CKPOINT) { RESTART_X=6000; RESTART_Y=607; MIN=2; SEC=30; RETIME=true;  }
    else         { RESTART_X=2894; RESTART_Y=628; MIN=2; SEC=30; RETIME=false; }
    PL->resetPlayer(RESTART_X,RESTART_Y); PL->setSide(1);
 	// Blade
 	BL[0]->setStartX(1895); BL[0]->setStartY(108+448); BL[0]->setMaxY(158+448); BL[0]->setCount(5);
 	BL[1]->setStartX(2087); BL[1]->setStartY(121+448); BL[1]->setMaxY(160+448); BL[1]->setCount(2);
    BL[2]->setStartX(2247); BL[2]->setStartY(121+448); BL[2]->setMaxY(160+448); BL[2]->setCount(7);
    BL[3]->setStartX(1863); BL[3]->setStartY(9+448);   BL[3]->setMaxY(60+448);  BL[3]->setCount(4);
    BL[4]->setStartX(2071); BL[4]->setStartY(-4+448);  BL[4]->setMaxY(48+448);  BL[4]->setCount(8);
    BL[5]->setStartX(2503); BL[5]->setStartY(121+448); BL[5]->setMaxY(160+448); BL[5]->setCount(8);
    BL[6]->setStartX(2663); BL[6]->setStartY(46+448);  BL[6]->setMaxY(126+448); BL[6]->setCount(3);
    for(int i=0;i<N_BL;i++) BL[i]->resetObj(true);
 	// Zombie
 	for(int i=0;i<N_ZO;i++) { ZO[i]->setStartX(5000); ZO[i]->setStartY(0); ZO[i]->resetObj(true); }
 	// Plant mk2
 	P2[0]->setStartX(5460); P2[0]->setStartY(611);
 	P2[1]->setStartX(5198); P2[1]->setStartY(505);
 	P2[2]->setStartX(4876); P2[2]->setStartY(505);
 	P2[3]->setStartX(4532); P2[3]->setStartY(613);
 	P2[4]->setStartX(4164); P2[4]->setStartY(537);
 	P2[5]->setStartX(3962); P2[5]->setStartY(613);
    P2[6]->setStartX(3780); P2[6]->setStartY(537);
 	for(int i=0;i<N_P2;i++) P2[i]->resetObj(true);
 	// Volture
 	VO[0]->setStartX(940);  VO[0]->setStartY(550);
 	for(int i=0;i<N_VO;i++) VO[i]->resetObj(true);
 	// Red arremer
 	RA[0]->setStartX(5356); RA[0]->setStartY(610);
 	RA[1]->setStartX(4372); RA[1]->setStartY(610);
 	RA[2]->setStartX(3194); RA[2]->setStartY(596);
 	for(int i=0;i<N_RA;i++) RA[i]->resetObj(true);
 	// Red arremer King
 	RK[0]->setStartX(60);   RK[0]->setStartY(606);
 	RK[0]->setExitKeyP(KEY);	 	
 	for(int i=0;i<N_RK;i++) RK[i]->resetObj(true);
 	// Pork
 	for(int i=0;i<2;i++) PO[i]->resetObj(true);
 	PO[0]->setMinX(4962); PO[0]->setMaxX(5406); *PO[0]->getPosXP()=5130; *PO[0]->getPosYP()=224;
 	PO[1]->setMinX(4590); PO[1]->setMaxX(5330); *PO[1]->getPosXP()=5000; *PO[1]->getPosYP()=350;
 	// Flower
 	FL[0]->setStartX(288);  FL[0]->setStartY(607); FL[0]->setMinX(120);  FL[0]->setMaxX(600);
 	FL[1]->setStartX(4596); FL[1]->setStartY(558); FL[1]->setMinX(4400); FL[1]->setMaxX(5244);
 	FL[2]->setStartX(4730); FL[2]->setStartY(525); FL[2]->setMinX(4400); FL[2]->setMaxX(5244);
 	for(int i=0;i<N_FL;i++) FL[i]->resetObj(true);

    // Mobile (da 0 a 11 Magic box)
    MOBILE[0]->setStartX(5896); MOBILE[0]->setStartY(614); MOBILE[0]->setDisX(-70); MOBILE[0]->setDisY(-56);  MOBILE[0]->setFGround(true);
    MOBILE[1]->setStartX(5030); MOBILE[1]->setStartY(614); MOBILE[1]->setDisX(-1);  MOBILE[1]->setDisY(-90);  MOBILE[1]->setFGround(true);

    MOBILE[2]->setStartX(1496); MOBILE[2]->setStartY(587); MOBILE[2]->setDisX(-1);  MOBILE[2]->setDisY(-35);  MOBILE[2]->setFGround(true);
    MOBILE[3]->setStartX(2218); MOBILE[3]->setStartY(608); MOBILE[3]->setDisX(-1);  MOBILE[3]->setDisY(-134); MOBILE[3]->setFGround(true);
    MOBILE[4]->setStartX(3100); MOBILE[4]->setStartY(607); MOBILE[4]->setDisX(10);  MOBILE[4]->setDisY(-72);  MOBILE[4]->setFGround(true);
    MOBILE[5]->setStartX(3800); MOBILE[5]->setStartY(608); MOBILE[5]->setDisX(96);  MOBILE[5]->setDisY(50);   MOBILE[5]->setFGround(true);
    MOBILE[6]->setStartX(4366); MOBILE[6]->setStartY(607); MOBILE[6]->setDisX(80);  MOBILE[6]->setDisY(-22);  MOBILE[6]->setFGround(true);
    MOBILE[7]->setStartX(5228); MOBILE[7]->setStartY(505); MOBILE[7]->setDisX(-1);  MOBILE[7]->setDisY(-22);  MOBILE[7]->setFGround(true);
    MOBILE[8]->setStartX(4560); MOBILE[8]->setStartY(450); MOBILE[8]->setDisX(-50); MOBILE[8]->setDisY(-36);  MOBILE[8]->setFGround(true);
    MOBILE[9]->setStartX(5042); MOBILE[9]->setStartY(230); MOBILE[9]->setDisX(-50); MOBILE[9]->setDisY(-36);  MOBILE[9]->setFGround(true);
    MOBILE[10]->setStartX(0);   MOBILE[10]->setStartY(0);  MOBILE[10]->setDisX(0);  MOBILE[10]->setDisY(0);   MOBILE[10]->setFGround(true);
    MOBILE[11]->setStartX(0);   MOBILE[11]->setStartY(0);  MOBILE[11]->setDisX(0);  MOBILE[11]->setDisY(0);   MOBILE[11]->setFGround(true);

    MOBILE[12]->setStartX(6080);  MOBILE[12]->setStartY(618); MOBILE[12]->setStartFrame(23); MOBILE[12]->setEndFrame(23); MOBILE[12]->setType(0); MOBILE[12]->setFGround(true);
    MOBILE[13]->setStartX(5820);  MOBILE[13]->setStartY(618); MOBILE[13]->setStartFrame(21); MOBILE[13]->setEndFrame(21); MOBILE[13]->setType(0); MOBILE[13]->setFGround(true);
    MOBILE[14]->setStartX(5658);  MOBILE[14]->setStartY(618); MOBILE[14]->setStartFrame(23); MOBILE[14]->setEndFrame(23); MOBILE[14]->setType(0); MOBILE[14]->setFGround(true);
    MOBILE[15]->setStartX(5410);  MOBILE[15]->setStartY(618); MOBILE[15]->setStartFrame(20); MOBILE[15]->setEndFrame(20); MOBILE[15]->setType(0); MOBILE[15]->setFGround(true);
    MOBILE[16]->setStartX(5180);  MOBILE[16]->setStartY(618); MOBILE[16]->setStartFrame(21); MOBILE[16]->setEndFrame(21); MOBILE[16]->setType(0); MOBILE[16]->setFGround(true);
    MOBILE[17]->setStartX(4950);  MOBILE[17]->setStartY(618); MOBILE[17]->setStartFrame(23); MOBILE[17]->setEndFrame(23); MOBILE[17]->setType(0); MOBILE[17]->setFGround(true);
    MOBILE[18]->setStartX(4620);  MOBILE[18]->setStartY(618); MOBILE[18]->setStartFrame(21); MOBILE[18]->setEndFrame(21); MOBILE[18]->setType(0); MOBILE[18]->setFGround(true);
    MOBILE[19]->setStartX(4420);  MOBILE[19]->setStartY(618); MOBILE[19]->setStartFrame(23); MOBILE[19]->setEndFrame(23); MOBILE[19]->setType(0); MOBILE[19]->setFGround(true);
    MOBILE[20]->setStartX(4112);  MOBILE[20]->setStartY(618); MOBILE[20]->setStartFrame(21); MOBILE[20]->setEndFrame(21); MOBILE[20]->setType(0); MOBILE[20]->setFGround(true);
    MOBILE[21]->setStartX(5260);  MOBILE[21]->setStartY(510); MOBILE[21]->setStartFrame(23); MOBILE[21]->setEndFrame(23); MOBILE[21]->setType(0); MOBILE[21]->setFGround(true);
    MOBILE[22]->setStartX(5080);  MOBILE[22]->setStartY(510); MOBILE[22]->setStartFrame(21); MOBILE[22]->setEndFrame(21); MOBILE[22]->setType(0); MOBILE[22]->setFGround(true);
    MOBILE[23]->setStartX(4760);  MOBILE[23]->setStartY(510); MOBILE[23]->setStartFrame(23); MOBILE[23]->setEndFrame(23); MOBILE[23]->setType(0); MOBILE[23]->setFGround(true);
    MOBILE[24]->setStartX(3900);  MOBILE[24]->setStartY(542); MOBILE[24]->setStartFrame(23); MOBILE[24]->setEndFrame(23); MOBILE[24]->setType(0); MOBILE[24]->setFGround(true);
    MOBILE[25]->setStartX(3476);  MOBILE[25]->setStartY(608); MOBILE[25]->setStartFrame(21); MOBILE[22]->setEndFrame(21); MOBILE[25]->setType(0); MOBILE[25]->setFGround(true);
    MOBILE[26]->setStartX(3150);  MOBILE[26]->setStartY(602); MOBILE[26]->setStartFrame(23); MOBILE[26]->setEndFrame(23); MOBILE[26]->setType(0); MOBILE[26]->setFGround(true);

    for(int i=0; i<=1;i++)  MOBILE[i]->resetObj(true);
    for(int i=12;i<=26;i++) MOBILE[i]->resetObj(true);

 	// Other
 	WA_SFX[0]->setStartX(0);    WA_SFX[0]->setStartY(655); WA_SFX[0]->resetSFX(true); WA_SFX[0]->setScrollSpeed(0); WA_SFX[0]->setWaveSpeed(-1); WA_SFX[0]->setWaterLen(21); WA_SFX[0]->setAlpha(200);
 	TH_SFX[0]->setStartX(100);  TH_SFX[0]->setStartY(448); TH_SFX[0]->resetSFX(true); TH_SFX[0]->setDeep(2); // 3 = lampo ravvicinato
 	TH_SFX[1]->setStartX(800);  TH_SFX[1]->setStartY(448); TH_SFX[1]->resetSFX(true); TH_SFX[1]->setDeep(2);
 	TH_SFX[2]->setStartX(1500); TH_SFX[2]->setStartY(448); TH_SFX[2]->resetSFX(true); TH_SFX[2]->setDeep(2);
 	TH_SFX[3]->setStartX(2200); TH_SFX[3]->setStartY(448); TH_SFX[3]->resetSFX(true); TH_SFX[3]->setDeep(2);
   }
 }

void restartStage(int stage)
 {
  int dress;
  bool key_taked,shild;

  if(MAX_STAGE < stage) MAX_STAGE = stage;
  key_taked=PL->getKeyTaked(); dress=PL->getDress(); shild=PL->getShild();
  resetObj();
  if(key_taked) { if(dress!=1 && dress!=3) PL->setDress(dress); PL->setShild(shild); }
  if(LIVES==0) { gameOver(); GAME_OVER=1; return; }
  READY_GO=READY_GO_TIME;
  if(!GAME_OVER) stageMap();
 }

void initObjects()
 {
  BITMAP *back;
  int txtcol;

  txtcol=makecol32(255,255,0);
  drawBar(0,txtcol,SCREENX);
  back=load_bmp("data/background.bmp",NULL);
  stretch_blit(back,screen,0,0,640,480,0,0,SCREENX,SCREENY);
  // LOADING OBJECTS ...
  textprintf_ex(screen,font,1,12*2,txtcol,-1,"Loading Objects ..."); drawBar(5,txtcol,SCREENX);
  // Player
  PL=new   player("data/obj/player/player",0,0,VSCR);
  PL->setKeyboardLayout(KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYJUMP,KEYSHOOT);
  PL->setNStage(N_STAGE);
  PL->setStageP(&STAGE);
  PL->setInterludeP(&INTERLUDE);
  PL->setEpilogueP(&EPILOGUE);
  PL->setLivesP(&LIVES);
  PL->setScoreP(&SCORE);
  PL->setCkPointP(&CKPOINT);
  // OSD
  OSD=new osd("data/obj/player/osd.bmp",0,0,VSCR);
  OSD->setLivesP(&LIVES); OSD->setScoreP(&SCORE); OSD->setMagicpwP(PL->getMagicpwP()); OSD->setMagicGaugeP(PL->getMagicGaugeP()); OSD->setMinSecP(&MIN,&SEC);
  PL->setOSD(OSD);
  // Blades
  BL[0]=new blade("data/obj/enemies/blade",0,0,0,0,VSCR);
  for(int i=1;i<N_BL-1;i++) BL[i]=new blade("",0,0,0,0,VSCR,0,i,BL[0]);
  BL[6]=new blade("",0,0,0,0,VSCR,1,6,BL[0]);
  // Rockfall
  RF[0]=new rockfall("data/obj/enemies/rockfall",0,0,0,0,VSCR);
  for(int i=1;i<N_RF;i++) RF[i]=new rockfall("",0,0,0,0,VSCR,0,i,RF[0]);
  // Skeleton
  SK[0]=new skeleton("data/obj/enemies/skeleton",0,0,VSCR);
  for(int i=1;i<N_SK;i++) SK[i]=new skeleton("",0,0,VSCR,i,SK[0]);
  // Zombie
  ZO[0]=new zombie("data/obj/enemies/zombie",0,0,VSCR);
  for(int i=1;i<N_ZO;i++) ZO[i]=new zombie("",0,0,VSCR,i,ZO[0]);
  // Volture
  VO[0]=new volture("data/obj/enemies/volture",0,0,VSCR);
  for(int i=1;i<N_VO;i++) VO[i]=new volture("",0,0,VSCR,i,VO[0]);
  // Red arremer
  RA[0]=new red_arremer("data/obj/enemies/red_arremer",0,0,VSCR);
  for(int i=1;i<N_RA;i++) RA[i]=new red_arremer("",0,0,VSCR,i,RA[0]);
  for(int i=0;i<N_RA;i++) { RA[i]->setCtrlXP(PL->getGlobXP()); RA[i]->setCtrlYP(PL->getGlobYP()); }
  // Red arremer King
  RK[0]=new red_arremer_king("data/obj/enemies/red_arremer_king",0,0,VSCR);
  for(int i=0;i<N_RK;i++) { RK[i]->setCtrlXP(PL->getGlobXP()); RK[i]->setCtrlYP(PL->getGlobYP()); }
  // Plant mk2
  P2[0]=new plant_mk2("data/obj/enemies/plant_mk2",0,0,VSCR);
  for(int i=1;i<N_P2;i++) P2[i]=new plant_mk2("",0,0,VSCR,i,P2[0]);
  for(int i=0;i<N_P2;i++) { P2[i]->setCtrlXP(PL->getGlobXP()); P2[i]->setCtrlYP(PL->getGlobYP()); }
  // Stalagmite
  ST[0]=new stalagmite("data/obj/enemies/stalagmite",0,0,VSCR);
  for(int i=1;i<N_ST;i++) ST[i]=new stalagmite("",0,0,VSCR,i,ST[0]);
  for(int i=0;i<N_ST;i++) { ST[i]->setCtrlXP(PL->getGlobXP()); ST[i]->setCtrlYP(PL->getGlobYP()); }
  // Ghost
  GO[0]=new ghost("data/obj/enemies/ghost",0,0,VSCR);
  for(int i=1;i<N_GO;i++) GO[i]=new ghost("",0,0,VSCR,i,GO[0]);
  for(int i=0;i<N_GO;i++) { GO[i]->setCtrlXP(PL->getGlobXP()); GO[i]->setCtrlYP(PL->getGlobYP()); }
  // Bat
  BA[0]=new bat("data/obj/enemies/bat",0,0,VSCR);
  for(int i=1;i<N_BA;i++) BA[i]=new bat("",0,0,VSCR,i,BA[0]);
  for(int i=0;i<N_BA;i++) { BA[i]->setCtrlXP(PL->getGlobXP()); BA[i]->setCtrlYP(PL->getGlobYP()); }
  // Giant
  GI[0]=new giant("data/obj/enemies/giant",0,0,VSCR);
  for(int i=1;i<N_GI;i++) GI[i]=new giant("",0,0,VSCR,i,GI[0]);
  for(int i=0;i<N_GI;i++) { GI[i]->setCtrlXP(PL->getGlobXP()); GI[i]->setCtrlYP(PL->getGlobYP()); }
  // Firewall
  FW[0]=new firewall("data/obj/enemies/firewall",0,0,0,0,VSCR);
  for(int i=1;i<N_FW;i++) FW[i]=new firewall("",0,0,0,0,VSCR,i,FW[0]);
  // Insect
  IN[0]=new insect("data/obj/enemies/insect",0,0,VSCR);
  for(int i=1;i<N_IN;i++) IN[i]=new insect("",0,0,VSCR,i,IN[0]);
  for(int i=0;i<N_IN;i++) { IN[i]->setCtrlXP(PL->getGlobXP()); IN[i]->setCtrlYP(PL->getGlobYP()); }
  // Fire grizzly
  FG[0]=new fire_grizzly("data/obj/enemies/fire_grizzly",0,0,VSCR);
  for(int i=1;i<N_FG;i++) FG[i]=new fire_grizzly("",0,0,VSCR,i,FG[0]);
  for(int i=0;i<N_FG;i++) { FG[i]->setCtrlXP(PL->getGlobXP()); FG[i]->setCtrlYP(PL->getGlobYP()); }
  // Clam
  CL[0]=new clam("data/obj/enemies/clam",0,0,VSCR);
  for(int i=1;i<N_CL;i++) CL[i]=new clam("",0,0,VSCR,i,CL[0]);
  for(int i=0;i<N_CL;i++) { CL[i]->setCtrlXP(PL->getGlobXP()); CL[i]->setCtrlYP(PL->getGlobYP()); }
  // Turtle
  TU[0]=new turtle("data/obj/enemies/turtle",0,0,VSCR);
  for(int i=1;i<N_TU;i++) TU[i]=new turtle("",0,0,VSCR,i,TU[0]);
  for(int i=0;i<N_TU;i++) { TU[i]->setCtrlXP(PL->getGlobXP()); TU[i]->setCtrlYP(PL->getGlobYP()); }
  // Demon
  DE[0]=new demon("data/obj/enemies/demon",0,0,VSCR);
  for(int i=1;i<N_DE;i++) DE[i]=new demon("",0,0,VSCR,i,DE[0]);
  for(int i=0;i<N_DE;i++) { DE[i]->setCtrlXP(PL->getGlobXP()); DE[i]->setCtrlYP(PL->getGlobYP()); }
  // Little devils
  LD[0]=new little_devil("data/obj/enemies/little_devil",0,0,VSCR);
  for(int i=1;i<N_LD;i++) LD[i]=new little_devil("",0,0,VSCR,i,LD[0]);
  for(int i=0;i<N_LD;i++) { LD[i]->setCtrlXP(PL->getGlobXP()); LD[i]->setCtrlYP(PL->getGlobYP()); }
  // Big crab
  BC[0]=new big_crab("data/obj/enemies/big_crab",0,0,VSCR);
  for(int i=1;i<N_BC;i++) BC[i]=new big_crab("",0,0,VSCR,i,BC[0]);
  for(int i=0;i<N_BC;i++) { BC[i]->setCtrlXP(PL->getGlobXP()); BC[i]->setCtrlYP(PL->getGlobYP()); }
  // Pork
  PO[0]=new pork("data/obj/enemies/pork",0,0,VSCR);
  for(int i=1;i<N_PO;i++) PO[i]=new pork("",0,0,VSCR,i,PO[0]);
  for(int i=0;i<N_PO;i++) { PO[i]->setCtrlXP(PL->getGlobXP()); PO[i]->setCtrlYP(PL->getGlobYP()); }
  // Magician
  MA[0]=new magician("data/obj/enemies/magician",0,0,VSCR);
  for(int i=1;i<N_MA;i++) MA[i]=new magician("",0,0,VSCR,i,MA[0]);
  for(int i=0;i<N_MA;i++) { MA[i]->setCtrlXP(PL->getGlobXP()); MA[i]->setCtrlYP(PL->getGlobYP()); }
  // Flower
  FL[0]=new flower("data/obj/enemies/flower",0,0,VSCR);
  for(int i=1;i<N_FL;i++) FL[i]=new flower("",0,0,VSCR,i,FL[0]);
  for(int i=0;i<N_FL;i++) { FL[i]->setCtrlXP(PL->getGlobXP()); FL[i]->setCtrlYP(PL->getGlobYP()); }
  // Immobile enemies
  IMMOBILE[0]=new immobile("data/obj/enemies/immobile",0,0,0,0,VSCR);
  for(int i=1;i<N_IMMOBILE;i++) IMMOBILE[i]=new immobile("",0,0,0,0,VSCR,0,i,IMMOBILE[0]);

  // Mobile (da 0 a 11 Magic box)
  MOBILE=(mobile **)malloc(sizeof(mobile *)*N_PLAT);
  MOBILE[0] =new mobile("data/obj/mobile/mobile.bmp","data/obj/mobile/mobile.dat",0,0,44,49,2,VSCR);
  MOBILE[1] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,1, MOBILE[0]);
  MOBILE[2] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,2, MOBILE[0]);
  MOBILE[3] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,3, MOBILE[0]);
  MOBILE[4] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,4, MOBILE[0]);
  MOBILE[5] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,5, MOBILE[0]);
  MOBILE[6] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,6, MOBILE[0]);
  MOBILE[7] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,7, MOBILE[0]);
  MOBILE[8] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,8, MOBILE[0]);
  MOBILE[9] =new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,9, MOBILE[0]);
  MOBILE[10]=new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,10,MOBILE[0]);
  MOBILE[11]=new mobile("","data/obj/mobile/mobile.dat",0,0,00,00,2,VSCR,11,MOBILE[0]);
  MOBILE[12]=new mobile("","data/obj/mobile/mobile.dat",0,0,01,01,0,VSCR,12,MOBILE[0]);
  MOBILE[13]=new mobile("","data/obj/mobile/mobile.dat",0,0,01,01,0,VSCR,13,MOBILE[0]);
  MOBILE[14]=new mobile("","data/obj/mobile/mobile.dat",0,0,02,02,0,VSCR,14,MOBILE[0]);
  MOBILE[15]=new mobile("","data/obj/mobile/mobile.dat",0,0,01,01,0,VSCR,15,MOBILE[0]);
  MOBILE[16]=new mobile("","data/obj/mobile/mobile.dat",0,0,03,04,1,VSCR,16,MOBILE[0]);
  MOBILE[17]=new mobile("","data/obj/mobile/mobile.dat",0,0,03,04,1,VSCR,17,MOBILE[0]);
  MOBILE[18]=new mobile("","data/obj/mobile/mobile.dat",0,0,03,04,1,VSCR,18,MOBILE[0]);
  MOBILE[19]=new mobile("","data/obj/mobile/mobile.dat",0,0,05,05,0,VSCR,19,MOBILE[0]);
  MOBILE[20]=new mobile("","data/obj/mobile/mobile.dat",0,0,05,05,0,VSCR,20,MOBILE[0]);
  MOBILE[21]=new mobile("","data/obj/mobile/mobile.dat",0,0,05,05,0,VSCR,21,MOBILE[0]);
  MOBILE[22]=new mobile("","data/obj/mobile/mobile.dat",0,0,23,23,0,VSCR,22,MOBILE[0]);
  MOBILE[23]=new mobile("","data/obj/mobile/mobile.dat",0,0,23,23,0,VSCR,23,MOBILE[0]);
  MOBILE[24]=new mobile("","data/obj/mobile/mobile.dat",0,0,24,24,0,VSCR,24,MOBILE[0]);
  MOBILE[25]=new mobile("","data/obj/mobile/mobile.dat",0,0,22,22,0,VSCR,25,MOBILE[0]);
  MOBILE[26]=new mobile("","data/obj/mobile/mobile.dat",0,0,22,22,0,VSCR,26,MOBILE[0]);
  MOBILE[27]=new mobile("","data/obj/mobile/mobile.dat",0,0,25,25,0,VSCR,27,MOBILE[0]);
  MOBILE[28]=new mobile("","data/obj/mobile/mobile.dat",0,0,25,25,0,VSCR,28,MOBILE[0]);
  MOBILE[29]=new mobile("","data/obj/mobile/mobile.dat",0,0,26,26,0,VSCR,29,MOBILE[0]);
  MOBILE[30]=new mobile("","data/obj/mobile/mobile.dat",0,0,27,27,0,VSCR,30,MOBILE[0]);
  MOBILE[31]=new mobile("","data/obj/mobile/mobile.dat",0,0,28,28,0,VSCR,31,MOBILE[0]);
  for(int i=0;i<=11;i++) { MOBILE[i]->setBonusSeqP(&BONUS_SEQ); MOBILE[i]->setDifficultyP(&DIFFICULTY); MOBILE[i]->setCtrlXP(PL->getGlobXP()); MOBILE[i]->setCtrlYP(PL->getGlobYP()); }
  // Bonus for magic box and bonus vase
  BONUSB[0]=new bonus("data/obj/player/bonus.bmp",0,0,VSCR);
  for(int i=1;i<N_BBOX;i++)   BONUSB[i]=new bonus("",0,0,VSCR,0,i,BONUSB[0]);
  for(int i=0;i<N_BVASE;i++)  BVASE[i] =new bonus("",0,0,VSCR,0,1,BONUSB[0]); // Bonus vase
  for(int i=0;i<N_BVASE;i+=2) BVASE[i]->setInvaseP(BVASE[i+1]); // Pointer to bonus inside the vase
  KEY   = new bonus("",0,0,VSCR,0,1,BONUSB[0]); // Key (next level)
  SHILD = new bonus("",0,0,VSCR,0,1,BONUSB[0]); // Shild (when armour hitted shild is missed)
  // Particle
  PA_SFX[0]=new particle("data/sfx/particle.bmp",0,0,VSCR);
  for(int i=1;i<N_PARFX;i++) PA_SFX[i]=new particle("",0,0,VSCR,i,PA_SFX[0]);

  // LOADING GRAPHIC ...
  textprintf_ex(screen,font,1,12*3,txtcol,-1,"Loading Graphic ..."); drawBar(40,txtcol,SCREENX);
  char s1[255],s2[255],s3[255],s4[255],s5[255],s6[255],s7[255],lev[255];
  for(int i=0;i<N_STAGE;i++)
   {
	if(i<=7 || i>=15)
	 {
      sprintf(lev,"data/maps/l%d",i+1);
      sprintf(s1,"%s/map0.bmp",lev);
      sprintf(s2,"%s/0.map",lev);
      MAP0[i]=new map(s1,s2,VSCR);
      MAP0[i]->setCtrlXP(PL->getScrollXP()); MAP0[i]->setCtrlYP(PL->getScrollYP());
      MAP0[i]->setSpeedX(1); MAP0[i]->setSpeedY(1);
      sprintf(s1,"%s/map1.bmp",lev);
      sprintf(s2,"%s/1.map",lev);
      sprintf(s3,"%s/platform.map",lev);
      sprintf(s4,"%s/platform.dat",lev);
      sprintf(s5,"%s/stair.map",lev);
      sprintf(s6,"%s/wall.map",lev);
      sprintf(s7,"%s/wall.dat",lev);
      MAP1[i]=new map(s1,s2,VSCR,false,s3,s4,s5,s6,s7,true);
      MAP1[i]->setCtrlXP(PL->getScrollXP()); MAP1[i]->setCtrlYP(PL->getScrollYP());
      MAP1[i]->setSpeedX(2); MAP1[i]->setSpeedY(2);
      MAP1[i]->setMobileP(MOBILE,N_PLAT);
      sprintf(s1,"%s/maph.bmp",lev);
      sprintf(s2,"%s/h.map",lev);
      MAPH[i]=new map(s1,s2,VSCR,true);
      MAPH[i]->setCtrlXP(PL->getScrollXP()); MAPH[i]->setCtrlYP(PL->getScrollYP());
      MAPH[i]->setSpeedX(2); MAPH[i]->setSpeedY(2);
     }
    if(i==8)  { MAP0[i] = MAP0[6]; MAP1[i] = MAP1[6]; MAPH[i] = MAPH[6]; }
    if(i==9)  { MAP0[i] = MAP0[5]; MAP1[i] = MAP1[5]; MAPH[i] = MAPH[5]; }
    if(i==10) { MAP0[i] = MAP0[4]; MAP1[i] = MAP1[4]; MAPH[i] = MAPH[4]; }
    if(i==11) { MAP0[i] = MAP0[3]; MAP1[i] = MAP1[3]; MAPH[i] = MAPH[3]; }
    if(i==12) { MAP0[i] = MAP0[2]; MAP1[i] = MAP1[2]; MAPH[i] = MAPH[2]; }
    if(i==13) { MAP0[i] = MAP0[1]; MAP1[i] = MAP1[1]; MAPH[i] = MAPH[1]; }
    if(i==14) { MAP0[i] = MAP0[0]; MAP1[i] = MAP1[0]; MAPH[i] = MAPH[0]; }
   }

  // LOADING SPECIAL FX ...
  textprintf_ex(screen,font,1,12*4,txtcol,-1,"Loading Special FX ..."); drawBar(80,txtcol,SCREENX);
  FL_SFX=new flash(4802+768,520,VSCR);
  FL_SFX->loadSample("data/sound/fx/thunder.wav",0);
  FL_SFX->setCtrlXP(PL->getGlobXP()); FL_SFX->setCtrlYP(PL->getGlobYP()); 
  FO_SFX=new fog(2300,448,VSCR);
  FO_SFX->setCtrlXP(PL->getGlobXP()); FO_SFX->setCtrlYP(PL->getGlobYP()); 
  FI_SFX=new fire(2300,448,VSCR);
  FI_SFX->setCtrlXP(PL->getGlobXP()); FI_SFX->setCtrlYP(PL->getGlobYP()); 
  WA_SFX[0]=new water(2300,448,VSCR);
  WA_SFX[0]->setCtrlXP(PL->getGlobXP()); WA_SFX[0]->setCtrlYP(PL->getGlobYP()); 
  WA_SFX[1]=new water(2300,448,VSCR);
  WA_SFX[1]->setCtrlXP(PL->getGlobXP()); WA_SFX[1]->setCtrlYP(PL->getGlobYP()); 
  RA_SFX[0]=new rain(2300,400,VSCR);
  RA_SFX[0]->setCtrlXP(PL->getGlobXP()); RA_SFX[0]->setCtrlYP(PL->getGlobYP()); 
  RA_SFX[1]=new rain(2300,80,VSCR);
  RA_SFX[1]->setCtrlXP(PL->getGlobXP()); RA_SFX[1]->setCtrlYP(PL->getGlobYP()); 
  SN_SFX[0]=new snow(2300,500,VSCR);
  SN_SFX[0]->setCtrlXP(PL->getGlobXP()); SN_SFX[0]->setCtrlYP(PL->getGlobYP()); 
  SN_SFX[1]=new snow(2300,200,VSCR);
  SN_SFX[1]->setCtrlXP(PL->getGlobXP()); SN_SFX[1]->setCtrlYP(PL->getGlobYP()); 
  TH_SFX[0]=new thunder(2300,448,VSCR);
  TH_SFX[0]->setCtrlXP(PL->getGlobXP()); TH_SFX[0]->setCtrlYP(PL->getGlobYP()); 
  TH_SFX[1]=new thunder(2300,448,VSCR);
  TH_SFX[1]->setCtrlXP(PL->getGlobXP()); TH_SFX[1]->setCtrlYP(PL->getGlobYP()); 
  TH_SFX[2]=new thunder(2300,448,VSCR);
  TH_SFX[2]->setCtrlXP(PL->getGlobXP()); TH_SFX[2]->setCtrlYP(PL->getGlobYP());
  TH_SFX[3]=new thunder(2300,448,VSCR);
  TH_SFX[3]->setCtrlXP(PL->getGlobXP()); TH_SFX[3]->setCtrlYP(PL->getGlobYP());

  // LOADING SOUNDS ...
  textprintf_ex(screen,font,1,12*5,txtcol,-1,"Loading Sounds ..."); drawBar(90,txtcol,SCREENX);
  PL->loadSample("data/sound/fx/gauge_down.wav",0);
  PL->loadSample("data/sound/fx/land.wav"      ,1);
  PL->loadSample("data/sound/fx/armour01.wav"  ,2);
  PL->loadSample("data/sound/fx/dead.wav"      ,3);
  PL->loadSample("data/sound/fx/armour02.wav"  ,4);
  PL->loadSample("data/sound/fx/shild01.wav"   ,5);
  PL->loadSample("data/sound/fx/bonus.wav"     ,6);
  PL->loadSample("data/sound/fx/gauge_up.wav"  ,7);
  PL->loadSample("data/sound/fx/throw01.wav"   ,8);
  PL->loadSample("data/sound/fx/throw02.wav"   ,9);
  PL->loadSample("data/sound/fx/throw01.wav"   ,10); // TODO: effetto AXE
  PL->loadSample("data/sound/fx/throw04.wav"   ,11);
  PL->loadSample("data/sound/fx/throw01.wav"   ,12); // TODO: effetto ARROW
  PL->loadSample("data/sound/fx/throw01.wav"   ,13); // TODO: effetto FLAME

  // Skeleton
  SK[0]->loadSample("data/sound/fx/rise01.wav",0);
  SK[0]->loadSample("data/sound/fx/kill01.wav",1);
  for(int i=1;i<N_SK;i++) { SK[i]->assignSample(0,SK[0]); SK[i]->assignSample(1,SK[0]); }
  // Zombie
  ZO[0]->loadSample("data/sound/fx/rise02.wav",0);
  ZO[0]->loadSample("data/sound/fx/kill02.wav",1);
  for(int i=1;i<N_ZO;i++) { ZO[i]->assignSample(0,ZO[0]); ZO[i]->assignSample(1,ZO[0]); }
  // Volture
  VO[0]->loadSample("data/sound/fx/hit02.wav",0);
  VO[0]->loadSample("data/sound/fx/kill04.wav",1);
  for(int i=1;i<N_VO;i++) { VO[i]->assignSample(0,VO[0]); VO[i]->assignSample(1,VO[0]); }
  // Blades
  BL[0]->loadSample("data/sound/fx/blade.wav",0);
  for(int i=1;i<N_BL;i++) BL[i]->assignSample(0,BL[0]);
  // Red arremer
  RA[0]->loadSample("data/sound/fx/hit01.wav",0);
  RA[0]->loadSample("data/sound/fx/kill03.wav",1);
  for(int i=1;i<N_RA;i++) { RA[i]->assignSample(0,RA[0]); RA[i]->assignSample(1,RA[0]); }
  // Red arremer King
  RK[0]->loadSample("data/sound/fx/hit02.wav",0);
  RK[0]->loadSample("data/sound/fx/kill04.wav",1);
  for(int i=1;i<N_RK;i++) { RK[i]->assignSample(0,RK[0]); RK[i]->assignSample(1,RK[0]); }
  // Plant mk2
  P2[0]->loadSample("data/sound/fx/hit02.wav",0);
  P2[0]->loadSample("data/sound/fx/kill04.wav",1);
  for(int i=1;i<N_P2;i++) { P2[i]->assignSample(0,P2[0]); P2[i]->assignSample(1,P2[0]); } 
  // Stalagmite
  ST[0]->loadSample("data/sound/fx/hit02.wav",0);
  ST[0]->loadSample("data/sound/fx/kill04.wav",1);
  ST[0]->loadSample("data/sound/fx/shoot01.wav",2);
  for(int i=1;i<N_ST;i++) { ST[i]->assignSample(0,ST[0]); ST[i]->assignSample(1,ST[0]); ST[i]->assignSample(2,ST[0]); }
  // Ghost
  GO[0]->loadSample("data/sound/fx/hit02.wav",0);
  GO[0]->loadSample("data/sound/fx/kill04.wav",1);
  for(int i=1;i<N_GO;i++) { GO[i]->assignSample(0,GO[0]); GO[i]->assignSample(1,GO[0]); } 
  // Bat
  BA[0]->loadSample("data/sound/fx/hit02.wav",0);
  BA[0]->loadSample("data/sound/fx/kill02.wav",1);
  for(int i=1;i<N_BA;i++) { BA[i]->assignSample(0,BA[0]); BA[i]->assignSample(1,BA[0]); } 
  // Giant
  GI[0]->loadSample("data/sound/fx/hit01.wav",0);
  GI[0]->loadSample("data/sound/fx/kill03.wav",1);
  GI[0]->loadSample("data/sound/fx/step.wav",2);
  for(int i=1;i<N_GI;i++) { GI[i]->assignSample(0,GI[0]); GI[i]->assignSample(1,GI[0]); GI[i]->assignSample(2,GI[0]); } 
  // Firewall
  FW[0]->loadSample("data/sound/fx/fireup.wav",0);
  for(int i=1;i<N_FW;i++) FW[i]->assignSample(0,FW[0]);
  // Insect
  IN[0]->loadSample("data/sound/fx/hit02.wav",0);
  IN[0]->loadSample("data/sound/fx/kill04.wav",1);
  for(int i=1;i<N_IN;i++) { IN[i]->assignSample(0,IN[0]); IN[i]->assignSample(1,IN[0]); } 
  // Fire grizzly
  FG[0]->loadSample("data/sound/fx/hit01.wav",0);
  FG[0]->loadSample("data/sound/fx/kill03.wav",1);
  FG[0]->loadSample("data/sound/fx/step.wav",2);
  for(int i=1;i<N_FG;i++) { FG[i]->assignSample(0,FG[0]); FG[i]->assignSample(1,FG[0]); FG[i]->assignSample(2,FG[0]); } 
  // Clam
  CL[0]->loadSample("data/sound/fx/hit02.wav",0);
  CL[0]->loadSample("data/sound/fx/kill04.wav",1);
  for(int i=1;i<N_CL;i++) { CL[i]->assignSample(0,CL[0]); CL[i]->assignSample(1,CL[0]); } 
  // Turtle
  TU[0]->loadSample("data/sound/fx/hit02.wav",0);
  TU[0]->loadSample("data/sound/fx/kill04.wav",1);
  TU[0]->loadSample("data/sound/fx/boing.wav",2);
  for(int i=1;i<N_TU;i++) { TU[i]->assignSample(0,TU[0]); TU[i]->assignSample(1,TU[0]); TU[i]->assignSample(2,TU[0]); }
  // Demon
  DE[0]->loadSample("data/sound/fx/hit01.wav",0);
  DE[0]->loadSample("data/sound/fx/kill03.wav",1);
  DE[0]->loadSample("data/sound/fx/boing.wav",2);
  for(int i=1;i<N_DE;i++) { DE[i]->assignSample(0,DE[0]); DE[i]->assignSample(1,DE[0]); DE[i]->assignSample(2,DE[0]); }
  // Little devil
  LD[0]->loadSample("data/sound/fx/hit02.wav",0);
  LD[0]->loadSample("data/sound/fx/kill04.wav",1);
  LD[0]->loadSample("data/sound/fx/boing.wav",2);
  for(int i=1;i<N_LD;i++) { LD[i]->assignSample(0,LD[0]); LD[i]->assignSample(1,LD[0]); LD[i]->assignSample(2,LD[0]); }
  // Big crab
  BC[0]->loadSample("data/sound/fx/hit02.wav",0);
  BC[0]->loadSample("data/sound/fx/kill04.wav",1);
  BC[0]->loadSample("data/sound/fx/boing.wav",2);
  for(int i=1;i<N_BC;i++) { BC[i]->assignSample(0,BC[0]); BC[i]->assignSample(1,BC[0]); BC[i]->assignSample(2,BC[0]); }
  // Pork
  PO[0]->loadSample("data/sound/fx/hit02.wav",0);
  PO[0]->loadSample("data/sound/fx/kill04.wav",1);
  PO[0]->loadSample("data/sound/fx/boing.wav",2);
  for(int i=1;i<N_PO;i++) { PO[i]->assignSample(0,PO[0]); PO[i]->assignSample(1,PO[0]); PO[i]->assignSample(2,PO[0]); }
  // Magician
  MA[0]->loadSample("data/sound/fx/hit02.wav",0);
  MA[0]->loadSample("data/sound/fx/kill04.wav",1);
  MA[0]->loadSample("data/sound/fx/boing.wav",2);
  for(int i=1;i<N_MA;i++) { MA[i]->assignSample(0,MA[0]); MA[i]->assignSample(1,MA[0]); MA[i]->assignSample(2,MA[0]); }
  // Flower
  FL[0]->loadSample("data/sound/fx/hit02.wav",0);
  FL[0]->loadSample("data/sound/fx/kill04.wav",1);
  FL[0]->loadSample("data/sound/fx/boing.wav",2);
  for(int i=1;i<N_FL;i++) { FL[i]->assignSample(0,FL[0]); FL[i]->assignSample(1,FL[0]); FL[i]->assignSample(2,FL[0]); }

  // Bonus for magic box and bonus vase
  BONUSB[0]->loadSample("data/sound/fx/blade.wav",0);
  for(int i=1;i<N_BBOX;i++)  BONUSB[i]->assignSample(0,BONUSB[0]);
  for(int i=0;i<N_BVASE;i++) BVASE[i]->assignSample(0,BONUSB[0]);
  KEY->assignSample(0,BONUSB[0]);
  SHILD->assignSample(0,BONUSB[0]);

  music[0]=load_midi("data/sound/music/l1.mid");
  music[1]=load_midi("data/sound/music/l2.mid");
  music[2]=load_midi("data/sound/music/l3.mid");
  music[3]=load_midi("data/sound/music/l4.mid");
  music[4]=load_midi("data/sound/music/l5.mid");
  music[5]=load_midi("data/sound/music/l6.mid");
  music[6]=load_midi("data/sound/music/l7.mid");
  music[7]=load_midi("data/sound/music/l8.mid");
  music[8] =music[6];
  music[9] =music[5];
  music[10]=music[4];
  music[11]=music[3];
  music[12]=music[2];
  music[13]=music[1];
  music[14]=music[0];
  music[15]=load_midi("data/sound/music/l16.mid");

  drawBar(100,txtcol,SCREENX);
  destroy_bitmap(back);
 }

void readConf(int *screenres, int *scanlines, bool *fscreen, bool *stretch,bool *debug,int *keyleft,int *keyright,int *keyup,int *keydown,int *keyjump,int *keyshoot,int *music_volume,int *sound_volume)
 {
  FILE *fd;
  char dummy[50];
  int aux;

  fd=fopen(CFGFILE,"r");
  if(fd)
   {
    // Read video options
    fscanf(fd,"%s%d",dummy,screenres);
    fscanf(fd,"%s%d",dummy,scanlines);
    fscanf(fd,"%s%d",dummy,&aux); if(aux==0) *fscreen=false; else *fscreen=true;
    fscanf(fd,"%s%d",dummy,&aux); if(aux==0) *stretch=false; else *stretch=true;
    // Read debug options
    fscanf(fd,"%s%d",dummy,&aux); if(aux==0) *debug=false;   else *debug=true;
    // Read keyboard layout
    fscanf(fd,"%s%d",dummy,keyleft);
    fscanf(fd,"%s%d",dummy,keyright);
    fscanf(fd,"%s%d",dummy,keyup);
    fscanf(fd,"%s%d",dummy,keydown);
    fscanf(fd,"%s%d",dummy,keyjump);
    fscanf(fd,"%s%d",dummy,keyshoot);
    // Read audio options
    fscanf(fd,"%s%d",dummy,music_volume);
    fscanf(fd,"%s%d",dummy,sound_volume);
 	fclose(fd);
   }
  else writeConf(*screenres,*scanlines,*fscreen,*stretch,*debug,*keyleft,*keyright,*keyup,*keydown,*keyjump,*keyshoot,*music_volume,*sound_volume);
 }

void writeConf(int screenres, int scanlines, bool fscreen, bool stretch, bool debug,int keyleft,int keyright,int keyup,int keydown,int keyjump,int keyshoot,int music_volume,int sound_volume)
 {
  FILE *fd;

  fd=fopen(CFGFILE,"w");
  // Write video options
  fprintf(fd,"[screenres]\n%d"  ,screenres);
  fprintf(fd,"\n[scanlines]\n%d",scanlines);
  fprintf(fd,"\n[fscreen]\n%d"  ,fscreen);
  fprintf(fd,"\n[stretch]\n%d"  ,stretch);
  // Write debug options
  fprintf(fd,"\n[debug]\n%d"    ,debug);
  // Write keyboard layout
  fprintf(fd,"\n[keyleft]\n%d" ,keyleft);
  fprintf(fd,"\n[keyright]\n%d",keyright);
  fprintf(fd,"\n[keyup]\n%d"   ,keyup);
  fprintf(fd,"\n[keydown]\n%d" ,keydown);
  fprintf(fd,"\n[keyjump]\n%d" ,keyjump);
  fprintf(fd,"\n[keyshoot]\n%d",keyshoot);
  // Write audio options
  fprintf(fd,"\n[music_volume]\n%d",music_volume);
  fprintf(fd,"\n[sound_fx_volume]\n%d",sound_volume);
  fclose(fd);
 }

void checkGame()
 {
  if(PL->getReadyGo()) restartStage(STAGE);
  if(STAGE==0)
   {
    if(PL->getGlobX()<3600+768)
 	 {
 	  // Vscroll off
 	  PL->setScrollCenterY(2);
 	  MAP1[0]->setVScroll(false,448);
 	  MAP0[0]->setVScroll(false,448/2);
 	  MAPH[0]->setVScroll(false,448);
 	 }
 	else
 	 {
 	  // Vscroll on
 	  PL->setScrollCenterY(106);
 	  MAP1[0]->setVScroll(true);
 	  MAP0[0]->setVScroll(true);
 	  MAPH[0]->setVScroll(true);
 	 }
   }
  if(STAGE==1)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==2)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==3)
   {
  	if(PL->getGlobX()<3600)
  	 {
      // Vscroll off
 	  PL->setScrollCenterY(2);
 	  MAP1[0]->setVScroll(false,448);
 	  MAP0[0]->setVScroll(false,448/2);
 	  MAPH[0]->setVScroll(false,448);
 	 }
 	else
 	 {
 	  // Vscroll on
 	  PL->setScrollCenterY(106);
 	  MAP1[0]->setVScroll(true);
 	  MAP0[0]->setVScroll(true);
 	  MAPH[0]->setVScroll(true);
 	 }
   }
  if(STAGE==4)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==5)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==6)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==7)
   {
    // Vscroll off
    PL->setScrollCenterY(2);
    MAP1[0]->setVScroll(false,448);
    MAP0[0]->setVScroll(false,448/2);
    MAPH[0]->setVScroll(false,448);
   }
  if(STAGE==8)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==9)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==10)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==11)
   {
  	if(PL->getGlobX()<3600)
  	 {
      // Vscroll off
 	  PL->setScrollCenterY(2);
 	  MAP1[0]->setVScroll(false,448);
 	  MAP0[0]->setVScroll(false,448/2);
 	  MAPH[0]->setVScroll(false,448);
 	 }
 	else
 	 {
 	  // Vscroll on
 	  PL->setScrollCenterY(106);
 	  MAP1[0]->setVScroll(true);
 	  MAP0[0]->setVScroll(true);
 	  MAPH[0]->setVScroll(true);
 	 }
   }
  if(STAGE==12)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==13)
   {
    // Vscroll on
    PL->setScrollCenterY(130);
    MAP1[1]->setVScroll(true);
    MAP0[1]->setVScroll(true);
    MAPH[1]->setVScroll(true);
   }
  if(STAGE==14)
   {
    if(PL->getGlobX()<3600+768)
 	 {
 	  // Vscroll off
 	  PL->setScrollCenterY(2);
 	  MAP1[0]->setVScroll(false,448);
 	  MAP0[0]->setVScroll(false,448/2);
 	  MAPH[0]->setVScroll(false,448);
 	 }
 	else
 	 {
 	  // Vscroll on
 	  PL->setScrollCenterY(106);
 	  MAP1[0]->setVScroll(true);
 	  MAP0[0]->setVScroll(true);
 	  MAPH[0]->setVScroll(true);
 	 }
   }
  if(STAGE==15)
   {
    // Vscroll off
 	PL->setScrollCenterY(2);
 	MAP1[0]->setVScroll(false,448);
 	MAP0[0]->setVScroll(false,448/2);
 	MAPH[0]->setVScroll(false,448);
   }
 }

void readyGo()
 {
  if(READY_GO>0)
   {
  	READY_GO--;
  	if(READY_GO == READY_GO_TIME) stop_midi();
  	textprintf_ex(VSCR,font,144,106,makecol32(255,0,0),-1,"Ready Go");
    if(READY_GO == 0) play_midi(music[STAGE],1);
   }
 }

void checkTimer()
 {
  if(READY_GO==0)
   {
    if(!TIMEOVER)
     {
      if(AR_Clock()-TIME_MS>=1000) { SEC--; TIME_MS=AR_Clock(); }
 	  if(MIN==0 && SEC==0) { MIN=SEC=0; PL->setDress(1); PL->setHitted(true); TIMEOVER=true; }
 	  if(SEC<0) if(MIN>0)  { SEC=59; MIN--; }
 	 }
   }
  if(SEC==0 && MIN==0 && TIMEOVER) textprintf_ex(VSCR,font,150,106,makecol32(255,0,0),-1,"Time Over");
  if(STAGE==0)  if(PL->getGlobX()>=4100)                        { CKPOINT=1; if(RETIME) { MIN=2; SEC=30; RETIME=false; } }
  if(STAGE==1)  if(PL->getGlobX()>=3600)                        { CKPOINT=1; if(RETIME) { MIN=3; SEC=00; RETIME=false; } }
  if(STAGE==2)  if(PL->getGlobX()>=3250)                        { CKPOINT=1; if(RETIME) { MIN=2; SEC=30; RETIME=false; } }
  if(STAGE==3)  if(PL->getGlobX()>=3700)                        { CKPOINT=1; if(RETIME) { MIN=2; SEC=00; RETIME=false; } }
  if(STAGE==4)  if(PL->getGlobX()>=2390 && PL->getGlobY()>=500) { CKPOINT=1; if(RETIME) { MIN=4; SEC=00; RETIME=false; } }
  if(STAGE==5)  if(PL->getGlobX()>=1632)                        { CKPOINT=1; if(RETIME) { MIN=2; SEC=00; RETIME=false; } }
  if(STAGE==6)  if(PL->getGlobX()>=1060)                        { CKPOINT=1; if(RETIME) { MIN=3; SEC=00; RETIME=false; } }

  if(STAGE==8)  if(PL->getGlobX()<=1230)                        { CKPOINT=1; if(RETIME) { MIN=3; SEC=00; RETIME=false; } }
  if(STAGE==9)  if(PL->getGlobX()<=1484)                        { CKPOINT=1; if(RETIME) { MIN=3; SEC=00; RETIME=false; } }
  if(STAGE==10) if(PL->getGlobX()<=2300 && PL->getGlobY()<=600) { CKPOINT=1; if(RETIME) { MIN=3; SEC=00; RETIME=false; } }
  if(STAGE==11) if(PL->getGlobX()<=3200)                        { CKPOINT=1; if(RETIME) { MIN=2; SEC=30; RETIME=false; } }
  if(STAGE==12) if(PL->getGlobX()<=3100)                        { CKPOINT=1; if(RETIME) { MIN=3; SEC=30; RETIME=false; } }
  if(STAGE==13) if(PL->getGlobX()<=3900)                        { CKPOINT=1; if(RETIME) { MIN=3; SEC=00; RETIME=false; } }
  if(STAGE==14) if(PL->getGlobX()<=3700)                        { CKPOINT=1; if(RETIME) { MIN=2; SEC=30; RETIME=false; } }

  if(STAGE==15) if(PL->getGlobX()<=2894)                        { CKPOINT=1; if(RETIME) { MIN=2; SEC=30; RETIME=false; } }
  readyGo();
 }

void prologue()
 {
  BITMAP *prologue,*evil;
  int count,part,alpha;

  prologue = load_bmp("data/prologue.bmp",NULL);
  evil     = create_bitmap(G_RESX,G_RESY);
  blit(prologue,evil,G_RESX*0,G_RESY*1,0,0,G_RESX,G_RESY);
  count=part=alpha=0;

  PL->setX(-32); PL->setY(150); PL->setCurrentFrame(1);

  clear_to_color(VSCR,makecol(0,0,0));
  clear_keybuf();
  while(count<=580 && !key[PL->getKeyJump()] && !key[PL->getKeyShoot()] && !key[KEY_SPACE] && !key[KEY_ESC])
   {
	blit(prologue,VSCR,G_RESX*0,G_RESY*0,0,0,G_RESX,G_RESY);
	count++;
	if(count==104) part=1;
	if(count==120) part=2;
	if(count==200) part=1;
	if(count==250) part=3;
	if(count==400) part=4;
	if(count==470) part=0;
	if(part==0)
	 {
	  draw_sprite(VSCR,PL->getFrame(PL->getCurrentFrame()),PL->getX(),PL->getY());
	  PL->setX(PL->getX()+2); if(count%5==0) PL->setCurrentFrame(PL->getCurrentFrame()+1); if(PL->getCurrentFrame()>6) PL->setCurrentFrame(1);
     }
    if(part==1)
	 {
	  draw_sprite(VSCR,PL->getFrame(0),PL->getX(),PL->getY());
     }
    if(part==2)
	 {
	  draw_sprite(VSCR,PL->getFrame(7),PL->getX(),PL->getY());
     }
    if(part==3)
     {
	  draw_sprite(VSCR,PL->getFrame(0),PL->getX(),PL->getY());
	  set_trans_blender(0,0,0,alpha);
	  draw_trans_sprite(VSCR,evil,123,20);
	  set_trans_blender(0,0,0,255);
	  if(alpha <= 164) alpha+=2;
	 }
    if(part==4)
     {
	  draw_sprite(VSCR,PL->getFrame(0),PL->getX(),PL->getY());
	  set_trans_blender(0,0,0,alpha);
	  draw_trans_sprite(VSCR,evil,123,20);
	  set_trans_blender(0,0,0,255);
	  if(alpha >= 6) alpha-=4;
	 }
    textprintf_ex(VSCR,font,10,10,makecol32(255,0,0),-1,"Prologue ...");
  	rePaint(MFPS);
   }
  for(int i=0;i<FADETIME;i++)
   {
  	fade32(VSCR);
  	rePaint(MFPS);
   }
  clear_to_color(screen,makecol(0,0,0));
  destroy_bitmap(evil);
  destroy_bitmap(prologue);
 }

void interlude()
 {
  int count;

  count = 0;
  clear_to_color(VSCR,makecol(0,0,0));
  clear_keybuf();

  while(count<=100 && !key[PL->getKeyJump()] && !key[PL->getKeyShoot()] && !key[KEY_SPACE] && !key[KEY_ESC])
   {
	count++;
	textprintf_ex(VSCR,font,10, 10,makecol32(255,0,0),-1,"Interlude ...");
  	rePaint(MFPS);
   }
  for(int i=0;i<FADETIME;i++)
   {
  	fade32(VSCR);
  	rePaint(MFPS);
   }
  clear_to_color(screen,makecol(0,0,0));
 }

void epilogue()
 {
  int count;

  count = 0;
  clear_to_color(VSCR,makecol(0,0,0));
  clear_keybuf();

  while(count<=150 && !key[PL->getKeyJump()] && !key[PL->getKeyShoot()] && !key[KEY_SPACE] && !key[KEY_ESC])
   {
	count++;
	textprintf_ex(VSCR,font,10, 10,makecol32(255,0,0),-1,"Congratulation!");
	textprintf_ex(VSCR,font,10, 20,makecol32(255,0,0),-1,"You have defeated all the enemies!");
	textprintf_ex(VSCR,font,10, 90,makecol32(255,0,0),-1,"The end!");
	textprintf_ex(VSCR,font,10,120,makecol32(255,0,0),-1,"Final score: %d",SCORE);
  	rePaint(MFPS);
   }
  for(int i=0;i<FADETIME;i++)
   {
  	fade32(VSCR);
  	rePaint(MFPS);
   }
  clear_to_color(screen,makecol(0,0,0));
  GAME_OVER=1; gameOver();
 }

void gameOver()
 {
  clear_to_color(VSCR,makecol(0,0,0));
  if(L_B)   layer_back();
  if(L_0)   layer_0();
  if(L_1)   layer_1();
  if(L_H)   layer_h();
  if(L_OSD) layer_OSD(50);
  textprintf_ex(VSCR,font,150,106,makecol32(255,0,0),-1,"GAME OVER");
  rePaint(MFPS);
  wait(2500);
 }

void Intro()
 {
  BITMAP *tit;
  MIDI *intro_music;

  tit=load_bmp("data/competition.bmp",NULL);
  intro_music=load_midi("data/sound/music/intro.mid");
  play_midi(intro_music,0);
  stretch_blit(tit,screen,0,0,640,480,0,0,SCREENX,SCREENY);
  textprintf_ex(screen,font,(SCREENX-8*12)>>1,SCREENY-24,makecol32(50,50,50),-1,"press any key...");
  clear_keybuf(); while(!keypressed());
  for(int i=0;i<FADETIME;i++)
   {
  	fade32(tit);
  	stretch_blit(tit,screen,0,0,640,480,0,0,SCREENX,SCREENY);
  	vsync();
   }
  clear_to_color(screen,makecol(0,0,0));
  destroy_bitmap(tit); destroy_midi(intro_music);
  if(key[KEY_ESC]) exit(0);
 }

void Title()
 {
  BITMAP *tit,*art;
  MIDI *intro_music;
  int POS;
  int k,actual_key,max_stage;
  int menu,stage,lives,difficulty,screenres,scanlines;
  bool fscreen,stretch,debug;

  actual_key = 0;
  POS   = 0; menu  = 0;
  stage = 0; lives = MENU_LIVES; difficulty = DIFFICULTY;
  if(DEBUG) max_stage=N_STAGE-1; else max_stage=MAX_STAGE;
  readConf(&screenres,&scanlines,&fscreen,&stretch,&debug,&KEYLEFT,&KEYRIGHT,&KEYUP,&KEYDOWN,&KEYJUMP,&KEYSHOOT,&MUSIC_VOLUME,&SOUND_VOLUME);
  tit=load_bmp("data/title.bmp",NULL);
  art=load_bmp("data/arthur.bmp",NULL);
  intro_music=load_midi("data/sound/music/intro.mid");
  play_midi(intro_music,0);
  clear_keybuf();
  while(1)
   {
    blit(tit,VSCR,0,0,0,0,G_RESX,G_RESY);
    if(menu==0) // Main menu
     {
      textprintf_ex(VSCR,font,1,204,makecol32(255,0,0),-1,"2006/2011 www.valarsoft.com (games section)");
      textprintf_ex(VSCR,font,1,216,makecol32(255,0,0),-1,"Made with allegro library");
      textprintf_ex(VSCR,font,328,216,makecol32(255,0,0),-1,"V. %s",VER);

      textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"Options >>> ");
      textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Start game  ");
      textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Exit        ");
      draw_sprite(VSCR,art,136,162+POS*12);
  	  if(key[KEY_DOWN]) { if(POS<2) POS++; key[KEY_DOWN]=false; }
 	  if(key[KEY_UP])   { if(POS>0) POS--; key[KEY_UP]=false; }
 	  if(key[KEY_SPACE] || key[KEY_ENTER] || key[KEY_LCONTROL] || key[KEY_ALT] || POS==0)
 	   {
 	    if(POS==0) { if(key[KEY_RIGHT]) menu=1; key[KEY_RIGHT]=false; }
 	    if(POS==1) break;
 	    if(POS==2) { destroy_bitmap(tit);destroy_bitmap(art); destroy_midi(intro_music); exit(0); }
 	   }
     }
    if(menu==1) // Option menu
     {
      textprintf_ex(VSCR,font,1,165+12*0,makecol32(255,0,0),-1,"(Options menu)");

      textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"<<< Back    ");
      textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Game  options >>");
      textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Video options >>");
      textprintf_ex(VSCR,font,155,165+12*3,makecol32(255,255,0),-1,"Audio options >>");
  	  draw_sprite(VSCR,art,136,162+POS*12);
  	  if(key[KEY_DOWN]) { if(POS<3) POS++; key[KEY_DOWN]=false; }
 	  if(key[KEY_UP])   { if(POS>0) POS--; key[KEY_UP]=false; }
 	  if(key[KEY_ESC] || key[KEY_SPACE] || key[KEY_ENTER] || key[KEY_LCONTROL] || key[KEY_ALT] || POS==0 || POS==1 || POS==2 || POS==3)
 	   {
 	    //if(key[KEY_ESC]) { menu=0; key[KEY_ESC]=false; POS=0; stage=0; }
 	    if(POS==0) { if(key[KEY_LEFT])  menu=0; key[KEY_LEFT]=false;  }
 	    if(POS==1) { if(key[KEY_RIGHT]) { menu=2; POS=0; } key[KEY_RIGHT]=false; }
 	    if(POS==2) { if(key[KEY_RIGHT]) { menu=3; POS=0; } key[KEY_RIGHT]=false; }
 	    if(POS==3) { if(key[KEY_RIGHT]) { menu=4; POS=0; } key[KEY_RIGHT]=false; }
 	   }
 	 }
    if(menu==2) // Game menu
     {
      textprintf_ex(VSCR,font,1,165+12*0,makecol32(255,0,0),-1,"(Game menu)");

      textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"<<< Back    ");
      if(stage<9) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Starting stage: 0%d   ",stage+1);
      else        textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Starting stage: %d    ",stage+1);
      textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Lives:          0%d   ",lives);
      if(difficulty==1) textprintf_ex(VSCR,font,155,165+12*3,makecol32(255,255,0),-1,"Difficulty:     %s","Easy");
      if(difficulty==2) textprintf_ex(VSCR,font,155,165+12*3,makecol32(255,255,0),-1,"Difficulty:     %s","Normal");
      if(difficulty==3) textprintf_ex(VSCR,font,155,165+12*3,makecol32(255,255,0),-1,"Difficulty:     %s","Hard");
      textprintf_ex(VSCR,font,155,165+12*4,makecol32(255,255,0),-1,"Configure game keys >>");
  	  draw_sprite(VSCR,art,136,162+POS*12);
  	  if(key[KEY_DOWN]) { if(POS<4) POS++; key[KEY_DOWN]=false; }
 	  if(key[KEY_UP])   { if(POS>0) POS--; key[KEY_UP]=false; }
 	  if(key[KEY_ESC] || key[KEY_SPACE] || key[KEY_ENTER] || key[KEY_LCONTROL] || key[KEY_ALT] || POS==0 || POS==1 || POS==2 || POS==3 || POS==4)
 	   {
 	    if(POS==0) { if(key[KEY_LEFT]) menu=1; key[KEY_LEFT]=false; }
 	    if(POS==1) {
 	 	            if(key[KEY_LEFT])  { if(stage>0)         stage--; }
                    if(key[KEY_RIGHT]) { if(stage<max_stage) stage++; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	           }
 	    if(POS==2) {
 	 	            if(key[KEY_LEFT])  { if(lives>3) lives-=2; }
 	 	            if(key[KEY_RIGHT]) { if(lives<7) lives+=2; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	           }
        if(POS==3) {
 	 	            if(key[KEY_LEFT])  { if(difficulty>1) difficulty--; }
                    if(key[KEY_RIGHT]) { if(difficulty<3) difficulty++; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
			       }
        if(POS==4) { if(key[KEY_RIGHT]) { menu = 5; POS = 0; actual_key = 0; } key[KEY_RIGHT]=false; }
 	   }
 	 }
    if(menu==5) // Config game keys menu
     {
      textprintf_ex(VSCR,font,1,165+12*0,makecol32(255,0,0),-1,"(Keys config)");

      if(actual_key == 1) textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"Enter key for LEFT      ");
      if(actual_key == 2) textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"Enter key for RIGHT     ");
      if(actual_key == 3) textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"Enter key for UP        ");
      if(actual_key == 4) textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"Enter key for DOWN      ");
      if(actual_key == 5) textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"Enter key for JUMP      ");
      if(actual_key == 6) textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"Enter key for SHOOT     ");
      if(actual_key == 8) textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"<<< Back & save keys config");
  	  draw_sprite(VSCR,art,136,162);

      if(actual_key > 1 && actual_key < 8)
       {
		clear_keybuf(); k = readkey() >> 8;
		if(actual_key == 2) KEYLEFT  = k;
		if(actual_key == 3) KEYRIGHT = k;
		if(actual_key == 4) KEYUP    = k;
		if(actual_key == 5) KEYDOWN  = k;
		if(actual_key == 6) KEYJUMP  = k;
		if(actual_key == 7) KEYSHOOT = k;
		key[KEY_LEFT] = false;
	   }
      if(actual_key == 8) { if(key[KEY_LEFT]) { menu = 2; POS = 3; writeConf(screenres,scanlines,fscreen,stretch,debug,KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYJUMP,KEYSHOOT,MUSIC_VOLUME,SOUND_VOLUME); PL->setKeyboardLayout(KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYJUMP,KEYSHOOT); key[KEY_LEFT]=false; } }
      if(actual_key < 8) actual_key++;
 	 }

    if(menu==3) // Video menu
     {
      textprintf_ex(VSCR,font,1,165+12*0,makecol32(255,0,0),-1,"(Video menu)");
      textprintf_ex(VSCR,font,1,165+12*3,makecol32(255,0,0),-1,"You must restart");
      textprintf_ex(VSCR,font,1,165+12*4,makecol32(255,0,0),-1,"to apply changes");
     	
      textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"<<< Back    ");
      if(screenres==0) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Video mode:     320x200");
      if(screenres==1) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Video mode:     320x240");
      if(screenres==2) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Video mode:     384x224");
      if(screenres==3) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Video mode:     400x300");
      if(screenres==4) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Video mode:     512x384");
      if(screenres==5) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Video mode:     640x480");
      if(screenres==6) textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Video mode:     768x448");
      if(scanlines==0) textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Scanlines:      None");
      if(scanlines==1) textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Scanlines:      Soft");
      if(scanlines==2) textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Scanlines:      Normal");
      if(scanlines==3) textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Scanlines:      Hard");
      if(fscreen)      textprintf_ex(VSCR,font,155,165+12*3,makecol32(255,255,0),-1,"Fullscreen:     On");
      else             textprintf_ex(VSCR,font,155,165+12*3,makecol32(255,255,0),-1,"Fullscreen:     Off");
      if(stretch)      textprintf_ex(VSCR,font,155,165+12*4,makecol32(255,255,0),-1,"Fit resolution: On");
      else             textprintf_ex(VSCR,font,155,165+12*4,makecol32(255,255,0),-1,"Fit resolution: Off");
  	  draw_sprite(VSCR,art,136,162+POS*12);
  	  if(key[KEY_DOWN]) { if(POS<4) POS++; key[KEY_DOWN]=false; }
 	  if(key[KEY_UP])   { if(POS>0) POS--; key[KEY_UP]=false; }
 	  if(key[KEY_ESC] || key[KEY_SPACE] || key[KEY_ENTER] || key[KEY_LCONTROL] || key[KEY_ALT] || POS==0 || POS==1 || POS==2 || POS==3 || POS==4)
 	   {
 	    //if(key[KEY_ESC]) { menu=0; key[KEY_ESC]=false; POS=0; stage=0; }
 	    if(POS==0) { 
 	                if(key[KEY_LEFT]) menu=1;
 	 	 		    key[KEY_LEFT]=false;
                    writeConf(screenres,scanlines,fscreen,stretch,debug,KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYJUMP,KEYSHOOT,MUSIC_VOLUME,SOUND_VOLUME);
 	 	           }
 	    if(POS==1) {
 	 	            if(key[KEY_LEFT])  { if(screenres>0) screenres--; }
 	 	            if(key[KEY_RIGHT]) { if(screenres<6) screenres++; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	           }
 	 	if(POS==2) {
 	 	            if(key[KEY_LEFT])  { if(scanlines>0) scanlines--; }
 	 	            if(key[KEY_RIGHT]) { if(scanlines<3) scanlines++; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	           }
 	 	if(POS==3) {
 	 	            if(key[KEY_LEFT])  { if(fscreen)  fscreen=false; }
 	 	            if(key[KEY_RIGHT]) { if(!fscreen) fscreen=true; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	           }
 	    if(POS==4) {
 	 	            if(key[KEY_LEFT])  { if(stretch)  stretch=false; }
 	 	            if(key[KEY_RIGHT]) { if(!stretch) stretch=true; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	           }
 	   }
 	 }
    if(menu==4) // Audio menu
     {
      textprintf_ex(VSCR,font,1,165+12*0,makecol32(255,0,0),-1,"(Audio menu)");

      textprintf_ex(VSCR,font,155,165+12*0,makecol32(255,255,0),-1,"<<< Back    ");
      textprintf_ex(VSCR,font,155,165+12*1,makecol32(255,255,0),-1,"Music volume:");
      textprintf_ex(VSCR,font,155,165+12*2,makecol32(255,255,0),-1,"Sound Fx volume:");
      rect(VSCR,285,165+12*1,374,(165+12*1)+7,makecol32(255,255,0));
      rect(VSCR,285,165+12*2,374,(165+12*2)+7,makecol32(255,255,0));
      rectfill(VSCR,287,167+12*1,287+(int)(MUSIC_VOLUME/3),(167+12*1)+3,makecol32(255,255,0));
      rectfill(VSCR,287,167+12*2,287+(int)(SOUND_VOLUME/3),(167+12*2)+3,makecol32(255,255,0));
  	  draw_sprite(VSCR,art,136,162+POS*12);
  	  if(key[KEY_DOWN]) { if(POS<2) POS++; key[KEY_DOWN]=false; }
 	  if(key[KEY_UP])   { if(POS>0) POS--; key[KEY_UP]  =false; }
 	  if(key[KEY_ESC] || key[KEY_SPACE] || key[KEY_ENTER] || key[KEY_LCONTROL] || key[KEY_ALT] || POS==0 || POS==1 || POS==2 || POS==3 || POS==4)
 	   {
 	    //if(key[KEY_ESC]) { menu=0; key[KEY_ESC]=false; POS=0; stage=0; }
 	    if(POS==0) { 
 	                if(key[KEY_LEFT]) menu=1;
 	 	 		    key[KEY_LEFT]=false;
                    writeConf(screenres,scanlines,fscreen,stretch,debug,KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYJUMP,KEYSHOOT,MUSIC_VOLUME,SOUND_VOLUME);
 	 	 		   }
 	    if(POS==1) {
 	 	            if(key[KEY_LEFT])  { MUSIC_VOLUME-=3; if(MUSIC_VOLUME<0)   MUSIC_VOLUME=0;   }
 	 	            if(key[KEY_RIGHT]) { MUSIC_VOLUME+=3; if(MUSIC_VOLUME>255) MUSIC_VOLUME=255; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	            set_volume(SOUND_VOLUME,MUSIC_VOLUME);
 	 	           }
 	    if(POS==2) {
 	 	            if(key[KEY_LEFT])  { SOUND_VOLUME-=3; if(SOUND_VOLUME<0)   SOUND_VOLUME=0;   }
 	 	            if(key[KEY_RIGHT]) { SOUND_VOLUME+=3; if(SOUND_VOLUME>255) SOUND_VOLUME=255; }
 	 	            key[KEY_LEFT]=false; key[KEY_RIGHT]=false;
 	 	            set_volume(SOUND_VOLUME,MUSIC_VOLUME);
 	 	           }
 	   }
 	 }
 	rePaint(MFPS);
   }
  for(int i=0;i<FADETIME;i++)
   {
    fade32(tit);
 	blit(tit,VSCR,0,0,0,0,G_RESX,G_RESY);
 	rePaint(MFPS);
   }
  destroy_bitmap(tit); destroy_bitmap(art); destroy_midi(intro_music);
  clear_to_color(screen,makecol(0,0,0));
  MENU_LIVES = lives;
  initGame(stage,lives,difficulty);
 }

void stageMap()
 {
  BITMAP *smap,*arrow;
  SAMPLE *jingle;
  int pos,dx,count,x,y;
  count = 0;

  if(EPILOGUE)  { epilogue();  EPILOGUE  = 0; return; }
  if(PROLOGUE)  { prologue();  PROLOGUE  = 0; }
  if(INTERLUDE) { interlude(); INTERLUDE = 0; }
  if(STAGE==0) if(!CKPOINT) { x=30;  y=100; } else { x=100; y=100; }
  if(STAGE==1) if(!CKPOINT) { x=160; y=95;  } else { x=160; y=95; }

  if(STAGE<=7) { pos=10;  dx=1;  }
  if(STAGE> 7) { pos=330; dx=-1; }
  stop_midi();
  jingle = load_sample("data/sound/fx/stagemap.wav");
  smap   = load_bmp("data/stagemap.bmp",NULL);
  arrow  = load_bmp("data/arrow.bmp",NULL);
  play_sample(jingle,120,150,1000,0);
  clear_to_color(VSCR,makecol(0,0,0)); blit(smap,VSCR,pos,0,0,0,G_RESX,G_RESY); draw_sprite(VSCR,arrow,x-pos,y+(count%25)); rePaint(MFPS); wait(400);
  clear_keybuf();

  while(count<400 && !key[PL->getKeyJump()] && !key[PL->getKeyShoot()] && !key[KEY_SPACE] && !key[KEY_ESC])
   {
	count++;
	if(STAGE<=7) if(pos==305) dx=0;
    if(STAGE> 7) if(pos==20)  dx=0;
    blit(smap,VSCR,pos,0,0,0,G_RESX,G_RESY);
    draw_sprite(VSCR,arrow,x-pos,y+(count%25));
    if(count>60) pos+=dx;
  	rePaint(MFPS);
   }
  for(int i=0;i<FADETIME;i++)
   {
  	fade32(smap);
  	blit(smap,VSCR,pos,0,0,0,G_RESX,G_RESY);
  	rePaint(MFPS);
   }
  clear_to_color(screen,makecol(0,0,0));
  destroy_sample(jingle); destroy_bitmap(smap); destroy_bitmap(arrow); clear_keybuf();
  key[PL->getKeyJump()] = 0; key[PL->getKeyShoot()] = 0; key[KEY_SPACE] = 0; key[KEY_ESC] = 0;
 }

void layer_back()
 {
  clear_to_color(VSCR,makecol(0,0,0));
  FL_SFX->timeSFX(); FL_SFX->animSFX();	FL_SFX->drawSFX(0,0);
 }

void layer_0()
 {
  for(int i=0;i<N_THSFX;i++) { TH_SFX[i]->timeSFX(); TH_SFX[i]->animSFX(); TH_SFX[i]->drawSFX(MAP0[STAGE]->getScrollX(),MAP0[STAGE]->getScrollY()); }
  MAP0[STAGE]->drawMap();
 }

void layer_1()
 {
  //for(int i=0;i<N_PARFX;i++) { PA_SFX[i]->timeSFX(); PA_SFX[i]->animSFX(); PA_SFX[i]->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY()); } // Particle
  RA_SFX[0]->timeSFX(); RA_SFX[0]->animSFX();	RA_SFX[0]->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  SN_SFX[0]->timeSFX(); SN_SFX[0]->animSFX();	SN_SFX[0]->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  MAP1[STAGE]->drawMap();
  for(int i=0;i<N_PLAT;i++) if(!MOBILE[i]->getFGround()) MOBILE[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_PARFX;i++) { PA_SFX[i]->timeSFX(); PA_SFX[i]->animSFX(); PA_SFX[i]->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY()); } // Particle
 }

void layer_s() 
 {
  for(int i=0;i<N_BL;i++) BL[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_RF;i++) RF[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_ZO;i++) ZO[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_SK;i++) SK[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_VO;i++) VO[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_RA;i++) RA[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_RK;i++) RK[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_P2;i++) P2[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_GO;i++) GO[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_BA;i++) BA[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_GI;i++) GI[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_FW;i++) FW[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_IN;i++) IN[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_FG;i++) FG[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_CL;i++) CL[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_TU;i++) TU[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_DE;i++) DE[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_LD;i++) LD[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_BC;i++) BC[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_PO;i++) PO[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_MA;i++) MA[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_FL;i++) FL[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());

  for(int i=0;i<N_BBOX;i++)  BONUSB[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=N_BVASE-1;i>=0;i--) BVASE[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY()); // Draw BVASE from last to first due to a good visualization
  SHILD->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  KEY->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  PL->drawSprite();
  for(int i=0;i<N_PLAT;i++) if(MOBILE[i]->getFGround()) MOBILE[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  
  // The follow enemies need to be displayed after the platforms 
  for(int i=0;i<N_ST;i++) ST[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());

  for(int i=0;i<PL->getNWeapon();i++) PL->getWeapon(i)->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  PL->getMagic()->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());

  // Immobile enemies (draw after player)
  for(int i=0;i<N_IMMOBILE;i++) IMMOBILE[i]->drawSprite(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
 }

void layer_h()
 {
  MAPH[STAGE]->drawMap();
  RA_SFX[1]->timeSFX(); RA_SFX[1]->animSFX();	RA_SFX[1]->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  SN_SFX[1]->timeSFX(); SN_SFX[1]->animSFX();	SN_SFX[1]->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  for(int i=0;i<N_WASFX;i++) { WA_SFX[i]->timeSFX(); WA_SFX[i]->animSFX(); WA_SFX[i]->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY()); }
  FO_SFX->timeSFX(); FO_SFX->animSFX();	FO_SFX->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
  FI_SFX->timeSFX(); FI_SFX->animSFX();	FI_SFX->drawSFX(MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY());
 }

void layer_OSD(int alpha) {	OSD->drawOSD(0,0); }

void Player()
 {
  for(int i=0;i<PL->getNWeapon();i++) // Weapons
   {
 	PL->getWeapon(i)->timeSprite();
 	PL->getWeapon(i)->animSprite();
   }
  // Magic
  PL->getMagic()->timeSprite();
  PL->getMagic()->animSprite();

  PL->timeSprite();
  PL->animSprite();
  if(READY_GO==0) PL->moveSprite();
  PL->platformSprite();
 }

void Enemies()
 {
  if(READY_GO==0)
   { 
    for(int i=0;i<N_ZO;i++) { ZO[i]->timeSprite(); ZO[i]->animSprite(); ZO[i]->platformSprite(); }
    for(int i=0;i<N_SK;i++) { SK[i]->timeSprite(); SK[i]->animSprite(); SK[i]->platformSprite(); }
    for(int i=0;i<N_VO;i++) { VO[i]->timeSprite(); VO[i]->animSprite(); /*VO[i]->platformSprite();*/ }
    for(int i=0;i<N_BL;i++) { BL[i]->timeSprite(); BL[i]->animSprite(); /*BL[i]->platformSprite();*/ }
    for(int i=0;i<N_RF;i++) { RF[i]->timeSprite(); RF[i]->animSprite(); /*RF[i]->platformSprite();*/ }
    for(int i=0;i<N_RA;i++) { RA[i]->timeSprite(); RA[i]->animSprite(); /*RA[i]->platformSprite();*/ }
    for(int i=0;i<N_RK;i++) { RK[i]->timeSprite(); RK[i]->animSprite(); /*RK[i]->platformSprite();*/ }
    for(int i=0;i<N_P2;i++) { P2[i]->timeSprite(); P2[i]->animSprite(); /*P2[i]->platformSprite();*/ }
    for(int i=0;i<N_ST;i++) { ST[i]->timeSprite(); ST[i]->animSprite(); /*ST[i]->platformSprite();*/ }
    for(int i=0;i<N_GO;i++) { GO[i]->timeSprite(); GO[i]->animSprite(); /*GO[i]->platformSprite();*/ }
    for(int i=0;i<N_BA;i++) { BA[i]->timeSprite(); BA[i]->animSprite(); /*BA[i]->platformSprite();*/ }
    for(int i=0;i<N_GI;i++) { GI[i]->timeSprite(); GI[i]->animSprite(); GI[i]->platformSprite(); }
    for(int i=0;i<N_FW;i++) { FW[i]->timeSprite(); FW[i]->animSprite(); /*FW[i]->platformSprite();*/ }
    for(int i=0;i<N_IN;i++) { IN[i]->timeSprite(); IN[i]->animSprite(); /*IN[i]->platformSprite();*/ }
    for(int i=0;i<N_FG;i++) { FG[i]->timeSprite(); FG[i]->animSprite(); FG[i]->platformSprite(); }
    for(int i=0;i<N_CL;i++) { CL[i]->timeSprite(); CL[i]->animSprite(); /*CL[i]->platformSprite();*/ }
    for(int i=0;i<N_TU;i++) { TU[i]->timeSprite(); TU[i]->animSprite(); TU[i]->platformSprite(); }
    for(int i=0;i<N_DE;i++) { DE[i]->timeSprite(); DE[i]->animSprite(); DE[i]->platformSprite(); }
    for(int i=0;i<N_LD;i++) { LD[i]->timeSprite(); LD[i]->animSprite(); /*LD[i]->platformSprite();*/ }
    for(int i=0;i<N_BC;i++) { BC[i]->timeSprite(); BC[i]->animSprite(); BC[i]->platformSprite(); }
    for(int i=0;i<N_PO;i++) { PO[i]->timeSprite(); PO[i]->animSprite(); PO[i]->platformSprite(); }
    for(int i=0;i<N_MA;i++) { MA[i]->timeSprite(); MA[i]->animSprite(); /*MA[i]->platformSprite();*/ }
    for(int i=0;i<N_FL;i++) { FL[i]->timeSprite(); FL[i]->animSprite(); FL[i]->platformSprite(); }    
    for(int i=0;i<N_IMMOBILE;i++) { IMMOBILE[i]->timeSprite(); IMMOBILE[i]->animSprite(); /*IMMOBILE[i]->platformSprite();*/ }
   }
  // Magic box, bonus and vase
  for(int i=0;i<N_BBOX;i++)  { BONUSB[i]->timeSprite(); BONUSB[i]->animSprite(); BONUSB[i]->platformSprite(); }
  for(int i=0;i<N_BVASE;i++) { BVASE[i]->timeSprite(); BVASE[i]->platformSprite(); BVASE[i]->animSprite(); } // platformSprite its called before animSprite only for BVASE
  SHILD->timeSprite(); SHILD->animSprite(); SHILD->platformSprite();
  KEY->timeSprite(); KEY->animSprite(); KEY->platformSprite();
  for(int i=0;i<N_PLAT;i++) { MOBILE[i]->timeSprite(); MOBILE[i]->animSprite(); /*MOBILE[i]->platformSprite();*/ }

  // Attach the vase to an enemy
  for(int j=0;j<N_BVASE;j+=2)
   {
    for(int i=0;i<N_SK;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) SK[i]->attachVase(BVASE[j]);
    for(int i=0;i<N_RA;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) RA[i]->attachVase(BVASE[j]);
    for(int i=0;i<N_TU;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) TU[i]->attachVase(BVASE[j]);
    //for(int i=0;i<N_DE;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) DE[i]->attachVase(BVASE[j]);
    for(int i=0;i<N_LD;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) LD[i]->attachVase(BVASE[j]);
    for(int i=0;i<N_BC;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) BC[i]->attachVase(BVASE[j]); // Non serve per big_crab poichè e indistruttibile ...
    for(int i=0;i<N_PO;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) PO[i]->attachVase(BVASE[j]);
    for(int i=0;i<N_FL;i++) if(!BVASE[j]->isActive() && !BVASE[j+1]->isActive()) FL[i]->attachVase(BVASE[j]);
   }
 }

void Scroll()
 {
  MAP0[STAGE]->scrollMap();
  MAP1[STAGE]->scrollMap();
  MAPH[STAGE]->scrollMap();
 }

void Collision()
 {
  collisionPlayerBonus();
  collisionShildBullets();
  collisionWeaponEnemies();
  collisionMagicEnemies();
  collisionPlayerEnemies();
  collisionPlayerBullets();
 }

void collisionPlayerBonus()
 {
  // Player con bonus
  if(!PL->isMagicFire()) // Evita di cambiare arma in fase di magia in corso
   { 	 
    for(int i=0;i<N_BBOX;i++) // Bonus for magic box
     {
   	  if(BONUSB[i]->isActive() && !BONUSB[i]->isTaked() && PL->getDress()!=3)
       if(hitTestFast8(PL->getActFrameColP(),BONUSB[i]->getActFrameColP(),PL->getX(),PL->getY(),BONUSB[i]->getX(),BONUSB[i]->getY()) && BONUSB[i]->isTakeble())
        if(BONUSB[i]->getType()!=6) { PL->takeBonus(BONUSB[i]->getType(),BONUSB[i]->getExtra()); if(BONUSB[i]->getType()==4) BONUSB[i]->setTaked(true); else BONUSB[i]->resetObj(false); }
        else if(PL->isHittable()) PL->setHitted(true);
     }
    for(int i=1;i<N_BVASE;i+=2) // Bonus in the vase
     {
   	  if(BVASE[i]->isActive() && !BVASE[i]->isTaked() && PL->getDress()!=3)
       if(hitTestFast8(PL->getActFrameColP(),BVASE[i]->getActFrameColP(),PL->getX(),PL->getY(),BVASE[i]->getX(),BVASE[i]->getY()) && BVASE[i]->isTakeble())
        { PL->takeBonus(BVASE[i]->getType(),BVASE[i]->getExtra()); if(BVASE[i]->getType()==4) BVASE[i]->setTaked(true); else BVASE[i]->resetObj(false); }
     }
    if(SHILD->isActive() && PL->getDress()!=3) // Shild (when missed)
     if(hitTestFast8(PL->getActFrameColP(),SHILD->getActFrameColP(),PL->getX(),PL->getY(),SHILD->getX(),SHILD->getY()))
      { PL->takeBonus(SHILD->getType()); SHILD->resetObj(false); }
    if(KEY->isActive() && PL->getDress()!=3) // Key (next level)
     if(hitTestFast8(PL->getActFrameColP(),KEY->getActFrameColP(),PL->getX(),PL->getY(),KEY->getX(),KEY->getY()))
      { PL->takeBonus(KEY->getType()); KEY->resetObj(false); }
   }
 }

void collisionShildBullets()
 {
  // Shild con bullets
  if(PL->getShild() && PL->isHittable() && !PL->isGold() && !PL->isMagicFire())
   {
    for(int i=0;i<N_P2;i++) // Plant mk2 bullets
     {
   	  for(int j=0;j<P2[i]->getNBullet();j++)
   	   if(P2[i]->getBullet(j)->isFired() && P2[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(P2[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),P2[i]->getBullet(j)->getX(),P2[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); P2[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_ST;i++) // Stalagmite bullets
     {
   	  for(int j=0;j<ST[i]->getNBullet();j++)
   	   if(ST[i]->getBullet(j)->isFired() && ST[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(ST[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),ST[i]->getBullet(j)->getX(),ST[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); ST[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_GI;i++) // Giant bullets
     {
   	  for(int j=0;j<GI[i]->getNBullet();j++)
   	   if(GI[i]->getBullet(j)->isFired() && GI[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(GI[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),GI[i]->getBullet(j)->getX(),GI[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); GI[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_FG;i++) // Fire grizzly bullets
     {    	
   	  for(int j=0;j<FG[i]->getNBullet();j++)
   	   if(FG[i]->getBullet(j)->isFired() && FG[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(FG[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),FG[i]->getBullet(j)->getX(),FG[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); FG[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_CL;i++) // Clam bullets
     {
   	  for(int j=0;j<CL[i]->getNBullet();j++)
   	   if(CL[i]->getBullet(j)->isFired() && CL[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(CL[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),CL[i]->getBullet(j)->getX(),CL[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); CL[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_TU;i++) // Turtle bullets
     {
   	  for(int j=0;j<TU[i]->getNBullet();j++)
   	   if(TU[i]->getBullet(j)->isFired() && TU[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(TU[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),TU[i]->getBullet(j)->getX(),TU[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); TU[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_DE;i++) // Demon bullets
     {
   	  for(int j=0;j<DE[i]->getNBullet();j++)
   	   if(DE[i]->getBullet(j)->isFired() && DE[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(DE[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),DE[i]->getBullet(j)->getX(),DE[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); DE[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_LD;i++) // Little devil bullets
     {
   	  for(int j=0;j<LD[i]->getNBullet();j++)
   	   if(LD[i]->getBullet(j)->isFired() && LD[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(LD[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),LD[i]->getBullet(j)->getX(),LD[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); LD[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_BC;i++) // Big crab bullets (non spara)
     {
   	  for(int j=0;j<BC[i]->getNBullet();j++)
   	   if(BC[i]->getBullet(j)->isFired() && BC[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(BC[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),BC[i]->getBullet(j)->getX(),BC[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); BC[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_PO;i++) // Pork bullets
     {
   	  for(int j=0;j<PO[i]->getNBullet();j++)
   	   if(PO[i]->getBullet(j)->isFired() && PO[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(PO[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),PO[i]->getBullet(j)->getX(),PO[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); PO[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_FL;i++) // Flower bullets
     {
   	  for(int j=0;j<FL[i]->getNBullet();j++)
   	   if(FL[i]->getBullet(j)->isFired() && FL[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(FL[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(true),FL[i]->getBullet(j)->getX(),FL[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),3,0,0))
         { PL->setShildHit(true); FL[i]->getBullet(j)->hit(); return; }
     }     
   }
 }

void collisionPlayerEnemies()
 {
  // Player con enemies
  if(PL->isHittable() && !PL->isGold() && !PL->isMagicFire())
   {
 	  for(int i=0;i<N_BL;i++) // Blade
 	   {
 	   	if(BL[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),BL[i]->getActFrameColP(),PL->getX(),PL->getY(),BL[i]->getX(),BL[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_RF;i++) // Rockfall
 	   {
 	   	if(RF[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),RF[i]->getActFrameColP(),PL->getX(),PL->getY(),RF[i]->getX(),RF[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_SK;i++) // Skeleton
 	   {
 	   	if(SK[i]->canHit() && SK[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),SK[i]->getActFrameColP(),PL->getX(),PL->getY(),SK[i]->getX(),SK[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_ZO;i++) // Zombie
 	   {
 	   	if(ZO[i]->canHit() && ZO[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),ZO[i]->getActFrameColP(),PL->getX(),PL->getY(),ZO[i]->getX(),ZO[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_VO;i++) // Volture
 	   {
 	   	if(VO[i]->canHit() && VO[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),VO[i]->getActFrameColP(),PL->getX(),PL->getY(),VO[i]->getX(),VO[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_RA;i++) // Red Arremer
 	   {
 	   	if(RA[i]->canHit() && RA[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),RA[i]->getActFrameColP(),PL->getX(),PL->getY(),RA[i]->getX(),RA[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_RK;i++) // Red Arremer King
 	   {
 	   	if(RK[i]->canHit() && RK[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),RK[i]->getActFrameColP(),PL->getX(),PL->getY(),RK[i]->getX(),RK[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_P2;i++) // Plant mk2
 	   {
 	   	if(P2[i]->canHit() && P2[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),P2[i]->getActFrameColP(),PL->getX(),PL->getY(),P2[i]->getX(),P2[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_ST;i++) // Stalagmite
 	   {
 	   	if(ST[i]->canHit() && ST[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),ST[i]->getActFrameColP(),PL->getX(),PL->getY(),ST[i]->getX(),ST[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_GO;i++) // Ghost
 	   {
 	   	if(GO[i]->canHit() && GO[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),GO[i]->getActFrameColP(),PL->getX(),PL->getY(),GO[i]->getX(),GO[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_BA;i++) // Bat
 	   {
 	   	if(BA[i]->canHit() && BA[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),BA[i]->getActFrameColP(),PL->getX(),PL->getY(),BA[i]->getX(),BA[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_GI;i++) // Giant
 	   {
 	   	if(GI[i]->canHit() && GI[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),GI[i]->getActFrameColP(),PL->getX(),PL->getY(),GI[i]->getX(),GI[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_FW;i++) // Firewall
 	   {
 	   	if(FW[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),FW[i]->getActFrameColP(),PL->getX(),PL->getY(),FW[i]->getX(),FW[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_IN;i++) // Insect
 	   {
 	   	if(IN[i]->canHit() && IN[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),IN[i]->getActFrameColP(),PL->getX(),PL->getY(),IN[i]->getX(),IN[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_FG;i++) // Fire grizzly
 	   {
 	   	if(FG[i]->canHit() && FG[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),FG[i]->getActFrameColP(),PL->getX(),PL->getY(),FG[i]->getX(),FG[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_CL;i++) // Clam
 	   {
 	   	if(CL[i]->canHit() && CL[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),CL[i]->getActFrameColP(),PL->getX(),PL->getY(),CL[i]->getX(),CL[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_TU;i++) // Turtle
 	   {
 	   	if(TU[i]->canHit() && TU[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),TU[i]->getActFrameColP(),PL->getX(),PL->getY(),TU[i]->getX(),TU[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_DE;i++) // Demon
 	   {
 	   	if(DE[i]->canHit() && DE[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),DE[i]->getActFrameColP(),PL->getX(),PL->getY(),DE[i]->getX(),DE[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_LD;i++) // Little devil
 	   {
 	   	if(LD[i]->canHit() && LD[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),LD[i]->getActFrameColP(),PL->getX(),PL->getY(),LD[i]->getX(),LD[i]->getY(),7,2,2))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_BC;i++) // Big crab
 	   {
 	   	if(BC[i]->canHit() && BC[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),BC[i]->getActFrameColP(),PL->getX(),PL->getY(),BC[i]->getX(),BC[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_PO;i++) // Pork
 	   {
 	   	if(PO[i]->canHit() && PO[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),PO[i]->getActFrameColP(),PL->getX(),PL->getY(),PO[i]->getX(),PO[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_MA;i++) // Magician
 	   {
 	   	if(MA[i]->canHit() && MA[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),MA[i]->getActFrameColP(),PL->getX(),PL->getY(),MA[i]->getX(),MA[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_FL;i++) // Flower
 	   {
 	   	if(FL[i]->canHit() && FL[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),FL[i]->getActFrameColP(),PL->getX(),PL->getY(),FL[i]->getX(),FL[i]->getY()))
 	      PL->setHitted(true);
 	   }
 	  for(int i=0;i<N_IMMOBILE;i++) // Immobile enemies
 	   {
 	   	if(IMMOBILE[i]->isEnabled())
 	     if(hitTestFast8(PL->getActFrameColP(),IMMOBILE[i]->getActFrameColP(),PL->getX(),PL->getY(),IMMOBILE[i]->getX(),IMMOBILE[i]->getY(),6,2,2))
 	      PL->setHitted(true);
 	   }
 	 }
 }

void collisionWeaponEnemies()
 {
  // Weapon con enemies
  for(int j=0;j<PL->getNWeapon();j++)
   {
 	  if(PL->getWeapon(j)->isFired() && PL->getWeapon(j)->canHit())
 	   {
 	    for(int i=0;i<N_SK;i++) // Skeleton
 	     {
 	     	if(SK[i]->isHittable() && SK[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),SK[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),SK[i]->getX(),SK[i]->getY(),4,0,0))
 	        { SK[i]->setHitted(true); SCORE+=SK[i]->getScore(); PL->getWeapon(j)->hit(); return; }
 	     }
 	    for(int i=0;i<N_ZO;i++) // Zombie
 	     {
 	      if(ZO[i]->isHittable() && ZO[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),ZO[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),ZO[i]->getX(),ZO[i]->getY(),4,0,0))
 	        { ZO[i]->setHitted(true); SCORE+=ZO[i]->getScore(); PL->getWeapon(j)->hit(); return; }
 	     }
 	    for(int i=0;i<N_BL;i++) // Blade
 	     {
          if(BL[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),BL[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),BL[i]->getX(),BL[i]->getY(),4,0,0))
 	        { PL->getWeapon(j)->hitHard(); return; }
 	     }
 	    for(int i=0;i<N_RF;i++) // Rockfall
 	     {
          if(RF[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),RF[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),RF[i]->getX(),RF[i]->getY(),4,0,0))
 	        { PL->getWeapon(j)->hitHard(); return; }
 	     }
 	    for(int i=0;i<N_VO;i++) // Volture
 	     {
 	      if(VO[i]->isHittable() && VO[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),VO[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),VO[i]->getX(),VO[i]->getY(),4,0,0))
 	        { VO[i]->setHitted(true); SCORE+=VO[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_RA;i++) // Red arremer
 	     {
 	     	if(RA[i]->isHittable() && RA[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),RA[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),RA[i]->getX(),RA[i]->getY(),4,0,0))
 	        { RA[i]->setHitted(true); SCORE+=RA[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_RK;i++) // Red arremer King
 	     {
 	     	if(RK[i]->isHittable() && RK[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),RK[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),RK[i]->getX(),RK[i]->getY(),4,0,0))
 	        { RK[i]->setHitted(true); SCORE+=RK[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_P2;i++) // Plant mk2
 	     {
 	     	if(P2[i]->isHittable() && P2[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),P2[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),P2[i]->getX(),P2[i]->getY(),4,0,0))
 	        { P2[i]->setHitted(true); SCORE+=P2[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_ST;i++) // Stalagmite
 	     {
          if(ST[i]->isHittable() && ST[i]->isEnabled())
           if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),ST[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),ST[i]->getX(),ST[i]->getY(),4,0,0))
 	        { ST[i]->setHitted(true); SCORE+=ST[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_GO;i++) // Ghost
 	     {
 	     	if(GO[i]->isHittable() && GO[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),GO[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),GO[i]->getX(),GO[i]->getY(),4,0,0))
 	        { GO[i]->setHitted(true); SCORE+=GO[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_BA;i++) // Bat
 	     {
 	     	if(BA[i]->isHittable() && BA[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),BA[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),BA[i]->getX(),BA[i]->getY(),4,0,0))
 	        { BA[i]->setHitted(true); SCORE+=BA[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_GI;i++) // Giant
 	     {
 	     	if(GI[i]->isHittable() && GI[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),GI[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),GI[i]->getX(),GI[i]->getY(),4,0,0))
 	        { GI[i]->setHitted(true); SCORE+=GI[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_FW;i++) // Firewall
 	     {
 	      if(FW[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),FW[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),FW[i]->getX(),FW[i]->getY(),4,0,10))
 	        { PL->getWeapon(j)->hitHard(); return; }
 	     }
 	    for(int i=0;i<N_IN;i++) // Insect
 	     {
 	     	if(IN[i]->isHittable() && IN[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),IN[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),IN[i]->getX(),IN[i]->getY(),4,0,0))
 	        { IN[i]->setHitted(true); SCORE+=IN[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_FG;i++) // Fire grizzly
 	     {
 	     	if(FG[i]->isHittable() && FG[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),FG[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),FG[i]->getX(),FG[i]->getY(),4,0,0))
 	        { FG[i]->setHitted(true); SCORE+=FG[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_CL;i++) // Clam
 	     {
 	     	if(CL[i]->isHittable() && CL[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),CL[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),CL[i]->getX(),CL[i]->getY(),4,0,0))
 	        { CL[i]->setHitted(true); SCORE+=CL[i]->getScore(); PL->getWeapon(j)->hitSoft(); return; }
 	     }
 	    for(int i=0;i<N_TU;i++) // Turtle
 	     {
 	     	if(TU[i]->isEnabled())
 	     	 if(TU[i]->isHittable() || TU[i]->isHard())
 	        if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),TU[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),TU[i]->getX(),TU[i]->getY(),4,0,0))
 	         { if(TU[i]->isHard()) PL->getWeapon(j)->hitHard(); else { TU[i]->setHitted(true); SCORE+=TU[i]->getScore(); PL->getWeapon(j)->hitSoft(); } return; }
 	     }
 	    for(int i=0;i<N_DE;i++) // Demon
 	     {
 	      if(DE[i]->isEnabled())
 	       if(DE[i]->isHittable() || DE[i]->isHard())
 	        if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),DE[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),DE[i]->getX(),DE[i]->getY(),4,0,0))
 	         { if(DE[i]->isHard()) PL->getWeapon(j)->hitHard(); else { DE[i]->setHitted(true); SCORE+=DE[i]->getScore(); PL->getWeapon(j)->hitSoft(); } return; }
 	     }
 	    for(int i=0;i<N_LD;i++) // Little devil
 	     {
 	     	if(LD[i]->isEnabled())
 	     	 if(LD[i]->isHittable() || LD[i]->isHard())
 	        if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),LD[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),LD[i]->getX(),LD[i]->getY(),4,0,0))
 	         { if(LD[i]->isHard()) PL->getWeapon(j)->hitHard(); else { LD[i]->setHitted(true); SCORE+=LD[i]->getScore(); PL->getWeapon(j)->hitSoft(); } return; }
 	     }
 	    for(int i=0;i<N_BC;i++) // Big crab
 	     {
 	     	if(BC[i]->isEnabled())
 	     	 if(BC[i]->isHittable() || BC[i]->isHard())
 	        if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),BC[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),BC[i]->getX(),BC[i]->getY(),4,0,0))
 	         { PL->getWeapon(j)->hitHard(); return; }
 	     }
 	    for(int i=0;i<N_PO;i++) // Pork
 	     {
 	     	if(PO[i]->isEnabled())
 	     	 if(PO[i]->isHittable() || PO[i]->isHard())
 	        if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),PO[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),PO[i]->getX(),PO[i]->getY(),4,0,0))
 	         { if(PO[i]->isHard()) PL->getWeapon(j)->hitHard(); else { PO[i]->setHitted(true); SCORE+=PO[i]->getScore(); PL->getWeapon(j)->hitSoft(); } return; }
 	     }
 	    for(int i=0;i<N_MA;i++) // Magician
 	     {
 	     	if(MA[i]->isEnabled())
 	     	 if(MA[i]->isHittable() || MA[i]->isHard())
 	        if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),MA[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),MA[i]->getX(),MA[i]->getY(),4,0,0))
 	         { if(MA[i]->isHard()) PL->getWeapon(j)->hitHard(); else { MA[i]->setHitted(true); SCORE+=MA[i]->getScore(); PL->getWeapon(j)->hitSoft(); } return; }
 	     }
 	    for(int i=0;i<N_FL;i++) // Flower
 	     {
 	     	if(FL[i]->isEnabled())
 	     	 if(FL[i]->isHittable() || FL[i]->isHard())
 	        if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),FL[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),FL[i]->getX(),FL[i]->getY(),4,0,0))
 	         { if(FL[i]->isHard()) PL->getWeapon(j)->hitHard(); else { FL[i]->setHitted(true); SCORE+=FL[i]->getScore(); PL->getWeapon(j)->hitSoft(); } return; }
 	     }
 	    for(int i=0;i<N_IMMOBILE;i++) // Immobile enemies
 	     {
          if(IMMOBILE[i]->isEnabled())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),IMMOBILE[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),IMMOBILE[i]->getX(),IMMOBILE[i]->getY(),6,4,2))
 	        { PL->getWeapon(j)->hitHard(); return; }
 	     }
 	    // Weapon con magic box
 	    for(int i=0;i<=11;i++)
 	     {
 	      if(MOBILE[i]->isHittable() && MOBILE[i]->isActive())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),MOBILE[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),MOBILE[i]->getX(),MOBILE[i]->getY(),4,4,2))
 	        {
 	         PL->getWeapon(j)->hitSoft(); MOBILE[i]->setHitted(true);
 	         if(MOBILE[i]->getEnergy()==1)
 	          {
 	           if(MOBILE[i]->getBonusSeq(PL->getDress(),PL->getShild(),PL->getPowerUp())==0) { MA[i]->setPosX(*MOBILE[i]->getPosXP()+6); MA[i]->setPosY(*MOBILE[i]->getPosYP()-8); MA[i]->startAction(); } // Magician
           	   BONUSB[i]->setStartX(*MOBILE[i]->getPosXP()); BONUSB[i]->setStartY(*MOBILE[i]->getPosYP()-10); BONUSB[i]->resetObj(true,MOBILE[i]->getBonusSeq(PL->getDress(),PL->getShild(),PL->getPowerUp()));
 	          }
 	         return;
 	        }
 	     } 	    
 	    // Weapon con mobile and static platforms
 	    for(int i=12;i<N_PLAT;i++)
 	     {
 	      if(MOBILE[i]->isHittable() && MOBILE[i]->isActive())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),MOBILE[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),MOBILE[i]->getX(),MOBILE[i]->getY(),4,MOBILE[i]->getHHit(),MOBILE[i]->getVHit()))
 	        { PL->getWeapon(j)->hitHard(); return; }
 	     }
 	   }
 	 }
 }

void collisionMagicEnemies()
 {
  // Magic con enemies
  if(PL->getMagic()->isFired() && PL->getMagic()->canHit())
   {
    for(int i=0;i<N_SK;i++) // Skeleton
     {
      if(SK[i]->isHittable() && SK[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),SK[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),SK[i]->getX(),SK[i]->getY()))
         { SK[i]->setHitted(true); SCORE+=SK[i]->getScore(); /*PL->getMagic()->hit();*/ return; }
     }
    for(int i=0;i<N_ZO;i++) // Zombie
 	 {
 	  if(ZO[i]->isHittable() && ZO[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)
 	    if(hitTestFast8(PL->getMagic()->getActFrameColP(j),ZO[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),ZO[i]->getX(),ZO[i]->getY()))
 	     { ZO[i]->setHitted(true); SCORE+=ZO[i]->getScore(); /*PL->getWeapon(j)->hit();*/ return; }
 	 }
 	    /*for(int i=0;i<N_BL;i++) // Blade
 	     {
 	     	if(BL[i]->isActive())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),BL[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),BL[i]->getX(),BL[i]->getY(),4,4,2))
 	        { PL->getMagic()->hitHard(j); }
 	     }*/
    for(int i=0;i<N_VO;i++) // Volture
 	 {
 	  if(VO[i]->isHittable() && VO[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)    
 	    if(hitTestFast8(PL->getMagic()->getActFrameColP(j),VO[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),VO[i]->getX(),VO[i]->getY(),4,6,2))
 	     { VO[i]->setHitted(true); SCORE+=VO[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
 	 }
 	for(int i=0;i<N_RA;i++) // Red arremer
 	 {
 	  if(RA[i]->isHittable() && RA[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)
 	    if(hitTestFast8(PL->getMagic()->getActFrameColP(j),RA[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),RA[i]->getX(),RA[i]->getY()))
 	     { RA[i]->setHitted(true); SCORE+=RA[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
 	 }
 	for(int i=0;i<N_RK;i++) // Red arremer King
 	 {
 	  if(RK[i]->isHittable() && RK[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)
 	    if(hitTestFast8(PL->getMagic()->getActFrameColP(j),RK[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),RK[i]->getX(),RK[i]->getY()))
 	     { RK[i]->setHitted(true); SCORE+=RK[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
 	 }
    for(int i=0;i<N_P2;i++) // Plant mk2
     {
      if(P2[i]->isHittable() && P2[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),P2[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),P2[i]->getX(),P2[i]->getY()))
         { P2[i]->setHitted(true); SCORE+=P2[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
    for(int i=0;i<N_ST;i++) // Stalagtite
     {
      if(ST[i]->isHittable() && ST[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),ST[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),ST[i]->getX(),ST[i]->getY()))
         { ST[i]->setHitted(true); SCORE+=ST[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
 	for(int i=0;i<N_GO;i++) // Ghost
 	 {
 	  if(GO[i]->isHittable() && GO[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),GO[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),GO[i]->getX(),GO[i]->getY()))
 	     { GO[i]->setHitted(true); SCORE+=GO[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
 	 }
    for(int i=0;i<N_BA;i++) // Bat
     {
      if(BA[i]->isHittable() && BA[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),BA[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),BA[i]->getX(),BA[i]->getY(),4,4,2))
         { BA[i]->setHitted(true); SCORE+=BA[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
    for(int i=0;i<N_GI;i++) // Giant
     {
      if(GI[i]->isHittable() && GI[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),GI[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),GI[i]->getX(),GI[i]->getY()))
         { GI[i]->setHitted(true); SCORE+=GI[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     } 	    
 	    /*for(int i=0;i<N_FW;i++) // Firewall
 	     {
 	     	if(FW[i]->isActive())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),FW[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),FW[i]->getX(),FW[i]->getY(),4,0,10))
 	        { PL->getMagic()->hitHard(j); }
 	     }*/
 	for(int i=0;i<N_IN;i++) // Insect
 	 {
 	  if(IN[i]->isHittable() && IN[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),IN[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),IN[i]->getX(),IN[i]->getY()))
 	       { IN[i]->setHitted(true); SCORE+=IN[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
 	 }
    for(int i=0;i<N_FG;i++) // Fire grizzly
     {
      if(FG[i]->isHittable() && FG[i]->isEnabled())
 	   for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),FG[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),FG[i]->getX(),FG[i]->getY()))
         { FG[i]->setHitted(true); SCORE+=FG[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
    for(int i=0;i<N_CL;i++) // Clam
     {
      if(CL[i]->isHittable() && CL[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),CL[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),CL[i]->getX(),CL[i]->getY(),4,6,2))
         { CL[i]->setHitted(true); SCORE+=CL[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
    for(int i=0;i<N_TU;i++) // Turtle
     {
      if(TU[i]->isHittable() && TU[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),TU[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),TU[i]->getX(),TU[i]->getY()))
         { TU[i]->setHitted(true); SCORE+=TU[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
    for(int i=0;i<N_DE;i++) // Demon
     {
      if(DE[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),DE[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),DE[i]->getX(),DE[i]->getY()))
         {
		  if(DE[i]->isHittable()) { DE[i]->setHitted(true); SCORE+=DE[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
		  if(DE[i]->isHard())     { PL->getMagic()->hitHard(j); return; }
         }
     }
    for(int i=0;i<N_LD;i++) // Little devil
     {
      if(LD[i]->isHittable() && LD[i]->isEnabled())
       for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),LD[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),LD[i]->getX(),LD[i]->getY(),4,4,2))
         { LD[i]->setHitted(true); SCORE+=LD[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
 	    /*for(int i=0;i<N_BC;i++) // Big crab
 	     {
 	     	if(BC[i]->isActive())
 	       if(hitTestFast8(PL->getWeapon(j)->getActFrameColP(),BC[i]->getActFrameColP(),PL->getWeapon(j)->getX(),PL->getWeapon(j)->getY(),BC[i]->getX(),BC[i]->getY(),4,4,2))
 	        { PL->getMagic()->hitHard(j); }
 	     }*/
    for(int i=0;i<N_PO;i++) // Pork
     {
     	if(PO[i]->isHittable() && PO[i]->isEnabled())
     	 for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),PO[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),PO[i]->getX(),PO[i]->getY()))
         { PO[i]->setHitted(true); SCORE+=PO[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
    for(int i=0;i<N_MA;i++) // Magician
     {
     	if(MA[i]->isHittable() && MA[i]->isEnabled())
     	 for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),MA[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),MA[i]->getX(),MA[i]->getY()))
         { MA[i]->setHitted(true); SCORE+=MA[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
    for(int i=0;i<N_FL;i++) // Flower
     {
     	if(FL[i]->isHittable() && FL[i]->isEnabled())
     	 for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),FL[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),FL[i]->getX(),FL[i]->getY()))
         { FL[i]->setHitted(true); SCORE+=FL[i]->getScore(); PL->getMagic()->hitSoft(j); return; }
     }
 	  // Magic con magic box
 	  for(int i=0;i<=11;i++)
 	   {
 	   	if(MOBILE[i]->isHittable() && MOBILE[i]->isActive())
 	   	 for(int j=0;j<PL->getNMagic();j++)
        if(hitTestFast8(PL->getMagic()->getActFrameColP(j),MOBILE[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),MOBILE[i]->getX(),MOBILE[i]->getY(),6,6,3))
 	       {
 	        PL->getMagic()->hitSoft(j); MOBILE[i]->setHitted(true);
 	        if(MOBILE[i]->getEnergy()==1) 
 	         {
 	         	if(MOBILE[i]->getBonusSeq(PL->getDress(),PL->getShild(),PL->getPowerUp())==0) { MA[i]->setPosX(*MOBILE[i]->getPosXP()+6); MA[i]->setPosY(*MOBILE[i]->getPosYP()-8); MA[i]->startAction(); } // Magician
 	         	BONUSB[i]->setStartX(*MOBILE[i]->getPosXP()); BONUSB[i]->setStartY(*MOBILE[i]->getPosYP()-10); BONUSB[i]->resetObj(true,MOBILE[i]->getBonusSeq(PL->getDress(),PL->getShild(),PL->getPowerUp()));
 	         }
 	        return;
 	       }
 	   }	    
 	  // Magic con mobile and static platforms
 	  for(int i=12;i<N_PLAT;i++)
 	   {
 	   	if(MOBILE[i]->isHittable() && MOBILE[i]->isActive())
 	   	 for(int j=0;j<PL->getNMagic();j++)
 	   	  if(hitTestFast8(PL->getMagic()->getActFrameColP(j),MOBILE[i]->getActFrameColP(),PL->getMagic()->getX(j),PL->getMagic()->getY(j),MOBILE[i]->getX(),MOBILE[i]->getY(),4,MOBILE[i]->getHHit(),MOBILE[i]->getVHit()))
 	       { PL->getMagic()->hitHard(j); return; /*break;*/ }
 	   }
   }
 }

void collisionPlayerBullets()
 {
  // Player con bullets
  if(PL->isHittable() && !PL->isGold() && !PL->isMagicFire())
   {
    for(int i=0;i<N_P2;i++) // Plant mk2 bullets
     {
   	  for(int j=0;j<P2[i]->getNBullet();j++)
   	   if(P2[i]->getBullet(j)->isFired() && P2[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(P2[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),P2[i]->getBullet(j)->getX(),P2[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); P2[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_ST;i++) // Stalagtite bullets
     {
   	  for(int j=0;j<ST[i]->getNBullet();j++)
   	   if(ST[i]->getBullet(j)->isFired() && ST[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(ST[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),ST[i]->getBullet(j)->getX(),ST[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); ST[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_GI;i++) // Giant bullets
     {
   	  for(int j=0;j<GI[i]->getNBullet();j++)
   	   if(GI[i]->getBullet(j)->isFired() && GI[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(GI[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),GI[i]->getBullet(j)->getX(),GI[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); GI[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_FG;i++) // Fire grizzly bullets
     {
   	  for(int j=0;j<FG[i]->getNBullet();j++)
   	   if(FG[i]->getBullet(j)->isFired() && FG[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(FG[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),FG[i]->getBullet(j)->getX(),FG[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); FG[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_CL;i++) // Clam bullets
     {
   	  for(int j=0;j<CL[i]->getNBullet();j++)
   	   if(CL[i]->getBullet(j)->isFired() && CL[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(CL[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),CL[i]->getBullet(j)->getX(),CL[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); CL[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_TU;i++) // Turtle bullets
     {
   	  for(int j=0;j<TU[i]->getNBullet();j++)
   	   if(TU[i]->getBullet(j)->isFired() && TU[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(TU[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),TU[i]->getBullet(j)->getX(),TU[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); TU[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_DE;i++) // Demon bullets
     {
   	  for(int j=0;j<DE[i]->getNBullet();j++)
   	   if(DE[i]->getBullet(j)->isFired() && DE[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(DE[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),DE[i]->getBullet(j)->getX(),DE[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); DE[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_LD;i++) // Little devil bullets
     {
   	  for(int j=0;j<LD[i]->getNBullet();j++)
   	   if(LD[i]->getBullet(j)->isFired() && LD[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(LD[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),LD[i]->getBullet(j)->getX(),LD[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); LD[i]->getBullet(j)->hit(); return; }
     }
    for(int i=0;i<N_MA;i++) // Magician magic bullets (TODO Player transformation instead of hit...)
     {
   	  for(int j=0;j<MA[i]->getNBullet();j++)
   	   if(MA[i]->getBullet(j)->isFired() && MA[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(MA[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),MA[i]->getBullet(j)->getX(),MA[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); MA[i]->hitAllBullets(); return; }
     }
    for(int i=0;i<N_FL;i++) // Flower bullets
     {
   	  for(int j=0;j<FL[i]->getNBullet();j++)
   	   if(FL[i]->getBullet(j)->isFired() && FL[i]->getBullet(j)->canHit())
   	    if(hitTestFast8(FL[i]->getBullet(j)->getActFrameColP(),PL->getActFrameColP(),FL[i]->getBullet(j)->getX(),FL[i]->getBullet(j)->getY(),PL->getX(),PL->getY(),4,0,0))
         { PL->setHitted(true); FL[i]->getBullet(j)->hit(); return; }
     }
   }
 }

void rePaint(int max_fps)
 {
 	int diff;
 	
 	// Posteffects (experimental)
 	if(SCANLINES) _scanline(VSCR,SCANLINES);
 	// Blur
 	/*for(int x=1;x<VSCR->w-1;x+=3)
 	 for(int y=1;y<VSCR->h-1;y++)
 	  {
 	   int col0=getpixel(VSCR,x-1,y);
 	   int col1=getpixel(VSCR,x,y);
 	   int col2=getpixel(VSCR,x+1,y);

     int r0=getr(col0);
     int g0=getg(col0);
     int b0=getb(col0);
 	     
     int r1=getr(col1);
     int g1=getg(col1);
     int b1=getb(col1);

     int r2=getr(col2);
     int g2=getg(col2);
     int b2=getb(col2);

     int r=(r0+r1+r1+r2)>>2;
     int g=(g0+g1+g1+g2)>>2;
     int b=(b0+b1+b1+b2)>>2;
       
     int col=makecol32(r,g,b);
     putpixel(VSCR,x,y,col);
 	  }*/ 	
 	// B&W
 	/*for(int x=0;x<VSCR->w;x++)
 	 for(int y=0;y<VSCR->h;y++)
 	  {
 	   int col=getpixel(VSCR,x,y);
     int r=getr(col);
     int g=getg(col);
     int b=getb(col);

 	   int sum=r+g+b;
 	   if(sum>384) r=g=b=0; else r=g=b=255;

     putpixel(VSCR,x,y,makecol32(r,g,b));
 	  }*/
 	// Grayscale
 	/*for(int x=0;x<VSCR->w;x++)
 	 for(int y=0;y<VSCR->h;y++)
 	  {
 	   int col=getpixel(VSCR,x,y);
     int r=getr(col);
     int g=getg(col);
     int b=getb(col);

 	   int sum=(r+g+b)/3;
     putpixel(VSCR,x,y,makecol32(sum,sum,sum));
 	  }*/
 	// Underwater
 	/*for(int x=0;x<VSCR->w;x++)
 	 for(int y=0;y<VSCR->h;y++)
 	  {
 	   int col=getpixel(VSCR,x,y);
     putpixel(VSCR,x+random(3)-2,y+random(3)-2,col);
 	  }*/
 	// Quake
 	/*if(random(2)==0)
 	 {
 	 	int dx=random(2); int dy=random(2);
 	  for(int x=0;x<VSCR->w;x++)
 	   for(int y=0;y<VSCR->h;y++)
 	    {
 	     int col=getpixel(VSCR,x,y);
       putpixel(VSCR,x-dx,y-dy,col);
 	    }
 	 }*/

 	if(STRETCH) stretch_blit(VSCR,screen,0,0,G_RESX,G_RESY,0,0,SCREENX,SCREENY);
 	else blit(VSCR,screen,0,0,(SCREENX-G_RESX)/2,(SCREENY-G_RESY)/2,SCREENX,SCREENY);
 	if(VSYNC) vsync();
 	if(AR_Clock()>=MS+MSEC_TO_TIMER(1)) { MS=AR_Clock(); fps=FPS; FPS=0; } FPS++;
 	diff=(MSEC_TO_TIMER(1)/max_fps)-(AR_Clock()-OMS); if(diff>0) wait(diff);
 	OMS=AR_Clock();
 }

void debug(BITMAP *bmp, int alpha)
 {
  if(key[KEY_F1])   { L_0=!L_0;     key[KEY_F1]=0; }
  if(key[KEY_F2])   { L_1=!L_1;     key[KEY_F2]=0; }
  if(key[KEY_F3])   { L_S=!L_S;     key[KEY_F3]=0; }
  if(key[KEY_F4])   { L_H=!L_H;     key[KEY_F4]=0; }
  if(key[KEY_F5])   { L_OSD=!L_OSD; key[KEY_F5]=0; }
  if(key[KEY_F6])   { L_B=!L_B;     key[KEY_F6]=0; }
  if(key[KEY_F7])   { VSYNC=!VSYNC; key[KEY_F7]=0; }
  if(key[KEY_F8])   { DBUG=!DBUG;   key[KEY_F8]=0; }
  if(key[KEY_F9])   { SCANLINES++;  if(SCANLINES>3) SCANLINES=0; key[KEY_F9]=0; }
  if(key[KEY_PGUP]) { MFPS++;       key[KEY_PGUP]=0; }
  if(key[KEY_PGDN]) { MFPS--;       key[KEY_PGDN]=0; }
  if(key[KEY_1])    { PL->takeBonus(3,LANCE); key[KEY_1]=0; }
  if(key[KEY_2])    { PL->takeBonus(3,KNIFE); key[KEY_2]=0; }
  if(key[KEY_3])    { PL->takeBonus(3,AXE);   key[KEY_3]=0; }
  if(key[KEY_4])    { PL->takeBonus(3,SWORD); key[KEY_4]=0; }
  if(key[KEY_5])    { PL->takeBonus(3,ARROW); key[KEY_5]=0; }
  if(key[KEY_6])    { PL->takeBonus(3,FLAME); key[KEY_6]=0; }	
  if(key[KEY_D]) PL->setDress(2);
  if(key[KEY_F]) PL->takeBonus(5);
  if(key[KEY_G]) PL->takeBonus(7);
  if(DBUG)
   {
    textprintf_ex(bmp,font,0,216,makecol32(255,0,0),-1,"V.%s",VER);
    textprintf_ex(bmp,font,0,0, makecol32(255,0,0),-1,"          | FPS: %d | Bonus seq.: %d | P. Layer %d",fps,BONUS_SEQ,PL->getActPlatLayer());
    textprintf_ex(bmp,font,0,12,makecol32(255,0,0),-1,"(Pl) PosX:%d PosY:%d GlobX:%d GlobY:%d",PL->getPosX(),PL->getPosY(),PL->getGlobX(),PL->getGlobY());
 	textprintf_ex(bmp,font,0,24,makecol32(255,0,0),-1,"(Pl) ActTileX:%d (OS:%d) ActTileY:%d (OS:%d)",PL->getGlobX()/MAP1[STAGE]->getTileW(),PL->getGlobX()%MAP1[STAGE]->getTileW(),PL->getGlobY()/MAP1[STAGE]->getTileH(),PL->getGlobY()%MAP1[STAGE]->getTileH());
 	textprintf_ex(bmp,font,0,36,makecol32(255,0,0),-1,"(M1) ActTileY:%d (Offset:%d)",MAP1[STAGE]->getActTileY(),MAP1[STAGE]->getTileOffsetY());
 	textprintf_ex(bmp,font,0,48,makecol32(255,0,0),-1,"(M1) ScrollX:%d  ScrollY:%d | CP: %d | RT: %d",MAP1[STAGE]->getScrollX(),MAP1[STAGE]->getScrollY(),CKPOINT,RETIME);
 	textprintf_ex(bmp,font,0,60,makecol32(255,0,0),-1,"Vase:");
 	for(int i=0;i<N_BVASE;i+=2) textprintf_ex(bmp,font,0+48+i*32,60,makecol32(255,0,0),-1,"%d.[%d %d]",i/2,BVASE[i]->isActive(),BVASE[i+1]->isActive());
 	PL->debug();
 	for(int i=0;i<N_PLAT;i++) MOBILE[i]->debug();
 	MAP1[STAGE]->debug();
   }
 }
