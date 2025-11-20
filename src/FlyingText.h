#ifndef FLYINGTEXT_H
#define FLYINGTEXT_H

#include "GameObject.h"
#include <string>


class FlyingText: public GameObject
{

	public:
		bool goingDown;
		int lifetime;
		std::string text;
		
		//float offset;
		//bool direction;
		
		FlyingText(float x, float y, const std::string & text, bool goDown);
		
		void update();
		void draw() const;
		
};
#endif
