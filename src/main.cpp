#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <gl/Glu.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

#define DEGTORAD 0.017453f
#define RADTODEG 57.29577f

float angleX, angleY; // Углы поворота камеры
float size = 32.0f;
const int map_size = 256;

bool mass[map_size][map_size][map_size];

bool check(int x, int y, int z)
{
   if ((x < 0) || (x >= map_size) || 
	   (y < 0) || (y >= map_size) || 
	   (z < 0) || (z >= map_size)) return false;
  
   return mass[x][y][z];
}

void DrawCube(float size, sf::Texture* pTexture)
{
    sf::Texture::bind(pTexture);

    glBegin(GL_QUADS);
        glVertex3f(-size, -size, -size); glTexCoord2f(0, 0); 
        glVertex3f(size,  -size, -size); glTexCoord2f(0, 1); 
        glVertex3f(size,   size, -size); glTexCoord2f(1, 1); 
        glVertex3f(-size,  size, -size); glTexCoord2f(1, 0); 

        glVertex3f(size,  -size, size); glTexCoord2f(0, 0); 
        glVertex3f(-size, -size, size); glTexCoord2f(0, 1); 
        glVertex3f(-size, size, size);  glTexCoord2f(1, 1); 
        glVertex3f(size,  size, size);  glTexCoord2f(1, 0); 

        glVertex3f(-size, -size,  size); glTexCoord2f(0, 0); 
        glVertex3f(-size, -size, -size); glTexCoord2f(0, 1); 
        glVertex3f(-size,  size, -size); glTexCoord2f(1, 1); 
        glVertex3f(-size,  size,  size); glTexCoord2f(1, 0);  

        glVertex3f(size, -size, -size); glTexCoord2f(0, 0); 
        glVertex3f(size,  -size, size); glTexCoord2f(0, 1); 
        glVertex3f(size,  size,  size); glTexCoord2f(1, 1); 
        glVertex3f(size, size,  -size); glTexCoord2f(1, 0); 
    glEnd();

    sf::Texture::bind(pTexture + 1);

    glBegin(GL_QUADS);
        glVertex3f(-size, -size,  size); glTexCoord2f(0, 0); 
        glVertex3f(-size, -size, -size); glTexCoord2f(0, 1); 
        glVertex3f( size, -size, -size); glTexCoord2f(1, 1); 
        glVertex3f( size, -size,  size); glTexCoord2f(1, 0); 
    glEnd();

    sf::Texture::bind(pTexture + 2);

    glBegin(GL_QUADS);
        glVertex3f(-size, size,  size); glTexCoord2f(0, 0); 
        glVertex3f(-size, size, -size); glTexCoord2f(0, 1); 
        glVertex3f( size, size, -size); glTexCoord2f(1, 1); 
        glVertex3f( size, size,  size); glTexCoord2f(1, 0); 
    glEnd();
}

void DrawSkyBox(float size, sf::Texture* pTexture)
{
    glEnable(GL_TEXTURE_2D);

    sf::Texture::bind(pTexture);

    glBegin(GL_QUADS);
        glVertex3f(size,  -size, size); glTexCoord2f(0, 0); 
        glVertex3f(-size, -size, size); glTexCoord2f(0, 1); 
        glVertex3f(-size, size, size);  glTexCoord2f(1, 1); 
        glVertex3f(size,  size, size);  glTexCoord2f(1, 0); 
    glEnd();

    sf::Texture::bind(pTexture + 1);

    glBegin(GL_QUADS);
        glVertex3f(-size, -size, -size); glTexCoord2f(0, 0); 
        glVertex3f(size,  -size, -size); glTexCoord2f(0, 1); 
        glVertex3f( size,  size, -size); glTexCoord2f(1, 1); 
        glVertex3f( -size, size, -size); glTexCoord2f(1, 0); 
    glEnd();

    sf::Texture::bind(pTexture + 2);

    glBegin(GL_QUADS);
        glVertex3f(-size, -size,  size); glTexCoord2f(0, 0); 
        glVertex3f(-size, -size, -size); glTexCoord2f(0, 1); 
        glVertex3f(-size,  size, -size); glTexCoord2f(1, 1); 
        glVertex3f(-size,  size,  size); glTexCoord2f(1, 0);  
    glEnd();

    sf::Texture::bind(pTexture + 3);

    glBegin(GL_QUADS);
        glVertex3f(size, -size,-size); glTexCoord2f(0, 0); 
        glVertex3f(size, -size, size); glTexCoord2f(0, 1); 
        glVertex3f(size, size,  size); glTexCoord2f(1, 1); 
        glVertex3f(size, size, -size); glTexCoord2f(1, 0); 
    glEnd();

    sf::Texture::bind(pTexture + 4);

    glBegin(GL_QUADS);
        glVertex3f(size,  -size, -size); glTexCoord2f(0, 0); 
        glVertex3f(-size, -size, -size); glTexCoord2f(0, 1); 
        glVertex3f(-size, -size,  size); glTexCoord2f(1, 1); 
        glVertex3f(size,  -size,  size); glTexCoord2f(1, 0); 
    glEnd();

    sf::Texture::bind(pTexture + 5);

    glBegin(GL_QUADS);
        glVertex3f(size,  size, size);  glTexCoord2f(0, 0); 
        glVertex3f(-size, size, size);  glTexCoord2f(0, 1); 
        glVertex3f(-size, size, -size); glTexCoord2f(1, 1); 
        glVertex3f(size,  size, -size); glTexCoord2f(1, 0); 
    glEnd();
}

