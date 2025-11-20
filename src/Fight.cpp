#include "Fight.h"
#include "Ant.h"

Fight::Fight(Ant & ant1, Ant & ant2): ant{ &ant1, &ant2 }
{
    
}


bool Fight::is_equal(Fight & other)
{
    return (this->ant[0] == other.ant[0] && this->ant[1] == other.ant[1]);
    //return ( (this->ant[0] == other.ant[0] && this->ant[1] == other.ant[1]) || 
    //         (this->ant[0] == other.ant[1] && this->ant[1] == other.ant[0]) );
}


void Fight::update()
{
    
    if (valid() )
    {
        ant[0]->hit(*ant[1]);
    }
    
}


bool Fight::valid() const
{
    return (ant[0] && ant[0]->active && ant[1] && ant[1]->active);
}
