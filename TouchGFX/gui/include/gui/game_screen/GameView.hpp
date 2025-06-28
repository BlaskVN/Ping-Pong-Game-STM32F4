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
    
    // Game state management
    void resetGame(); // Reset game to initial state
    void resetBall(); // Reset ball to center position
    void updateScoreDisplay(); // Update score display on UI
    void handleGameInput(); // Handle input when game is over (restart)
    int getPlayer1Score() const { return player1_score; }
    int getPlayer2Score() const { return player2_score; }
    bool isGameOver() const { return game_over; }

protected:
    float ball_dx; // Velocity in X direction
    float ball_dy; // Velocity in Y direction
    const float ball_speed; // Speed of the ball (e.g., 3.0f pixels per tick)
    
    // Game state variables
    int player1_score; // Player 1 (bottom pad) score
    int player2_score; // Player 2 (top pad) score
    bool game_over; // Game over flag
    bool ball_reset_pending; // Flag to reset ball position
    int reset_timer; // Timer for ball reset delay
    int game_over_timer; // Timer for auto-restart after game over
    const int MAX_SCORE; // Score to win (e.g., 5 points)
    const int RESET_DELAY; // Delay before ball reset (in ticks)
};

#endif // GAMEVIEW_HPP
