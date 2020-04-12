#include "game.h"
#include "resource_manager.h"
#include "sprite.h"
#include "particle.h"
#include "post_processor.h"

// test
#include <glm/gtx/string_cast.hpp>

// 初始化挡板的大小
const glm::vec2 PLAYER_SIZE(100, 20);
// 初始化当班的速率
const GLfloat PLAYER_VELOCITY(500.0f);
GLfloat            ShakeTime = 1.0f;
GameObject      *Player;

float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height) 
{ 

}

Game::~Game()
{

}

SpriteRenderer  *Renderer;

// 初始化球的速度
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// 球的半径
const GLfloat BALL_RADIUS = 12.5f;

BallObject     *Ball;

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};    

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;    

Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // 上
        glm::vec2(1.0f, 0.0f),  // 右
        glm::vec2(0.0f, -1.0f), // 下
        glm::vec2(-1.0f, 0.0f)  // 左
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++)
    {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}    


Collision CheckCollision(BallObject &one, GameObject &two) // AABB - Circle collision
{
    // 获取圆的中心 
    glm::vec2 center(one.Position + one.Radius);
    // 计算AABB的信息（中心、半边长）
    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x, 
        two.Position.y + aabb_half_extents.y
    );
    // 获取两个中心的差矢量
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
    glm::vec2 closest = aabb_center + clamped;
    // 获得圆心center和最近点closest的矢量并判断是否 length <= radius
    difference = closest - center;
    if (glm::length(difference) <= one.Radius)
        return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
    else
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}      

GLboolean CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // x轴方向碰撞？
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // y轴方向碰撞？
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // 只有两个轴向都有碰撞时才碰撞
    return collisionX && collisionY;
}  

ParticleGenerator   *Particles; 
PostProcessor   *Effects;
std::string srcRoot = "../src/";
void Game::Init()
{

  // test
  ResourceManager::LoadShader((srcRoot + "shaders/triangle.vert").c_str(), (srcRoot + "shaders/triangle.frag").c_str(), nullptr, "triangle");
  ResourceManager::LoadShader((srcRoot + "shaders/postprocess.vert").c_str(), (srcRoot + "shaders/postprocess.frag").c_str(), nullptr, "postprocessing");
   
  ResourceManager::LoadShader((srcRoot + "shaders/sprite.vert").c_str(), (srcRoot + "shaders/sprite.frag").c_str(), nullptr, "sprite");
  ResourceManager::LoadShader((srcRoot + "shaders/particle.vert").c_str(), (srcRoot + "shaders/particle.frag").c_str(), nullptr, "particle");
  glm::mat4 projection = glm::ortho(
    0.0f, static_cast<GLfloat>(this->Width),
    static_cast<GLfloat>(this->Height), 0.0f,
    -1.0f, 1.0f
  );

  // glm::mat4 projection = glm::ortho(
  //   0.0f, 1.0f,
  //   1.0f, 0.0f,
  //   -1.0f, 1.0f
  // );

  fprintf(stdout, "%s", glm::to_string(projection).c_str());
  ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
  ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
  ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
  ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
  Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
  // ResourceManager::LoadTexture((srcRoot + "textures/awesomeface.png").c_str(), GL_FALSE, "face");
  ResourceManager::LoadTexture((srcRoot + "textures/awesomeface.png").c_str(), GL_TRUE, "face");
  ResourceManager::LoadTexture((srcRoot + "textures/powerup_speed.png").c_str(), GL_TRUE, "powerup_speed");
  ResourceManager::LoadTexture((srcRoot + "textures/powerup_sticky.png").c_str(), GL_TRUE, "powerup_sticky");
  ResourceManager::LoadTexture((srcRoot + "textures/powerup_increase.png").c_str(), GL_TRUE, "powerup_increase");
  ResourceManager::LoadTexture((srcRoot + "textures/powerup_confuse.png").c_str(), GL_TRUE, "powerup_confuse");
  ResourceManager::LoadTexture((srcRoot + "textures/powerup_chaos.png").c_str(), GL_TRUE, "powerup_chaos");
  ResourceManager::LoadTexture((srcRoot + "textures/powerup_passthrough.png").c_str(), GL_TRUE, "powerup_passthrough");
  ResourceManager::LoadTexture((srcRoot + "textures/background.jpg").c_str(), GL_FALSE, "background");
  ResourceManager::LoadTexture((srcRoot + "textures/awesomeface.png").c_str(), GL_TRUE, "face");
  ResourceManager::LoadTexture((srcRoot + "textures/block.png").c_str(), GL_FALSE, "block");
  ResourceManager::LoadTexture((srcRoot + "textures/block_solid.png").c_str(), GL_FALSE, "block_solid");
  ResourceManager::LoadTexture((srcRoot + "textures/particle.png").c_str(), GL_TRUE, "particle"); 
  GameLevel one; one.Load((srcRoot +"levels/one").c_str(), this->Width, this->Height * 0.5);
  GameLevel two; two.Load((srcRoot +"levels/two").c_str(), this->Width, this->Height * 0.5);
  GameLevel three; three.Load((srcRoot +"levels/three").c_str(), this->Width, this->Height * 0.5);
  GameLevel four; four.Load((srcRoot +"levels/four").c_str(), this->Width, this->Height * 0.5);
  this->Levels.push_back(one);
  this->Levels.push_back(two);
  this->Levels.push_back(three);
  this->Levels.push_back(four);
  this->Level = 0;
  Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

  ResourceManager::LoadTexture((srcRoot +"textures/paddle.png").c_str(), true, "paddle");
  glm::vec2 playerPos = glm::vec2(
      this->Width / 2 - PLAYER_SIZE.x / 2, 
      this->Height - PLAYER_SIZE.y
  );
  Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

  glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
  Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
      ResourceManager::GetTexture("face"));

  Particles = new ParticleGenerator(
    ResourceManager::GetShader("particle"), 
    ResourceManager::GetTexture("particle"), 
    500
  );
    // Effects->Shake = GL_TRUE;
    // Effects->Confuse = GL_TRUE;
    // Effects->Chaos = GL_TRUE;
};

