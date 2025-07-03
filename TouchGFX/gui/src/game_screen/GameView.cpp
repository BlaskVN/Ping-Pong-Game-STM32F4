#include <gui/game_screen/GameView.hpp>
#include <touchgfx/widgets/Box.hpp> 
#include <cmath>   
#include "main.h"
#include "cmsis_os.h"

extern uint16_t pad1Xleft;
extern uint16_t pad1Xright;
extern uint16_t pad1Yup;
extern uint16_t pad1Ydown;

extern uint16_t pad2Xleft;
extern uint16_t pad2Xright;
extern uint16_t pad2Yup;
extern uint16_t pad2Ydown;

extern uint16_t pad1X;
extern uint16_t pad1Y;

uint16_t pad2_X;
uint16_t pad2_Y;

extern osMessageQueueId_t myQueueButtonHandle;
extern osMessageQueueId_t myQueueJoystickHandle;


extern int min_Pad1_movement_X;
extern int max_Pad1_movement_X;
extern int min_Pad1_movement_Y;
extern int max_Pad1_movement_Y;

extern uint32_t jtPad1_X;
extern uint32_t jtPad1_Y;


GameView::GameView() : GameViewBase(), ball_dx(0.0f), ball_dy(0.0f), ball_speed(1.0f),
                      player1_score(0), player2_score(0), game_over(false), 
                      ball_reset_pending(false), reset_timer(0), game_over_timer(0),
                      MAX_SCORE(5), RESET_DELAY(120), second_hit_allowed(false), last_hit_tick(0)
{

}

void GameView::setupScreen()
{
    GameViewBase::setupScreen();
    
    point_pad1.setRange(0, MAX_SCORE);
    point_pad2.setRange(0, MAX_SCORE);
    
    pad1_Win.setVisible(false);
    pad2_Win.setVisible(false);
    
//    const int screenWidth = 240; 
//	const int screenHeight = 320; 
//	const int screenCenterY = screenHeight / 2;
//
//	const int upperHalfMaxY = screenCenterY;
//
//	min_Pad1_movement_Y = screenCenterY; 
//	max_Pad1_movement_Y = screenHeight - static_cast<int>(pad1.getHeight() / 2);
//
//	min_Pad1_movement_X = static_cast<int>(pad1.getWidth() / 2);
//	max_Pad1_movement_X = screenWidth - static_cast<int>(pad1.getWidth() / 2);

	pad1.moveTo(91, 259);

    resetGame(); 
}

void GameView::tearDownScreen()
{
    GameViewBase::tearDownScreen();
}

void GameView::handlePad1Movement() {
	pad1.setTouchable(false);
	uint16_t cnt = osMessageQueueGetCount(myQueueJoystickHandle);
	if (cnt > 0) {
//		uint8_t cmd;
//		osMessageQueueGet(myQueueJoystickHandle, &cmd, NULL, 0);
		uint8_t mv_cmd;
		osMessageQueueGet(myQueueJoystickHandle, &mv_cmd, NULL, 0);
//		if (cmd == 'J') {


			int x = pad1.getX();
			int y = pad1.getY();

			switch (mv_cmd) {
			case 'A':
				y -= 15;
				break; // UP
			case 'B':
				y += 15;
				break; // DOWN
			case 'C':
				x -= 15;
				break; // LEFT
			case 'D':
				x += 15;
				break; // RIGHT
			default:
				return;
			}

			pad1.moveTo(x, y);
			pad1.invalidate();

			/*
			 //            int newX = jtPad1_X;
			 //            int newY = jtPad1_Y;
			 //
			 //            if (newX < min_Pad1_movement_X) newX = min_Pad1_movement_X;
			 //            if (newX > max_Pad1_movement_X) newX = max_Pad1_movement_X;
			 //
			 //            if (newY < min_Pad1_movement_Y) newY = min_Pad1_movement_Y;
			 //            if (newY > max_Pad1_movement_Y) newY = max_Pad1_movement_Y;
			 //




			 pad1.moveTo(jtPad1_X-10, jtPad1_Y-10);
			 pad1.invalidate();

			 */
		}
//	}
}


