#include "SuperMarioGame.h"



#include <iomanip>
#include "tinyxml2.h"





void Timer::invoke(const std::function <void()>& func, int delay)
{
	m_invoke_list.push_back(std::make_pair<>(m_time + delay, func));
}

void Timer::update(int delta_time)
{
	m_time += delta_time;

	for (auto it = m_invoke_list.begin(); it != m_invoke_list.end(); )
	{
		if (m_time > it->first)
		{
			it->second();
			it = m_invoke_list.erase(it);
		}
		else
			++it;
	}

}

//--------------------------------------------------------------------------------
CMarioGame* CMarioGame::s_instance = NULL;

CMarioGame::CMarioGame() : CGame("SuperMario", {1280,720})
{
	//Load resourses
	textureManager().loadFromFile("texture", "res/MarioRes/Charaster.png");
	textureManager().loadFromFile("Tiles", "res/MarioRes/tiles.png");
	textureManager().loadFromFile("AnimTiles", "res/MarioRes/AnimTiles.png");
	textureManager().loadFromFile("Enemies", "res/MarioRes/Enemies.png");
	textureManager().loadFromFile("Bowser", "res/MarioRes/Bowser.png");
	textureManager().loadFromFile("Items", "res/MarioRes/Items.png");
	textureManager().loadFromFile("Sky", "res/MarioRes/Sky.png");
	textureManager().loadFromFile("Underground", "res/MarioRes/Underground.png");
	textureManager().loadFromFile("Water", "res/MarioRes/Water.png");
	textureManager().get("Sky")->setRepeated(true);
	fontManager().loadFromFile("arial", "C:/Windows/Fonts/Calibri.ttf");
    fontManager().loadFromFile("menu_font", "res/menu_font.ttf");
	fontManager().loadFromFile("main_font", "res/main_font.ttf");
	fontManager().loadFromFile("score_font", "res/score_font.ttf");
	fontManager().loadFromFile("some_font", "res/some_font.ttf");

	soundManager().loadFromFile("breakblock", "res/MarioRes/Sounds/breakblock.wav");
	soundManager().loadFromFile("bump", "res/MarioRes/Sounds/bump.wav");
	soundManager().loadFromFile("coin", "res/MarioRes/Sounds/coin.wav");
	soundManager().loadFromFile("fireball", "res/MarioRes/Sounds/fireball.wav");
	soundManager().loadFromFile("jump_super", "res/MarioRes/Sounds/jump_super.wav");
	soundManager().loadFromFile("kick", "res/MarioRes/Sounds/kick.wav");
	soundManager().loadFromFile("stomp", "res/MarioRes/Sounds/stomp.wav");
	soundManager().loadFromFile("powerup_appears", "res/MarioRes/Sounds/powerup_appears.wav");
	soundManager().loadFromFile("powerup", "res/MarioRes/Sounds/powerup.wav");
	soundManager().loadFromFile("pipe", "res/MarioRes/Sounds/pipe.wav");
	soundManager().loadFromFile("flagpole", "res/MarioRes/Sounds/flagpole.wav");
	soundManager().loadFromFile("bowser_falls", "res/MarioRes/Sounds/bowser_falls.wav");
	soundManager().loadFromFile("bowser_fire", "res/MarioRes/Sounds/bowser_fire.wav");

	//Config input
	inputManager().registerKey(sf::Keyboard::Left);
	inputManager().registerKey(sf::Keyboard::Right);
	inputManager().registerKey(sf::Keyboard::Up);
	inputManager().registerKey(sf::Keyboard::Down);
	inputManager().registerKey(sf::Keyboard::Space);
	inputManager().registerKey(sf::Keyboard::LShift);
 
}


CMarioGame::~CMarioGame()
{

}
 

CMarioGame* CMarioGame::instance()
{
	if (s_instance == NULL)
		s_instance = new CMarioGame();
	return s_instance;
}


void CMarioGame::syncMarioRank(CGameObject* from_scene, CGameObject* to_scene)
{
	auto mario_rank = from_scene->findObjectByName<CMario>("Mario")->getRank();
	to_scene->findObjectByName<CMario>("Mario")->setRank(mario_rank);
}


void CMarioGame::clearScenes()
{
	if (!m_scene_stack.empty())
	{
		for (auto m_scene : m_scene_stack)
			getRootObject()->removeObject(m_scene);
		m_scene_stack.clear();
	}
	m_current_scene = NULL;
}


