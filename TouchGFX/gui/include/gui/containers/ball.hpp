#ifndef BALL_HPP
#define BALL_HPP

#include <gui_generated/containers/ballBase.hpp>

class ball : public ballBase
{
public:
    ball();
    virtual ~ball() {}

    virtual void initialize();
protected:
};

#endif // BALL_HPP
