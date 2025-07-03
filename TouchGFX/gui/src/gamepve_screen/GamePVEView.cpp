#include <gui/gamepve_screen/GamePVEView.hpp>
#include <touchgfx/widgets/Box.hpp> 
#include <cmath>                    

static float ball_dx = 0.0f;          
static float ball_dy = 0.0f;          
static const float ball_speed = 1.7f; 

static int player1_score = 0;           
static int ai_score = 0;                
static bool game_over = false;          
static bool ball_reset_pending = false; 
static int reset_timer = 0;            
static int game_over_timer = 0;        
static const int MAX_SCORE = 5;      
static const int RESET_DELAY = 120;    
static bool second_hit_allowed = false;
static int last_hit_tick = 0;
static constexpr int SECOND_HIT_DELAY = 30;

static float ai_target_x = 0.0f;               
static const float ai_speed = 2.8f;           
static const float ai_prediction_factor = 0.8f; 

GamePVEView::GamePVEView()
{
    ball_dx = 0.0f;
    ball_dy = 0.0f;
    player1_score = 0;
    ai_score = 0;
    game_over = false;
    ball_reset_pending = false;
    reset_timer = 0;
    game_over_timer = 0;
    second_hit_allowed = false;
    last_hit_tick = 0;
}

void GamePVEView::setupScreen()
{
    GamePVEViewBase::setupScreen();
    
    point_pad1.setRange(0, MAX_SCORE);
    point_pad2.setRange(0, MAX_SCORE);

    pad1_Win.setVisible(false);
    pad1_Lose.setVisible(false);
    
    resetGame(); 
}

void GamePVEView::tearDownScreen()
{
    GamePVEViewBase::tearDownScreen();
}