void CMarioGame::loadLevel(const std::string& level_name)
{
	setScene(new CMarioGameScene(MARIO_RES_PATH + level_name  + ".tmx"));
	m_gui_object->moveToFront();
}

void CMarioGame::loadSubLevel(const std::string& sublevel_name)
{
	pushScene(new CMarioGameScene(MARIO_RES_PATH + sublevel_name + ".tmx"));
	m_gui_object->moveToFront();
}

void  CMarioGame::unloadSubLevel()
{
	popScene();
}

void CMarioGame::setScene(CGameObject* new_scene)
{
	if (m_current_scene)
		syncMarioRank(m_current_scene, new_scene);

	clearScenes();

	m_current_scene = new_scene;
	new_scene->turnOff();
	getRootObject()->addObject(new_scene);
	m_scene_stack.push_back(new_scene);
	m_level_name = new_scene->castTo<CMarioGameScene>()->getLevelName();
	m_game_state = GameState::load_level;
}


void CMarioGame::pushScene(CGameObject* new_scene)
{
	if (m_current_scene)
	{
		syncMarioRank(m_current_scene, new_scene);
		m_current_scene->turnOff();
	}

	    m_current_scene = new_scene;
		m_scene_stack.push_back(m_current_scene);
		getRootObject()->addObject(m_current_scene);
		
}

void CMarioGame::popScene()
{
	if (m_scene_stack.empty())
		return;

	m_scene_stack.pop_back();;

	auto old_scene = m_current_scene;
	m_current_scene = m_scene_stack.back();

	syncMarioRank(m_current_scene, old_scene);
	getRootObject()->removeObject(old_scene);
	m_current_scene->turnOn();
}; 


CMarioGUI* CMarioGame::GUI()
{
	return m_gui_object;
}
 

void CMarioGame::init()
{
	getRootObject()->addObject(m_gui_object = new CMarioGUI());
	loadLevel("WORLD 1-1");
}
  
void CMarioGame::updateGUI() 
{
	m_gui_object->setGameTime(m_game_time / 1000);
	m_gui_object->setLevelName(m_level_name);
	m_gui_object->setLives(m_lives);
	m_gui_object->setCoins(m_coins);
	m_gui_object->setScore(m_score);
	m_gui_object->setMarioRank(m_current_scene->findObjectByType<CMario>()->getRank());
}
void CMarioGame::update(int delta_time)
{
	CGame::update(delta_time);
 
	m_timer.update(delta_time);

 
	switch (m_game_state)
	{
	    case (GameState::load_level):
		{
			m_game_time = 180000;
			updateGUI();
			m_gui_object->showStatus(true);
			m_game_state = GameState::status;
			m_delay_timer = 2500;
			break;
		}
		case (GameState::status):
		{
				m_delay_timer -= delta_time;
				if (m_delay_timer < 0)
				{
					m_gui_object->showStatus(false);
					m_game_state = GameState::playing;
					m_current_scene->turnOn();
				}
				break;
		}
		case (GameState::playing):
		{
					m_game_time -= delta_time;
					m_gui_object->setGameTime(m_game_time / 1000);
					break;
		}
	    case (GameState::level_over):
		{
					if (m_game_time > 0)
					{
						m_delay_timer -= delta_time;
						if (m_delay_timer < 0)
						{
							m_delay_timer = 20;
							m_game_time -= 1000;
							addScore(50);
							if (m_game_time < 0)
								m_game_time = 0;

							m_gui_object->setGameTime(m_game_time / 1000);
						}
					}
					break;
		 }
	};

 
 

}
 
 
Timer& CMarioGame::timer()
{
	return m_timer;
}

 
void CMarioGame::addScore(int value, const Vector& vector)
{
	  m_score += value;
	  GUI()->setScore(m_score);

	  if (vector != Vector::zero)
	  {
		  auto flow_text = m_gui_object->createFlowText();
		  flow_text->splash(vector, toString(value));
		  m_current_scene->addObject(flow_text);
		//  timer().invoke([flow_text] { (destroyObject(flow_text)); }, 2000);
	  }
}

void CMarioGame::addCoin()
{
	GUI()->setCoins(++m_coins);
}

void CMarioGame::setScore(int score)
{
	m_score = score;
	GUI()->setScore(m_score);
}
int CMarioGame::getScore() const
{
	return m_score;
}
int CMarioGame::getGameTime() const
{
	return m_game_time;
}

