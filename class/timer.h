class timer
 {
  private:
   bool on;
   clock_t ms;
   int tick;
   int count;
   int Clock();

  public:
   timer(int);
   ~timer();
   bool isOn();
   void time();
   void reset();
   void setTick(int);
 };

timer::timer(int tms)
 {
  count=0;
  tick=tms;
  ms=Clock(); on=0;
 }

timer::~timer() { }

int timer::Clock() { return count; }

bool timer::isOn() { return on; }

void timer::time()
 {
  count++;
  if(on) ms=Clock(); on=0;
  if(Clock()-ms>tick) on=1; else on=0;
 }

void timer::reset()
 {
  ms=Clock(); on=0;
 }

void timer::setTick(int tms) { tick=tms; }
