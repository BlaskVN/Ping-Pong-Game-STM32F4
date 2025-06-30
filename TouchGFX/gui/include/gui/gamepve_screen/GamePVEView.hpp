#ifndef GAMEPVEVIEW_HPP
#define GAMEPVEVIEW_HPP

#include <gui_generated/gamepve_screen/GamePVEViewBase.hpp>

class GamePVEView : public GamePVEViewBase
{
public:
    GamePVEView();
    virtual ~GamePVEView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    // Game management methods
    void resetGame();          // Reset game to initial state
    void resetBall();          // Reset ball to center position
    void updateScoreDisplay(); // Update score display on UI
    void handleGameInput();    // Handle input when game is over (restart)

    // AI methods
    void updateAI(); // Update AI movement logic

private:
};

#endif // GAMEPVEVIEW_HPP