class Player
{
public:
    Player(float x0, float y0, float z0)
    {
        x = x0; y = y0; z = z0;
        dx = 0; dy = 0; dz = 0;
        w = 5; h = 20; d = 5;
        speed = 50;
        onGround = false;
    }

    void update(float dt)
    {
        if (!onGround)
            dy -= 120 * dt;
        onGround = 0;

        x += dx * dt;
        collision(dx, 0, 0);
        y += dy * dt;
        collision(0, dy, 0);
        z += dz * dt;
        collision(0, 0, dz);

        dx = dz = 0;
    }

    void collision(float Dx, float Dy, float Dz)
    {
        for (int X = (x - w) / size; X < (x + w) / size; X++)
            for (int Y = (y - h) / size; Y < (y + h) / size; Y++)
                for (int Z = (z - d) / size; Z < (z + d) / size; Z++)
                    if (check(X, Y, Z))
                    {
                        if (Dx > 0) x = X * size - w;
                        if (Dx < 0) x = X * size + size + w;
                        if (Dy > 0) y = Y * size - h;
                        if (Dy < 0) { y = Y * size + size + h; onGround = true; dy = 0; }
                        if (Dz > 0) z = Z * size - d;
                        if (Dz < 0) z = Z * size + size + d;
                    }
    }

    void keyboard()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && onGround)
        {
            onGround = false;
            dy = 100;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            dx = -sin(angleX * DEGTORAD) * speed;
            dz = -cos(angleX * DEGTORAD) * speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            dx = sin(angleX * DEGTORAD) * speed;
            dz = cos(angleX * DEGTORAD) * speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            dx = sin((angleX + 90) * DEGTORAD) * speed;
            dz = cos((angleX + 90) * DEGTORAD) * speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            dx = sin((angleX - 90) * DEGTORAD) * speed;
            dz = cos((angleX - 90) * DEGTORAD) * speed;
        }
    }

    float x, y, z;
    float dx, dy, dz;
    float w, h, d; // width, height, depth
    bool onGround;
    float speed;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML OpenGL", sf::Style::Default, sf::ContextSettings(32));
    window.setFramerateLimit(60);

//  Height map
    sf::Image height_map_image; 
    height_map_image.loadFromFile("res/textures/heightmap.png");

    for (int x = 0; x < map_size; ++x)
        for (int z = 0; z < map_size; ++z)
        {
            int c = height_map_image.getPixel(x, z).r >> 4;

            for (int y = 0; y < c; ++y)
                if (y > c - 3)
                    mass[x][y][z] = 1;
        }

//  Cubes
    sf::Image cube_images[3];
    cube_images[0].loadFromFile("res/textures/grassBox/side.jpg");  cube_images[0].flipVertically();
	cube_images[1].loadFromFile("res/textures/grassBox/bottom.jpg"); 
	cube_images[2].loadFromFile("res/textures/grassBox/top.jpg");    

    sf::Texture cube_textures[3];
    cube_textures[0].loadFromImage(cube_images[0]);
    cube_textures[1].loadFromImage(cube_images[1]);
    cube_textures[2].loadFromImage(cube_images[2]);

