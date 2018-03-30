#pragma once
#include "Charasters.h"
#include "Blocks.h"
#include "Items.h"
#include "SuperMarioGame.h"
#include <String>



CMarioBullet::CMarioBullet(const Vector& pos, const Vector& direction)
{
	m_direction = direction;
	setPosition(pos);
	m_speed = direction*bullet_speed;

	m_animator.create("fly", *MarioGame().textureManager().get("texture"), { { 0,160,16,16 }, { 16,160,16,16 }, { 16,160,-16,16 }, { 16,176,16,-16 } }, 0.01);
	m_animator.create("splash", *MarioGame().textureManager().get("texture"),  Vector(31,160), Vector(16,16), 3,1, 0.02, AnimType::forward_backward_cycle);
}
void CMarioBullet::draw(sf::RenderWindow* render_window) 
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}


void CMarioBullet::setState(State state)
{
	m_state = state;

	if (m_state == State::fly)
		m_animator.play("fly");
	else if (m_state == State::splash)
		m_animator.play("splash");

	m_timer = 0;
}

void CMarioBullet::start()
{
	m_blocks = getParent()->findObjectByName<CBlocks>("Blocks");
}
void CMarioBullet::update(int delta_time) 
{
	m_timer += delta_time;
	if (m_timer > 3000)
		getParent()->removeObject(this);


	if (m_state == State::fly)
	{
		m_speed += Vector::down*gravity_force * delta_time;
		move(m_speed*delta_time);
		Vector block = m_blocks->toBlockCoordinates(getPosition());
		if (m_blocks->isBlockInBounds(block) && m_blocks->isCollidableBlock(block.x, block.y))
		{
			Rect block_bounds = m_blocks->getBlockBounds(block.x, block.y);
			float dw = std::min(abs(block_bounds.left() - getPosition().x), abs(block_bounds.right() - getPosition().x));
			float dh = std::min(abs(block_bounds.top() - getPosition().y), abs(block_bounds.bottom() - getPosition().y));
			if (dw < dh && (dw > 4 && dh > 4))     // kick side (and angles - r=4)
				setState(State::splash);
			else                                   // kick top or bottom
				m_speed.y = -0.35f;
		}

		auto enemies = getParent()->findObjectsByType<CEnemy>();

		for (auto enemy : enemies)
			if (enemy->getBounds().isContain(getPosition()))
			{
				enemy->fired(NULL);
				setState(State::splash);
				break;
			}
	}
	else
	{
		if (m_timer > 250)
			getParent()->removeObject(this);
	}
	m_animator.update(delta_time);
}


void CMario::reciveDamage()
{
	if (!m_spawn_timer)
		if (m_rank != MarioRank::small)
			demote();
		else
			m_spawn_timer = 3000;
}
bool CMario::isSmall() const
{
	return getRank() == MarioRank::small;
}



CMario::CMario()
{
	m_blocks = NULL;
	const sf::Texture& texture = *MarioGame().textureManager().get("texture");
	addObject(m_animator = new Animator());
	m_animator->create("idle_big", texture, { 0,32,32,64 });
	m_animator->create("walk_big", texture, { 32, 32 }, { 32, 64 }, 3, 1, 0.01);
	m_animator->create("swim_big", texture, { 321, 32 }, { 40, 64 }, 3, 1, 0.01);
	m_animator->create("jump_big", texture, { 160,32,32,64 });
	m_animator->create("slip_big", texture, { 128,32,32,64 });
	m_animator->create("seat_big", texture, { 192,52,32,44 });
	m_animator->create("climb_big", texture, Vector(256, 32), Vector(32, 64), 2, 1, 0.01);


	m_animator->create("growing", texture, { { 0,32,32,64 }, { 0,96,32,32 } }, 0.01 );
	m_animator->setSpriteOffset("growing", 1, { 0, 32 });
	m_animator->create("demoting", texture, { { 401,32,40,64 },{ 288,96,32,32 } }, 0.01);
	m_animator->setSpriteOffset("demoting", 1, { 4, 32 });


	m_animator->create("firing", texture, { { 0,32,32,64 },{ 224,32,32,64 } }, 0.01);

	m_animator->create("idle_small", texture, { 0,96,32,32 });
	m_animator->create("walk_small", texture, { 32, 96 }, { 32, 32 }, 3, 1, 0.01);
	m_animator->create("swim_small", texture,   { 288, 96 }, { 32, 32 }, 3, 1, 0.01);
	m_animator->create("jump_small", texture, { 160,96,32,32 });
	m_animator->create("slip_small", texture, { 128,96,32,32 });
	m_animator->create("seat_small", texture, { 192,96,32,32 });
	m_animator->create("climb_small", texture, Vector(224, 96), Vector(32, 32), 2, 1, 0.01);
 
	m_animator->create("shoot", texture, { 224,32,32,64 });

   
	setRank(MarioRank::small);
	setState(MarioState::normal);

	m_fire_pallete.create({ sf::Color(64,64,128), sf::Color(64,96,192), sf::Color(160,32,0), sf::Color(192,0,64), sf::Color(224,32,64)},
	                      { sf::Color(64,128,0),sf::Color(96,160,0),sf::Color(192,192,128),sf::Color(224,224,128), sf::Color(255,251,240) });

	m_black_pallete.create({ sf::Color(64,64,128), sf::Color(64,96,192), sf::Color(160,32,0), sf::Color(192,0,64), sf::Color(224,32,64) },
	                       { sf::Color(64,128,0),sf::Color(96,160,0),sf::Color(192,192,128),sf::Color(224,224,128), sf::Color(255,251,240) });

}

 


void CMario::promote()
{
	if (m_rank != MarioRank::fire)
	 setState(MarioState::promoting);
}

void CMario::playAnimation(const std::string& name)
{
	m_animator->play(name);
}


void CMario::demote()
{
	MarioGame().playSound("pipe");
	setState(MarioState::demoting);
}


void CMario::setRank(MarioRank rank)
{
  

	if (rank == MarioRank::small)
	{
		setSize(small_mario_size);
		 if (m_rank != MarioRank::small)
		   move(small_mario_size - big_mario_size);
		m_seated = false;
		m_animator->setPallete(NULL);
	 
	}
	else if (rank == MarioRank::big)
	{
		setSize(big_mario_size);
		move(small_mario_size - big_mario_size);
		m_animator->setPallete(NULL);
	}
	else if (rank == MarioRank::fire)
	{
		setSize(big_mario_size);
		m_animator->setPallete(&m_fire_pallete);
	}

	m_rank = rank;

	//std::string  rank_string = (m_rank == MarioRank::small) ? "_small" : "_big";
	//playAnimation("idle" + rank_string);
}

MarioRank CMario::getRank() const
{
	return m_rank;
}




void CMario::draw(sf::RenderWindow* render_window)
{
	m_animator->setPosition(getPosition());
	m_animator->draw(render_window);
}




void CMario::kickBlocksProcessing()
{
	if (m_collision_tag & ECollisionTag::cell)
	{
		Vector block_left = m_blocks->toBlockCoordinates(getBounds().leftTop() + Vector::up * 16, false);
		Vector block_right = m_blocks->toBlockCoordinates(getBounds().rightTop() + Vector::up * 16, false);

		bool block_left_exist = m_blocks->isBlockInBounds(block_left) && m_blocks->isCollidableBlock(block_left);
		bool block_right_exist = m_blocks->isBlockInBounds(block_right) && m_blocks->isCollidableBlock(block_right);


		if (block_left_exist || block_right_exist)
		{
			if (block_left_exist ^ block_right_exist) //only one block touched
			{
				if (block_left_exist)
				 m_blocks->kickBlock(block_left.x, block_left.y, this);
				else
				 m_blocks->kickBlock(block_right.x, block_right.y, this);
			}
			else                //Kick closer block between both touched blocks
			{
				Rect block_left_rect = m_blocks->getBlockBounds(block_left);
				Rect block_right_rect = m_blocks->getBlockBounds(block_right);

				float mario_X = getBounds().center().x;

				if (abs(block_left_rect.center().x - mario_X) < abs(block_right_rect.center().x - mario_X))
					m_blocks->kickBlock(block_left.x, block_left.y, this);
				else
					m_blocks->kickBlock(block_right.x, block_right.y, this);
			}

		}
	}
}
void CMario::kickEnemiesProcessing(float delta_time)
{
	
	if (m_spawn_timer == 0)
	{
		auto enemies = getParent()->findObjectsByType<CEnemy>();
		for (auto enemy : enemies)
		{
			if (enemy->isAlive() && getBounds().isIntersect(enemy->getBounds()))
			{
				if (!m_in_water)
				{
					Rect intersection = getBounds().getIntersection(enemy->getBounds());
					if (intersection.height() / 8 < intersection.width() &&  // touch from above
						intersection.bottom() == getBounds().bottom())
					{
						enemy->kickFromTop(this);
						move(-m_speed*delta_time);
						m_speed.y = -0.3f;
					}
					else                                                     /// touch from side
						enemy->touchSide(this);
				}
				else enemy->touchSide(this);
			}
		}
	}

}



Vector CMario::getInputDirection()
{
	return m_input_direcition;
}

