#pragma once
#include "GameEngine.h"

class CBlocks;

class CMarioBullet : public CGameObject
{
public:
	CMarioBullet(const Vector& pos, const Vector& direction);
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
private:
	enum class State { fly, splash } m_state = State::fly;
	void setState(State state);
	void start() override;
	const float bullet_speed = 0.33;
	const float gravity_force = 0.0015;
	Vector m_direction;
	float m_timer = 0;
	Vector m_speed;
	CBlocks* m_blocks = NULL;
	Animator m_animator;
};
 
enum class MarioState { normal, promoting, demoting};

class CMario;
enum class MarioRank;

class IMarioState
{
 public:
	 ~IMarioState() = default;
	 virtual void onEnter() {};
	 virtual void onLeave() {};
	 virtual void update(int delta_time) {};
	 void setMario(CMario* mario);
protected:
	 CMario* mario();
	 void setMarioSpawnProtection();
	 void playAnimation(const std::string& anim_name);
	 void setMarioRank(const MarioRank& rank);
	 void setMarioState(const MarioState& state);
	 void setMarioNormalPallete();
	 void setMarioFirePallete();
	 void enableScene(bool value);
private:
	CMario* m_mario;
};



class CNormalMarioState : public IMarioState
{
public:
	void onEnter() override;
	void onLeave() override;
	void update(int delta_time) override;
};


class CDiedMarioState : public IMarioState
{
 public:
	 void onEnter() override;
	 void onLeave() override;
	 void update(int delta_time) override;
};


class CPromotingMarioState : public IMarioState
{
public:
	void onEnter() override;
	void onLeave() override;
	void update(int delta_time) override;

private:
	float m_promoting_timer = 0;
};

class CDemotingMarioState : public IMarioState
{
public:
	void onEnter() override;
	void onLeave() override;
	void update(int delta_time) override;

private:
	float m_promoting_timer = 0;
};


class CTransitionMarioState : public IMarioState
{
public:
	CTransitionMarioState(const Vector&  speed, int time);
	void onEnter() override;
	void onLeave() override;
	void update(int delta_time) override;
private:
	Vector m_speed;
	int m_timer = 0;
	int m_time;
};


class CGoToCastleMarioState : public IMarioState
{
private:
	enum class State {start,go_down, overturn, walk, wait, next_level} m_state = State::start;
	Vector m_speed;
	int m_timer = 0;
	int m_cell_y;
	int m_delay_timer = 0;
	std::string m_next_level;
	std::string m_next_sub_level;
public:
	CGoToCastleMarioState();
	void onEnter() override;
	void onLeave() override;
	void update(int delta_time) override;
};


class CGoToPortalState : public IMarioState
{
public:
	CGoToPortalState();
	void onEnter() override;
	void onLeave() override;
	void update(int delta_time) override;
private:

};

class CPrincess;
class CGoToPrincessState : public IMarioState
{
public:
	CGoToPrincessState();
	void onEnter() override;
	void onLeave() override;
	void update(int delta_time) override;
private:
	CPrincess* m_princess = NULL;
};



class CLadder;


enum class MarioRank { small, big, fire };

class CMario : public CGameObject
{
public:
	CMario();
	MarioRank getRank() const;
	void setRank(MarioRank rank);
	void promote();
	void demote();
	Vector getSpeed() const;
	void setSpeed(const Vector& speed);
	void addImpulse(Vector _speed);
	void reciveDamage();
	Vector getInputDirection();
	bool isGrounded() const;
	void setUnclimb();
	bool isSmall() const;
	void setState(MarioState state);
	void setState(IMarioState* state);
 
private:
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	void setGrounded();
	void inputProcessing(float delta_time);
	void physicProcessing(float delta_time);
	void collisionProcessing(float delta_time);
	void kickBlocksProcessing();
	void kickEnemiesProcessing(float delta_time);
	void animationProcessing(float delta_time);
	void playAnimation(const std::string& name);
	void onActivated() override;
	void start() override;
	const float jump_force = 0.3f;
	const float jump_time = 300.f;
	const float walk_speed = 0.2f;
	const float run_speed = 0.3f;
	const float acceleration = 0.001f;
	const float friction_force = 0.0005f;
	const float gravity_force = 0.0015f;
	const float climb_speed = 0.1f;
	const int fire_rate = 400; //bullet per ms
	const Vector small_mario_size = { 31,32 };
	const Vector big_mario_size = { 31,64 };
	const Vector seated_mario_size = { 31,44 };
	float m_jumping_timer = 0;
	void fire();
	MarioRank m_rank = MarioRank::small;
	MarioState m_state = MarioState::normal;
	IMarioState* m_current_state = NULL;
	ECollisionTag m_collision_tag;
	Vector m_speed;
	float m_x_max_speed;
	bool m_grounded = false;
	bool m_jumped = false;
	bool m_seated = false;
	bool m_climb = false;
	bool m_in_water = false;
	CLadder* m_used_ladder = NULL;
	CBlocks* m_blocks;
	Animator* m_animator;
	float m_fire_timer = 0;
	float m_spawn_timer = 0;
	Vector m_direction = Vector::right;
	Vector m_input_direcition;
	friend class IMarioState;
	friend class CPromotingMarioState;
	friend class CNormalMarioState;
	friend class CTransitionMarioState;
	friend class CGoToCastleMarioState;
	friend class CGoToPortalState;
	friend class CGoToPrincessState;
	Pallete m_fire_pallete, m_black_pallete;
};

