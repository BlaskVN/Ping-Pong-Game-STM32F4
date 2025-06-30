#include <gui/game_screen/GameView.hpp>
#include <touchgfx/widgets/Box.hpp> // Để sử dụng touchgfx::Rect, cần include Box.hpp
#include <cmath>             // Cần thiết cho std::sqrt và std::sin
#include "main.h"

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



// Khởi tạo GameView, bao gồm cả ball_speed và game state
GameView::GameView() : GameViewBase(), ball_dx(0.0f), ball_dy(0.0f), ball_speed(1.0f),
                      player1_score(0), player2_score(0), game_over(false), 
                      ball_reset_pending(false), reset_timer(0), game_over_timer(0),
                      MAX_SCORE(5), RESET_DELAY(120), second_hit_allowed(false), last_hit_tick(0)
{

}

void GameView::setupScreen()
{
    GameViewBase::setupScreen();
    
    // Thiết lập range cho TextProgress widgets
    // Range từ 0 đến MAX_SCORE (5 điểm)
    point_pad1.setRange(0, MAX_SCORE);
    point_pad2.setRange(0, MAX_SCORE);
    
    // Ban đầu ẩn các thông báo chiến thắng
    pad1_Win.setVisible(false);
    pad2_Win.setVisible(false);
    
    resetGame(); // Initialize game state
}

