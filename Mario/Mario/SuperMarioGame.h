#pragma once

#include "Blocks.h"
#include "Charasters.h"
#include "Items.h"
#include "GameEngine.h"


const std::string  MARIO_RES_PATH = "res/MarioRes/";

using CTextureManager = ResourceManager<sf::Texture>;
using CFontManager = ResourceManager<sf::Font>;
using CSoundManager = ResourceManager<sf::SoundBuffer>;


class CMarioGameScene;
class CMarioGUI;
 

class Timer
{
 private:
	 int m_time = 0;
	 std::list<std::pair<int, std::function<void()>>> m_invoke_list; 
public:
	 void invoke(const std::function <void()>& func, int delay);
	 void update(int delta_time);
};

class CMarioGame : public CGame
{
private:
	CMarioGame();
	Timer m_timer;
	CMarioGUI* m_gui_object = NULL;
	static CMarioGame* s_instance;
	CGameObject* m_current_scene = NULL;
	std::vector<CGameObject*> m_scene_stack;
	int m_game_time = 300000;
	void syncMarioRank(CGameObject* from_scene, CGameObject* to_scene);
	enum class GameState {load_level, status, playing, level_over} m_game_state = GameState::playing;
	int m_delay_timer = 0;
	int m_lives = 3;
	int m_score = 0;;
	int m_coins = 0;
	std::string m_level_name;
	void updateGUI();
	void clearScenes();
	void init();
	void update(int delta_time) override;
	CMarioGUI* GUI();
	void setScene(CGameObject* game_object);
	void pushScene(CGameObject* game_object);
	void popScene();
public:
	static CMarioGame* CMarioGame::instance();
	~CMarioGame();
	Timer& timer();
 
	void addScore(int value, const Vector& vector = Vector::zero);
	void addCoin();
	void addLive();
	void setScore(int score);
	int getScore() const;
	int getGameTime() const;
	void setEndLevelStatus();
	void loadLevel(const std::string& level_name);
	void loadSubLevel(const std::string& sublevel_name);
	void unloadSubLevel();
 
	

	CLabel* createText(const std::string& text, const Vector& pos);

};  

CMarioGame& MarioGame();

class CBlocks;
class CMoveablePlatform;
class CMario;


class CMarioGameScene : public CGameObject
{
private:
	CMario* m_mario = NULL;
	CBlocks* m_blocks = NULL;
	sf::View m_view;
	Vector screen_size;
	const float scale_factor = 1.5f;
	std::string m_level_name;
public:
	const std::string& getLevelName() const;
	CMarioGameScene();
	CMarioGameScene(const std::string& filepath);
	~CMarioGameScene();
	void loadFromFile(const std::string& filepath);
	Rect cameraRect() const;
	void setCameraOnTarget();
	Vector pointToScreen(const Vector& vector);
	Vector screenToPoint(const Vector& vector);
protected:
    virtual void update(int delta_time) override;
	virtual void draw(sf::RenderWindow* render_window) override;
};


class CMarioGUI : public CGameObject
{
public:
	CMarioGUI();
	void setScore(int value);
	void setCoins(int value);
	void setGameTime(int time);
	void setLevelName(const std::string& string);
	void setLives(int value);
	void setMarioRank(MarioRank rank);
	void showStatus(bool value);
	
	CFlowText* createFlowText();
	CLabel* createLabel();
	void update(int delta_time) override;
	void addText(const std::string& str, const Vector& pos);
	void clearText();

protected:
	void postDraw(sf::RenderWindow* render_window) override;
	void draw(sf::RenderWindow* render_window) override;
private:
	Animator* m_mario_pix;
	Pallete m_fire_pallete;
	bool m_status_mode = false;
	CLabel* m_score_lab = NULL;
	CLabel* m_coin_counter_lab = NULL;
	CLabel* m_world_lab = NULL;
	CLabel* m_timer = NULL;
	CLabel* m_level_name = NULL;
	CLabel* m_lives = NULL;
	CFlowText* m_flow_text;

};