void CMario::inputProcessing(float delta_time)
{
	auto& input_manager = MarioGame().inputManager();
	
	m_input_direcition = Vector::zero;
	if (input_manager.isKeyPressed(sf::Keyboard::Left)) m_input_direcition += Vector::left;
	if (input_manager.isKeyPressed(sf::Keyboard::Right)) m_input_direcition += Vector::right;
	if (input_manager.isKeyPressed(sf::Keyboard::Up)) m_input_direcition += Vector::up;
	if (input_manager.isKeyPressed(sf::Keyboard::Down)) m_input_direcition += Vector::down;

	if (!m_climb)
	{
		// walk-run
		m_x_max_speed = (input_manager.isKeyPressed(sf::Keyboard::LShift) && !m_jumped && !m_speed.y) ? run_speed : walk_speed;

		// left-right
		if (!m_seated || m_jumped)
		{
			float k = 1;
			if (m_jumped) k = 0.7;
			if ((m_input_direcition.x<0)   && !(m_collision_tag & ECollisionTag::left))
				addImpulse(Vector::left*(acceleration*delta_time*k));
			if ((m_input_direcition.x>0)  && !(m_collision_tag & ECollisionTag::right))
				addImpulse(Vector::right*(acceleration*delta_time*k));

			if (m_grounded && m_input_direcition.x)
				m_direction.x = m_input_direcition.x;
		}

	
		// seat-down / stand-up
		if (m_grounded && m_rank != MarioRank::small)
		{
			if (m_input_direcition.y > 0)
			{
				if (!m_seated)
				{
					m_seated = true;
					setSize(seated_mario_size);
					move(big_mario_size - seated_mario_size);
				}
			}
			else
			{
				if (m_seated)
				{
					m_seated = false;
					setSize(big_mario_size);
					move(seated_mario_size - big_mario_size);
				}
			}
		}

		if (m_grounded)
			m_jumped = false;

		//fire 
		if (input_manager.isKeyJustPressed(sf::Keyboard::LShift) && !m_seated && m_fire_timer > fire_rate && m_rank == MarioRank::fire)
		{
			fire();
			m_fire_timer = 0;
		}
		m_fire_timer += delta_time;
	}
	else  // climbing
	{

		if (!m_input_direcition.x)
		{
			if (m_input_direcition == Vector::up)
			{
				if (getPosition().y >= m_used_ladder->getPosition().y)
				  move(climb_speed*Vector::up*delta_time);
			}
			else if (m_input_direcition == Vector::down)
			{
				if (getBounds().bottom() <= m_used_ladder->getBounds().bottom())
				  move(climb_speed*Vector::down*delta_time);
			}

		} else {
			addImpulse({ run_speed*m_input_direcition.x,-0.3f });
			m_climb = false;
		}
 
		if (m_collision_tag & ECollisionTag::floor)
			m_climb = false;
	}

	m_jumping_timer += delta_time;


	// jump
	if (!m_in_water)
	{

		if (input_manager.isKeyPressed(sf::Keyboard::Space))
		{
			if (m_grounded)
			{
				m_jumping_timer = 0;
				MarioGame().playSound("jump_super");
				m_grounded = m_climb = false;
				m_jumped = true;
				m_jumping_timer = 0.65*jump_time;
				addImpulse(1.5*Vector::up*jump_force);
			}
			else
			{
				if (m_jumping_timer < jump_time)
					addImpulse(Vector::up*jump_force* ((jump_time - m_jumping_timer) / (jump_time)));

			}
		}
		else
		{
			m_jumping_timer = jump_time;
		}
	}
	else
	{
		if (input_manager.isKeyPressed(sf::Keyboard::Space))
		{
			if (m_grounded)
				m_jumped = true;
			if (m_jumping_timer > 100)
			{
				addImpulse(0.5*Vector::up*jump_force);
				m_jumping_timer = 0;
			}
		}
	}


	if (input_manager.isKeyPressed(sf::Keyboard::Space) && m_climb && !m_input_direcition.y)
	{
		addImpulse(0.4*Vector::up*jump_force);
		m_grounded = m_climb = false;
		m_jumped = true;
	}


}

void CMario::animationProcessing(float delta_time)
{
	 std::string  rank_string = (m_rank == MarioRank::small) ? "_small" : "_big";


	if (m_climb)
	{
		playAnimation("climb" + rank_string);
		m_animator->setSpeed("climb" + rank_string, m_input_direcition.y?0.005f:0.f);
	}
	else if (m_in_water)
	{
		if (!m_speed.y)
		{
			if (m_speed.x)
			{
				playAnimation("walk" + rank_string);
				m_animator->flipX(m_speed.x < 0);

				m_animator->setSpeed("walk" + rank_string, 0.02f *(abs(m_speed.x) / walk_speed));

				if ((m_direction == Vector::left && m_speed.x > 0) ||
					(m_direction == Vector::right && m_speed.x < 0))
					playAnimation("slip" + rank_string);
			}
			else
				playAnimation("idle" + rank_string);
		}
		else
		{
			playAnimation("swim" + rank_string);
			m_animator->flipX(m_speed.x < 0);
			m_animator->setSpeed("swim" + rank_string, 0.02f *(abs(m_speed.x) / walk_speed));
		}
	}
	else // normal
	{
		if (getRank() == MarioRank::fire && m_fire_timer < 200 && !m_seated)
			playAnimation("shoot");
		else if (m_seated)
			playAnimation("seat" + rank_string);
		else if (m_jumped)
			playAnimation("jump" + rank_string);
		else if (m_grounded)
		{
			if (m_speed.x)
			{
				playAnimation("walk" + rank_string);
				m_animator->flipX(m_speed.x < 0);

				m_animator->setSpeed("walk" + rank_string, 0.02f *(abs(m_speed.x) / walk_speed));

				if ((m_direction == Vector::left && m_speed.x > 0) ||
					(m_direction == Vector::right && m_speed.x < 0))
					playAnimation("slip" + rank_string);
			}
			else
				playAnimation("idle" + rank_string);
		}
		else 
			m_animator->setSpeed("walk" + rank_string, 0.f);
	}

	//Damaged flashing
	if (m_spawn_timer)
	{
		m_spawn_timer -= delta_time;

		if (int(m_spawn_timer / 80) % 2)
			m_animator->setColor(sf::Color::White);
		else
			m_animator->setColor(sf::Color::Transparent);

		if (m_spawn_timer < 0)
		{
			m_animator->setColor(sf::Color::White);
			m_spawn_timer = 0;
		}
	}
}
 
void CMario::physicProcessing(float delta_time)
{
	if (m_climb)
		return;

	if (!m_in_water)
	{
		if (m_grounded)
			addImpulse(-Vector::right*math::sign(m_speed.x)*friction_force*delta_time); //Ground friction force
		else
			addImpulse(Vector::down*gravity_force * delta_time);                // Gravity force

																				   // Speed limits				
		if (abs(m_speed.x) < 0.01) m_speed.x = 0;
		m_speed.x = math::clamp(m_speed.x, -m_x_max_speed, m_x_max_speed);
		move(m_speed*delta_time);
	}
	else
	{

		if (m_grounded)
			addImpulse(-Vector::right*math::sign(m_speed.x)*friction_force*delta_time); //Ground friction force
		else
			addImpulse(0.25*Vector::down*gravity_force * delta_time);                // Gravity force

																				// Speed limits				
		if (abs(m_speed.x) < 0.01) m_speed.x = 0;
		m_speed.x = math::clamp(m_speed.x, -m_x_max_speed/2, m_x_max_speed/2);
		move(m_speed*delta_time);
		if (getPosition().y < 0)
			m_speed.y = 0.1f;

	}

 

	
	 

}



 
Vector CMario::getSpeed() const
{
	return m_speed;
}

void CMario::update(int delta_time)
{
	if (!m_blocks)
		m_blocks = getParent()->findObjectByType<CBlocks>();
	assert(m_blocks);



	m_current_state->update(delta_time);

	CGameObject::update(delta_time);
}

void CMario::setState(MarioState state)
{
	if (m_current_state)
	{
		m_current_state->onLeave();
		delete m_current_state;
	}
	if (state == MarioState::normal)
		m_current_state = new CNormalMarioState();
	else if (state == MarioState::promoting)
		m_current_state = new CPromotingMarioState();
	else if (state == MarioState::demoting)
		m_current_state = new CDemotingMarioState();

	m_current_state->setMario(this);
	m_current_state->onEnter();

}

void CMario::setState(IMarioState* state)
{
	if (m_current_state)
	{
		m_current_state->onLeave();
		delete m_current_state;
	}
	m_current_state = state;
	m_current_state->setMario(this);
	m_current_state->onEnter();
}



void CMario::fire()
{
	Vector pos = getBounds().center() + m_direction * 25 + Vector::up*8;
	getParent()->addObject(new CMarioBullet(pos,m_direction));
	MarioGame().playSound("fireball");
}

void CMario::addImpulse(Vector _speed)
{
	m_speed += _speed;
}

bool CMario::isGrounded() const
{
	return m_grounded;
}

void CMario::setUnclimb()
{
	m_climb = false;
	m_jumped = false;
	m_grounded = false;
	m_speed = Vector::down*0.2f;
	playAnimation("jump_" + std::string(isSmall() ? "small" : "big"));
}

void CMario::collisionProcessing(float delta_time)
{
	m_collision_tag = ECollisionTag::none;
	setPosition(m_blocks->collsionResponse(this->getBounds(), m_speed, delta_time, m_collision_tag));
	
 
	auto items = getParent()->findObjectsByType<CItem>();
	CMario* mario = this;

	for (auto item : items)
		if (item->getBounds().isIntersect(mario->getBounds()))
			item->collsionResponse(this, m_collision_tag, delta_time);

 

	if (!m_climb && m_input_direcition.y)
	{
		auto ladders = getParent()->findObjectsByType<CLadder>();
		for (auto ladder : ladders)
			if (ladder->getBounds().isIntersect(mario->getBounds()))
			{
				m_used_ladder = ladder;
				m_climb = true;
				m_jumped = true;

				int x_ladder = ladder->getBounds().center().x;
				if (x_ladder > getBounds().center().x)
				{
					setPosition(x_ladder - getBounds().width(), getPosition().y);
					m_animator->flipX(false);
				}
				else
				{
					setPosition(x_ladder, getPosition().y);
					m_animator->flipX(true);
				}
				m_speed = Vector::zero;
				break;
			}
	}

	m_grounded = m_collision_tag & ECollisionTag::floor;

	  if ((m_collision_tag & ECollisionTag::right) && m_input_direcition != Vector::left) m_speed.x = 0;
      if ((m_collision_tag & ECollisionTag::left) && m_input_direcition != Vector::right) m_speed.x = 0;


	 if (m_collision_tag & ECollisionTag::floor) m_speed.y = 0.f;
	 if (m_collision_tag & ECollisionTag::cell) m_speed.y = 0.1f;
	 	  

 
}
 
