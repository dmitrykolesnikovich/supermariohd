#pragma once

#include "TileMap.h"
#include "GameEngine.h"

enum EBlock { empty = 0, ground, armor,brick, question };

class CMario;
class CBlocks;

class AbstractBlock
{
public:
	AbstractBlock(int id);
	virtual ~AbstractBlock();
	virtual void draw(sf::RenderWindow* render_window);
	virtual void update(int delta_time);
	virtual void kick(CMario* mario);
	void setPosition(const Vector& pos);
	bool isColliable() const;
	void setParent(CBlocks* blocks);
	int code() const;
	static const int BRICK_TILE_CODE = 2;
	static const int MONEY_TILE_CODE = 3;
	static const int COIN_TILE_CODE = 11;
	static const int MUSHROOM_TILE_CODE = 12;
	static const int LADDER_TILE_CODE = 17;
	static const int BRIDGE_TILE_CODE = 18;
	static const int CHAIN_TILE_CODE = 81;
protected:
	void killEnemiesAbove(CMario* mario);
	bool m_colliable = true;
	friend class CBlocks;
	static int block_size;
	static float timer;
	CBlocks* m_blocks = NULL;
	Vector position;
	static CSpriteSheet m_question_block_sprite;
	static CSpriteSheet m_water_sprite;
	static CSpriteSheet m_lava_sprite;
	int m_id;
};


class CStaticBlock : public AbstractBlock
{
public:
	CStaticBlock(int id);
	virtual void draw(sf::RenderWindow* render_window);
	virtual void kick(CMario* mario) override;
private:
	friend class CBlocks;
	static CSpriteSheet m_sprite_sheet;
	sf::Sprite* cur_sprite = NULL;
};



class CBricksBlock : public AbstractBlock
{
public:
	CBricksBlock();
	virtual void draw(sf::RenderWindow* render_window);
	virtual void kick(CMario* mario) override;
	virtual void update(int delta_time);
private:
	CSpriteSheet m_sprite_sheet;
	float kicked_value = 0;
	int kicked_dir = 0;
};


class CMoneyBox : public AbstractBlock
{
public:
	CMoneyBox();
	virtual void draw(sf::RenderWindow* render_window);
	virtual void kick(CMario* mario) override;
	virtual void update(int delta_time);
private:
	CSpriteSheet m_sprite_sheet;
	int m_coin_left = 5;
	float kicked_value = 0;
	int kicked_dir = 0;
};


class CQuestionBlock : public AbstractBlock
{
public:
	CQuestionBlock(int id);
	virtual void draw(sf::RenderWindow* render_window);
	virtual void update(int delta_time);
	virtual void kick(CMario* mario) override;
private:
	virtual CGameObject* getObject(const Vector& pos, CMario* mario) = 0;
	float kicked_value = 0;
	bool kicked = false;
	int kicked_dir = 0;
	friend class CBlocks;
	static int q_index;
	CSpriteSheet  m_kicked_sprite;
	CSpriteSheet* m_current_sprite = NULL;
};


class CCoinBox : public CQuestionBlock 
{
public:
	CCoinBox() : CQuestionBlock(AbstractBlock::COIN_TILE_CODE) {};
	virtual CGameObject* getObject(const Vector& pos, CMario* mario) override;
};


class CMushroomBox : public CQuestionBlock
{
public:
	CMushroomBox() : CQuestionBlock(AbstractBlock::MUSHROOM_TILE_CODE) {};
	virtual CGameObject* getObject(const Vector& pos, CMario* mario) override;
};


class CLadderBlock : public CQuestionBlock
{
public:
	CLadderBlock() : CQuestionBlock(AbstractBlock::LADDER_TILE_CODE) {};
	virtual CGameObject* getObject(const Vector& pos, CMario* mario) override;
};
 
 

class CBackground : public CGameObject
{
public:
	CBackground();

	virtual void draw(sf::RenderWindow* render_window);
private:
	void start() override;
	void onActivated() override;
	sf::Sprite m_background;
};


class CBlocks : public CGameObject
{
private:
	Vector kicked_block;
	Rect view_rect;
	bool m_night_view_filter = false;
	TileMap<AbstractBlock*>* m_map;
	sf::RectangleShape m_shape;
	float m_timer = 0;
	int m_block_size = 32;
 
public:
	CBlocks(int cols, int rows, int block_size);
	~CBlocks();
	Vector blockSize() const;
	AbstractBlock* getBlock(int x, int y);
	int rows() const;
	int cols() const;
	int width() const;
	int height() const;
	Vector toBlockCoordinates(const Vector& pixel, bool rounded = false) const;
	Vector toPixelCoordinates(const Vector& block) const;
	Rect getBlockBounds(int x, int y) const;
	Rect getBlockBounds(const Vector& pos) const;
	Vector traceLine(const Vector& start_cell, const Vector& direction);
	bool isBlockInBounds(int x, int y) const;
	bool isBlockInBounds(const Vector& block) const;
 
	std::vector<Vector> getBridgeBlocks();
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	void clearBlock(int x, int y);
	void kickBlock(int x, int y, CMario* mario);
	void enableNightViewFilter();
	void loadFromString(const std::string& data, std::function<AbstractBlock*(char)> fabric);
	bool isCollidableBlock(int x, int y) const;
	bool isCollidableBlock(const Vector& block) const;
	Vector collsionResponse(const Rect& body_rect, const Vector& body_speed, float delta_time, ECollisionTag& collision_tag);
};

class COneBrick : public CGameObject
{
 public:
	 COneBrick(const Vector& pos, const Vector& speed_vector);
	 void draw(sf::RenderWindow* render_window) override;
	 void update(int delta_time) override;
 private:
	 Vector m_speed;
	 CSpriteSheet m_sprite_sheet;
	 int m_timer = 0;
};

class CTwistedCoin : public CGameObject
{
public:
	CTwistedCoin(const Vector& pos);
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
private:
	Vector m_speed;
	Animator m_animator;
	int m_timer = 0;
};


class CMushroom : public CGameObject
{
public:
	CMushroom(const Vector& pos);
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
	void addImpulse(const Vector& speed);
protected:
	void start() override;
private:
	CBlocks* m_blocks;
	CMario* m_mario;
	Vector m_speed;
	enum State { wait, borning, normal } m_state = State::wait;
	sf::Sprite m_sprite;
	float m_timer = 0;
	float run_speed = 0.1;
	const float gravity_force = 0.0015;
};



class CFireFlower : public CGameObject
{
public:
	CFireFlower(const Vector& pos);
	void draw(sf::RenderWindow* render_window) override;
	void update(int delta_time) override;
private:
	enum State { wait, borning, normal } m_state = State::wait;
	sf::Sprite m_sprite;
	float m_timer = 0;
};