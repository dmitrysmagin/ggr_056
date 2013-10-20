class timer
 {
 	private:
 	 bool on;
 	 clock_t ms;
 	 int tick;
 	 int count;
 	 int clock2();

 	public:
 	 timer(int);
 	 ~timer();
 	 bool isOn();
 	 void time();
 	 void reset();
 };

timer::timer(int tms)
 {
 	count=0;
 	tick=tms/10;
  ms=clock2(); on=0;
 }

timer::~timer() { }

int timer::clock2() {	return count; }

bool timer::isOn() { return on; }

void timer::time()
 {
 	count++;
  if(on) ms=clock2(); on=0;
  if(clock2()-ms>tick) on=1; else on=0;
 }

void timer::reset()
 {
  ms=clock2(); on=0;
 }