void GamePVEView::handleTickEvent()
{
    last_hit_tick++;

    if (ball_reset_pending)
    {
        reset_timer--;
        if (reset_timer <= 0)
        {
            resetBall();
            ball_reset_pending = false;
        }
        return; 
    }

    if (game_over)
    {
        handleGameInput(); 
        return;
    }

    updateAI();

    int ballX = ball1.getX();
    int ballY = ball1.getY();
    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();

    ballX += static_cast<int>(ball_dx);
    ballY += static_cast<int>(ball_dy);

    const int screenWidth = 240;
    const int screenHeight = 320;
    const int screenCenterY = screenHeight / 2;

    const int upperHalfMaxY = screenCenterY; 
    const int lowerHalfMinY = screenCenterY; 

    int ball_centerY_int = ballY + ballHeight / 2;

    if (ball_centerY_int <= upperHalfMaxY && std::abs(ball_dy) < 0.5f && std::abs(ball_dx) < 0.5f)
    {
        if (!game_over)
        {
            player1_score++;
            updateScoreDisplay();
            if (player1_score >= MAX_SCORE)
            {
                game_over = true;
                pad1_Win.setVisible(true);
                pad1_Win.invalidate();
            }
            else
            {
                ball_reset_pending = true;
                reset_timer = RESET_DELAY;
            }
            ballX = ballX;
            ballY = ballY;
            ball_dx = 0.0f;
            ball_dy = 0.0f;
        }
    }
    else if (ball_centerY_int >= lowerHalfMinY && std::abs(ball_dy) < 0.5f && std::abs(ball_dx) < 0.5f)
    {
        if (!game_over)
        {
            ai_score++;
            updateScoreDisplay();
            if (ai_score >= MAX_SCORE)
            {
                game_over = true;
                pad1_Lose.setVisible(true);
                pad1_Lose.invalidate();
            }
            else
            {
                ball_reset_pending = true;
                reset_timer = RESET_DELAY;
            }
            ballX = ballX;
            ballY = ballY;
            ball_dx = 0.0f;
            ball_dy = 0.0f;
        }
    }

    int pad1X = pad1.getX();
    int pad1Y = pad1.getY();
    if (pad1Y < lowerHalfMinY)
    {
        pad1Y = lowerHalfMinY;
        pad1.moveTo(pad1X, pad1Y);
    }

    if (pad1X < 0)
        pad1X = 0;
    if (pad1X + pad1.getWidth() > screenWidth)
        pad1X = screenWidth - pad1.getWidth();
    pad1.moveTo(pad1X, pad1Y);

    if (pad1Y + pad1.getHeight() > screenHeight)
    {
        pad1Y = screenHeight - pad1.getHeight();
    }
    pad1.moveTo(pad1X, pad1Y);

    int pad2X = pad2.getX();
    int pad2Y = pad2.getY();
    if (pad2Y < 0)
    {
        pad2Y = 0;
    }
    if (pad2Y + pad2.getHeight() > upperHalfMaxY)
    {
        pad2Y = upperHalfMaxY - pad2.getHeight();
    }
    pad2.moveTo(pad2X, pad2Y);

    if (pad2X < 0)
        pad2X = 0;
    if (pad2X + pad2.getWidth() > screenWidth)
        pad2X = screenWidth - pad2.getWidth();
    pad2.moveTo(pad2X, pad2Y);

    if (ballX <= 0)
    {
        ballX = 0;
        ball_dx = -ball_dx;
    }
    else if (ballX + ballWidth >= screenWidth)
    {
        ballX = screenWidth - ballWidth;
        ball_dx = -ball_dx;
    }

    if (ballY <= 0)
    {
        if (!game_over)
        {
            player1_score++;
            updateScoreDisplay();
            if (player1_score >= MAX_SCORE)
            {
                game_over = true;
                pad1_Win.setVisible(true);
                pad1_Win.invalidate();
            }
            else
            {
                ball_reset_pending = true;
                reset_timer = RESET_DELAY;
            }
        }
        ballY = 0;
        ball_dx = 0.0f;
        ball_dy = 0.0f;
    }
    else if (ballY + ballHeight >= screenHeight)
    {
        if (!game_over)
        {
            ai_score++;
            updateScoreDisplay();
            if (ai_score >= MAX_SCORE)
            {
                game_over = true;
                pad1_Lose.setVisible(true); 
                pad1_Lose.invalidate();
            }
            else
            {
                ball_reset_pending = true;
                reset_timer = RESET_DELAY;
            }
        }
        ballY = screenHeight - ballHeight;
        ball_dx = 0.0f;
        ball_dy = 0.0f;
    }
    ball1.moveTo(ballX, ballY);

    float ball_centerX = static_cast<float>(ball1.getX() + ballWidth / 2);
    float ball_centerY = static_cast<float>(ball1.getY() + ballHeight / 2);
    float ball_radius = static_cast<float>(ballWidth / 2);

    float pad1_centerX = static_cast<float>(pad1.getX() + pad1.getWidth() / 2);
    float pad1_centerY = static_cast<float>(pad1.getY() + pad1.getHeight() / 2);
    float pad1_radius = static_cast<float>(pad1.getWidth() / 2);

    float pad2_centerX = static_cast<float>(pad2.getX() + pad2.getWidth() / 2);
    float pad2_centerY = static_cast<float>(pad2.getY() + pad2.getHeight() / 2);
    float pad2_radius = static_cast<float>(pad2.getWidth() / 2);

    bool collisionDetected = false;
    float normalX = 0.0f, normalY = 0.0f;

    if ((ballY >= lowerHalfMinY && ball_dy > 0) || second_hit_allowed)
    {
        float distanceX = ball_centerX - pad1_centerX;
        float distanceY = ball_centerY - pad1_centerY;
        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        if (distance < (ball_radius + pad1_radius))
        {
            collisionDetected = true;

            if (distance > 0.001f)
            {
                normalX = distanceX / distance;
                normalY = distanceY / distance;
            }
            else
            {
                normalX = 0.0f;
                normalY = -1.0f;
            }

            float overlap = (ball_radius + pad1_radius) - distance + 2.0f;
            ballX += static_cast<int>(normalX * overlap);
            ballY += static_cast<int>(normalY * overlap);

            ball_centerX = static_cast<float>(ballX + ballWidth / 2);
            ball_centerY = static_cast<float>(ballY + ballHeight / 2);
        }
    }

    if ((!collisionDetected && (ballY + ballHeight <= upperHalfMaxY) && ball_dy < 0) || second_hit_allowed)
    {
        float distanceX = ball_centerX - pad2_centerX;
        float distanceY = ball_centerY - pad2_centerY;
        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        if (distance < (ball_radius + pad2_radius))
        {
            collisionDetected = true;

            if (distance > 0.001f)
            {
                normalX = distanceX / distance;
                normalY = distanceY / distance;
            }
            else
            {
                normalX = 0.0f;
                normalY = 1.0f;
            }

            float overlap = (ball_radius + pad2_radius) - distance + 2.0f;
            ballX += static_cast<int>(normalX * overlap);
            ballY += static_cast<int>(normalY * overlap);

            ball_centerX = static_cast<float>(ballX + ballWidth / 2);
            ball_centerY = static_cast<float>(ballY + ballHeight / 2);
        }
    }

    if (collisionDetected)
    {
        float dotProduct = ball_dx * normalX + ball_dy * normalY;
        ball_dx = ball_dx - 2.0f * dotProduct * normalX;
        ball_dy = ball_dy - 2.0f * dotProduct * normalY;

        float targetSpeed = ball_speed * 4.0f;

        if (ball_centerY >= lowerHalfMinY)
        {
            if (ball_dy > 0)
            {
                ball_dy = -ball_dy;
            }
        }
        else
        {
            if (ball_dy < 0)
            {
                ball_dy = -ball_dy;
            }
        }

        float currentSpeed = std::sqrt(ball_dx * ball_dx + ball_dy * ball_dy);
        if (currentSpeed > 0.001f)
        {
            ball_dx = (ball_dx / currentSpeed) * targetSpeed;
            ball_dy = (ball_dy / currentSpeed) * targetSpeed;
        }

        float angleThreshold = 1.0f;
        bool tooHorizontal = std::abs(ball_dx) > 0.001f && (std::abs(ball_dy) / std::abs(ball_dx)) < angleThreshold;

        if (tooHorizontal)
        {
            if (second_hit_allowed)
            {
                last_hit_tick = 0;
            }
            else
            {
                if (last_hit_tick >= SECOND_HIT_DELAY)
                {
                    second_hit_allowed = true;
                    last_hit_tick = 0;
                }
                else
                {
                    return;
                }
            }
        }
        else
        {
            second_hit_allowed = false;
            last_hit_tick = 0;
        }
    }

    if (!game_over && !ball_reset_pending)
    {
        last_hit_tick++;
    }
}

