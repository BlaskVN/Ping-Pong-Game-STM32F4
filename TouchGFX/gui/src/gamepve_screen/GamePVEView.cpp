#include <gui/gamepve_screen/GamePVEView.hpp>
#include <touchgfx/widgets/Box.hpp> // Để sử dụng touchgfx::Rect, cần include Box.hpp
#include <cmath>                    // Cần thiết cho std::sqrt và std::sin

// Static variables to maintain game state (since header doesn't have them)
static float ball_dx = 0.0f;          // Velocity in X direction
static float ball_dy = 0.0f;          // Velocity in Y direction
static const float ball_speed = 1.7f; // Speed of the ball

// Game state variables
static int player1_score = 0;           // Player 1 (bottom pad) score - Human player
static int ai_score = 0;                // AI (top pad) score
static bool game_over = false;          // Game over flag
static bool ball_reset_pending = false; // Flag to reset ball position
static int reset_timer = 0;             // Timer for ball reset delay
static int game_over_timer = 0;         // Timer for auto-restart after game over
static const int MAX_SCORE = 5;         // Score to win
static const int RESET_DELAY = 120;     // Delay before ball reset (in ticks)
static bool second_hit_allowed = false;
static int last_hit_tick = 0;
static constexpr int SECOND_HIT_DELAY = 30;

// AI variables
static float ai_target_x = 0.0f;                // AI target position
static const float ai_speed = 2.8f;             // AI movement speed
static const float ai_prediction_factor = 0.8f; // How well AI predicts ball movement

GamePVEView::GamePVEView()
{
    // Reset game state
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
    resetGame(); // Initialize game state
}

void GamePVEView::tearDownScreen()
{
    GamePVEViewBase::tearDownScreen();
}