CMarioGame& MarioGame()
{
	return *CMarioGame::instance();
}

CLabel* CMarioGame::createText(const std::string& text, const Vector& pos)
{
	CLabel* label = GUI()->createLabel();
	label->setString(text);
	label->setPosition(pos);
	return label;
}

void  CMarioGame::setEndLevelStatus()
{
	m_game_state = GameState::level_over;
}

//--------------------------------------------------------------------------------

std::map<std::string, Property> parseProperties(tinyxml2::XMLElement* object)
{
	std::map<std::string, Property> parsed;

	//common properties
	parsed["x"] = Property(toFloat(object->Attribute("x")));
	parsed["y"] = Property(toFloat(object->Attribute("y")));
	parsed["width"] = Property(toFloat(object->Attribute("width")));
	parsed["height"] = Property(toFloat(object->Attribute("height")));
	parsed["name"] = Property(toString(object->Attribute("name")));

	//specific properties
	tinyxml2::XMLElement* properties = object->FirstChildElement("properties");
	if (properties)
		for (auto property = properties->FirstChildElement("property"); property != NULL; property = property->NextSiblingElement())
		{
			
			std::string type("string");
			if (property->Attribute("type")) type = property->Attribute("type");
			std::string name = property->Attribute("name");
			std::string value = property->Attribute("value");

			if (type == "int")
				parsed[name] = toInt(value);
			else if (type == "float")
				parsed[name] = toFloat(value);
			else if (type == "bool")
				parsed[name] = toBool(value);
			else 
				parsed[name] = toString(value);
		}
	
	
	return parsed;
}

template <typename T>
CGameObject* goFabric()
{
	return new T;
}

CGameObject* parseGameObject(tinyxml2::XMLElement* element)
{
	std::string obj_type = element->Attribute("type");
	CGameObject* object = NULL;

	static std::map<std::string, CGameObject* (*)()> fabrics = 
	{
		{ "Mario",             goFabric<CMario> },
	    { "Goomba",            goFabric<CGoomba> },
	    { "Koopa",             goFabric<CKoopa> },
		{ "HammerBro",         goFabric<CHammerBro> },
		{ "Bowser",            goFabric<CBowser> },
		{ "BuzzyBeetle",       goFabric<ÑBuzzyBeetle> },
		{ "LakitySpawner",     goFabric<CLakitySpawner> },
		{ "CheepCheep",        goFabric<CCheepCheep> },
		{ "Blooper",           goFabric<CBlooper> },
		{ "CheepCheepSpawner", goFabric<CCheepCheepSpawner>},
		{ "BulletBillSpawner", goFabric<CBulletBillSpawner> },
		{ "PiranhaPlant",      goFabric<CPiranhaPlant> },
		{ "Podoboo",           goFabric<CPodoboo> },
		{ "Coin",              goFabric<CCoin> },
		{ "Jumper",            goFabric<CJumper> },
		{ "FireBar",           goFabric<CFireBar> },
		{ "MoveablePlatform",  goFabric<CMoveablePlatform> },
		{ "FallingPlatform",   goFabric<CFallingPlatform> },
		{ "PlatformSystem",    goFabric<CPlatformSystem> },
		{ "Background",        goFabric<CBackground> },
	    { "LevelPortal",       goFabric<CLevelPortal> },
	    { "EndLevelFlag",      goFabric<CEndLevelFlag> },
		{ "EndLevelKey",       goFabric<CEndLevelKey> },
		{ "CastleFlag",        goFabric<CCastleFlag> },
	    { "Princess",          goFabric<CPrincess> }
	};

	auto object_fabric = fabrics[obj_type];
	assert(object_fabric); //there is no fabric for such object
	object = object_fabric();

	auto properties = parseProperties(element);
	for (auto& property : properties)
		object->setProperty(property.first, property.second);

 	return object;
}