void CMario::setSpeed(const Vector& _speed)
{
	m_speed = _speed;
}
void CMario::setGrounded()
{
	m_grounded = true;
}

void CMario::onActivated() 
{
	if (getProperty("InWater").isValid() && getProperty("InWater").asBool() == true)
		m_in_water = true;
}
void CMario::start()
{
	if (getProperty("SpawnDirection").isValid())
	{
		Vector direction = toVector(getProperty("SpawnDirection").asString());
		if (direction != Vector::zero)
		{
			move(-direction*0.03f * 2000);
			setState(new CTransitionMarioState(direction*0.03f, 2000));
		}
	}

	if (getProperty("StartScript").isValid())
	{
		auto script = getProperty("StartScript").asString();
		if (script == "GoToPortal")
			setState(new CGoToPortalState());
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------
void IMarioState::setMario(CMario* mario)
{
	m_mario = mario;
}
CMario* IMarioState::mario()
{
	return m_mario;
}

void IMarioState::playAnimation(const std::string& anim_name)
{
	mario()->playAnimation(anim_name);
}
void IMarioState::setMarioRank(const MarioRank& rank)
{
	mario()->setRank(rank);
}
void IMarioState::setMarioNormalPallete()
{
	mario()->m_animator->setPallete(NULL);
}
void IMarioState::setMarioFirePallete()
{
	mario()->m_animator->setPallete(&(mario()->m_fire_pallete));
}

void  IMarioState::enableScene(bool value)
{
	auto m = mario();
	mario()->getParent()->foreachObject([m, value](CGameObject* obj) {if (obj != m) if (value) obj->enable(); else  obj->disable(); });
}
void  IMarioState::setMarioState(const MarioState& state)
{
	mario()->setState(state);
}

void IMarioState::setMarioSpawnProtection()
{
	mario()->m_spawn_timer = 3000;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
	void CPromotingMarioState::onEnter()
	{
		m_promoting_timer = 1500;
		enableScene(false);
		if (mario()->getRank() ==  MarioRank::small)
		{
			setMarioRank(MarioRank::big);
			playAnimation("growing");
		}
		else  if (mario()->getRank() == MarioRank::big)
			setMarioRank(MarioRank::fire);
		
		mario()->show();
	}
	void CPromotingMarioState::onLeave()
	{
		enableScene(true);

		if (mario()->getRank() == MarioRank::fire)
			setMarioFirePallete();
	}
	void CPromotingMarioState::update(int delta_time)
	{

		auto _mario = mario();

		if (_mario->getRank() == MarioRank::fire)
			_mario->m_animator->setPallete((int(m_promoting_timer / 100) % 2) ? &(_mario->m_fire_pallete) : NULL);

		m_promoting_timer -= delta_time;
		if (m_promoting_timer < 0)
			_mario->setState(MarioState::normal);
	}
 
//---------------------------------------------------------------------------------------------------------------------------------------------
 
	void CDemotingMarioState::onEnter()
	{
		enableScene(false);
		m_promoting_timer = 1500;

		setMarioNormalPallete();
		setMarioRank(MarioRank::small);
		playAnimation("demoting");
		mario()->show();
	}
	void CDemotingMarioState::onLeave()
	{
		enableScene(true);
		setMarioSpawnProtection();
	}
	void CDemotingMarioState::update(int delta_time)
	{
		playAnimation("demoting");
		m_promoting_timer -= delta_time;
		if (m_promoting_timer < 0)
			 mario()->setState(MarioState::normal);
	}
 
 

//----------------------------------------------------------------------------------------------------------------------------------------------
	void CNormalMarioState::onEnter()
	{
	//	mario()->playAnimation("idle_small");
	}
	void CNormalMarioState::onLeave()
	{

	}
	void CNormalMarioState::update(int delta_time)
	{
		auto _mario = mario();
		_mario->inputProcessing(delta_time);
		_mario->physicProcessing(delta_time);
		_mario->collisionProcessing(delta_time);
		_mario->kickBlocksProcessing();
		_mario->kickEnemiesProcessing(delta_time);
		_mario->animationProcessing(delta_time);
	}

//----------------------------------------------------------------------------------------------------------------------------------------------

	CTransitionMarioState::CTransitionMarioState(const Vector&  speed, int time)
	{
		m_speed = speed;
		m_timer = time;
	}
	void CTransitionMarioState::onEnter() 
	{
		 mario()->setSpeed(m_speed.normalized()*mario()->walk_speed/2.f);
		 mario()->moveUnderTo(mario()->getParent()->findObjectByType<CBlocks>());

		auto piranas_plant = mario()->getParent()->findObjectsByType<CPiranhaPlant>();
		for (auto& pirana : piranas_plant)
			pirana->hideInTube();

		if (m_speed.y>0 || m_speed.x>0)
			MarioGame().playSound("pipe");
	}
	void CTransitionMarioState::onLeave() 
	{
		 mario()->moveToFront();
		mario()->setSpeed(Vector::zero);
	}
	void CTransitionMarioState::update(int delta_time)
	{
		if (m_timer > 0)
		{
			m_timer -= delta_time;
			mario()->move(delta_time*m_speed);
		    mario()->animationProcessing(delta_time);
		}
		else
			mario()->setState(MarioState::normal);
		
 
	}
//----------------------------------------------------------------------------------------------------------------------------------------------

	CGoToCastleMarioState::CGoToCastleMarioState()
	{

	}
	void CGoToCastleMarioState::onEnter()
	{
		auto m_block = mario()->getParent()->findObjectByType<CBlocks>();
		m_cell_y = m_block->height() - m_block->blockSize().y * 4;
		if (!mario()->isSmall())
			m_cell_y -= 32;
		 mario()->m_input_direcition = Vector::zero;
	}
	void CGoToCastleMarioState::onLeave() 
	{ 

	}
	void CGoToCastleMarioState::update(int delta_time) 
	{
		if (m_state == State::start)
		{
			mario()->m_animator->flipX(false);
			auto anim_name = std::string("climb_") + (mario()->isSmall() ? "small" : "big");
			mario()->playAnimation(anim_name);
			mario()->m_animator->setSpeed(anim_name, 0.005f);

			m_state = State::go_down;
			MarioGame().playSound("flagpole");
		}
		else if (m_state == State::go_down)
		{
			if (mario()->getPosition().y < m_cell_y)
				mario()->move(Vector::down*delta_time*0.25f);
			else
			{
				m_state = State::overturn;
				mario()->m_animator->flipX(true);
				mario()->move(Vector::right * 32.f);
				m_delay_timer = 500;
			}
		}
		else if (m_state == State::overturn)
		{
			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
			{
				m_state = State::walk;
				auto anim_name = std::string("walk_") + (mario()->isSmall() ? "small" : "big");
				mario()->playAnimation(anim_name);
				mario()->m_animator->setSpeed(anim_name, 0.003f);
				mario()->m_animator->flipX(false);
			}
		}
		else if (m_state == State::walk)
		{
			mario()->addImpulse(Vector::right*10.f);
			mario()->physicProcessing(delta_time);
			mario()->collisionProcessing(delta_time);

			auto portals = mario()->getParent()->findObjectsByType<CLevelPortal>();
			for (auto& portal : portals)
				if (portal->getBounds().isContainByX(mario()->getPosition()))
				{
					m_state = State::wait;
					mario()->hide();
					m_next_level = portal->getProperty("Level").asString();
					if (portal->getProperty("SubLevel").isValid())
						m_next_sub_level = portal->getProperty("SubLevel").asString();
 
 
					auto flag = mario()->getParent()->findObjectByType<CCastleFlag>();
					assert(flag); // there is no castle flag in level
					flag->liftUp();
					MarioGame().setEndLevelStatus();
				}
		}
		else if (m_state == State::wait)
		{
			if (MarioGame().getGameTime() == 0)
			{
				m_delay_timer = 2000;
				m_state = State::next_level;
			}
		}
		else if (m_state == State::next_level)
		{
			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
			{
				MarioGame().loadLevel(m_next_level);
				if (!m_next_sub_level.empty())
					MarioGame().loadSubLevel(m_next_sub_level);
				return;
			}
		}
		mario()->m_animator->update(delta_time);
	}

//----------------------------------------------------------------------------------------------------------------------------------------------

	CGoToPortalState::CGoToPortalState()
	{

	}
	void CGoToPortalState::onEnter() 
	{
		auto anim_name = std::string("walk_") + (mario()->isSmall() ? "small" : "big");
		mario()->playAnimation(anim_name);
		mario()->m_animator->setSpeed(anim_name, 0.003f);
	}
	void CGoToPortalState::onLeave() 
	{

	}
	void CGoToPortalState::update(int delta_time) 
	{
		mario()->m_input_direcition = Vector::right;
		mario()->move(Vector::right*0.08f*delta_time);
		mario()->physicProcessing(delta_time);
		mario()->collisionProcessing(delta_time);
 
		mario()->m_animator->update(delta_time);
	}

//----------------------------------------------------------------------------------------------------------------------------------------------

	CGoToPrincessState::CGoToPrincessState()
	{

	}
	void CGoToPrincessState::onEnter()
	{
		m_princess = mario()->getParent()->findObjectByType<CPrincess>();
		mario()->m_input_direcition = Vector::right;
		mario()->m_direction = Vector::right;
		auto anim_name = std::string("walk_") + (mario()->isSmall() ? "small" : "big");
		mario()->playAnimation(anim_name);
		mario()->m_animator->setSpeed(anim_name, 0.003f);
		mario()->m_animator->flipX(false);
	}
	void CGoToPrincessState::onLeave()
	{

	}
	void CGoToPrincessState::update(int delta_time)
	{
 
		if (m_princess->getPosition().x - mario()->getBounds().right() > 16)
		{
			mario()->m_speed.x = mario()->walk_speed*0.6f;
			mario()->physicProcessing(delta_time);
			mario()->collisionProcessing(delta_time);
			if (mario()->isGrounded())
				mario()->m_animator->update(delta_time);
		}
		else
		{
			mario()->playAnimation("idle_" + std::string(mario()->isSmall() ? "small" : "big"));
			mario()->m_speed.x = 0;
		}
	}

//----------------------------------------------------------------------------------------------------------------------------------------------
void CEnemy::fired(CMario* mario)
{
	kickFromBottom(mario);
}
void CEnemy::checkNextTileUnderFoots()
{
	auto blocks = getParent()->findObjectByName<CBlocks>("Blocks");
	if (m_speed.y == 0)
	{
		Vector next_under_foot = blocks->toBlockCoordinates(getBounds().center() + Vector::down * 30 + math::sign(m_speed.x)*Vector::right * 20);
		Vector prev_back = blocks->toBlockCoordinates(getBounds().center() - math::sign(m_speed.x)*Vector::right * 50);
		if (!blocks->isCollidableBlock(next_under_foot.x, next_under_foot.y) && !blocks->isCollidableBlock(prev_back.x, prev_back.y))
			m_speed.x = -m_speed.x;
	}
}
void CEnemy::checkCollideOtherCharasters()
{
	auto enemies = getParent()->findObjectsByType<CEnemy>();
	for (auto enemy : enemies)
		if (enemy != this && enemy->isAlive() && enemy->getBounds().isIntersect(getBounds()))
		{
		//	if (abs(enemy->m_speed.x) == abs(m_speed.x))
			//	this->kickFromBottom(NULL);
			enemy->kickFromBottom(NULL);
		}
}
void CEnemy::updateCollision(float delta_time)
{
	m_collision_tag = ECollisionTag::none;
	setPosition(m_blocks->collsionResponse(getBounds(), m_speed, delta_time, m_collision_tag));
}
void CEnemy::updatePhysics(float delta_time, float gravity)
{
	m_speed += Vector::down*gravity * delta_time;
	move(delta_time*m_speed);
}
void CEnemy::checkFallUndergound()
{
	if (getPosition().y > 1000)
		getParent()->removeObject(this);
}
CMario* CEnemy::mario()
{
	return m_mario;
}
void CEnemy::addScoreToPlayer(int score)
{
	MarioGame().addScore(score, getBounds().center());
}
void CEnemy::start()
{
	m_mario = getParent()->findObjectByName<CMario>("Mario");
	m_blocks = getParent()->findObjectByType<CBlocks>();
	assert(m_mario && m_blocks);
}
void CEnemy::update(int delta_time) 
{
	checkFallUndergound();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
CGoomba::CGoomba()
{
	setSize({ 32,32 });
	m_animator.create("walk", *MarioGame().textureManager().get("Enemies"), { { 0,0,32,32 },{ 32,0,32,32 } }, 0.005);
	m_animator.create("cramped", *MarioGame().textureManager().get("Enemies"),  { 64,0,32,32 } );
	m_animator.create("fall", *MarioGame().textureManager().get("Enemies"),  { 0,32,32,-32 } );
	m_animator.setSpriteOffset("cramped", 0, { 0,8 });
}
void CGoomba::draw(sf::RenderWindow* render_window)
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}
void CGoomba::update(int delta_time)
{
	CEnemy::update(delta_time);

	switch (m_state)
	{
	    case (State::Deactivated):
	    {
		    auto scene = getParent()->castTo<CMarioGameScene>();
		    Rect camera_rect = scene->cameraRect();
			if (abs(getPosition().x - camera_rect.center().x) < camera_rect.width() / 2)
				setState(State::Normal);
	        break;
	    }
	    case (State::Normal):
		{
			updatePhysics(delta_time, gravity_force);
			updateCollision(delta_time);
			if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
				m_speed.y = 0;
			if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
				m_speed.x = -m_speed.x;
			break;
		}

		case (State::Died):
	    {
			updatePhysics(delta_time, gravity_force);
			break;
		}
		case(State::Cramped):
	    {
		    m_timer += delta_time;
		    if (m_timer > 3000)
			 getParent()->removeObject(this);
		    break;
	    }
	}


	switch (m_state)
	{
	 case(State::Normal): 
	 { 
	   m_animator.play("walk"); 
	   m_animator.update(delta_time); 
	   break; 
	 };
	 case(State::Cramped): { m_animator.play("cramped"); break; };
	 case(State::Died): { m_animator.play("fall"); break; };
	}
}
void CGoomba::kickFromTop(CMario* mario)
{
	setState(State::Cramped);
}
void CGoomba::kickFromBottom(CMario* mario)
{
	if (m_state != Died)
	{
		setState(State::Died);
		m_speed += 0.4*Vector::up;
	}
}
void CGoomba::touchSide(CMario* mario)
{
	if (m_state == State::Normal)
		mario->reciveDamage();
}
bool CGoomba::isAlive() const
{
	return m_state == Normal;
}
void CGoomba::setState(State state)
{
	m_state = state;
	switch (m_state)
	{
	case State::Normal:
	{
		m_speed.x = m_run_speed;
		break;
	}
	case State::Cramped:
	{
		m_speed.x = 0;
		addScoreToPlayer(100);
		MarioGame().playSound("stomp");
		break;
	}
	case State::Died:
	{
		m_speed.x = 0;
		addScoreToPlayer(100);
		MarioGame().playSound("kick");
		break;
	}
	}
	m_timer = 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
CKoopa::CKoopa()
{
	setSize({ 32, 48 });
	const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	m_animator.create("walk", texture, { { 0,32,32,48 },{ 32,32,32,48 } }, 0.005);
	m_animator.create("flying", texture, { 224,32 }, { 32, 48 }, 2, 1, 0.005);
	m_animator.create("climb", texture, { { 64,48,32,32 },{ 192,48,32,32 } }, 0.005);
	m_animator.create("hidden", texture, { 64,48,32,32 });
	m_animator.create("bullet", texture, { 64, 48 }, { 32, 32 }, 4, 1, 0.01);
	m_animator.create("fall", texture,  { 0,80, 32, -48 } );
}


void CKoopa::setFlying()
{
	m_is_jumping = true;
}

void  CKoopa::kickFromTop(CMario* mario)
{
	switch (m_state)
	{
	case CKoopa::Jumping: 
	{ 
		m_speed.y = 0;  
		setState(State::Normal); 
		addScoreToPlayer(100);
		MarioGame().playSound("stomp");
		break; 
	}
	case CKoopa::Normal: 
	{
		if (m_speed.y == 0)
		{
			setState(State::Hidden);
			addScoreToPlayer(100);
			MarioGame().playSound("stomp");
		}
		 break;
	}
	case CKoopa::Hidden: 
	case CKoopa::Climb:  
	{
		setState(State::Bullet); 
		m_speed.x = (mario->getBounds().center().x > this->getBounds().center().x) ? -abs(m_run_speed) * 6 : abs(m_run_speed) * 6;
		addScoreToPlayer(400);
		MarioGame().playSound("kick");
		break; 
	}
	case CKoopa::Bullet:
	{
		setState(Hidden);
		break;
	}
	}
}

void  CKoopa::kickFromBottom(CMario* mario)
{
	if (m_state != State::Died)
	{
		setState(State::Died);

		addScoreToPlayer(500);
		MarioGame().playSound("kick");
	}
}
bool  CKoopa::isAlive() const
{
	return m_state != Died;
}

void CKoopa::touchSide(CMario* mario)
{
	if (m_state == State::Hidden || m_state == State::Climb)
	{
		setState(State::Bullet);
		m_speed.x = (mario->getBounds().center().x > this->getBounds().center().x) ? -abs(m_run_speed) * 6 : abs(m_run_speed) * 6;
		move(14 * Vector::right*math::sign(m_speed.x));
		addScoreToPlayer(400);
		MarioGame().playSound("kick");
	}
	else if (m_state == State::Normal || m_state == State::Bullet)
		mario->reciveDamage();
}

void CKoopa::draw(sf::RenderWindow* render_window)
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}

void CKoopa::setState(State state)
{
	if (m_state != state)
	{
		m_state = state;
		switch (m_state)
		{
		case (State::Jumping):
		{
			m_animator.play("flying");
			m_speed.x = m_run_speed;
			break;
		}
		case (State::Normal):
		{
			m_speed.x = m_run_speed;
			m_animator.play("walk");
            
			move(getBounds().size() - full_size);
			setSize(full_size);

			break;
		}
		case (State::Hidden):
		{
			move(getBounds().size() - hidden_size);
			setSize(hidden_size);
			m_speed.x = 0;
			m_animator.play("hidden");
			break;
		}
		case (State::Bullet):
		{
			move(getBounds().size() - hidden_size);
			setSize(hidden_size);
			m_animator.play("bullet");
			break;
		}
		case (State::Climb):
		{
			move(getBounds().size() - hidden_size);
			setSize(hidden_size);
			m_animator.play("climb");
			break;
		}
		case (State::Died):
		{
			m_speed.y = -0.4f;
			m_animator.play("fall");
			break;
		}
		}
		m_timer = 0;
	}
}

void CKoopa::update(int delta_time)
{
	CEnemy::update(delta_time);


	    if (m_state == State::Deactivated)
	    {
			Rect camera_rect = getParent()->castTo<CMarioGameScene>()->cameraRect();
			if (abs(getPosition().x - camera_rect.center().x) < camera_rect.width() / 2)
			    setState(m_is_jumping?State::Jumping:State::Normal);
        }
		else
		{
			updatePhysics(delta_time, gravity_force);
			if (m_state != State::Died)
			{
				updateCollision(delta_time);

				if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
					m_speed.x = -m_speed.x;

				if (m_collision_tag & ECollisionTag::floor)
					if (m_state == State::Jumping)
						m_speed.y = -0.4f;
					else
						m_speed.y = 0.f;
				if (m_collision_tag & ECollisionTag::cell)
					m_speed.y = 0;
			}

			switch (m_state)
			{
			 case(State::Normal):
			 {
				checkNextTileUnderFoots();
				break;
			 } 
			case(State::Jumping):
			{
				m_speed += Vector::up*delta_time*gravity_force / 2; //anti-grav force
				break;
			}
			case(State::Hidden):
			{
				m_timer += delta_time;
				if (m_timer > 3000)
					setState(State::Climb);
				break;
			}
			case(State::Climb):
			{
				m_timer += delta_time;
				if (m_timer > 5000)
					setState(State::Normal);
				break;
			}
			case(State::Bullet):
			{
				checkCollideOtherCharasters();
				break;
			}
			}
		}
 

		  if (m_state == State::Bullet && ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right)))
			MarioGame().playSound("bump");
		 

	  	  m_animator.flipX(m_speed.x > 0);
		  m_animator.update(delta_time);
}
void CKoopa::onActivated()  
{
	if (getProperty("Flying").asInt() == 1)
		setFlying();
}

//--------------------------------------------------------------------------------------------------------------
ÑBuzzyBeetle::ÑBuzzyBeetle()
{
	setSize({ 32, 32 });

	const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");

	m_animator.create("walk", texture, { { 96,0,32,32 },{ 128,0,32,32 } }, 0.005);
	m_animator.create("hidden", texture, { 160,0,32,32 });
	m_animator.create("bullet", texture, { 160, 0 }, { 32, 32 }, 4, 1, 0.01);
	m_animator.create("fall", texture, { 96,32, 32, -32 });
	setState(State::Normal);
}
void  ÑBuzzyBeetle::kickFromTop(CMario* mario)
{
 
    if (m_state == Normal)
	{
		if (m_speed.y == 0)
		{
			setState(State::Hidden);
			addScoreToPlayer(100);
			MarioGame().playSound("stomp");
		}
	}
	else if (m_state == Hidden)
	{
		setState(State::Bullet);
		m_speed.x = (mario->getBounds().center().x > this->getBounds().center().x) ? -abs(m_run_speed) * 6 : abs(m_run_speed) * 6;
		addScoreToPlayer(400);
		MarioGame().playSound("kick");
	}
	else if (m_state == Bullet)
		setState(Hidden);
	
	m_timer = 0;
}
void  ÑBuzzyBeetle::kickFromBottom(CMario* mario)
{
	if (m_state != Died)
	{
		setState(State::Died);
		m_speed.y = -0.4f;
		m_timer = 0;
		addScoreToPlayer(800);
		MarioGame().playSound("kick");
	}
}
bool  ÑBuzzyBeetle::isAlive() const
{
	return m_state != Died;
}
void ÑBuzzyBeetle::touchSide(CMario* mario)
{
	if (m_state == State::Hidden)
	{
		setState(State::Bullet);
		m_speed.x = (mario->getBounds().center().x > this->getBounds().center().x) ? -abs(m_run_speed) * 6 : abs(m_run_speed) * 6;
		move(14 * Vector::right*math::sign(m_speed.x));
		MarioGame().playSound("kick");
	}
	else if (m_state == State::Normal || m_state == State::Bullet)
		mario->reciveDamage();
}
void ÑBuzzyBeetle::fired(CMario* mario)
{
	//nothing happen
}
void ÑBuzzyBeetle::draw(sf::RenderWindow* render_window)
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}
void ÑBuzzyBeetle::setState(State state)
{
	m_state = state;
	switch (m_state)
	{
	case (State::Normal):
	{
		m_speed.x = m_run_speed;
		m_animator.play("walk");
		break;
	}
	case (State::Hidden):
	{
		m_speed.x = 0;
		m_animator.play("hidden");
		break;
	}
	case (State::Bullet):
	{
		m_speed.x = 6 * m_run_speed;
		m_animator.play("bullet");
		break;
	}
	case (State::Died):
	{
		m_speed.y = -0.4f;
		m_animator.play("fall");
		break;
	}
	}
	m_timer = 0;
}
void ÑBuzzyBeetle::update(int delta_time)
{
	CEnemy::update(delta_time);

	switch (m_state)
	{
	case(State::Deactivated):
	{
		auto scene = getParent()->castTo<CMarioGameScene>();
		Rect camera_rect = scene->cameraRect();
		if (abs(getPosition().x - camera_rect.center().x) < camera_rect.width() / 2)
			setState(Normal);
		break;
	}
	case(State::Normal):
	{
		updatePhysics(delta_time, gravity_force);
		updateCollision(delta_time);

		if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
		{
			m_speed.x = -m_speed.x;
		}
		if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
			m_speed.y = 0;

		checkNextTileUnderFoots();
		break;
	}
 
	case(State::Hidden):
	{
		m_timer += delta_time;
		if (m_timer > 5000)
		{
			setState(State::Normal);
			m_timer = 0;
		}
		updatePhysics(delta_time, gravity_force);
		updateCollision(delta_time);

		if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
			m_speed.x = -m_speed.x;
		if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
			m_speed.y = 0;

		break;
	}
    case(State::Bullet):
	{
		updatePhysics(delta_time, gravity_force);
		updateCollision(delta_time);

		if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
		{
			m_speed.x = -m_speed.x;
			MarioGame().playSound("bump");
		}
		if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
			m_speed.y = 0;

		checkCollideOtherCharasters();
		break;
	}
	case(State::Died):
	{
		updatePhysics(delta_time, gravity_force);
		break;
	}
	}

 
	
		m_animator.flipX(m_speed.x > 0);
		m_animator.update(delta_time);
  
}
//---------------------------------------------------------------------------------------------------------------
	CHammer::CHammer(CMario* target)
	{
		m_animator.create("fly", *MarioGame().textureManager().get("Enemies"), Vector(96,112), Vector(32,32),4,1, 0.01);
		m_animator.create("in_hand", *MarioGame().textureManager().get("Enemies"), {96,112,32,32});
		m_animator.play("in_hand");
		m_target = target;
		setSize({ 32,32 });
	}
	void CHammer::update(int delta_time)
	{
	
		if (m_state == in_hand)
		{
 

		}
		else if (m_state == fly)
		{
			m_animator.update(delta_time);
			m_speed += Vector::down*gravity_force * delta_time;
			move(m_speed*delta_time);

			if (getPosition().y > 1000)
				getParent()->removeObject(this);

			if (m_target->getBounds().isContain(getBounds().center()))
				m_target->reciveDamage();
		}
	
	};
	void  CHammer::throwAway(const Vector& speed)
	{
		m_speed = speed;
		m_animator.play("fly");
		m_state = fly;
	}
	void  CHammer::draw(sf::RenderWindow* render_window)
	{
		m_animator.setPosition(getPosition());
		m_animator.draw(render_window);
	}

