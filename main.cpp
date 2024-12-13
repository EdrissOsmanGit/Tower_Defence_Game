#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

// node for the rows, colums, costs and 
struct Node {
    int row, col;
    float fCost, gCost, hCost;
    Node* prev;

    Node(int r, int c){
        row = r;
        col = c;
        fCost = 0;
        gCost = 0;
        hCost = 0;
        prev = nullptr;
    } 
};

// a queue to find the highest fCost after finding f = g + h
class queue {
    std::vector<Node*> value;

public:

    void push(Node* node) {
        value.push_back(node);
    }

    Node* pop() {
        if (value.empty()) {
            return nullptr;
        }
        // pushes the node into the vector
        Node* best = value[0];
        int bestIndex = 0;

        // pops the highets fCost
        for (int i = 1; i < value.size(); i++) {
            if (value[i]->fCost < best->fCost) {
                best = value[i];
                bestIndex = i;
            }
        }

        // findes the highest fCost
        value.erase(value.begin() + bestIndex);
        return best;
    }
    
    // deletes that value and returns it
    bool isEmpty() {
        return value.empty();
    }
};

// calculates the value of hCost 
float heuristic(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

// A* Pathfinding function
std::vector<std::pair<int, int>> findPath(const std::vector<std::vector<int>>& grid, int startX, int startY, int goalX, int goalY) {
    int rows = grid.size();
    int cols = grid[0].size();

    queue list;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    // adds the start grid to the queue
    Node* startNode = new Node(startX, startY);
    list.push(startNode);
    // checks if the space has been visited 
    visited[startX][startY] = true;

    // arrays for the different directions the baloon can go. (up, down, left, right)
    const int dx[] = {-1, 1, 0, 0};
    const int dy[] = {0, 0, -1, 1};

    while (!list.isEmpty()) {
        // where the ballon is
        Node* curr = list.pop();
        // if the ballon is at the goal stop and return path
        if (curr->row == goalX && curr->col == goalY) {

            std::vector<std::pair<int, int>> path;

            while (curr != nullptr) {
                path.push_back({curr->row, curr->col});
                curr = curr->prev;
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        // calculates the path needed
        for (int i = 0; i < 4; i++) {
            //goes through all the different directions we can go
            int newX = curr->row + dx[i];
            int newY = curr->col + dy[i];
            
            // constraintes
            // - needs to be in the grid
            // - and it cant be going to the grass or number 0 in the grid
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && grid[newX][newY] == 0 && !visited[newX][newY]) {
                Node* neighbor = new Node(newX, newY);
                // stores the previus path
                neighbor->prev = curr;
                // calculates the gCost which is the current node to the next 
                neighbor->gCost = curr->gCost + 1;
                // calculates the hCost which is the amount of the current node to the target
                neighbor->hCost = heuristic(newX, newY, goalX, goalY);
                // calculates the the total (fCost) to find the best direction to go to.
                neighbor->fCost = neighbor->gCost + neighbor->hCost;
                
                // pushes the neighbor into the queue which then later pops the best of the fCost as the next positon
                list.push(neighbor);
                // checks if the space has been visited 
                visited[newX][newY] = true;
            }
        }
    }


    return {};
}

// base class for the placed towers
class Tower {
public:
    sf::Sprite sprite;
    sf::Vector2f position;
    int damage;
    float fireRate;
    float range;
    sf::CircleShape rangeIndicator;
    float shootTimer;  
    bool isSelected;
    int cost;
    int upgradeCost;
    

    Tower(sf::Texture& texture) : 
        damage(1), 
        fireRate(1.0f), 
        range(100.0f), 
        shootTimer(0.0f), 
        isSelected(false), // for upgrade menue
        cost(60)
        
    {
        sprite.setTexture(texture);
        sprite.setPosition(position);
        sprite.setScale(0.28f, 0.28f);  
        

        // Setup range indicator
        rangeIndicator.setRadius(range);
        rangeIndicator.setFillColor(sf::Color(255, 255, 255, 50));  
        rangeIndicator.setOutlineThickness(2);
        rangeIndicator.setOutlineColor(sf::Color::Blue);
    }

    virtual ~Tower() = default;  
    
};

// class for the rock that stops balloons path
class Rock {
public:
    sf::Sprite sprite;
    sf::Vector2f position;
    int cost;

    Rock(sf::Texture& texture)   
    {
        cost = 120;
        sprite.setTexture(texture);
        sprite.setPosition(position);
        sprite.setScale(0.28f, 0.28f); 
    
    }
    
};
// the first tower
class BasicTower : public Tower {
public:
    BasicTower(sf::Texture& texture) : Tower(texture) {
        cost = 60;
        damage = 1;
        fireRate = 1.0f;
        range = 100.0f;
        rangeIndicator.setRadius(range);
        rangeIndicator.setPosition(position);
    }

};
// the second tower
class SniperTower : public Tower {
public:
    SniperTower(sf::Texture& texture) : Tower(texture) {
        cost = 130;
        damage = 3;       
        fireRate = 4.0f;  
        range = 300.0f;   
        rangeIndicator.setRadius(range);
        rangeIndicator.setPosition(position);
    }
};
// the third tower
class SplashTower : public Tower {
public:
    SplashTower(sf::Texture& texture) : Tower(texture) {
        cost = 100;
        damage = 2;    
        fireRate = 0.5f;  
        range = 120.0f;   
        rangeIndicator.setRadius(range);
        rangeIndicator.setPosition(position);
    }

};

// class to dragg sprites to there location
class Dragg {
public:
    sf::Sprite sprite;
    bool isBeingDragged;

    Dragg(sf::Texture& texture) : isBeingDragged(false) {
        sprite.setTexture(texture);
    }
    
    // checks if the mouse is ontop of the sprite
    bool contains(sf::Vector2f point) {
        return sprite.getGlobalBounds().contains(point);
    }
};

// class to drop the sprite on to its respective location
class Drop {
public:
    sf::Sprite sprite;

    Drop(sf::Texture& texture) {
        sprite.setTexture(texture);
    }

    // checks if the mouse is ontop of the sprite
    bool contains(sf::Vector2f point) {
        return sprite.getGlobalBounds().contains(point);
    }
};

// gets distance for the tower and balloon using distance formula
float getDistance(const sf::Vector2f& a, const sf::Vector2f& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

int main() {
    
    // opens gui
    sf::RenderWindow window(sf::VideoMode(1150, 800), "Tower Defence Game");

    // defines the differnet screens using enum
    enum GameState {
        StartScreen,
        Playing,
        GameOver
    };
    
    GameState gameState = StartScreen;


    // texture background for starting menu
    sf::Texture startMenuBackgroundTexture;
    if (!startMenuBackgroundTexture.loadFromFile("/Users/ishaq/Desktop/Tower Defence Game (Honors)/Tower-Defense-Game/macOS/texture/Tower Defence Game.jpg")) {
        std::cerr << "Failed to load start menu background image." << std::endl;
        return -1;
    }

    // sets the scale and texture of the backgroud texture
    sf::Sprite startMenuBackgroundSprite;
    startMenuBackgroundSprite.setTexture(startMenuBackgroundTexture);
    startMenuBackgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / startMenuBackgroundTexture.getSize().x,
        static_cast<float>(window.getSize().y) / startMenuBackgroundTexture.getSize().y
    );

    // grid map (grass = 1, path = 0)
    std::vector<std::vector<int>> gridMap = {
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    };

    // defines grid sizes
    int gridSize = 50; 
    int rows = gridMap.size();
    int cols = gridMap[0].size();

    sf::Texture grassTexture;
    if (!grassTexture.loadFromFile("/Users/ishaq/Desktop/Tower Defence Game (Honors)/Tower-Defense-Game/macOS/texture/grassTexture.png")) {
        return -1;
    }

    // sets the texture of the grass and path block to each cell in the grid
    std::vector<std::vector<sf::RectangleShape>> grid(rows, std::vector<sf::RectangleShape>(cols));
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            sf::RectangleShape cell(sf::Vector2f(gridSize - 1, gridSize - 1));
            cell.setPosition(col * gridSize, row * gridSize);

     
            switch(gridMap[row][col]) {
                case 1: 
                    cell.setTexture(&grassTexture);
                    std::cout << "grass" << std::endl;
                    break;
                case 0:
                    cell.setFillColor(sf::Color::White);
                    std::cout << "white" << std::endl;
                    break;
                default:
                    cell.setFillColor(sf::Color::Black);
                    break;
            }
            grid[row][col] = cell;
        }
    }

    // start and end of A* algorithm
    int startX = 0, startY = 7; // starting point 
    int goalX = 9, goalY = 7;  // goal
    // path vector with where the balloons should go
    std::vector<std::pair<int, int>> path = findPath(gridMap, startX, startY, goalX, goalY);

    // balloon sprite texture 
    sf::Texture spriteTexture;
    if (!spriteTexture.loadFromFile("/Users/ishaq/Desktop/Tower Defence Game (Honors)/Tower-Defense-Game/macOS/texture/redBallon.png")) {
        return -1;
    }

    // balloon class for health and activity
    struct Balloon {
        sf::Sprite sprite;
        sf::Vector2f currPos;
        sf::Vector2f targetPos;
        int pathIndex;
        float startDelay;
        bool active;
        bool reachedEnd;
        bool destroyed;
        
     
        int maxHealth;
        int currentHealth;
        sf::RectangleShape healthBarBackground;
        sf::RectangleShape healthBarForeground;

        Balloon(sf::Texture& texture) : 
            pathIndex(0), 
            startDelay(0), 
            active(false), 
            reachedEnd(false),
            maxHealth(2), 
            currentHealth(2), 
            destroyed(false)
        {
            sprite.setTexture(texture);
            sprite.setScale(0.1f, 0.1f);

            // health bar texture
            healthBarBackground.setSize(sf::Vector2f(40, 5));
            healthBarBackground.setFillColor(sf::Color::Red);

            healthBarForeground.setSize(sf::Vector2f(40, 5));
            healthBarForeground.setFillColor(sf::Color::Green);
        }

        // class for balloon damange 
        void takeDamage(int damage) {
            
            currentHealth -= damage;

            if (currentHealth < 0) {
                currentHealth = 0;
            }

            // health bar mechanics 
            float healthPercentage = static_cast<float>(currentHealth) / maxHealth;
            healthBarForeground.setSize(sf::Vector2f(40 * healthPercentage, 5));
        }
    };

 
    sf::Font font;
    if (!font.loadFromFile("/Users/ishaq/Desktop/Tower Defence Game (Honors)/Tower-Defense-Game/atop-font/Atop-R99O3.ttf")) {
        return -1;
    }

    // some defining variales for game machanics
    int currentWave = 0;
    const int MAX_WAVES = 10;
    int lives = 10;
    int score = 120;
    bool waveStarted = false;
    std::vector<Balloon> balloons;

    std::vector<sf::RectangleShape> towerOptions;
    sf::Texture towerTexture1, towerTexture2, rockTexture; 
    Tower* selectedTower = nullptr;

    
    // textures for the towers
    
    if (!towerTexture1.loadFromFile("/Users/ishaq/Desktop/Tower Defence Game (Honors)/Tower-Defense-Game/macOS/texture/3.png")) {
        std::cerr << "Failed to load tower texture 1" << std::endl;
        return -1;
    }
    if (!towerTexture2.loadFromFile("/Users/ishaq/Desktop/Tower Defence Game (Honors)/Tower-Defense-Game/macOS/texture/6.png")) {
        std::cerr << "Failed to load tower texture 2" << std::endl;
        return -1;
    }
    if (!rockTexture.loadFromFile("/Users/ishaq/Desktop/Tower Defence Game (Honors)/Tower-Defense-Game/macOS/texture/52.png")) {
        std::cerr << "Failed to load rock texture" << std::endl;
        return -1;
    }

    
    // for tower images below the sprite
    std::vector<sf::Vector2f> originalTowerPositions;

    sf::RectangleShape towerOption1(sf::Vector2f(50, 50));
    towerOption1.setTexture(&towerTexture1);
    towerOption1.setPosition(30 + (1 * 60), 550); 
    towerOptions.push_back(towerOption1);
    originalTowerPositions.push_back(towerOption1.getPosition());

    sf::RectangleShape towerOption2(sf::Vector2f(50, 50));
    towerOption2.setTexture(&towerTexture2);
    towerOption2.setPosition(30 + (3 * 60), 550); 
    towerOptions.push_back(towerOption2);
    originalTowerPositions.push_back(towerOption2.getPosition());

    
    // defines dragg class
    Dragg tower1(towerTexture1);
    Dragg tower2(towerTexture2);
    tower1.sprite.setPosition(30 + (1 * 60), 550);
    tower2.sprite.setPosition(30 + (3 * 60), 550);

    // sets the sprite size to 50pxx50px
    sf::Vector2u textureSize = tower1.sprite.getTexture()->getSize();
    float scaleX = 50.0f / textureSize.x;
    float scaleY = 50.0f / textureSize.y;

    tower1.sprite.setScale(scaleX, scaleY);
    tower2.sprite.setScale(scaleX, scaleY);

    // set up dragg for rock texture 
    Dragg rock(rockTexture);
    rock.sprite.setScale(scaleX, scaleY);
    rock.sprite.setPosition(30 + (5 * 60), 550);


    // A vector list that will have the tower objects and the rock objects in it 
    std::vector<Tower> placedTowers;
    std::vector<Rock> placedRocks;

    sf::Color tan(210, 180, 140);

    // buttons for start and menu
    sf::RectangleShape startButton(sf::Vector2f(2000, 2000));
    startButton.setFillColor(sf::Color::Transparent);
    startButton.setPosition(0, 0); 

    sf::RectangleShape bottomMenu(sf::Vector2f(750, 300));
    startButton.setFillColor(sf::Color::Transparent);
    startButton.setPosition(0, 300); 

    // text for the game 
    sf::Text startText("Start Game", font, 80);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(320, 320);
   
    sf::Text scoreText("Score: 100", font, 24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, 710);

    sf::Text livesText("Lives: 10", font, 24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(20, 750);

    sf::Text wavesText("Wave: 0/10", font, 24);
    wavesText.setFillColor(sf::Color::White);
    wavesText.setPosition(20, 670);

    // start wave button
    sf::RectangleShape startWaveButton(sf::Vector2f(200, 40));
    startWaveButton.setFillColor(sf::Color::Blue);
    startWaveButton.setPosition(500, 740);

    sf::Text buttonText("Start Wave", font, 20);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setPosition(520, 745);

    sf::Text towersText("Towers", font, 24);
    towersText.setFillColor(sf::Color::White);
    towersText.setPosition(20, 510);

    sf::Text cost("Cost:", font, 22);
    cost.setFillColor(sf::Color::White);
    cost.setPosition(10, 600);

    sf::Text tower1Price("60", font, 22);
    tower1Price.setFillColor(sf::Color::White);
    tower1Price.setPosition(30 + (1 * 60), 600);

    sf::Text tower2Price("130", font, 22);
    tower2Price.setFillColor(sf::Color::White);
    tower2Price.setPosition(30 + (3 * 60), 600);

    sf::Text rockPrice("120", font, 22);
    rockPrice.setFillColor(sf::Color::White);
    rockPrice.setPosition(30 + (5 * 60), 600);


    //Upgrade Menu
    sf::RectangleShape exitButton(sf::Vector2f(50, 50));
    exitButton.setFillColor(sf::Color::Transparent);
    exitButton.setOutlineColor(sf::Color::Blue);
    exitButton.setOutlineThickness(2);
    exitButton.setPosition(800, 50);

    sf::Text upgradeText("Upgrade", font, 40);
    upgradeText.setFillColor(sf::Color::White);
    upgradeText.setPosition(850, 150);

    sf::RectangleShape upgradeFireRateButton(sf::Vector2f(180, 90));
    upgradeFireRateButton.setFillColor(sf::Color::Transparent);
    upgradeFireRateButton.setOutlineColor(sf::Color::Blue);
    upgradeFireRateButton.setOutlineThickness(2);
    upgradeFireRateButton.setPosition(850, 250);

    sf::RectangleShape upgradeDamageButton(sf::Vector2f(180, 90));
    upgradeDamageButton.setFillColor(sf::Color::Transparent);
    upgradeDamageButton.setOutlineColor(sf::Color::Blue);
    upgradeDamageButton.setOutlineThickness(2);
    upgradeDamageButton.setPosition(850, 400);

    sf::RectangleShape upgradeRangeButton(sf::Vector2f(180, 90));
    upgradeRangeButton.setFillColor(sf::Color::Transparent);
    upgradeRangeButton.setOutlineColor(sf::Color::Blue);
    upgradeRangeButton.setOutlineThickness(2);
    upgradeRangeButton.setPosition(850, 550);

    sf::Text upgradeDamageText("Damage: $50", font, 20);
    upgradeDamageText.setFillColor(sf::Color::White);
    upgradeDamageText.setPosition(850, 250);

    sf::Text upgradeFireRateText("Fire Rate: $40", font, 20);
    upgradeFireRateText.setFillColor(sf::Color::White);
    upgradeFireRateText.setPosition(850, 400);

    sf::Text upgradeRangeText("Range: $60", font, 20);
    upgradeRangeText.setFillColor(sf::Color::White);
    upgradeRangeText.setPosition(850, 550);



    // Game Over Screen
    sf::RectangleShape restartButton(sf::Vector2f(300, 100));
    restartButton.setFillColor(sf::Color::Green);
    restartButton.setPosition(225, 500);

    sf::Text restartText("Restart", font, 50);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(300, 520);

    sf::Text gameOverText("Game Over!", font, 80);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(200, 300);

   
    sf::Clock clock;
    float totalElapsedTime = 0.0f;
    float speed = 50.0f; // speed of balloon

    // auto resets game when you loose
    auto resetGame = [&]() {
       
        currentWave = 0;
        lives = 10;
        score = 120;
        waveStarted = false;
        balloons.clear();

        // clears towers
        placedRocks.clear();
        placedTowers.clear();

        gridMap = {
            {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0},
            {1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        };
        
        scoreText.setString("Score: 100");
        livesText.setString("Lives: 10");
        wavesText.setString("Wave: 0/10");

      
        gameState = StartScreen;
    };

    // resets the baloons per wave
    auto resetBalloons = [&]() {
        balloons.clear();
        // increments the number of balloons and the health
        int balloonsInWave = 8 + (currentWave * 4);
        int balloonHealth = currentWave;
        speed += 10.f;

        // finds the new path incase there is a rock in the way
        path = findPath(gridMap, startX, startY, goalX, goalY);

        // works the same as auto but goes though the balloons and checks the health and current path and position
        for (int i = 0; i < balloonsInWave; ++i) {

            balloons.emplace_back(spriteTexture);
            balloons[i].currPos = sf::Vector2f(path[0].second * gridSize, path[0].first * gridSize);
            balloons[i].sprite.setPosition(balloons[i].currPos);
            
        
            balloons[i].startDelay = i * 1.0f;
            
      
            balloons[i].targetPos = sf::Vector2f(path[1].second * gridSize, path[1].first * gridSize);
            
        
            balloons[i].maxHealth = balloonHealth;
            balloons[i].currentHealth = balloonHealth;

           
            balloons[i].healthBarForeground.setSize(sf::Vector2f(40, 5));
        }
    };

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
             
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                if (gameState == StartScreen && startButton.getGlobalBounds().contains(mousePosF)) {
                    gameState = Playing; // starts game
                }
            }

             // game play state event handling
            if (gameState == Playing) {
                // if you click the start wave button then it increments wave and calls resetBallons
                if(event.type == sf::Event::MouseButtonPressed){
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                    // increment wave and reset balloons when start wave button is pressed
                    if (startWaveButton.getGlobalBounds().contains(mousePosF)){
                        if (!waveStarted && currentWave < MAX_WAVES) {
                            currentWave++;
                            resetBalloons();
                            waveStarted = true;
                            totalElapsedTime = 0.0f;
                            // update wave counter display
                            wavesText.setString("Wave: " + std::to_string(currentWave) + "/10"); // increment wave on the string
                        }
                    }
                }

                // handle tower selection and potential upgrades
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    // check for tower selection
                    for (auto& tower : placedTowers) {
                        if (tower.sprite.getGlobalBounds().contains(mousePos)) {
                            // mark tower as selected
                            tower.isSelected = true;

                        } else {
                            // handle upgrade and exit options for selected tower
                            if(event.type == sf::Event::MouseButtonPressed){
                                if (event.mouseButton.button == sf::Mouse::Left) {
                                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                                    sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);
                                    // deselect tower when exit button is clicked
                                    if (exitButton.getGlobalBounds().contains(mousePosF)){
                                        tower.isSelected = false;
                                    }
                                    else if (tower.isSelected){
                                        int col = tower.position.x / gridSize;
                                        int row = tower.position.y / gridSize;
                                        // upgrade tower damage
                                        if (upgradeDamageButton.getGlobalBounds().contains(mousePosF)){
                                            
                                            if (score >= 50){
                                                if (gridMap[row][col] == 3) {
                                                    tower.damage++;
                                                    score -= 50;
                                                }
                                                if (gridMap[row][col] == 4) {
                                                    tower.damage += 2;
                                                    score -= 50;
                                                }
                                            }
                                            
                                            
                                        }
                                        // upgrade tower fire rate
                                        else if (upgradeFireRateButton.getGlobalBounds().contains(mousePosF)){
                                            if(score >= 40){
                                                if (gridMap[row][col] == 3) {
                                                    tower.fireRate -= .1f;
                                                    score -= 40;
                                                    std::cout << "tower1 presed" << std::endl;
                                                }
                                                if (gridMap[row][col] == 4) {
                                                    tower.fireRate += .2f;
                                                    score -= 40;
                                                }
                                            }
                                            
                                            
                                        }
                                        // upgrade tower range
                                        else if (upgradeRangeButton.getGlobalBounds().contains(mousePosF)){
                                            
                                            if (score >= 60){
                                                if (gridMap[row][col] == 3) {
                                                    tower.range += 50.f;
                                                    // update range indicator
                                                    tower.rangeIndicator.setRadius(tower.range);
                                                    tower.rangeIndicator.setPosition(
                                                        tower.position.x + (gridSize / 2) - tower.range, 
                                                        tower.position.y + (gridSize / 2) - tower.range
                                                    );
                                                    score -= 60;
                                                }
                                                if (gridMap[row][col] == 4) {
                                                    tower.range += 100.f;
                                                    // update range indicator
                                                    tower.rangeIndicator.setRadius(tower.range);
                                                    tower.rangeIndicator.setPosition(
                                                        tower.position.x + (gridSize / 2) - tower.range, 
                                                        tower.position.y + (gridSize / 2) - tower.range
                                                    );
                                                    score -= 60;
                                                }
                                            }
                                            
                                            
                                        }
                                    }
                                    
                                }
                            }
                        }
                    }
                }
                
              
                // handle dragging towers and rock
                if(event.type == sf::Event::MouseButtonPressed){
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                        // start dragging tower 1 when clicked
                        if (tower1.contains(mousePosF)) {
                            tower1.isBeingDragged = true;
                            std::cout << "tower1 presed" << std::endl;
                        }
                        // start dragging tower 2 when clicked
                        else if (tower2.contains(mousePosF)) {
                            tower2.isBeingDragged = true;
                            std::cout << "tower2 presed" << std::endl;
                        }
                        // start dragging rock when clicked
                        else if (rock.contains(mousePosF)){
                            rock.isBeingDragged = true;
                            std::cout << "rock presed" << std::endl;
                        }
                    }
                }

                // handle tower when pressed
                if(event.type == sf::Event::MouseButtonPressed){
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                        // calculate grid position
                        int col = mousePos.x / gridSize;
                        int row = mousePos.y / gridSize;

                        if(row >= 0 && row < rows && col >= 0 && col < cols){
                            if (gridMap[row][col] == 3) { 
                                std::cout << "tower1 says hello" << std::endl;
                            }
                            
                        }
                        if(row >= 0 && row < rows && col >= 0 && col < cols){
                            if (gridMap[row][col] == 4) { 
                                std::cout << "tower2 says hello" << std::endl;
                            }
                        }

                    }
                }
                
                // handle tower placement when mouse button is released
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mouseF = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                           
                    // calculate grid position
                    int col = mousePos.x / gridSize;
                    int row = mousePos.y / gridSize;

                    // place basic tower
                    if (tower1.isBeingDragged) {
                        if(row >= 0 && row < rows && col >= 0 && col < cols){
                            // check if grid cell is empty and player has enough score
                            if (gridMap[row][col] == 1 && score >= 60) { 
                                // mark grid cell as occupied by tower
                                gridMap[row][col] = 3; 
                                BasicTower newTower(towerTexture1);
                                // set tower position on grid
                                newTower.position = sf::Vector2f(col * gridSize, row * gridSize);
                                newTower.sprite.setPosition(newTower.position);
                                // set range indicator position
                                newTower.rangeIndicator.setPosition(
                                    newTower.position.x + (gridSize / 2) - newTower.range, 
                                    newTower.position.y + (gridSize / 2) - newTower.range
                                );

                                // add tower to placed towers and deduct cost
                                placedTowers.push_back(newTower);
                                score -= newTower.cost;
                            }
                        }
                        
                        // reset tower 1 to original position
                        tower1.isBeingDragged = false;
                        tower1.sprite.setPosition(30 + (1 * 60), 550); 
                        std::cout << "tower1 Released" << std::endl;
                    }

                    // place sniper tower
                    if (tower2.isBeingDragged) {
                        if(row >= 0 && row < rows && col >= 0 && col < cols){
                            if (gridMap[row][col] == 1 && score >= 100) { 
                                gridMap[row][col] = 4; 
                                SniperTower newTower(towerTexture2);
                                newTower.position = sf::Vector2f(col * gridSize, row * gridSize);
                                newTower.sprite.setPosition(newTower.position);
                                newTower.rangeIndicator.setPosition(
                                    newTower.position.x + (gridSize / 2) - newTower.range, 
                                    newTower.position.y + (gridSize / 2) - newTower.range
                                );

                                placedTowers.push_back(newTower);
                                score -= newTower.cost;

                            }
                        }
                        
                        tower2.isBeingDragged = false;
                        tower2.sprite.setPosition(30 + (3 * 60), 550); 
                        std::cout << "tower1 Released" << std::endl;
                        
                    }

                    // place rock 
                    if(rock.isBeingDragged){
                        if(row >= 0 && row < rows && col >= 0 && col < cols && !waveStarted){
                            if (gridMap[row][col] == 0 && placedRocks.size() < 2 && score >= 120) { 
                                gridMap[row][col] = 1; 
                                // calculate position
                                Rock newRock(rockTexture);
                                newRock.position = sf::Vector2f(col * gridSize, row * gridSize);
                                newRock.sprite.setPosition(newRock.position);
                                // add to placedRocks
                                placedRocks.push_back(newRock);
                                score -= newRock.cost;
                            }
                        }
                        
                        rock.isBeingDragged = false;
                        rock.sprite.setPosition(30 + (5 * 60), 550);
                        std::cout << "tower1 Released" << std::endl;
                    }
                }

                // for draging sprite tower or rock
                if (event.type == sf::Event::MouseMoved) {
                    sf::Vector2f mousePosF = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    if (tower1.isBeingDragged) {
                        tower1.sprite.setPosition(mousePosF);
                        
                    } else if (tower2.isBeingDragged) {
                        tower2.sprite.setPosition(mousePosF);
                        
                    }
                    else if (rock.isBeingDragged) {
                        rock.sprite.setPosition(mousePosF); 
                    }
                }
            }

            // Restart Game Button 
            else if (gameState == GameOver){
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);
                if(restartButton.getGlobalBounds().contains(mousePosF)) {
                    resetGame();
                }
            }

            
        }

    
        // tower shooting mechanics
        float deltaTime = clock.restart().asSeconds();

        // check each placed tower for shooting
        for (auto& tower : placedTowers) {
            tower.shootTimer += deltaTime;  

            // tower shoots when fire rate timer is reached
            if (tower.shootTimer >= tower.fireRate) {
                for (auto& balloon : balloons) {
                    // check if balloon is active and within tower's range
                    if (balloon.active && !balloon.reachedEnd && balloon.currentHealth > 0) {
                        float distance = getDistance(tower.position, balloon.currPos);
                        
                        // if balloon is in range, damage it and reset shoot timer
                        if (distance <= tower.range) {
                            balloon.takeDamage(tower.damage);  
                            tower.shootTimer = 0.0f;
                            break;  
                        }
                    }
                }
            }
        }
        
        // wave progression and balloon movement
        if (waveStarted) {
            totalElapsedTime += deltaTime;

            
            bool allBalloonsDone = true;

            for (auto& balloon : balloons) {
               
                if (!balloon.active && totalElapsedTime < balloon.startDelay) {
                    allBalloonsDone = false;
                    continue;
                }

                
                if (totalElapsedTime >= balloon.startDelay && !balloon.active) {
                    balloon.active = true;
                }

               
                if (balloon.destroyed) {
                    continue;
                }

                
                if (balloon.active && !balloon.reachedEnd) {
                    balloon.healthBarBackground.setPosition(balloon.currPos.x, balloon.currPos.y - 10);
                    balloon.healthBarForeground.setPosition(balloon.currPos.x, balloon.currPos.y - 10);

                    if (balloon.currPos != balloon.targetPos) {
                        sf::Vector2f direction = balloon.targetPos - balloon.currPos;
                        float distance = sqrt((direction.x * direction.x) + (direction.y * direction.y));

                        if (distance != 0.0f) {
                            direction /= distance;
                        }

                        balloon.currPos += direction * speed * deltaTime;

                        if (std::abs(balloon.currPos.x - balloon.targetPos.x) < 0.1f &&
                            std::abs(balloon.currPos.y - balloon.targetPos.y) < 0.1f) {
                            balloon.currPos = balloon.targetPos;
                        }

                        balloon.sprite.setPosition(balloon.currPos);
                    }

                    if (balloon.currPos == balloon.targetPos) {
                        if (balloon.pathIndex >= path.size() - 2) {
                            balloon.reachedEnd = true;
                            lives--;
                            livesText.setString("Lives: " + std::to_string(lives));
                        } else {
                            balloon.pathIndex++;
                            balloon.targetPos = sf::Vector2f(
                                path[balloon.pathIndex].second * gridSize,
                                path[balloon.pathIndex].first * gridSize
                            );
                        }
                    }

                    allBalloonsDone = false;
                }

                if(lives <= 0){
                    gameState = GameOver;
                }

                
                if (balloon.currentHealth <= 0 && !balloon.destroyed) {
                    balloon.destroyed = true;
                    score += 10;
                    scoreText.setString("Cash: " + std::to_string(score));
                }

                
                if (!balloon.destroyed && !balloon.reachedEnd) {
                    allBalloonsDone = false;
                }
            }

            if (allBalloonsDone) {
                waveStarted = false;

                if (currentWave >= MAX_WAVES) {
                    gameState = GameOver;
                }
            }

        }

        window.clear(tan);

        if (gameState == StartScreen) {
       
            window.draw(startMenuBackgroundSprite);
            window.draw(startButton);
            window.draw(startText);
        }
        else if (gameState == Playing) {
            
            
            for (int row = 0; row < rows; row++) {
                for (int col = 0; col < cols; col++) {
                    window.draw(grid[row][col]);
                }
            }

            for (const auto& balloon : balloons) {
                if (balloon.active && !balloon.reachedEnd && !balloon.destroyed) {
                    window.draw(balloon.sprite);
                }
            }

            for (const auto& balloon : balloons) {
                if (balloon.active && !balloon.reachedEnd && !balloon.destroyed) {
                   
                    window.draw(balloon.healthBarBackground);
                    window.draw(balloon.healthBarForeground);
                }
            }

           
            for (const auto& tower : placedTowers) {
                window.draw(tower.sprite);
                if (tower.isSelected){
                    window.draw(tower.rangeIndicator);
                    window.draw(upgradeDamageButton);
                    window.draw(exitButton);
                    window.draw(upgradeFireRateButton);
                    window.draw(upgradeRangeButton);
                    window.draw(upgradeText);
                    window.draw(upgradeDamageText);
                    window.draw(upgradeFireRateText);
                    window.draw(upgradeRangeText);
                }
            } 

            for (const auto& rock : placedRocks){
                window.draw(rock.sprite);
            }

            for (int i = 0; i < towerOptions.size(); i++) {
                window.draw(towerOptions[i]);
            }                                         

            scoreText.setString("Cash: " + std::to_string(score));
            window.draw(scoreText);
            window.draw(livesText);
            window.draw(startWaveButton);
            window.draw(buttonText);
            window.draw(towersText);
            window.draw(wavesText);
            window.draw(tower1.sprite);
            window.draw(tower2.sprite);
            window.draw(rock.sprite);
            window.draw(cost);
            window.draw(tower1Price);
            window.draw(tower2Price);
            window.draw(rockPrice);

        }
        else if (gameState == GameOver) {
            window.draw(gameOverText);
            window.draw(restartButton);
            window.draw(restartText);
        }

        window.display();
    }
    
    return 0;
}