void Game::ResetLevel()
{
    if (this->Level == 0)
      this->Levels[0].Load((srcRoot + "levels/one").c_str(), this->Width, this->Height * 0.5f);
    else if (this->Level == 1)
        this->Levels[1].Load((srcRoot + "levels/two").c_str(), this->Width, this->Height * 0.5f);
    else if (this->Level == 2)
        this->Levels[2].Load((srcRoot + "levels/three").c_str(), this->Width, this->Height * 0.5f);
    else if (this->Level == 3)
        this->Levels[3].Load((srcRoot + "levels/four").c_str(), this->Width, this->Height * 0.5f);
}

void Game::ResetPlayer()
{
    // Reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}

void Game::Update(GLfloat dt)
{
  Ball->Move(dt, this->Width);
  Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));
  this->DoCollisions();
  this->UpdatePowerUps(dt);

  if (Ball->Position.y >= this->Height) // 球是否接触底部边界？
  {
      this->ResetLevel();
      this->ResetPlayer();
  }
  if (ShakeTime > 0.0f)
  {
      ShakeTime -= dt;
      if (ShakeTime <= 0.0f)
          Effects->Shake = GL_FALSE;
  }
}

void ActivatePowerUp(PowerUp &powerUp)
{
    // 根据道具类型发动道具
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = GL_TRUE;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!Effects->Chaos)
            Effects->Confuse = GL_TRUE; // 只在chaos未激活时生效，chaos同理
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Effects->Confuse)
            Effects->Chaos = GL_TRUE;
    }
} 

GLboolean ShouldSpawn(GLuint chance)
{
    GLuint random = rand() % chance;
    return random == 0;
}
void Game::SpawnPowerUps(GameObject &block)
{
    if (ShouldSpawn(75)) // 1 in 75 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
    if (ShouldSpawn(1))
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
    if (ShouldSpawn(15)) // Negative powerups should spawn more often
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
    if (ShouldSpawn(15))
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));

}

GLboolean isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
    for (const PowerUp &powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}  

void Game::UpdatePowerUps(GLfloat dt)
{
    for (PowerUp &powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                // 之后会将这个道具移除
                powerUp.Activated = GL_FALSE;
                // 停用效果
                if (powerUp.Type == "sticky")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
                    {   // 仅当没有其他sticky效果处于激活状态时重置，以下同理
                        Ball->Sticky = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {
                        Ball->PassThrough = GL_FALSE;
                        Ball->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "confuse"))
                    {
                        Effects->Confuse = GL_FALSE;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
                    {
                        Effects->Chaos = GL_FALSE;
                    }
                }                
            }
        }
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}  

