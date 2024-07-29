
/**
* Author:Regan Zhu
* Assignment: Rise of the AI
* Date due: 2024-7-28, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "LevelA.h"
#include "Utility.h"

#include <SDL_mixer.h>

#define LEVEL_WIDTH 32
#define LEVEL_HEIGHT 11

constexpr char SPRITESHEET_FILEPATH[] = "ss.png",
           ENEMY_FILEPATH[]       = "enemy.png";

bool g_winner = false;
bool g_loser = false;
bool temp = true;
float bullettimeout = 3.0f;

unsigned int LEVELA_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,
    3, 2, 2, 2, 2, 2, 2, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,
    3, 2, 2, 2, 2, 2, 2, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("Screenshot 2024-07-24 at 8.05.41 PM.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 4, 1);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    int player_walking_animation[4][4] =
    {
        { 4,5,6,7 },  // for George to move to the left,
        { 8,9,10,11 }, // for George to move to the right,
        { 12,13,14,15 }, // for George to move upwards,
        { 0,1,2,3 }   // for George to move downwards
    };
    
    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        8.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );
        
    m_game_state.player->set_position(glm::vec3(9.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(5.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[3];

    for (int i = 0; i < 3; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    }
    
    //middle
    m_game_state.enemies[0].set_position(glm::vec3(15.0f, 1.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_game_state.enemies[0].set_entity_type(ENEMY);
    m_game_state.enemies[0].set_ai_type(CHARGER);
    m_game_state.enemies[0].set_ai_state(IDLE);
    
    //third
    m_game_state.enemies[1].set_position(glm::vec3(24.0f, 1.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_game_state.enemies[1].set_entity_type(ENEMY);
    m_game_state.enemies[1].set_ai_type(GUARD);
    m_game_state.enemies[1].set_ai_state(IDLE);
    
    //first
    m_game_state.enemies[2].set_position(glm::vec3(3.0f, 1.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_game_state.enemies[2].set_entity_type(ENEMY);
    m_game_state.enemies[2].set_ai_type(JUMPER);
    m_game_state.enemies[2].set_ai_state(IDLE);
    
    GLuint bullet_texture_id = Utility::load_texture("bullet.png");
    m_game_state.bullet = new Entity(bullet_texture_id,
                                     6.0f,                      // speed
                                     1.0f,                      // width
                                     1.0f,                       // height
                                     BULLET
                                     );
    m_game_state.bullet->set_position(glm::vec3(m_game_state.player->get_position().x,m_game_state.player->get_position().y,m_game_state.player->get_position().z));

    
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("background.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    m_game_state.jump_sfx = Mix_LoadWAV("jump.wav");
    if (!m_game_state.jump_sfx) {
        std::cerr << "Error loading jump sound effect: " << Mix_GetError() << std::endl;
    } else {
        Mix_VolumeChunk(m_game_state.jump_sfx, MIX_MAX_VOLUME );
    }
}

void LevelA::update(float delta_time)
{
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.enemies, 3, m_game_state.map);
    }
    if(m_game_state.player->get_collided_left() ||m_game_state.player->get_collided_right()){
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            if(m_game_state.enemies[i].check_collision(m_game_state.player) ){
                m_game_state.player-> deactivate();
                g_loser = true;
            }
        }
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if(m_game_state.player->check_collision_bottom(m_game_state.enemies[i])){
            std::cout<<"collision with player, enemy dead"<<std::endl;
            ENEMY_COUNT -= 1;
            for (int j = i; j < ENEMY_COUNT; j++)
                m_game_state.enemies[j] = m_game_state.enemies[j + 1];
//                m_game_state.enemies[i].deactivate();
        }
    }
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if(m_game_state.enemies[i].get_position().y<-10.0f){
            std::cout<<"falls off platform, enemy dead"<<std::endl;
            ENEMY_COUNT -= 1;
            for (int j = i; j < ENEMY_COUNT; j++)
                m_game_state.enemies[j] = m_game_state.enemies[j + 1];
//                m_game_state.enemies[i].deactivate();
        }
    }

    if(m_game_state.bullet->get_shoot() && m_game_state.player->facing_right() &&bullettimeout<0.0f){
        std::cout<<"fired";
            m_game_state.bullet->set_position(glm::vec3(m_game_state.player->get_position().x,m_game_state.player->get_position().y,m_game_state.player->get_position().z));
            m_game_state.bullet->set_movement(glm::vec3(-5.0f, 0.0f, 0.0f));
            bullettimeout = 3.0f;
        
    }else if(m_game_state.bullet->get_shoot() && bullettimeout<0.0f){
        std::cout<<"fired";
            m_game_state.bullet->set_position(glm::vec3(m_game_state.player->get_position().x,m_game_state.player->get_position().y,m_game_state.player->get_position().z));
            m_game_state.bullet->set_movement(glm::vec3(-5.0f, 0.0f, 0.0f));
            bullettimeout = 3.0f;
    }
    bullettimeout -= 0.01f;
    if(m_game_state.bullet->get_shoot()){
        m_game_state.bullet->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
        if(m_game_state.bullet->get_collided_left()||m_game_state.bullet->get_collided_right()){
            for (int i = 0; i < ENEMY_COUNT; i++)
            {
                float x_distance = fabs(m_game_state.bullet->get_position().x - m_game_state.enemies[i].get_position().x) - ((1.0f + 1.0f) / 2.0f);
                float y_distance = fabs(m_game_state.bullet->get_position().y - m_game_state.enemies[i].get_position().y) - ((1.0f + 1.0f) / 2.0f)-0.01f;
                if(x_distance + y_distance < 0.3f){
                    std::cout<<"collision with bullet, enemy dead"<<std::endl;
                    ENEMY_COUNT -= 1;
                    for (int j = i; j < ENEMY_COUNT; j++){
                        m_game_state.enemies[j] = m_game_state.enemies[j + 1];
                    }
                    m_game_state.bullet->deactivate();
                    m_game_state.bullet->set_shoot(false);
                    std::cout<<"deactivated bullet";
                }
            }
        }

    }
        
    
    if(ENEMY_COUNT == 0){
        g_winner = true;
    }


    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 1;
    
    
    
}

void LevelA::render(ShaderProgram *program)
{
    if(m_game_state.bullet->get_shoot()){
        m_game_state.bullet->render(program);
    }
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].render(program);
    }
    
    GLuint font_texture_id = Utility::load_texture("font1.png");
    if(g_loser){
        Utility::draw_text(program,font_texture_id, "y", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "o", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 0.5f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "u", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 1.0f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, " ", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 1.5f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "l", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 2.0f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "o", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 2.5f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "s", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 3.0f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "e", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 3.5f, -2.0f, 0.0f) );
    }
    if(g_winner){
        Utility::draw_text(program,font_texture_id, "y", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "o", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 0.5f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "u", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 1.0f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, " ", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 1.5f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "w", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 2.0f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "i", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 2.5f, -2.0f, 0.0f) );
        Utility::draw_text(program,font_texture_id, "n", 0.5f, 0.1f, glm::vec3(m_game_state.player->get_position().x + 3.0f, -2.0f, 0.0f) );
    }}