void GamePVEView::updateAI()
{
    const int screenWidth = 240;

    int ballX = ball1.getX();
    int ballY = ball1.getY();
    int ballWidth = ball1.getWidth();

    float ball_centerX = static_cast<float>(ballX + ballWidth / 2);

    float predicted_x = ball_centerX;
    if (ball_dy < 0) 
    {
        float time_to_reach = std::abs((ballY - pad2.getY()) / ball_dy);
        predicted_x = ball_centerX + (ball_dx * time_to_reach * ai_prediction_factor);
    }

    ai_target_x = predicted_x - (pad2.getWidth() / 2);

    int currentAI_X = pad2.getX();

    float diff = ai_target_x - currentAI_X;

    if (std::abs(diff) > ai_speed)
    {
        if (diff > 0)
        {
            currentAI_X += static_cast<int>(ai_speed);
        }
        else
        {
            currentAI_X -= static_cast<int>(ai_speed);
        }
    }
    else
    {
        currentAI_X = static_cast<int>(ai_target_x);
    }

    if (currentAI_X < 0)
        currentAI_X = 0;
    if (currentAI_X + pad2.getWidth() > screenWidth)
        currentAI_X = screenWidth - pad2.getWidth();

    pad2.moveTo(currentAI_X, pad2.getY());
}

void GamePVEView::resetGame()
{
    player1_score = 0;
    ai_score = 0;
    game_over = false;
    ball_reset_pending = false;
    reset_timer = 0;
    game_over_timer = 0;

    pad1_Win.setVisible(false);
    pad1_Lose.setVisible(false);
    pad1_Win.invalidate();
    pad1_Lose.invalidate();

    updateScoreDisplay();

    resetBall();
}

void GamePVEView::resetBall()
{
    const int screenWidth = 240;
    const int screenHeight = 320;

    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();
    int centerX = (screenWidth - ballWidth) / 2;
    int centerY = (screenHeight - ballHeight) / 2;

    ball1.moveTo(centerX, centerY);

    ball_dx = (player1_score + ai_score) % 2 == 0 ? ball_speed : -ball_speed;
    ball_dy = (player1_score + ai_score) % 4 < 2 ? ball_speed : -ball_speed;
}

void GamePVEView::updateScoreDisplay()
{
    point_pad1.setValue(player1_score);
    point_pad1.invalidate();
    
    point_pad2.setValue(ai_score);
    point_pad2.invalidate();
}

void GamePVEView::handleGameInput()
{
    game_over_timer++;

    if (game_over_timer >= 180)
    {
        game_over_timer = 0;
        resetGame();
    }
}