class CEnemy : public CGameObject
{
public:
	virtual void kickFromTop(CMario* mario) = 0;
	virtual void kickFromBottom(CMario* mario) = 0;
	virtual bool isAlive() const = 0;
	virtual void touchSide(CMario* mario) = 0;
	virtual void fired(CMario* mario);
	void update(int delta_time);
protected:
    virtual void start() override;
	CMario* mario();
	void addScoreToPlayer(int score);
	void checkNextTileUnderFoots();
	void checkCollideOtherCharasters();
	void checkFallUndergound();
	void updateCollision(float delta_time);
	void CEnemy::updatePhysics(float delta_time, float gravity);
	ECollisionTag m_collision_tag;
	const float gravity_force = 0.0015;
	Vector m_speed;
	Vector m_direction = Vector::left;
	CMario* m_mario = NULL;
	CBlocks* m_blocks = NULL;
	const float m_run_speed = -0.05;
};

class CGoomba : public CEnemy
{
public:
	CGoomba();
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;
private:
	enum State { Deactivated, Normal, Cramped, Died } m_state = Deactivated;
	void setState(State state);
	Animator m_animator;
	float m_timer = 0;
};

  
class CKoopa : public CEnemy
{
public:
	CKoopa();
	void setFlying();
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual bool isAlive() const override;
	virtual void touchSide(CMario* mario) override;
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;

private:
	const Vector full_size = { 32,48 };
	const Vector hidden_size = { 32,32 };
	void onActivated() override;
	enum State { Deactivated, Normal, Jumping, Hidden, Climb, Bullet, Died } m_state = Deactivated;
	void setState(State state);
	Animator m_animator;
	bool m_is_jumping = false;
	float m_timer = 0;
};


class ÑBuzzyBeetle : public CEnemy
{
public:
	ÑBuzzyBeetle();
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void fired(CMario* mario) override;
	virtual bool isAlive() const override;
	virtual void touchSide(CMario* mario) override;
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;

private:
	enum State { Deactivated, Normal, Hidden, Bullet, Died } m_state = Deactivated;
	void setState(State state);
	Animator m_animator;
	bool m_is_flying = false;
	float m_timer = 0;
};
 
class CHammer : public CGameObject
{
public:
	CHammer(CMario* target);
	virtual void update(int delta_time) override;
	void  throwAway(const Vector& speed);
	void  draw(sf::RenderWindow* render_window);
private:
	enum State { in_hand, fly } m_state = in_hand;
	CGameObject* m_parent;
	Vector m_direction;
	const float gravity_force = 0.0015f;
	CMario* m_target;
	Animator m_animator;
	Vector m_speed;
};

class CHammerBro : public CEnemy
{
public:
	CHammerBro();
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void onActivated() override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;
 
private:
	const int fire_rate = 1200;
	const int jump_rate = 5000;
	const int walk_amlitude = 25;
	bool isCanJumpUp() const;
	bool isCanJumpDown() const;
	bool m_collision_on = true;
	enum State { Normal, Died } m_state = Normal;
	Vector m_jump_direction = Vector::up;
	void setState(State state);
	CHammer* m_hummer = NULL;
	Animator m_animator;
	int m_center_x = 0;
	float m_jump_timer = 0;
	float m_fire_timer = 0;
	float m_drop_off_height = 0;
};

 
class CSpinny : public CEnemy
{
public:
	CSpinny(const Vector& position, const Vector& speed, const Vector& walk_direction);
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;
private:
	enum State {Egg, Normal,Died} m_state = Egg;
	Vector m_walk_direction;
	void setState(State state);
	Animator m_animator;
};