//---------------------------------------------------------------------------------------------------------------
CHammerBro::CHammerBro()
{
	setSize({ 32, 44 });
	const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	m_animator.create("died", texture,  { 96,160+48,32,-48 } );
	m_animator.create("walk", texture, Vector(96,160), Vector(32,48),2,1, 0.005);
	m_animator.create("walk_with_hammer", texture,Vector(160, 160), Vector(32, 48), 2, 1, 0.005);
	m_animator.play("walk_with_hammer");
	m_speed.x = m_run_speed;
}
void CHammerBro::draw(sf::RenderWindow* render_window) 
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}
void CHammerBro::update(int delta_time) 
{ 
	CEnemy::update(delta_time);

	if (m_speed.y == 0)  
	  m_animator.update(delta_time);

	switch (m_state)
	{
	  case(State::Normal):
	  {
		//Physic processing
		updatePhysics(delta_time, gravity_force / 2);
		if (m_collision_on)
		{
			updateCollision(delta_time);
			if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
				m_speed.x = -m_speed.x;
			if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
				m_speed.y = 0;
		}

		//Walk processing
		if (abs(getPosition().x - m_center_x) > walk_amlitude)
			m_speed.x = -m_speed.x;
		m_direction = (m_mario->getPosition().x < getPosition().x) ? Vector::left : Vector::right;
		m_animator.flipX(m_direction == Vector::right);

 
		// Jump processing
		m_jump_timer += delta_time;
		if (m_jump_timer > jump_rate)
		{
			if (m_jump_direction == Vector::up && !isCanJumpUp() && isCanJumpDown())
					m_jump_direction = Vector::down;
			if (m_jump_direction == Vector::down && !isCanJumpDown())
					m_jump_direction = Vector::up;

			if (m_jump_direction == Vector::up)  //jump up
				 m_speed += Vector::up*0.5;          
			else                                   //jump-off down
			{
				m_speed += Vector::up*0.25;
				m_drop_off_height = getPosition().y + getBounds().height() + 32.f;
			}
	
			m_collision_on = false;               //turn off collision check for moving through walls
			m_jump_timer = 0;
		}
		
		if (!m_collision_on)                      //turn on collision check for take ground
		{
			if (m_jump_direction == Vector::up)
			{
				if (m_speed.y > 0)
					m_collision_on = true;
			}
			else
			{
				if (getPosition().y > m_drop_off_height)
					m_collision_on = true;
			}
		}

		//Fire processing
		m_fire_timer += delta_time;
		if (m_fire_timer < fire_rate/2)  //just walk
		{
			
		} 
		else if (m_fire_timer < fire_rate) //get hammer in hand
		{
			  if (!m_hummer)
			  {
				m_hummer = new CHammer(m_mario);
				getParent()->addObject(m_hummer);
				m_animator.play("walk_with_hammer");
			  }
			  const Vector hand_off_set = { -3 * m_direction.x, -22.f };
			  m_hummer->setPosition(getPosition() + hand_off_set);
		}
		else                      //throw hummer
		{
			m_hummer->throwAway({ m_direction.x*0.15f, -0.55f });
			m_hummer = NULL;
			m_fire_timer = 0;
			m_animator.play("walk");
		};
		break;
	  }
	  case(State::Died):
	  {
		updatePhysics(delta_time, gravity_force);
	 	break;
	  }
	}
}
void CHammerBro::setState(State state)
{
		m_state = state;
		if (m_state == State::Died)
		{
			m_animator.play("died");
			m_speed.y = 0;
			if (m_hummer)
			{
				getParent()->removeObject(m_hummer);
				m_hummer = NULL;
			}
			addScoreToPlayer(1000);
			MarioGame().playSound("kick");
		}
}
void CHammerBro::onActivated()
{
	m_center_x = getPosition().x;
	m_fire_timer = rand() % 500;
	m_jump_timer = rand() % int(jump_rate/2);
}
void CHammerBro::kickFromTop(CMario* mario)  
{
	setState(State::Died);
}
void CHammerBro::kickFromBottom(CMario* mario)
{
	setState(State::Died);
}
void CHammerBro::touchSide(CMario* mario)
{
	 if (m_state == State::Normal)
		mario->reciveDamage();
}
bool CHammerBro::isAlive() const
{
	return m_state != State::Died;
}
bool CHammerBro::isCanJumpUp() const
{
	Vector begin_point = m_blocks->toBlockCoordinates(getBounds().center(),true);
	Vector end_point = m_blocks->traceLine(begin_point, Vector::up);

	if (end_point.y == 0) return false;

	if (abs(end_point.y - begin_point.y) == 2  && !m_blocks->isCollidableBlock(end_point + Vector::up*2))
		return true;
 
	return false;
}
bool CHammerBro::isCanJumpDown() const
{
	Vector begin_point = m_blocks->toBlockCoordinates(getBounds().center()) + Vector::down*2;
	if (!m_blocks->isCollidableBlock(begin_point))
		return true;
	return false;
}
//----------------------------------------------------------------------------------------------------------------