void Game::DoCollisions()
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // 如果collision 是 true
            {
                ShakeTime = 0.05f;
                Effects->Shake = GL_TRUE;
                // 如果砖块不是实心就销毁砖块
                if (!box.IsSolid)
                    box.Destroyed = GL_TRUE;
                    this->SpawnPowerUps(box);
                // 碰撞处理
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (!(Ball->PassThrough && !box.IsSolid)) { 
                  if (dir == LEFT || dir == RIGHT) // 水平方向碰撞
                  {
                      Ball->Velocity.x = -Ball->Velocity.x; // 反转水平速度
                      // 重定位
                      GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
                      if (dir == LEFT)
                          Ball->Position.x += penetration; // 将球右移
                      else
                          Ball->Position.x -= penetration; // 将球左移
                  }
                  else // 垂直方向碰撞
                  {
                      Ball->Velocity.y = -Ball->Velocity.y; // 反转垂直速度
                      // 重定位
                      GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
                      if (dir == UP)
                          Ball->Position.y -= penetration; // 将球上移
                      else
                          Ball->Position.y += penetration; // 将球下移
                  }
                }
            }
        }
    }
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {

        // 检查碰到了挡板的哪个位置，并根据碰到哪个位置来改变速度
        GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
        GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        GLfloat percentage = distance / (Player->Size.x / 2);
        // 依据结果移动
        GLfloat strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; 
        Ball->Velocity.y = -1 * abs(Ball->Velocity.y);  
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);

        Ball->Stuck = Ball->Sticky;
    } 

    for (PowerUp &powerUp : this->PowerUps)
    {
        if (!powerUp.Destroyed)
        {
            if (powerUp.Position.y >= this->Height)
                powerUp.Destroyed = GL_TRUE;
            if (CheckCollision(*Player, powerUp))
            {   // 道具与挡板接触，激活它！
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = GL_TRUE;
                powerUp.Activated = GL_TRUE;
            }
        }
    }  
}  

void Game::ProcessInput(GLfloat dt)
{
    if (this->State == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // 移动玩家挡板
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
    }
}

void Game::Render()
{
  // fprintf(stdout, "INFO: START RENDER");
  // Texture2D myTexture;
  // myTexture = ResourceManager::GetTexture("face");
  // Renderer->DrawSprite(myTexture, glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
  // Renderer->DrawSprite(ResourceManager::GetTexture("face"), 
  //   glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f)
  // );

  if(this->State == GAME_ACTIVE)
  {
    Effects->BeginRender();
      // 绘制背景
      Renderer->DrawSprite(ResourceManager::GetTexture("background"), 
          glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
      );
      // 绘制关卡
      this->Levels[this->Level].Draw(*Renderer);
      Particles->Draw();
      Ball->Draw(*Renderer);
      Player->Draw(*Renderer);
      for (PowerUp &powerUp : this->PowerUps)
        if (!powerUp.Destroyed)
          powerUp.Draw(*Renderer);

    Effects->EndRender();
    Effects->Render(glfwGetTime());
  }
 

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  // test
  // GLuint VBO;
  // float vertices[] = {
  //       // 位置     // 纹理
  //   0.0f, 1.0f, 0.0f, 1.0f,
  //   0.0f, 0.0f, 0.0f, 0.0f, 
  //   1.0f, 0.0f, 1.0f, 0.0f,

  //   0.0f, 1.0f, 0.0f, 1.0f,
  //   1.0f, 0.0f, 1.0f, 0.0f,
  //   1.0f, 1.0f, 1.0f, 1.0f,
  // };
  // // float vertices[] = {
  //     // 0.0f, -0.5f, 0.0f, 1.0f,
  //     // 0.5f, -0.5f, 0.0f, 1.0f,
  //     // 0.0f,  0.5f, 0.0f, 1.0f
  //   // 0.0f, -1.0f, 0.0f, 1.0f,
  //   // 1.0f, 0.0f, 0.0f, 1.0f,
  //   // 0.0f, 0.0f, 0.0f, 1.0f, 
  // // };
  // GLuint VAO;
  // glGenVertexArrays(1, &VAO);
  // glGenBuffers(1, &VBO);

  // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // glBindVertexArray(VAO);
  // // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*)0);
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
  
  // glEnableVertexAttribArray(0);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);

  // glBindVertexArray(VAO);

  // ResourceManager::GetShader("triangle").Use();
  // glDrawArrays(GL_TRIANGLES, 0, 6);
  // glBindVertexArray(0);
}