void GameView::tearDownScreen()
{
    GameViewBase::tearDownScreen();
}
void GameView::handleTickEvent()
{
	//Vi tri hien tai cua Pad 2 - namng



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
    
    // Vị trí và kích thước hiện tại của bóng
    int ballX = ball1.getX();
    int ballY = ball1.getY();
    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();

    // Cập nhật vị trí bóng dựa trên vận tốc hiện tại
    ballX += static_cast<int>(ball_dx);
    ballY += static_cast<int>(ball_dy);

    // Kích thước màn hình (giả sử 240x320 như trong cấu hình dự án)
    const int screenWidth = 240; // Hoặc this->getWidth() nếu View là toàn màn hình
    const int screenHeight = 320; // Hoặc this->getHeight()
//    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;
    
    // Chia màn hình thành 2 nửa
    const int upperHalfMaxY = screenCenterY; // Nửa trên: từ 0 đến screenCenterY
    const int lowerHalfMinY = screenCenterY; // Nửa dưới: từ screenCenterY đến screenHeight
    
    // Kiểm tra bóng ở nửa màn hình nào và xử lý thắng thua nếu cần
    int ball_centerY_int = ballY + ballHeight / 2;
    
    // Nếu bóng đang ở nửa trên (sân của Player 2) và di chuyển chậm hoặc dừng lại
    if (ball_centerY_int <= upperHalfMaxY && std::abs(ball_dy) < 0.5f && std::abs(ball_dx) < 0.5f)
    {
        // Player 2 thua điểm vì không đánh trả được bóng
        if (!game_over)
        {
            player1_score++;
            updateScoreDisplay();
            if (player1_score >= MAX_SCORE)
            {
                game_over = true;
                pad1_Win.setVisible(true); // Hiển thị thông báo Player 1 thắng
                pad1_Win.invalidate();
            }
            else
            {
                ball_reset_pending = true;
                reset_timer = RESET_DELAY;
            }
            ballX = ballX; // Giữ nguyên vị trí X
            ballY = ballY; // Giữ nguyên vị trí Y
            ball_dx = 0.0f;
            ball_dy = 0.0f;
        }
    }
    // Nếu bóng đang ở nửa dưới (sân của Player 1) và di chuyển chậm hoặc dừng lại
    else if (ball_centerY_int >= lowerHalfMinY && std::abs(ball_dy) < 0.5f && std::abs(ball_dx) < 0.5f)
    {
        // Player 1 thua điểm vì không đánh trả được bóng
        if (!game_over)
        {
            player2_score++;
            updateScoreDisplay();
            if (player2_score >= MAX_SCORE)
            {
                game_over = true;
                pad2_Win.setVisible(true); // Hiển thị thông báo Player 2 thắng
                pad2_Win.invalidate();
            }
            else
            {
                ball_reset_pending = true;
                reset_timer = RESET_DELAY;
            }
            ballX = ballX; // Giữ nguyên vị trí X
            ballY = ballY; // Giữ nguyên vị trí Y
            ball_dx = 0.0f;
            ball_dy = 0.0f;
        }
    }
    
    // Ràng buộc vị trí pad1 (nửa dưới)
    int pad1X = pad1.getX();
    int pad1Y = pad1.getY();
    if (pad1Y < lowerHalfMinY) {
        pad1Y = lowerHalfMinY;
        pad1.moveTo(pad1X, pad1Y);
    }

    // Giới hạn pad1 trong màn hình
    if (pad1X < 0) pad1X = 0;
    if (pad1X + pad1.getWidth() > screenWidth) pad1X = screenWidth - pad1.getWidth();
    pad1.moveTo(pad1X, pad1Y);

    // Ràng buộc pad1 không vượt quá đáy màn hình
    if (pad1Y + pad1.getHeight() > screenHeight) {
        pad1Y = screenHeight - pad1.getHeight();
    }
    pad1.moveTo(pad1X, pad1Y);
    
    // Ràng buộc pad2 trong nửa trên và trong màn hình
    int pad2X = pad2.getX();
    int pad2Y = pad2.getY();
    if (pad2Y < 0) {
        pad2Y = 0;
    }
    if (pad2Y + pad2.getHeight() > upperHalfMaxY) {
        pad2Y = upperHalfMaxY - pad2.getHeight();
    }
    pad2.moveTo(pad2X, pad2Y);

    // Giới hạn pad2 trong màn hình
    if (pad2X < 0) pad2X = 0;
    if (pad2X + pad2.getWidth() > screenWidth) pad2X = screenWidth - pad2.getWidth();
    pad2.moveTo(pad2X, pad2Y);



    // Xử lý va chạm với biên trái và phải (nảy lại)
    if (ballX <= 0)
    {
        ballX = 0;
        ball_dx = -ball_dx; // Đảo ngược hướng X
    }
    else if (ballX + ballWidth >= screenWidth)
    {
        ballX = screenWidth - ballWidth;
        ball_dx = -ball_dx; // Đảo ngược hướng X
    }

    // Xử lý va chạm với biên trên và dưới (xử lý thắng thua theo logic ping pong thật)
    if (ballY <= 0)
    {
        // Bóng chạm biên trên (nửa màn hình của Player 2) - Player 2 thua điểm, Player 1 ghi điểm
        if (!game_over)
        {
            player1_score++;
            updateScoreDisplay();
            if (player1_score >= MAX_SCORE)
            {
                game_over = true;
                pad1_Win.setVisible(true); // Hiển thị thông báo Player 1 thắng
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
        // Bóng chạm biên dưới (nửa màn hình của Player 1) - Player 1 thua điểm, Player 2 ghi điểm
        if (!game_over)
        {
            player2_score++;
            updateScoreDisplay();
            if (player2_score >= MAX_SCORE)
            {
                game_over = true;
                pad2_Win.setVisible(true); // Hiển thị thông báo Player 2 thắng
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
    ball1.moveTo(ballX, ballY); // Cập nhật vị trí bóng sau khi kiểm tra biên

    // Tính toán tâm và bán kính cho hitbox hình tròn của các pad
    float ball_centerX = static_cast<float>(ball1.getX() + ballWidth / 2);
    float ball_centerY = static_cast<float>(ball1.getY() + ballHeight / 2);
    float ball_radius = static_cast<float>(ballWidth / 2); // Giả sử bóng là hình tròn

    // Pad1 (nửa dưới) - hitbox hình tròn
    float pad1_centerX = static_cast<float>(pad1.getX() + pad1.getWidth() / 2);
    float pad1_centerY = static_cast<float>(pad1.getY() + pad1.getHeight() / 2);
    float pad1_radius = static_cast<float>(pad1.getWidth() / 2); // Sử dụng chiều rộng làm đường kính

    // Pad2 (nửa trên) - hitbox hình tròn  
    float pad2_centerX = static_cast<float>(pad2.getX() + pad2.getWidth() / 2);
    float pad2_centerY = static_cast<float>(pad2.getY() + pad2.getHeight() / 2);
    float pad2_radius = static_cast<float>(pad2.getWidth() / 2);

    bool collisionDetected = false;
    float normalX = 0.0f, normalY = 0.0f; // Vector pháp tuyến tại điểm va chạm

    // Kiểm tra va chạm với pad1 (chỉ trong nửa dưới màn hình)
    if ((ballY >= lowerHalfMinY && ball_dy > 0) || second_hit_allowed)
    {
        float distanceX = ball_centerX - pad1_centerX;
        float distanceY = ball_centerY - pad1_centerY;
        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
        
        if (distance < (ball_radius + pad1_radius))
        {
            collisionDetected = true;
            
            // Tính vector pháp tuyến (từ tâm pad đến tâm bóng)
            if (distance > 0.001f) // Tránh chia cho 0
            {
                normalX = distanceX / distance;
                normalY = distanceY / distance;
            }
            else
            {
                normalX = 0.0f;
                normalY = -1.0f; // Mặc định hướng lên
            }
            
            // Đẩy bóng ra khỏi pad1 để tránh stuck
            float overlap = (ball_radius + pad1_radius) - distance + 2.0f; // +2 để đảm bảo tách rời
            ballX += static_cast<int>(normalX * overlap);
            ballY += static_cast<int>(normalY * overlap);
            
            // Cập nhật lại tâm bóng sau khi đẩy
            ball_centerX = static_cast<float>(ballX + ballWidth / 2);
            ball_centerY = static_cast<float>(ballY + ballHeight / 2);
        }
    }

    // Kiểm tra va chạm với pad2 (chỉ trong nửa trên màn hình)
    if ((!collisionDetected && (ballY + ballHeight <= upperHalfMaxY) && ball_dy < 0) || second_hit_allowed)
    {
        float distanceX = ball_centerX - pad2_centerX;
        float distanceY = ball_centerY - pad2_centerY;
        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
        
        if (distance < (ball_radius + pad2_radius))
        {
            collisionDetected = true;
            
            // Tính vector pháp tuyến (từ tâm pad đến tâm bóng)
            if (distance > 0.001f) // Tránh chia cho 0
            {
                normalX = distanceX / distance;
                normalY = distanceY / distance;
            }
            else
            {
                normalX = 0.0f;
                normalY = 1.0f; // Mặc định hướng xuống
            }
            
            // Đẩy bóng ra khỏi pad2 để tránh stuck
            float overlap = (ball_radius + pad2_radius) - distance + 2.0f; // +2 để đảm bảo tách rời
            ballX += static_cast<int>(normalX * overlap);
            ballY += static_cast<int>(normalY * overlap);
            
            // Cập nhật lại tâm bóng sau khi đẩy
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

        // Tính vận tốc mới sau phản xạ
        ball_dx = ball_dx - 2.0f * dotProduct * normalX;
        ball_dy = ball_dy - 2.0f * dotProduct * normalY;

        // Set tốc độ 2x sau va chạm và hướng về phía đối thủ
        float targetSpeed = ball_speed * 4.0f;

        // Đảm bảo bóng hướng về phía đối thủ
        if (ball_centerY >= lowerHalfMinY) // Va chạm với pad1 (nửa dưới) - hướng lên trên (về pad2)
        {
            if (ball_dy > 0) // Nếu đang hướng xuống dưới, đảo ngược
            {
                ball_dy = -ball_dy;
            }
        }
        else // Va chạm với pad2 (nửa trên) - hướng xuống dưới (về pad1)
        {
            if (ball_dy < 0) // Nếu đang hướng lên trên, đảo ngược
            {
                ball_dy = -ball_dy;
            }
        }

        // Áp dụng tốc độ mới
        float currentSpeed = std::sqrt(ball_dx * ball_dx + ball_dy * ball_dy);
        if (currentSpeed > 0.001f)
        {
            ball_dx = (ball_dx / currentSpeed) * targetSpeed;
            ball_dy = (ball_dy / currentSpeed) * targetSpeed;
        }
        // Nếu bóng đi quá ngang (góc nhỏ hơn 45 độ), thì cho phép chạm nhiều lần
        float angleThreshold = 1.0f;
        bool tooHorizontal = std::abs(ball_dx) > 0.001f && (std::abs(ball_dy) / std::abs(ball_dx)) < angleThreshold;

        if (tooHorizontal)
        {
            if (second_hit_allowed)
            {
                // Được phép đánh lại — xử lý phản xạ như bình thường
                last_hit_tick = 0;
            }
            else
            {
                if (last_hit_tick >= SECOND_HIT_DELAY)
                {
                    second_hit_allowed = true;
                    last_hit_tick = 0;
                    // Xử lý phản xạ như bình thường
                }
                else
                {
                    // ❗ Cho phép bóng tiếp tục bay nhưng KHÔNG phản xạ lần nữa
                    // Bỏ qua xử lý phản xạ, nhưng KHÔNG return
                    return; // ← Chặn phản xạ nhưng vẫn để bóng bay thoát ra
                }
            }
        }
        else
        {
            second_hit_allowed = false;
            last_hit_tick = 0; // reset timer vì hướng bóng đã thay đổi
        }
    }
    if (!game_over && !ball_reset_pending)
    {
        last_hit_tick++;
    }
}

void GameView::resetGame()
{
    // Reset scores
    player1_score = 0;
    player2_score = 0;
    game_over = false;
    ball_reset_pending = false;
    reset_timer = 0;
    game_over_timer = 0;
    
    // Ẩn các thông báo chiến thắng
    pad1_Win.setVisible(false);
    pad2_Win.setVisible(false);
    pad1_Win.invalidate();
    pad2_Win.invalidate();
    
    // Update score display on TextProgress widgets
    updateScoreDisplay();
    
    // Reset ball to center
    resetBall();
}

void GameView::resetBall()
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
    
    // Thiết lập hướng di chuyển ngẫu nhiên (hoặc cố định)
    // Hướng chéo với tốc độ cơ bản
    ball_dx = (player1_score + player2_score) % 2 == 0 ? ball_speed : -ball_speed;
    ball_dy = (player1_score + player2_score) % 4 < 2 ? ball_speed : -ball_speed;
}

void GameView::updateScoreDisplay()
{
    // Cập nhật điểm số cho TextProgress widgets
    // point_pad1 hiển thị điểm của player 1 (pad dưới)
    point_pad1.setValue(player1_score);
    point_pad1.invalidate();
    
    // point_pad2 hiển thị điểm của player 2 (pad trên)
    point_pad2.setValue(player2_score);
    point_pad2.invalidate();
}

void GameView::handleGameInput()
{
    // Placeholder for restart game input handling
    // Trong TouchGFX, bạn có thể kiểm tra touch input hoặc button press
    // Ví dụ: nếu có nút restart hoặc chạm vào màn hình
    
    // Tự động restart sau một khoảng thời gian
    game_over_timer++;
    
    // Auto restart after 3 seconds (assuming 60 FPS -> 180 ticks)
    if (game_over_timer >= 180)
    {
        game_over_timer = 0;
        resetGame();
    }
}