CSpinny::CSpinny(const Vector& position, const Vector& speed, const Vector& walk_direction) 
{
	setPosition(position);
	m_speed = speed;
	m_walk_direction = walk_direction;
	setSize({ 31, 32 });

	const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
 
	
	m_animator.create("walk", texture, Vector(64,80), Vector(32,32),2,1, 0.005);
	m_animator.create("egg", texture, Vector(128,80), Vector(32, 32), 2, 1, 0.005);
	m_animator.create("died", texture, {64,80+32,32,-32}); 

	setState(State::Egg);
}
void CSpinny::draw(sf::RenderWindow* render_window)
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}
void CSpinny::setState(State state)
{
	m_state = state;

	if (m_state == State::Normal)
	{
		m_animator.play("walk");
		m_speed.x = m_run_speed;
		if (m_walk_direction == Vector::right)
			m_speed = -m_speed;
	}
	else if (m_state == State::Died)
	{
		m_animator.play("died");
		addScoreToPlayer(400);
		MarioGame().playSound("kick");
	}
	else if (m_state == State::Egg)
	{
		m_animator.play("egg");
	}
}
void CSpinny::update(int delta_time)
{
	CEnemy::update(delta_time);
	m_animator.update(delta_time);

	switch(m_state)
	{
	 case (State::Egg):
	 {
		 updatePhysics(delta_time, gravity_force / 2);
		 updateCollision(delta_time);
		 if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
			 m_speed.x = -m_speed.x;
		 if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
		 {
			 m_speed.y = 0;
			 setState(State::Normal);
		 }
		 break;
	 }
	 case (State::Normal):
	 {
		 updatePhysics(delta_time, gravity_force / 2);
		 updateCollision(delta_time);
		 if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
			 m_speed.x = -m_speed.x;
		 if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
			 m_speed.y = 0;
		 m_animator.flipX(m_speed.x > 0);
		 break;
	 }
	 case (State::Died):
	 {
		 updatePhysics(delta_time, gravity_force / 2);
		 break;
	 }

	}

}
void CSpinny::kickFromTop(CMario* mario)
{
	mario->reciveDamage();
}
void CSpinny::kickFromBottom(CMario* mario)
{
	setState(State::Died);
}
void CSpinny::touchSide(CMario* mario)
{
	mario->reciveDamage();
}
bool CSpinny::isAlive() const
{
	return m_state != State::Died;
}
//---------------------------------------------------------------------------------------------------------------
 
 CLakity::CLakity()
 {
	setName("Lakity");
	setSize({ 32, 48 });
	const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	m_animator.create("fire", texture, {0,128,32,48});
	m_animator.create("fly", texture, {32,128,32,48});
	m_animator.create("died", texture, {32,128+48,32,-48 });
	setState(State::Normal);
 }
 void CLakity::draw(sf::RenderWindow* render_window)
 {
	 m_animator.setPosition(getPosition());
	 m_animator.draw(render_window);
 }
 void CLakity::update(int delta_time)
 {
	 CEnemy::update(delta_time);

	 switch (m_state)
	 {
	   case (State::Normal):
	  {
		   //move porcessing
		   float diff_x = m_mario->getPosition().x - getPosition().x;
		   m_speed.x += math::sign(diff_x)*sqrt(abs(diff_x)) / 4000;
		   m_speed.x = math::clamp(m_speed.x, -0.35f, 0.35f);

		   move(m_speed*delta_time);
		 		   
		   //fire processing
		   m_fire_timer += delta_time;	   
		   if (m_fire_timer > fire_rate)
		   {
			   Vector fly_direction = (m_mario->getPosition().x > getPosition().x) ? Vector::right : Vector::left;
			   CSpinny* spinny = new CSpinny(getPosition() - Vector(0, 10), Vector(-0.05f*fly_direction.x, -0.2f), fly_direction);
			   getParent()->addObject(spinny);
			   m_fire_timer = 0;
			   m_animator.play("fly");
		   }
		   if (m_fire_timer > fire_rate*0.8f)
			   m_animator.play("fire");

		   break;
	   }
	   case (State::RunAway):
	   {
		   move(m_speed*delta_time);
		   m_died_timer += delta_time;
		   if (m_died_timer > 2000)
			   getParent()->removeObject(this);
		   break;
	   }
	   case (State::Died):
	   {
		 updatePhysics(delta_time, gravity_force / 2);
		 break;
	   }
	 }
 }
 void CLakity::kickFromTop(CMario* mario)
 {
	 setState(State::Died);
 }
 void CLakity::kickFromBottom(CMario* mario)
 {
	 setState(State::Died);
 }
 void CLakity::touchSide(CMario* mario)
 {
	 mario->reciveDamage();
 }
 bool CLakity::isAlive() const
 {
	 return m_state != State::Died;
 }
 void CLakity::setState(State state)
 {
	 m_state = state;
	 if (m_state == State::Died)
	 {
		 m_animator.play("died");
		 m_speed = Vector::zero;
		 addScoreToPlayer(1200);
		 MarioGame().playSound("kick");
	 }
	 else if (m_state == State::Normal)
		 m_animator.play("fly");
 } 
 void CLakity::runAway(const Vector& run_direction)
 {
	 m_speed.x = run_direction.x * 0.2f;
	 setState(State::RunAway);
 }
 void CLakity::start()
 {
	 CEnemy::start();
 }
