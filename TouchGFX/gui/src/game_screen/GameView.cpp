#include <gui/game_screen/GameView.hpp>
#include <touchgfx/widgets/Box.hpp> // Để sử dụng touchgfx::Rect, cần include Box.hpp
#include <cmath>             // Cần thiết cho std::sqrt và std::sin

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

    // Kích thước màn hình (giả sử 240x320 như trong cấu hình dự án)
    const int screenWidth = 240; // Hoặc this->getWidth() nếu View là toàn màn hình
    const int screenHeight = 320; // Hoặc this->getHeight()
    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;
    
    // Chia màn hình thành 2 nửa
    const int upperHalfMaxY = screenCenterY; // Nửa trên: từ 0 đến screenCenterY
    const int lowerHalfMinY = screenCenterY; // Nửa dưới: từ screenCenterY đến screenHeight
    
    // Ràng buộc vị trí pad1 (nửa dưới)
    int pad1X = pad1.getX();
    int pad1Y = pad1.getY();
    if (pad1Y < lowerHalfMinY) {
        pad1Y = lowerHalfMinY;
        pad1.moveTo(pad1X, pad1Y);
    }
    
    // Ràng buộc vị trí pad2 (nửa trên)
    int pad2X = pad2.getX();
    int pad2Y = pad2.getY();
    if (pad2Y + pad2.getHeight() > upperHalfMaxY) {
        pad2Y = upperHalfMaxY - pad2.getHeight();
        pad2.moveTo(pad2X, pad2Y);
    }

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
    if (ballY >= lowerHalfMinY)
    {
        float distanceX = ball_centerX - pad1_centerX;
        float distanceY = ball_centerY - pad1_centerY;
        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
        
        if (distance <= (ball_radius + pad1_radius))
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
            
            // Đẩy bóng ra khỏi pad1
            float overlap = (ball_radius + pad1_radius) - distance;
            ballX += static_cast<int>(normalX * overlap);
            ballY += static_cast<int>(normalY * overlap);
        }
    }

    // Kiểm tra va chạm với pad2 (chỉ trong nửa trên màn hình)
    if (!collisionDetected && ballY + ballHeight <= upperHalfMaxY)
    {
        float distanceX = ball_centerX - pad2_centerX;
        float distanceY = ball_centerY - pad2_centerY;
        float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);
        
        if (distance <= (ball_radius + pad2_radius))
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
            
            // Đẩy bóng ra khỏi pad2
            float overlap = (ball_radius + pad2_radius) - distance;
            ballX += static_cast<int>(normalX * overlap);
            ballY += static_cast<int>(normalY * overlap);
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
        
        // Đảm bảo tốc độ không đổi
        float currentSpeed = std::sqrt(ball_dx * ball_dx + ball_dy * ball_dy);
        if (currentSpeed > 0.001f)
        {
            ball_dx = (ball_dx / currentSpeed) * ball_speed;
            ball_dy = (ball_dy / currentSpeed) * ball_speed;
        }
    }
}
