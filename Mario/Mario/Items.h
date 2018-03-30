#pragma once

#include "GameEngine.h"
#include <array>
#include <functional>

enum class PlatformType {vertical, horizontal, elevator, skate, no_init = 100};


class CMario;
 
class CItem : public CGameObject
{
public:
	virtual void collsionResponse(CMario* mario, ECollisionTag& collision_tag, int delta_time) = 0;
};
 

class CJumper : public CItem
{
public:
	CJumper();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	virtual void collsionResponse(CMario* mario, ECollisionTag& collision_tag, int delta_time) override;
 
private:
	void onActivated() override;
	CSpriteSheet* m_sprite_sheet;
	CMario* m_mario = NULL;
	bool m_zipped = false;
	float m_bottom;
	int m_state = 0;
	float m_timer = 0;
};

class CPlatform : public CItem
{
 public:
	 virtual Vector getSpeedVector() = 0;
	 void CPlatform::collsionResponse(CMario* mario, ECollisionTag& collision_tag, int delta_time) override;
};


class CMoveablePlatform : public CPlatform
{
public:
	CMoveablePlatform();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	Vector getSpeedVector() override;
	virtual void collsionResponse(CMario* mario, ECollisionTag& collision_tag, int delta_time) override;
protected:
	void start() override;
private:
	const float SKATE_SPEED = 0.17f;
	PlatformType m_platform_type;
	Vector m_center;
	int m_size;
	int m_amplitude;
	float m_period_time = 0;
	int m_bottom = 0;
	float m_timer = 0;
	sf::Sprite m_sprtite;
	Vector m_speed;
	float m_last_delta;
	float m_acceleration = 0;
	Vector m_orientation;
	CMario* m_mario = NULL;
};


class CFallingPlatform : public CPlatform
{
public:
	CFallingPlatform();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	Vector getSpeedVector() override;
	void addImpulse(const Vector& speed);
	void setSpeed(const Vector& speed);
	void setMovingCallback(const std::function<void()>& func)
	{
		m_moving_callback = func;
	}
	void collsionResponse(CMario* mario, ECollisionTag& collision_tag, int delta_time) override;
protected:
	void onActivated() override;
	void start() override;

private:
 	std::function<void()> m_moving_callback;
	sf::Sprite m_sprtite;
	Vector m_speed;
	bool m_stayed = false;
	CMario* m_mario = NULL;
};


class CPlatformSystem : public CGameObject
{
 public:
	 CPlatformSystem();
	 void onActivated() override;
	 void draw(sf::RenderWindow* render_window) override;
	 void update(int delta_time) override;
 private:
	 CFallingPlatform* m_left_platform = NULL;
	 CFallingPlatform* m_right_platform = NULL;
	 void onLeftPlatformMove();
	 void onRightPlatformMove();
	 sf::Sprite m_sprites[5];
};

class CMario;

class CLadder : public CGameObject
{
 public:
	 CLadder();
	 void draw(sf::RenderWindow* render_window) override;
	 void update(int delta_time) override;
	 virtual void onActivated() override;
	 virtual void start() override;
private:
	sf::Sprite m_sprite;
	float m_height, m_width, m_bottom;
	float m_timer = 0;
};


class CFireBar : public CGameObject
{
 public:
	 CFireBar();
	 void draw(sf::RenderWindow* render_window) override;
	 void update(int delta_time) override;
protected:
	 void onActivated() override;
	 void start() override;
 private:
	 Animator m_animator;
	 CMario* m_mario;
	 sf::CircleShape m_shape;
	 std::vector<Vector> m_fire_pos;
	 float m_timer  = 0;
	 float m_speed = -1;
};


class CCoin : public CGameObject
{
public:
	CCoin();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	void kick();
protected:
	void onActivated() override;
	void start() override;
private:
	enum class State {twist, shane} m_state = State::twist;
	int m_remove_timer = 1000;
	Animator m_animator;
	CMario* m_mario = 0;
};



class CLevelPortal : public CGameObject
{
private:
	CMario* m_mario = NULL;
	bool m_used = false;
	CLevelPortal* m_came_back_portal = NULL;
	std::string m_level_name;
	std::string m_sub_level_name;
	void goToLevel();
	void goToSublevel();
	void cameBackFromSublevel();
	virtual void update(int delta_time) override;
	virtual void start() override;
	const int TRANSITION_TIME = 1500;
	float m_timer = 0;
	enum class PortalType { enterLevel, enterSublevel, comebackSublevel } m_portal_type;
	enum class State { wait, transition } m_state = State::wait;
	Vector m_direction;
public:
	CLevelPortal();
};




class CEndLevelFlag : public CGameObject
{

private:
	int cell_y = 0;
	Animator m_animator;
	CMario* m_mario;
	void start() override;
	bool m_touched = false;
public:
	CEndLevelFlag();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
};


class CBowser;
class CBlocks;

class CEndLevelKey : public CGameObject
{
private:
	enum class State {play, bridge_hiding, bowser_run, bowser_fall, mario_go_to_princes, text_show, go_to_next_level } m_state = State::play;
	sf::Sprite m_sprite;
	int m_delay_timer = 0;
	std::vector<Vector> m_bridge_blocks;
	CMario* m_mario = nullptr;
	CBowser* m_bowser = nullptr;
	CBlocks* m_blocks = nullptr;
	void enterState(State state);
	void start() override;
public:
	CEndLevelKey();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
};


class CCastleFlag : public CGameObject
{

private:
	Animator m_animator;
	int m_pos_y = 0;
	void start() override;
public:
	CCastleFlag();
	void liftUp();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
};