//----------------------------------------------------------------------------------------------------------------
 CCheepCheep::CCheepCheep(const Vector& initial_pos, const Vector& initial_speed)
 {
	 setSize({ 32, 32 });
	 m_speed = initial_speed;
	 setPosition(initial_pos);
	 const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	 m_animator.create("fly", texture, Vector(0, 176), Vector(32, 32), 2, 1, 0.005);
	 m_animator.create("died", texture, { 0,176 + 32,32,-32 });

	 setState(State::Normal);
 }


 CCheepCheep::CCheepCheep()
 {
	 setSize({ 32, 32 });
	 m_speed = Vector::left*0.05f;
	 const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	 m_animator.create("fly", texture, Vector(0, 176), Vector(32, 32), 2, 1, 0.005);
	 m_animator.create("died", texture, { 0,176 + 32,32,-32 });
	 setState(State::Underwater);
 }


 void CCheepCheep::draw(sf::RenderWindow* render_window)
 {
	 m_animator.setPosition(getPosition());
	 m_animator.draw(render_window);
 }
 void CCheepCheep::update(int delta_time)
 {
	 CEnemy::update(delta_time);
	 switch (m_state)
	 {
	 case (State::Normal):
	 {
		 updatePhysics(delta_time, gravity_force*0.4);
		 m_animator.update(delta_time);
		 break;
	 }
	 case (State::Underwater):
	 {
		 if (abs(mario()->getPosition().x - getPosition().x) < MarioGame().screenSize().x / 2)
		 {
			 move(m_speed*delta_time);
			 m_animator.update(delta_time);
		 }
		 break;
	 }
	 case (State::Died):
	 {
		 updatePhysics(delta_time, gravity_force);
		 break;
	 }
	 }
    
   }
 void CCheepCheep::kickFromTop(CMario* mario)
 {
	 setState(State::Died);
  }
 void CCheepCheep::kickFromBottom(CMario* mario)
 {
	 setState(State::Died);
 }
 void CCheepCheep::touchSide(CMario* mario)
 {
	 mario->reciveDamage();
 }
 bool CCheepCheep::isAlive() const
 {
	 return m_state != State::Died;
 }
 void CCheepCheep::setState(State state)
 {
	 m_state = state;
	 if (m_state == State::Normal)
	 {
		 m_animator.play("fly");
		 m_animator.flipX(m_speed.x > 0);
	 }
	 else  if (m_state == State::Died)
	 {
		 m_speed = Vector::zero;
		 m_animator.play("died");
		 addScoreToPlayer(200);
		 MarioGame().playSound("kick");
	 }

 }
 //---------------------------------------------------------------------------------------------------------------

 CBlooper::CBlooper()
 {
	 setSize({ 32, 48 });
	 const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	 m_animator.create("zig", texture, { 224,161,32,48 });
	 m_animator.create("zag", texture, { 256,161,32,48 });
	 m_animator.create("died", texture, { 224,161+48,32,-48 });
 }

 void CBlooper::enterState(State state)
 {
	 m_state = state;
	 switch (state)
	 {
	 case (CBlooper::Zig):
	 {
		 m_speed = -Vector(1, 1)*0.15f;
		 m_animator.play("zig");
		 m_delay_time = 400;
		 break;
	 }
	 case (CBlooper::Zag):
	 {
		 m_speed = Vector::down*0.05f;
		 m_animator.play("zag");
		 m_delay_time = 1200;
		 break;
	 }
	 case (CBlooper::Died):
	 {
		 m_animator.play("died");
		 m_speed = Vector::down*0.2f;
		 break;
	 }
	 }
 }

 void CBlooper::update(int delta_time)
 {

	 CEnemy::update(delta_time);
	 if (abs(mario()->getPosition().x - getPosition().x) < MarioGame().screenSize().x / 2)
	 {
		 switch (m_state)
		 {
		 case (CBlooper::Zig):
		 {
			 m_delay_time -= delta_time;
			 if (m_delay_time < 0)
				 enterState(State::Zag);
			 break;
		 }
		 case (CBlooper::Zag):
		 {
			 m_delay_time -= delta_time;
			 if (m_delay_time < 0)
				 enterState(State::Zig);
			 break;
		 }
		 }
		 move(delta_time*m_speed);
		 m_animator.update(delta_time);
	 }
 }

  
 void CBlooper::draw(sf::RenderWindow* render_window)
 {
	 m_animator.setPosition(getPosition());
	 m_animator.draw(render_window);
 }
 
 void CBlooper::kickFromTop(CMario* mario)
 {
	 mario->reciveDamage();
 }
 void CBlooper::kickFromBottom(CMario* mario)
 {
	 enterState(State::Died);
 }
 void CBlooper::touchSide(CMario* mario)
 {
	 mario->reciveDamage();
 }
 bool CBlooper::isAlive() const
 {
	 return m_state != State::Died;
 }