CBlocks* parseBlocks(tinyxml2::XMLElement* element)
{
	tinyxml2::XMLElement* data = element->FirstChildElement("layer")->FirstChildElement("data");

	//convert cvs format to string, where each char definite type of block
	std::string dirty_string = data->GetText();
	std::string new_string, buf;
	for (int i = 0; i < dirty_string.size(); ++i)
	{
		if (isdigit(dirty_string[i]))
			buf += dirty_string[i];
		else if (!buf.empty())
		{
			new_string += (char)toInt(buf);
			buf.clear();
		}
	}
	


	static auto blocks_fabric = [](char c) -> AbstractBlock*
	{
		switch (c)
		{
		case(0): return NULL;
		case(AbstractBlock::BRICK_TILE_CODE): return new CBricksBlock();
		case(AbstractBlock::MONEY_TILE_CODE): return new CMoneyBox();
		case(AbstractBlock::COIN_TILE_CODE): return new CCoinBox();
		case(AbstractBlock::MUSHROOM_TILE_CODE): return new CMushroomBox();
		case(AbstractBlock::LADDER_TILE_CODE): return new CLadderBlock();
		default: return new CStaticBlock(c);
		}
	};

	auto blocks = new CBlocks(toInt(element->Attribute("width")), 
		                      toInt(element->Attribute("height")), 
		                      toInt(element->Attribute("tilewidth")));
	blocks->loadFromString(new_string, blocks_fabric);
	return blocks;
}



void CMarioGameScene::loadFromFile(const std::string& filepath)
{
	setName("MarioGameScene");
	m_level_name = filepath;
	auto it1 = --m_level_name.end();

	
	while (*it1 != '.')
		it1--;
	auto it2 = it1;
	while (*it2 != '/' || it2 == m_level_name.begin())
		it2--;
	m_level_name = std::string(++it2, it1);

	clear();
	tinyxml2::XMLDocument documet;
	bool status = documet.LoadFile(filepath.c_str());
	assert(status == tinyxml2::XML_SUCCESS); // cant load file
	tinyxml2::XMLElement* root_element = documet.FirstChildElement();

	//Load tilemap
	m_blocks = parseBlocks(root_element);
	addObject(m_blocks);

	//Load objects
	tinyxml2::XMLElement* objects = root_element->FirstChildElement("objectgroup");
	for (auto obj = objects->FirstChildElement("object"); obj != NULL; obj = obj->NextSiblingElement())
	{
		auto object = parseGameObject(obj);
		if (object)
			addObject(object);
	}

	m_mario = findObjectByType<CMario>();
	m_mario->moveToFront();
	assert(m_mario); // no mario object in scene
	setCameraOnTarget();
}



const std::string& CMarioGameScene::getLevelName() const
{
	return m_level_name;
}
CMarioGameScene::CMarioGameScene()
{
	setName("MarioGameScene");
}

CMarioGameScene::CMarioGameScene(const std::string& filepath)
{
	setName("MarioGameScene");
	loadFromFile(filepath);
}

 
void CMarioGameScene::update(int delta_time)
{
	CGameObject::update(delta_time);

	m_view.setSize(screen_size);

	Vector camera_pos = m_view.getCenter();
	camera_pos.x += (m_mario->getBounds().center().x - camera_pos.x) *0.0075f*delta_time;
	camera_pos.y += (m_mario->getBounds().center().y - camera_pos.y) *0.0005f*delta_time;
	camera_pos.x = math::clamp(camera_pos.x, screen_size.x / 2.f, m_blocks->width() - screen_size.x / 2.f);
	camera_pos.y = math::clamp(camera_pos.x, screen_size.y / 2.f, m_blocks->height() - screen_size.y / 2.f);
	m_view.setCenter(camera_pos);
}


void CMarioGameScene::draw(sf::RenderWindow* render_window)
{
	screen_size = Vector(render_window->getSize().x / scale_factor, render_window->getSize().y / scale_factor);

	render_window->setView(m_view);
	CGameObject::draw(render_window);
	render_window->setView(render_window->getDefaultView());
}

Vector CMarioGameScene::pointToScreen(const Vector& vector)
{
	return (vector -  (Vector(m_view.getCenter()) - Vector(m_view.getSize()) / 2))*scale_factor;
}

Vector CMarioGameScene::screenToPoint(const Vector& vector)
{
	return vector / scale_factor + (Vector(m_view.getCenter()) - Vector(m_view.getSize()) / 2);
}
 

Rect CMarioGameScene::cameraRect() const
{
	return Rect( Vector(m_view.getCenter()) - Vector(m_view.getSize())*0.5,m_view.getSize() );
}

void CMarioGameScene::setCameraOnTarget()
{
	m_view.setCenter(m_mario->getBounds().center());
}


