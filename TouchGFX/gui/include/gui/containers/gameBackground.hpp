#ifndef GAMEBACKGROUND_HPP
#define GAMEBACKGROUND_HPP

#include <gui_generated/containers/gameBackgroundBase.hpp>

class gameBackground : public gameBackgroundBase
{
public:
    gameBackground();
    virtual ~gameBackground() {}

    virtual void initialize();
protected:
};

#endif // GAMEBACKGROUND_HPP
