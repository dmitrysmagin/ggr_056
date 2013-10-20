class timer
 {
 	private:
 	 bool on;
 	 clock_t ms;
 	 int tick;

 	public:
 	 timer(int);
 	 ~timer();
 	 bool isOn();
 	 void time();
 	 void reset();
 };

timer::timer(int tms)
 {
 	tick=tms;
  ms=clock(); on=0;
 }

timer::~timer() { }

bool timer::isOn() { return on; }

void timer::time()
 {
  if(on) ms=clock(); on=0;
  if(clock()-ms>tick) on=1; else on=0;
 }

void timer::reset()
 {
  ms=clock(); on=0;
 }