void GamePVEView::handleTickEvent()
{
    last_hit_tick++;

    // Handle ball reset timer
    if (ball_reset_pending)
    {
        reset_timer--;
        if (reset_timer <= 0)
        {
            resetBall();
            ball_reset_pending = false;
        }
        return; // Don't update ball physics during reset
    }

    // Don't update game if it's over
    if (game_over)
    {
        handleGameInput(); // Check for restart input
        return;
    }

    // Update AI movement
    updateAI();

    // Vị trí và kích thước hiện tại của bóng
    int ballX = ball1.getX();
    int ballY = ball1.getY();
    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();

    // Cập nhật vị trí bóng dựa trên vận tốc hiện tại
    ballX += static_cast<int>(ball_dx);
    ballY += static_cast<int>(ball_dy);

    // Kích thước màn hình
    const int screenWidth = 240;
    const int screenHeight = 320;
    const int screenCenterY = screenHeight / 2;

    // Chia màn hình thành 2 nửa
    const int upperHalfMaxY = screenCenterY; // Nửa trên: từ 0 đến screenCenterY (AI territory)
    const int lowerHalfMinY = screenCenterY; // Nửa dưới: từ screenCenterY đến screenHeight (Human player territory)

    // Kiểm tra bóng ở nửa màn hình nào và xử lý thắng thua nếu cần
    int ball_centerY_int = ballY + ballHeight / 2;

    // Nếu bóng đang ở nửa trên (sân của AI) và di chuyển chậm hoặc dừng lại
    if (ball_centerY_int <= upperHalfMaxY && std::abs(ball_dy) < 0.5f && std::abs(ball_dx) < 0.5f)
    {
        // AI thua điểm vì không đánh trả được bóng
        if (!game_over)
        {
            player1_score++;
            updateScoreDisplay();
            if (player1_score >= MAX_SCORE)
            {
                game_over = true;
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
    // Nếu bóng đang ở nửa dưới (sân của Human Player) và di chuyển chậm hoặc dừng lại
    else if (ball_centerY_int >= lowerHalfMinY && std::abs(ball_dy) < 0.5f && std::abs(ball_dx) < 0.5f)
    {
        // Human Player thua điểm vì không đánh trả được bóng
        if (!game_over)
        {
            ai_score++;
            updateScoreDisplay();
            if (ai_score >= MAX_SCORE)
            {
                game_over = true;
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

    // Ràng buộc vị trí pad1 (nửa dưới - Human player)
    int pad1X = pad1.getX();
    int pad1Y = pad1.getY();
    if (pad1Y < lowerHalfMinY)
    {
        pad1Y = lowerHalfMinY;
        pad1.moveTo(pad1X, pad1Y);
    }

    // Giới hạn pad1 trong màn hình
    if (pad1X < 0)
        pad1X = 0;
    if (pad1X + pad1.getWidth() > screenWidth)
        pad1X = screenWidth - pad1.getWidth();
    pad1.moveTo(pad1X, pad1Y);

    // Ràng buộc pad1 không vượt quá đáy màn hình
    if (pad1Y + pad1.getHeight() > screenHeight)
    {
        pad1Y = screenHeight - pad1.getHeight();
    }
    pad1.moveTo(pad1X, pad1Y);

    // Ràng buộc pad2 trong nửa trên và trong màn hình (AI controlled)
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

    // Giới hạn pad2 trong màn hình
    if (pad2X < 0)
        pad2X = 0;
    if (pad2X + pad2.getWidth() > screenWidth)
        pad2X = screenWidth - pad2.getWidth();
    pad2.moveTo(pad2X, pad2Y);

    // Xử lý va chạm với biên trái và phải (nảy lại)
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

    // Xử lý va chạm với biên trên và dưới
    if (ballY <= 0)
    {
        // Bóng chạm biên trên (nửa màn hình của AI) - AI thua điểm, Human Player ghi điểm
        if (!game_over)
        {
            player1_score++;
            updateScoreDisplay();
            if (player1_score >= MAX_SCORE)
            {
                game_over = true;
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
        // Bóng chạm biên dưới (nửa màn hình của Human Player) - Human Player thua điểm, AI ghi điểm
        if (!game_over)
        {
            ai_score++;
            updateScoreDisplay();
            if (ai_score >= MAX_SCORE)
            {
                game_over = true;
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

    // Collision detection logic (same as GameView)
    float ball_centerX = static_cast<float>(ball1.getX() + ballWidth / 2);
    float ball_centerY = static_cast<float>(ball1.getY() + ballHeight / 2);
    float ball_radius = static_cast<float>(ballWidth / 2);

    // Pad1 (nửa dưới - Human player) - hitbox hình tròn
    float pad1_centerX = static_cast<float>(pad1.getX() + pad1.getWidth() / 2);
    float pad1_centerY = static_cast<float>(pad1.getY() + pad1.getHeight() / 2);
    float pad1_radius = static_cast<float>(pad1.getWidth() / 2);

    // Pad2 (nửa trên - AI) - hitbox hình tròn
    float pad2_centerX = static_cast<float>(pad2.getX() + pad2.getWidth() / 2);
    float pad2_centerY = static_cast<float>(pad2.getY() + pad2.getHeight() / 2);
    float pad2_radius = static_cast<float>(pad2.getWidth() / 2);

    bool collisionDetected = false;
    float normalX = 0.0f, normalY = 0.0f;

    // Kiểm tra va chạm với pad1 (Human player - chỉ trong nửa dưới màn hình)
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

    // Kiểm tra va chạm với pad2 (AI - chỉ trong nửa trên màn hình)
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
        // Collision response logic (same as GameView)
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
    // AI logic for pad2 (top pad)
    const int screenWidth = 240;

    // Get current ball position
    int ballX = ball1.getX();
    int ballY = ball1.getY();
    int ballWidth = ball1.getWidth();

    // Calculate ball center
    float ball_centerX = static_cast<float>(ballX + ballWidth / 2);

    // Predict where the ball will be
    float predicted_x = ball_centerX;
    if (ball_dy < 0) // Ball moving towards AI
    {
        // Simple prediction: where will ball be when it reaches AI's Y level
        float time_to_reach = std::abs((ballY - pad2.getY()) / ball_dy);
        predicted_x = ball_centerX + (ball_dx * time_to_reach * ai_prediction_factor);
    }

    // Calculate desired AI position
    ai_target_x = predicted_x - (pad2.getWidth() / 2);

    // Get current AI position
    int currentAI_X = pad2.getX();

    // Move AI towards target with some speed limitation for realistic movement
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

    // Keep AI within screen bounds
    if (currentAI_X < 0)
        currentAI_X = 0;
    if (currentAI_X + pad2.getWidth() > screenWidth)
        currentAI_X = screenWidth - pad2.getWidth();

    // Update AI position
    pad2.moveTo(currentAI_X, pad2.getY());
}

void GamePVEView::resetGame()
{
    // Reset scores
    player1_score = 0;
    ai_score = 0;
    game_over = false;
    ball_reset_pending = false;
    reset_timer = 0;
    game_over_timer = 0;

    // Update score display
    updateScoreDisplay();

    // Reset ball to center
    resetBall();
}

void GamePVEView::resetBall()
{
    // Kích thước màn hình
    const int screenWidth = 240;
    const int screenHeight = 320;

    // Đặt bóng ở giữa màn hình
    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();
    int centerX = (screenWidth - ballWidth) / 2;
    int centerY = (screenHeight - ballHeight) / 2;

    ball1.moveTo(centerX, centerY);

    // Thiết lập hướng di chuyển ngẫu nhiên
    ball_dx = (player1_score + ai_score) % 2 == 0 ? ball_speed : -ball_speed;
    ball_dy = (player1_score + ai_score) % 4 < 2 ? ball_speed : -ball_speed;
}

void GamePVEView::updateScoreDisplay()
{
    // Placeholder for score display update
    // In TouchGFX, you can add TextArea widget to display scores
    // Example:
    // Unicode::snprintf(scoreBuffer, 20, "Player: %d - AI: %d", player1_score, ai_score);
    // scoreText.setWildcard(scoreBuffer);
    // scoreText.invalidate();
}

void GamePVEView::handleGameInput()
{
    // Auto restart after some time in PvE mode
    game_over_timer++;

    // Auto restart after 3 seconds (assuming 60 FPS -> 180 ticks)
    if (game_over_timer >= 180)
    {
        game_over_timer = 0;
        resetGame();
    }
}
