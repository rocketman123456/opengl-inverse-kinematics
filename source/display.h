#ifndef DISPLAY_H
#define DISPLAY_H

class Display
{
public:	
	Display();
	virtual ~Display();

	void init(int argc, char** argv, int limbs);
};

#endif //DISPLAY_H