//----------------------------------------------------------------------------------------------------------------
 CBulletBill::CBulletBill(const Vector& initial_pos, const Vector& initial_speed)
 {
	 setSize({ 32, 32 });
	 m_speed = initial_speed;
	 setPosition(initial_pos);
	 const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	 m_animator.create("fly", texture, Vector(64, 112), Vector(32, 32), 1, 3, 0.005);
	 m_animator.create("died", texture, { 64,176 + 32,32,-32 });

	 setState(State::Normal);
	 }

	 void CBulletBill::draw(sf::RenderWindow* render_window)
	 {
		 m_animator.setPosition(getPosition());
		 m_animator.draw(render_window);
	  }
	 void CBulletBill::update(int delta_time)
	  {
		 CEnemy::update(delta_time);

		  switch (m_state)
		  {
		  case (State::Normal):
		  {
			  updatePhysics(delta_time, 0);
			  m_animator.update(delta_time);
			  int camera_width = getParent()->castTo<CMarioGameScene>()->cameraRect().width();
			  if (abs(getPosition().x - mario()->getPosition().x) > camera_width)
				  setState(State::Died);

			  break;
		  }
		  case (State::Died):
		  {
			  updatePhysics(delta_time, gravity_force);
			  break;
		  }
		  }
	  }
	  void CBulletBill::kickFromTop(CMario* mario)
	  {
		  setState(State::Died);
	  }
	  void CBulletBill::kickFromBottom(CMario* mario)
	  {
		  setState(State::Died);
	  }
	  void CBulletBill::touchSide(CMario* mario)
	  {
		  mario->reciveDamage();
	  }
	  bool CBulletBill::isAlive() const
	  {
		  return m_state != State::Died;
	 }
	  void CBulletBill::setState(State state)
	  {
			  m_state = state;
			  if (m_state == State::Normal)
			  {
				  m_animator.play("fly");
				  m_animator.flipX(m_speed.x > 0);
			  }
			  else  if (m_state == State::Died)
			  {
				  m_speed = Vector::zero;
				  m_animator.play("died");
				  addScoreToPlayer(1000);
				  MarioGame().playSound("kick");
			  }
	  }
	  void CBulletBill::start()
	  {
		  CEnemy::start();
	  }

 //------------------------------------------------------------------------------------------------------------------

CPiranhaPlant::CPiranhaPlant()
{
	m_sprite.setTexture(*MarioGame().textureManager().get("Enemies"));
	m_sprite.setTextureRect({ 0,0,0,0 });

	setSize(size);
}
void CPiranhaPlant::kickFromTop(CMario* mario)
{
	mario->reciveDamage();
}

void CPiranhaPlant::kickFromBottom(CMario* mario)
{
	getParent()->removeObject(this);
	addScoreToPlayer(800);
	MarioGame().playSound("kick");
}
bool CPiranhaPlant::isAlive() const
{
	return true;
}

void CPiranhaPlant::touchSide(CMario* mario) 
{
	mario->reciveDamage();
}
void CPiranhaPlant::fired(CMario* mario)
{
	getParent()->removeObject(this);
	addScoreToPlayer(800);
	MarioGame().playSound("kick");
}
void CPiranhaPlant::draw(sf::RenderWindow* render_window)
{
	m_sprite.setPosition(getPosition());

	render_window->draw(m_sprite);
}
void CPiranhaPlant::update(int delta_time)
{
	CEnemy::update(delta_time);

	int sprite_index = 0;
	float height = 0;
	m_timer += delta_time;
	if (m_timer < period_time) //None
	{
		//too close to mario ===> no appear
		static const int distance = 20;
		if ((mario()->getBounds().center() - getBounds().center()).length() < 100)
			hideInTube();
		
		height = 0;
	}
	else  if (m_timer < 1.25 * period_time) //Appearing
	{
		sprite_index = 1; 
		height = ((m_timer - period_time) / (0.25f*period_time)) * size.y;
	}
	else  if (m_timer < 3 * period_time)   //in full size 
	{
		sprite_index = int(m_timer/100) % 2;
		height = size.y;
	}
	else  if (m_timer < 3.25 * period_time)  //Hiding
	{
		sprite_index = 0;
		height =  (1 - ((m_timer - 3*period_time)/ (0.25f*period_time))) * size.y;
	}
	else m_timer = 0;


	setSize({ size.x, height });
	move({ 0.f,m_buttom - getPosition().y - height });
	m_sprite.setTextureRect({ 32 * sprite_index,80,(int)size.x, (int)height });
}

void CPiranhaPlant::hideInTube()
{
	m_timer = 0;
	setSize({ size.x, 0.f });
	move({ 0.f,m_buttom - getPosition().y });
}

void CPiranhaPlant::onActivated()
{
	m_buttom = getPosition().y + size.y;
}

//-----------------------------------------------------------------------------------------------------------------
CPodoboo::CPodoboo()
{
	m_shape.setRadius(16);
	m_shape.setFillColor(sf::Color::Red);
	setSize({ 32,32 });
	const sf::Texture& texture = *MarioGame().textureManager().get("Enemies");
	m_animator.create("up", texture, Vector(192, 80), Vector(32, 32), 3, 1, 0.005);
	m_animator.create("down", texture, Vector(192, 112), Vector(32, -32), 3, 1, 0.005);
}
void CPodoboo::kickFromTop(CMario* mario) 
{
	mario->reciveDamage();
}
void CPodoboo::kickFromBottom(CMario* mario)
{
	//nothing
}
bool CPodoboo::isAlive() const
{
	return true;
}
void CPodoboo::touchSide(CMario* mario)
{
	mario->reciveDamage();
}
void CPodoboo::fired(CMario* mario)
{
	//nothing
}
void CPodoboo::draw(sf::RenderWindow* render_window)
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}
void CPodoboo::update(int delta_time)
{
	CEnemy::update(delta_time);
	m_timer += delta_time;
	
	if (getPosition().y > m_center.y)
		m_speed += Vector::up*m_acceleration*delta_time;
	else
		m_speed += Vector::down*m_acceleration*delta_time;
		
	if (m_timer > period_time) //synchronization
	{
		setPosition(m_center);
		m_speed = Vector::up*m_acceleration * period_time*0.25;
		m_timer = 0;
	}
 
	m_animator.update(delta_time);
	move(m_speed*delta_time);
	m_animator.play((m_speed.y < 0)?"down":"up");

}

void CPodoboo::onActivated()
{
	m_center = getPosition();// -Vector(16, 16);
	m_acceleration =   amplitude / (period_time*period_time*0.25f*0.25f); 
	m_speed = Vector::up*m_acceleration * period_time*0.25f;
}

//----------------------------------------------------------------------------------------------------------------

CLakitySpawner::CLakitySpawner()
{

}
void CLakitySpawner::update(int delta_time)
{
	CGameObject::update(delta_time);

	m_lakity_checker_timer += delta_time;
	if (m_lakity_checker_timer > check_interval)
	{
		m_lakity = getParent()->findObjectByName<CLakity>("Lakity");
		Rect camera_rect = getParent()->castTo<CMarioGameScene>()->cameraRect();


		if (!m_lakity)  	// Add Lakity to the scene processing
		{
			if (getBounds().isContainByX(m_mario->getPosition()))
			{
				m_lakity = new CLakity();
				m_lakity->setPosition(camera_rect.left() - 32, 64);
				getParent()->addObject(m_lakity);
			}
		}
		m_lakity_checker_timer = 0;
	}

		if (m_lakity)              // Throw Lakity from the scene processing
	    {
		 Rect camera_rect = getParent()->castTo<CMarioGameScene>()->cameraRect();
		 if (m_lakity->getPosition().x > getBounds().right() + camera_rect.size().x/2)
		 {
			m_lakity->runAway(Vector::left);
			m_lakity = NULL;
		 }
		 else if (m_lakity->getPosition().x < getBounds().left() - camera_rect.size().x/2)
		 {
			m_lakity->runAway(Vector::right);
			m_lakity = NULL;
		 }
	    }

	}
 
void CLakitySpawner::onActivated() 
{
	setSize({ getProperty("width").asFloat(), getProperty("height").asFloat() });
}

void CLakitySpawner::start()
{
	m_mario = getParent()->findObjectByName<CMario>("Mario");
}

//-----------------------------------------------------------------------------------------------------------------