CMarioGameScene::~CMarioGameScene()
{
 
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
CMarioGUI::CMarioGUI()
{
	const int y_gui_pos = 5;

	m_score_lab = new CLabel();
	m_score_lab->setFontName(*MarioGame().fontManager().get("some_font"));
	m_score_lab->setPosition({ 70,y_gui_pos });
	m_score_lab->setFontStyle(sf::Text::Bold);
	m_score_lab->setFontColor({255,255,220});
	m_score_lab->setFontSize(40);
	m_score_lab->setTextAlign(CLabel::left);
	addObject(m_score_lab);
	
	m_coin_counter_lab = createLabel();
	m_coin_counter_lab->setPosition({ 490,y_gui_pos });
	addObject(m_coin_counter_lab);

    m_world_lab = createLabel();
	m_world_lab->setPosition({ 720,y_gui_pos });
	addObject(m_world_lab);

	m_timer = createLabel();
	m_timer->setPosition({ 1080,y_gui_pos });
	addObject(m_timer);
	 
	m_level_name = createLabel();
	m_level_name->setPosition(MarioGame().screenSize() / 2.f + Vector::up*100.f);
	m_level_name->setTextAlign(CLabel::center);
	addObject(m_level_name);

	m_lives = createLabel();
	m_lives->setPosition(MarioGame().screenSize() / 2.f + Vector(-15,-18));
	addObject(m_lives);

	auto texture = MarioGame().textureManager().get("texture");
	m_mario_pix = new Animator();
	m_mario_pix->create("small", *texture, { 0,96,32,32 });
	m_mario_pix->setSpriteOffset("small", 0, Vector::down*22.f);
	m_mario_pix->create("big", *texture, { 0,32,32,64 });
	m_mario_pix->setPosition(MarioGame().screenSize() / 2.f + Vector(-64,-44));
	m_mario_pix->play("big");
	m_mario_pix->scale(1.3f, 1.3f);
	addObject(m_mario_pix);
	m_fire_pallete.create({ sf::Color(202,77,62), sf::Color(132,133,30) }, { sf::Color(255,255,255),sf::Color(202,77,62) });

	m_flow_text = new CFlowText(*MarioGame().fontManager().get("main_font"));
	m_flow_text->setTextColor(sf::Color::Red);
	m_flow_text->setSplashVector({ 0,-3 });
	m_flow_text->setTextSize(14);
	addObject(m_flow_text);
}


void CMarioGUI::setMarioRank(MarioRank rank)
{
	if (rank == MarioRank::small)
	{
		m_mario_pix->play("small");
	}
	else
		m_mario_pix->play("big");

	if (rank == MarioRank::fire)
	 m_mario_pix->setPallete(&m_fire_pallete);
	else
	 m_mario_pix->setPallete(NULL);
}


void CMarioGUI::update(int delta_time)
{
	CGameObject::update(delta_time);
}
void CMarioGUI::postDraw(sf::RenderWindow* render_window)
{
	//foreachObject([render_window](CGameObject* object) { object->draw(render_window); });
}

void CMarioGUI::draw(sf::RenderWindow* render_window)
{ 
	if (m_status_mode)
	{
		render_window->clear(sf::Color::Black);
		m_level_name->draw(render_window);
	}

	CGameObject::draw(render_window);
}

void CMarioGUI::setScore(int value)
{
	std::stringstream str_stream;
	str_stream << "MARIO: " << std::setw(6) << std::setfill('0') << value;
	m_score_lab->setString(str_stream.str());
}

void CMarioGUI::setCoins(int value)
{
	std::stringstream str_stream;
	str_stream << "x" << std::setw(2) << std::setfill('0') << value;
	m_coin_counter_lab->setString(str_stream.str());
}

void CMarioGUI::setGameTime(int time)
{
	std::stringstream str_stream;
	str_stream << "TIME: " << std::setw(3) << std::setfill('0') << time;
	m_timer->setString(str_stream.str());
}


CFlowText* CMarioGUI::createFlowText()
{
	return m_flow_text->clone();

}
CLabel* CMarioGUI::createLabel()
{
	return m_score_lab->clone();
}

void CMarioGUI::showStatus(bool value)
{
	m_status_mode = value;
	if (value)
	{
		m_level_name->turnOn();
		m_lives->turnOn();
		m_mario_pix->turnOn();
	}
	else
	{
		m_level_name->turnOff();
		m_lives->turnOff();
		m_mario_pix->turnOff();
	}
}

void CMarioGUI::setLevelName(const std::string& string)
{
	m_level_name->setString(string);
	m_world_lab->setString(string);
}


void CMarioGUI::setLives(int value)
{
	m_lives->setString("  x  " + toString(value));
}


 