class CLakity : public CEnemy
{
public:
	CLakity();
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;
	void runAway(const Vector& run_direction);
	void start() override;
private:
	const int fire_rate = 1000;
	float m_fire_timer = 0;
	float m_died_timer = 0;
	enum State { Normal, Died, RunAway} m_state = Normal;
	void setState(State state);
	Animator m_animator;
};

class CLakitySpawner : public CGameObject
{
public:
	CLakitySpawner();
	void update(int delta_time) override;
protected:
	void onActivated() override;
	void start() override;
private:
	const int check_interval = 5000;
	CLakity* m_lakity = NULL;
	CMario* m_mario = NULL;
	float m_lakity_checker_timer = 0;
};


class CCheepCheep : public CEnemy
{
public:
	CCheepCheep();
	CCheepCheep(const Vector& initial_pos, const Vector& initial_speed);
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;
private:
	enum State { Normal, Underwater, Died } m_state = Normal;
	void setState(State state);
	Animator m_animator;
};


 
class CCheepCheepSpawner : public CGameObject
{
public:
	CCheepCheepSpawner();
	void update(int delta_time) override;
	void onActivated() override;
	void start() override;
private:
	const int spawn_interval = 1000;
	const Vector spawn_speed = { 0.32f,-0.65f };
	float m_spawn_timer = 0;
	int m_map_height;
	CMario* m_mario = NULL;
};


class CBlooper : public CEnemy
{
public:
	CBlooper();
	void update(int delta_time) override;
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;
private:

	enum State {Zig, Zag, Died } m_state = State::Zig;
	void enterState(State state); 
	int m_delay_time = 0;
	Vector m_speed;
	Animator m_animator;
};


class CBulletBill : public CEnemy
{
public:
	CBulletBill(const Vector& initial_pos, const Vector& initial_speed);
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;
	virtual void start() override;
private:
	enum State { Normal, Died } m_state = Normal;
	void setState(State state);
	Animator m_animator;
};

class CBulletBillSpawner : public CGameObject
{
public:
	CBulletBillSpawner();
	void update(int delta_time) override;
	void onActivated() override;
	void start() override;
private:
	CMario* m_mario = NULL;
	const int spawn_interval = 4000;
	const float bullet_speed = 0.15f;
	float m_spawn_timer = 0;
};

 
class CPiranhaPlant : public CEnemy
{
public:
	CPiranhaPlant();
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual void fired(CMario* mario) override;
	virtual bool isAlive() const override;
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	void onActivated() override;
	void hideInTube();
private:
	const Vector size = { 32,46 };
	const float period_time = 2000;
	sf::Sprite m_sprite;
	float m_timer = 0;
	float m_buttom;
};


class CPodoboo : public CEnemy
{
public:
	CPodoboo();
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual void fired(CMario* mario) override;
	virtual bool isAlive() const override;
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	void onActivated() override;
private:
	float m_timer = 0;
	const float period_time = 3000.f;
	const float amplitude = 400.f;
	float m_acceleration = 0;
	float m_max_speed;
	Vector m_center;
	Vector m_speed;
	const Vector size = { 32,32 };
	Animator m_animator;
	sf::CircleShape m_shape;
};


class CFireball : public CGameObject
{
public:
	CFireball(const Vector& Position, const Vector& SpeedVector);
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void start() override;
private:
	int m_life_timer = 10000;
	CMario* m_mario = NULL;
	Vector m_speed;
	Animator m_animator;
};


class CBowser : public CEnemy
{
public:
	CBowser();
	virtual void draw(sf::RenderWindow* render_window) override;
	virtual void update(int delta_time) override;
	virtual void onActivated() override;
	virtual void kickFromTop(CMario* mario) override;
	virtual void kickFromBottom(CMario* mario) override;
	virtual void touchSide(CMario* mario) override;
	virtual bool isAlive() const override;

	const int c_walk_amlitude = 100;
	const int c_run_speed = 100;
	const int c_jump_period = 4000;
	void noBridge();
private:
	const float anim_speed = 0.008f;
	enum class State {walk, turn,jump,pre_jump,middle_fire,land_fire, no_bridge, fall,died} m_state = State::walk;
	void enterState(State state);
	int m_lives = 5;
	int m_delay_timer = 0;
	Animator m_animator;
	float m_center_x;
	Vector m_old_speed;
};


class CPrincess : public CGameObject
{
public:
	CPrincess();
	virtual void draw(sf::RenderWindow* render_window) override;

private:
	Animator m_animator;
};