CCheepCheepSpawner::CCheepCheepSpawner()
{

}
void CCheepCheepSpawner::update(int delta_time)
{
	m_spawn_timer += delta_time;
	if (m_spawn_timer > spawn_interval && getBounds().isContainByX(m_mario->getPosition()))
	{
		int camera_width = getParent()->castTo<CMarioGameScene>()->cameraRect().width();
		int x = rand() % int(camera_width) - camera_width / 2 + m_mario->getPosition().x;
		Vector direction = (m_mario->getPosition().x < x) ? Vector::left : Vector::right;
		getParent()->addObject(new CCheepCheep({ x, m_map_height + 32 }, { direction.x*spawn_speed.x, spawn_speed.y }));
		m_spawn_timer = 0;
	}
}
void CCheepCheepSpawner::onActivated()
{
	setSize({ getProperty("width").asFloat(), getProperty("height").asFloat() });
}

void CCheepCheepSpawner::start()
{
	m_map_height = getParent()->findObjectByName<CBlocks>("Blocks")->height();
	m_mario = getParent()->findObjectByName<CMario>("Mario");
}
//-------------------------------------------------------------------------------------------------------------

  CBulletBillSpawner::CBulletBillSpawner()
  {
	  m_spawn_timer = rand() % spawn_interval;
  }
  void CBulletBillSpawner::update(int delta_time)
  {	 
	  int camera_width = getParent()->castTo<CMarioGameScene>()->cameraRect().width();
	  m_spawn_timer += delta_time;
	  if (m_spawn_timer > spawn_interval && abs(m_mario->getPosition().x-getPosition().x) < camera_width)
	  {
		  Vector direction = (m_mario->getPosition().x < getPosition().x) ? Vector::left : Vector::right;
		  getParent()->addObject(new CBulletBill(getPosition(), direction*bullet_speed));
		  m_spawn_timer = 0;
	  }
  }
  void CBulletBillSpawner::onActivated()
	{

	}

  void CBulletBillSpawner::start()
  {
	  m_mario = getParent()->findObjectByName<CMario>("Mario");
  }

  //----------------------------------------------------------------------------------------------------------------

  CFireball::CFireball(const Vector& Position, const Vector& SpeedVector)
  {
	  auto texture = MarioGame().textureManager().get("Bowser");
	  m_animator.create("fire", *texture, { 0,364 }, { 32,36 }, 4, 1, 0.01f,AnimType::forward_backward_cycle);
	  m_speed = SpeedVector;
	  setPosition(Position);
	  m_animator.flipX(SpeedVector.x < 0);
	  m_animator.get("fire")->setOrigin({ 16,18 });
  }
  void CFireball::draw(sf::RenderWindow* render_window) 
  {
	  m_animator.setPosition(getPosition());
	  m_animator.draw(render_window);
  }
  void CFireball::update(int delta_time)  
  {
	  m_animator.update(delta_time);
	  m_life_timer -= delta_time;
	  if (m_life_timer < 0)
		  getParent()->removeObject(this);
	  move(delta_time*m_speed);

 
	  if (m_mario->getBounds().isContain(getPosition()))
		  m_mario->reciveDamage();
  }

  void  CFireball::start()
  {
	  m_mario = getParent()->findObjectByName<CMario>("Mario");
  }
 
 
	//----------------------------------------------------------------------------------------------------------------

  CBowser::CBowser()
  {
	  setSize({ 84,80 });
	  auto texture = MarioGame().textureManager().get("Bowser");

	  m_animator.create("walk", *texture, { 0,0 }, { 84,80 }, 6, 1, anim_speed,AnimType::forward_cycle);
	  m_animator.create("died", *texture, { 0,80,84,-80 });

	  m_animator.create("turn", *texture, { 381,122 }, { 74,85 }, 2, 1, anim_speed/2, AnimType::forward_stop);
	  m_animator.create("middle_fire", *texture, { 0,167 }, { 91,100 }, 4, 1, anim_speed, AnimType::forward_stop);
	  m_animator.create("land_fire", *texture, { 0,267 }, { 92,97 }, 6, 1, anim_speed, AnimType::forward_stop);
	  m_animator.create("pre_jump", *texture, { 0,80 }, { 91,79 }, 2, 1, anim_speed, AnimType::forward_stop);
	  m_animator.create("up_jump", *texture, { 182,80,84,87 });
	  m_animator.create("down_jump", *texture, { 266,80, 84,87});
	  m_animator.get("middle_fire")->setOrigin(Vector::down * 16);
	  m_animator.get("land_fire")->setOrigin(Vector::down * 16);
	  m_animator.get("turn")->setOrigin(Vector::down *5);
	  m_speed.x = m_run_speed;
  }
  void CBowser::draw(sf::RenderWindow* render_window)
  {
	  m_animator.setPosition(getPosition());
	  m_animator.draw(render_window);
  }
 

  void  CBowser::noBridge()
  {
	  enterState(State::no_bridge);
  }

  void CBowser::enterState(State state)
  {
	  if (state == m_state)
		  return;

	  switch (state)
	  {
	    case(State::walk):
	    {
			m_speed.x = m_old_speed.x;
			m_animator.play("walk");
			m_delay_timer = 2000;
			break;
	    }
		case(State::turn):
		{
			m_animator.play("turn");
			m_delay_timer = 400;
			break;
		}
		case(State::pre_jump):
		{
			m_old_speed.x = m_speed.x;
			m_old_speed.y = -1;
			m_speed.x = 0;
			m_animator.play("pre_jump");
			m_delay_timer = 300;
			break;
		}
		case(State::jump):
		{
			m_speed = 0.4f*Vector::up;
			break;
		}
		case(State::middle_fire):
		{
			m_old_speed.x = m_speed.x;
			m_speed.x = 0;
			m_animator.play("middle_fire");
			m_delay_timer = 500;
			break;
		}
		case(State::land_fire):
		{
			m_old_speed.x = m_speed.x;
			m_speed.x = 0;
			m_animator.play("land_fire");
			m_delay_timer = 700;
			break;
		}
		case(State::no_bridge):
		{
			m_animator.play("walk");
			m_animator.get("walk")->setSpeed(anim_speed * 2.5);
			m_delay_timer = 1000;
			break;
		}
		case(State::fall):
		{
			m_animator.get("walk")->setSpeed(0);
			MarioGame().playSound("bowser_falls");
			m_speed = Vector::zero;
			break;
		}
		case(State::died):
		{
			m_animator.play("died");
			m_speed = Vector::zero;
			break;
		}
	  }
	  m_state = state;
  }

  void CBowser::update(int delta_time)
  {
	  CEnemy::update(delta_time);

	  m_animator.update(delta_time);
	  if (m_state == State::walk || m_state == State::jump)
	  {
		  updatePhysics(delta_time, gravity_force / 2);
		  updateCollision(delta_time);
		  if ((m_collision_tag & ECollisionTag::left) || (m_collision_tag & ECollisionTag::right))
			  m_speed.x = -m_speed.x;
		  if ((m_collision_tag & ECollisionTag::floor) || (m_collision_tag & ECollisionTag::cell))
			  m_speed.y = 0;
	  }
     

	  switch (m_state)
	  {
	    case(State::walk):
	    {
			if (abs(getPosition().x - m_center_x) > c_walk_amlitude)
			{
				m_speed.x = -m_speed.x;
				move({ (c_walk_amlitude - abs(getPosition().x - m_center_x))*math::sign(-m_speed.x),0.f });
			}
			auto old_direction = m_direction;
			m_direction = (m_mario->getPosition().x < getPosition().x) ? Vector::left : Vector::right;

			if (old_direction != m_direction)
			{
				enterState(State::turn);
				break;
			}

			m_animator.flipX(m_direction == Vector::right);

			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
			{
				int d =  rand() % 3;
				if (d == 0) enterState(State::pre_jump);
				if (d == 1) enterState(State::middle_fire);
				if (d == 2) enterState(State::land_fire);
			}
				break;
	    }
		case(State::turn):
		{
			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
				enterState(State::walk);
			break;
		}
		case(State::jump):
		{
			if (m_speed.y > 0)
				m_animator.play("down_jump");
			else if (m_speed.y < 0)
				m_animator.play("up_jump");
			if (m_speed.y > 0 && m_old_speed.y < 0) //jump peak
			{
			
			  getParent()->addObject(new CFireball(getBounds().center() + m_direction * 50 + Vector::down * 20, m_direction*0.13f));
			  MarioGame().playSound("bowser_fire");
			}  
				m_old_speed.y = m_speed.y;
			if (m_collision_tag & ECollisionTag::floor)
				enterState(State::walk);
			break;
		}
		case(State::pre_jump):
		{  
			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
				enterState(State::jump);
			break;
		}
		case(State::land_fire):
		{
			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
			{
				getParent()->addObject(new CFireball(getBounds().center() + m_direction * 50 + Vector::down*10, m_direction*0.13f));
				MarioGame().playSound("bowser_fire");
				enterState(State::walk);
			}
			break;
		}
		case(State::middle_fire):
		{
			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
			{
				getParent()->addObject(new CFireball(getBounds().center() + m_direction * 50 + Vector::up * 10, m_direction*0.13f));
				MarioGame().playSound("bowser_fire");
				enterState(State::walk);
			}
			break;
		}
		case(State::no_bridge):
		{
			m_delay_timer -= delta_time;
			if (m_delay_timer < 0)
				enterState(State::fall);
			break;
		}
		case(State::died):
		case(State::fall):
		{
			updatePhysics(delta_time, gravity_force / 2);
			break;
		}
	  }

 
  }

  void CBowser::onActivated()
  {
	  m_center_x = getPosition().x;
  }
  void CBowser::kickFromTop(CMario* mario)
  {
	  mario->reciveDamage();
  }
  void CBowser::kickFromBottom(CMario* mario)
  {
	  m_lives--;
	  if (m_lives < 0)
		  enterState(State::died);
  }
  void CBowser::touchSide(CMario* mario)
  {
	  mario->reciveDamage();
  }
bool CBowser::isAlive() const
{
	return true;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
CPrincess::CPrincess()
{
	setSize({ 32,64 });
	m_animator.create("stay", *MarioGame().textureManager().get("texture"), { 0,176,32,64 });
}
void CPrincess::draw(sf::RenderWindow* render_window)
{
	m_animator.setPosition(getPosition());
	m_animator.draw(render_window);
}