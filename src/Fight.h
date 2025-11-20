#ifndef FIGHT_H
#define FIGHT_H

class Ant;

class Fight
{
    
    protected:
        
    private:
        
    public:
        Ant * ant[2];
        Fight(Ant & ant1, Ant & ant2);
        
        bool is_equal(Fight & other);
        void update();
        bool valid() const;
    
};
#endif
