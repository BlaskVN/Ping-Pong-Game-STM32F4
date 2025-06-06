#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <gui_generated/game_screen/GameViewBase.hpp>
#include <gui/game_screen/GamePresenter.hpp>

class GameView : public GameViewBase
{
public:
    GameView();
    virtual ~GameView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    
    // Handle click event
    virtual void handleTickEvent(); // Handle tick event

protected:
    float ball_dx; // Velocity in X direction
    float ball_dy; // Velocity in Y direction
    const float ball_speed; // Speed of the ball (e.g., 3.0f pixels per tick)
};

#endif // GAMEVIEW_HPP
