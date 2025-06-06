#include <gui/game_screen/GameView.hpp>
#include <touchgfx/widgets/Box.hpp> // Để sử dụng touchgfx::Rect, cần include Box.hpp
#include <cmath>             // Cần thiết cho std::sqrt

// Khởi tạo GameView, bao gồm cả ball_speed
GameView::GameView() : GameViewBase(), ball_dx(0.0f), ball_dy(0.0f), ball_speed(3.0f)
{

}

void GameView::setupScreen()
{
    GameViewBase::setupScreen();
    // Thiết lập hướng di chuyển ban đầu cho quả bóng (ví dụ)
    // Bạn có thể muốn bóng đứng yên ban đầu hoặc di chuyển theo một hướng cụ thể
    // Ví dụ: di chuyển chéo xuống dưới bên phải
    ball_dx = 1.0f * ball_speed; 
    ball_dy = 1.0f * ball_speed;
    // Hoặc để bóng đứng yên:
    // ball_dx = 0.0f;
    // ball_dy = 0.0f;
}

void GameView::tearDownScreen()
{
    GameViewBase::tearDownScreen();
}

void GameView::handleTickEvent()
{
    // Vị trí và kích thước hiện tại của bóng
    int ballX = ball1.getX();
    int ballY = ball1.getY();
    int ballWidth = ball1.getWidth();
    int ballHeight = ball1.getHeight();

    // Cập nhật vị trí bóng dựa trên vận tốc hiện tại
    ballX += static_cast<int>(ball_dx);
    ballY += static_cast<int>(ball_dy);
    // ball1.moveTo(ballX, ballY); // Di chuyển bóng đến vị trí mới

    // Kích thước màn hình (giả sử 240x320 như trong cấu hình dự án)
    const int screenWidth = 240; // Hoặc this->getWidth() nếu View là toàn màn hình
    const int screenHeight = 320; // Hoặc this->getHeight()
    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;

    // Xử lý va chạm với biên màn hình (ví dụ: nảy lại đơn giản)
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

    if (ballY <= 0)
    {
        ballY = 0;
        ball_dy = -ball_dy; // Đảo ngược hướng Y
        // Trong game Ping Pong thực tế, đây có thể là điểm cho đối thủ
    }
    else if (ballY + ballHeight >= screenHeight)
    {
        ballY = screenHeight - ballHeight;
        ball_dy = -ball_dy; // Đảo ngược hướng Y
        // Trong game Ping Pong thực tế, đây có thể là điểm cho người chơi
    }
    ball1.moveTo(ballX, ballY); // Cập nhật vị trí bóng sau khi kiểm tra biên

    // Tạo hình chữ nhật bao quanh cho bóng và các pad
    touchgfx::Rect ballRect(ball1.getX(), ball1.getY(), ball1.getWidth(), ball1.getHeight());
    touchgfx::Rect pad1Rect(pad1.getX(), pad1.getY(), pad1.getWidth(), pad1.getHeight());
    touchgfx::Rect pad2Rect(pad2.getX(), pad2.getY(), pad2.getWidth(), pad2.getHeight());

    bool collisionDetected = false;

    // Kiểm tra va chạm với pad1
    if (ballRect.intersect(pad1Rect))
    {
        collisionDetected = true;
        // Để tránh bóng bị "dính" vào pad, bạn có thể điều chỉnh vị trí bóng một chút
        // Ví dụ: đẩy bóng ra khỏi pad theo hướng va chạm
        // ballY = pad1.getY() - ballHeight; // Nếu pad1 ở dưới
        // ball_dy = -std::abs(ball_dy); // Đảm bảo bóng di chuyển ra xa pad
    }

    // Kiểm tra va chạm với pad2
    if (ballRect.intersect(pad2Rect))
    {
        collisionDetected = true;
        // Tương tự, xử lý để tránh dính bóng
        // ballY = pad2.getY() + pad2.getHeight(); // Nếu pad2 ở trên
        // ball_dy = std::abs(ball_dy); // Đảm bảo bóng di chuyển ra xa pad
    }

    if (collisionDetected)
    {
        // Tính toán hướng từ tâm bóng hiện tại đến tâm màn hình
        float currentBallCenterX = static_cast<float>(ball1.getX() + ballWidth / 2);
        float currentBallCenterY = static_cast<float>(ball1.getY() + ballHeight / 2);

        // Mục tiêu là tâm của bóng sẽ ở tâm màn hình
        float targetBallCenterX = static_cast<float>(screenCenterX);
        float targetBallCenterY = static_cast<float>(screenCenterY);
        
        float dirX = targetBallCenterX - currentBallCenterX;
        float dirY = targetBallCenterY - currentBallCenterY;

        // Chuẩn hóa vector hướng và áp dụng tốc độ
        float length = std::sqrt(dirX * dirX + dirY * dirY);

        if (length > 0) // Tránh chia cho 0 nếu bóng đã ở tâm
        {
            ball_dx = (dirX / length) * ball_speed;
            ball_dy = (dirY / length) * ball_speed;
        }
        else
        {
            // Bóng đã ở tâm, có thể dừng lại hoặc cho một hướng ngẫu nhiên
            ball_dx = 0.0f;
            ball_dy = 0.0f;
        }
        
        // Di chuyển bóng một chút theo hướng mới để tránh va chạm lặp lại ngay lập tức
        // Điều này giúp bóng "thoát" khỏi paddle sau va chạm.
        // ball1.moveTo(ball1.getX() + static_cast<int>(ball_dx), ball1.getY() + static_cast<int>(ball_dy));
    }
}