//  Background
    sf::Image sky_box_images[6];
	sky_box_images[0].loadFromFile("res/textures/skybox3/skybox_front.bmp");  sky_box_images[0].flipVertically();
	sky_box_images[1].loadFromFile("res/textures/skybox3/skybox_back.bmp");   sky_box_images[1].flipVertically();
	sky_box_images[2].loadFromFile("res/textures/skybox3/skybox_left.bmp");   sky_box_images[2].flipVertically();
    sky_box_images[3].loadFromFile("res/textures/skybox3/skybox_right.bmp");  sky_box_images[3].flipVertically();
	sky_box_images[4].loadFromFile("res/textures/skybox3/skybox_bottom.bmp"); sky_box_images[4].flipVertically();
	sky_box_images[5].loadFromFile("res/textures/skybox3/skybox_top.bmp");    sky_box_images[5].flipVertically();

    sf::Texture sky_box_textures[6];
    sky_box_textures[0].loadFromImage(sky_box_images[0]);
    sky_box_textures[1].loadFromImage(sky_box_images[1]);
    sky_box_textures[2].loadFromImage(sky_box_images[2]);
    sky_box_textures[3].loadFromImage(sky_box_images[3]);
    sky_box_textures[4].loadFromImage(sky_box_images[4]);
    sky_box_textures[5].loadFromImage(sky_box_images[5]);

//  Cursor
    ShowCursor(FALSE);

    sf::Texture cursor_texture;
    cursor_texture.loadFromFile("res/textures/cursor.png");

    sf::Sprite cursor_sprite(cursor_texture); 
    cursor_sprite.setOrigin(8,8); 
    cursor_sprite.setPosition(400,300); 

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClearDepth(1.f);

    // Disable lighting
    glDisable(GL_LIGHTING);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 2000.f);

    // Create a clock for measuring the time elapsed
    sf::Clock clock;

    Player player(100, 200, 100);

    bool mLeft=0, mRight=0; // mouse buttons

    // Start game loop
    while (window.isOpen())
    {
        float dt  = clock.restart().asSeconds();
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                window.close();

            // Escape key : exit
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
				{if (event.key.code == sf::Mouse::Right) mRight=1;
				 if (event.key.code == sf::Mouse::Left)  mLeft=1;}

            // Adjust the viewport when the window is resized
            if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);
        }

        player.keyboard();
        player.update(dt);

        POINT mousexy;
        GetCursorPos(&mousexy);
        int xt = window.getPosition().x + (window.getSize().x >> 1);
        int yt = window.getPosition().y + (window.getSize().y >> 1);

        angleX += (xt - mousexy.x) * 0.25f; // 4 — чувствительность
        angleY += (yt - mousexy.y) * 0.25f;

        if (angleY < -89.0)  
            angleY = -89.0;
        
        if (angleY > 89.0)   
            angleY = 89.0;   

        SetCursorPos(xt, yt);

        if (mRight || mLeft)
        {
            float x = player.x;
            float y = player.y + player.h * 0.5f;
            float z = player.z;

            int X = 0, Y = 0, Z = 0, oldX = 0, oldY = 0, oldZ = 0;
            int dist = 0;

            while (dist < 120) // радиус действия
            {
                dist++;

                x += -sin(angleX * DEGTORAD);
                X = x / size;
                y += tan(angleY * DEGTORAD);
                Y = y / size;
                z += -cos(angleX * DEGTORAD);
                Z = z / size;

                if (check(X, Y, Z))
                    if (mLeft)
                    {
                        mass[X][Y][Z] = 0;
                        break;
                    }
                    else
                    {
                        mass[oldX][oldY][oldZ] = 1;
                        break;
                    }

                oldX = X;
                oldY = Y;
                oldZ = Z;
            }
        }

        mLeft = mRight = false;

        window.setActive();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gluLookAt(player.x, player.y + player.h * 0.5f, player.z, 
                  player.x - sin(angleX * DEGTORAD), 
                  player.y + player.h * 0.5f + (tan(angleY * DEGTORAD)), 
                  player.z - cos(angleX * DEGTORAD), 
                  0, 1, 0);


        static int counter = 0;

        if(++counter > 120)
        {
            counter = 0;
            std::cout << player.x << '\t' << player.y << '\t' << player.z << '\n';
        }

        // Draw map
        int R = 20;
        int X = player.x / size;
        int Y = player.y / size;
        int Z = player.z / size;
        float half_size = size * 0.5f;

        for (int x = X - R; x < X + R; ++x)
            for (int y = 0; y < R; ++y)
                for (int z = Z - R; z < Z + R; ++z)
                {
                    if( x < 0 || z < 0) continue;

                    if (mass[x][y][z])
                    {
                        glPushMatrix();
                        glTranslatef(x * size + half_size, y * size + half_size, z * size + half_size);
                        DrawCube(size * 0.5f, &cube_textures[0]);
                        glPopMatrix();
                    }
                }

        glPushMatrix();
            glTranslatef(player.x, player.y, player.z);
            DrawSkyBox(1000, &sky_box_textures[0]);
        glPopMatrix();

        window.pushGLStates();
		window.draw(cursor_sprite);      
        window.popGLStates();

        window.display();
    }

    return EXIT_SUCCESS;
}