void GameView::handlePad2Movement() {
	pad2.setTouchable(false);
	uint16_t cnt = osMessageQueueGetCount(myQueueButtonHandle);
	if (cnt > 0) {
		uint8_t movement_cmd;
		osMessageQueueGet(myQueueButtonHandle, &movement_cmd, NULL, 0);

		int x = pad2.getX();
		int y = pad2.getY();

		switch (movement_cmd) {
		case 'A':
			y += 10;
			break; // UP
		case 'B':
			y -= 10;
			break; // DOWN
		case 'C':
			x -= 10;
			break; // LEFT
		case 'D':
			x += 10;
			break; // RIGHT
		default:
			return;
		}

		pad2.moveTo(x, y);
		pad2.invalidate();
	}
}

void GameView::handleTickEvent()
{
	//Vi tri hien tai cua Pad 2 - namng

	pad2_X = pad2.getX();
	pad2_Y = pad2.getY();

	//Di chuyen Pad 2
    handlePad2Movement();

	//Di chuyen Pad 1
    handlePad1Movement();

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
    
    int ballX = ball1.getX();
    int ballY = ball1.getY();
    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();

    ballX += static_cast<int>(ball_dx);
    ballY += static_cast<int>(ball_dy);

    const int screenWidth = 240; 
    const int screenHeight = 320; 
//    const int screenCenterX = screenWidth / 2;
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
            player2_score++;
            updateScoreDisplay();
            if (player2_score >= MAX_SCORE)
            {
                game_over = true;
                pad2_Win.setVisible(true);
                pad2_Win.invalidate();
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
    if (pad1Y < lowerHalfMinY) {
        pad1Y = lowerHalfMinY;
        pad1.moveTo(pad1X, pad1Y);
    }

    if (pad1X < 0) pad1X = 0;
    if (pad1X + pad1.getWidth() > screenWidth) pad1X = screenWidth - pad1.getWidth();
    pad1.moveTo(pad1X, pad1Y);

    if (pad1Y + pad1.getHeight() > screenHeight) {
        pad1Y = screenHeight - pad1.getHeight();
    }
    pad1.moveTo(pad1X, pad1Y);
    
    int pad2X = pad2.getX();
    int pad2Y = pad2.getY();
    if (pad2Y < 0) {
        pad2Y = 0;
    }
    if (pad2Y + pad2.getHeight() > upperHalfMaxY) {
        pad2Y = upperHalfMaxY - pad2.getHeight();
    }
    pad2.moveTo(pad2X, pad2Y);

    if (pad2X < 0) pad2X = 0;
    if (pad2X + pad2.getWidth() > screenWidth) pad2X = screenWidth - pad2.getWidth();
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
            player2_score++;
            updateScoreDisplay();
            if (player2_score >= MAX_SCORE)
            {
                game_over = true;
                pad2_Win.setVisible(true); 
                pad2_Win.invalidate();
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
        // Tính toán phản xạ dựa trên vector pháp tuyến
        // Công thức phản xạ: v' = v - 2(v·n)n
        // trong đó v là vector vận tốc cũ, n là vector pháp tuyến, v' là vector vận tốc mới

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

void GameView::resetGame()
{
    player1_score = 0;
    player2_score = 0;
    game_over = false;
    ball_reset_pending = false;
    reset_timer = 0;
    game_over_timer = 0;
    
    pad1_Win.setVisible(false);
    pad2_Win.setVisible(false);
    pad1_Win.invalidate();
    pad2_Win.invalidate();
    
    updateScoreDisplay();
    
    resetBall();
}

void GameView::resetBall()
{
    const int screenWidth = 240;
    const int screenHeight = 320;
    
    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();
    int centerX = (screenWidth - ballWidth) / 2;
    int centerY = (screenHeight - ballHeight) / 2;
    
    ball1.moveTo(centerX, centerY);
    
    ball_dx = (player1_score + player2_score) % 2 == 0 ? ball_speed : -ball_speed;
    ball_dy = (player1_score + player2_score) % 4 < 2 ? ball_speed : -ball_speed;
}

void GameView::updateScoreDisplay()
{

    point_pad1.setValue(player1_score);
    point_pad1.invalidate();
    
    point_pad2.setValue(player2_score);
    point_pad2.invalidate();
}

void GameView::handleGameInput()
{
    game_over_timer++;
    
    if (game_over_timer >= 180)
    {
        game_over_timer = 0;
        resetGame();
    }
}

