#pragma once

#ifndef  GAMEENGINE_H
#define GAMEENGINE_H

#include "SFML\Graphics.hpp"
#include "SFML\Audio.hpp"
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <list>

#include <vector>
#include <unordered_map>
#include <fstream>
#include "assert.h"
#include <iostream>


#include "Geometry.h"
#include "TileMap.h"


template <typename T>
std::string toString(const T& param)
{
	std::stringstream sstream;
	sstream << param;
	return sstream.str();
}

 

float toFloat(const std::string& str);
int toInt(const std::string& str);
bool toBool(const std::string& str);

namespace math
{
	int sign(float value);

	template <typename T>
	T clamp(const T& value, const T& min, const T& max)
	{
		if (value < min)
			return min; 
		if (value > max)
			return max;
		return value;
	}
}

void drawLinearSprite_v(sf::Sprite sprite, const sf::Rect<int>& draw_area, sf::RenderWindow* render_window);
void drawLinearSprite_h(sf::Sprite sprite, const sf::Rect<int>& draw_area, sf::RenderWindow* render_window);



class Property
{
public:
	Property();
	Property(bool bool_value);
	Property(int int_value);
	Property(const std::string& string_value);
	Property(float float_value);
	bool asBool() const;
	int asInt() const;
	float asFloat() const;
	const std::string& asString() const;
	bool isValid() const;

private:
	int int_data;
	float float_data;
	std::string string_data;
	bool bool_data;
	enum class Type { NoInit, Bool, Int, Float, String } m_type;
};


class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	//Basic
	void setName(const std::string& name);
	const std::string& getName() const;
	void setProperty(const std::string& name, const Property& property);
	Property getProperty(const std::string& name) const;
	void setParent(CGameObject* game_object);
	CGameObject* getParent() const;
	CGameObject* addObject(CGameObject* object);
	CGameObject* findObjectByName(const std::string& name);
	void moveToBack();
	void moveToFront();
	void moveUnderTo(CGameObject* obj);
	template <typename T>
	T* findObjectByName(const std::string& name)
	{
		auto it = std::find_if(m_objects.begin(), m_objects.end(), [this, &name](const CGameObject* obj) -> bool { return obj->getName() == name;  });
		if (it != m_objects.end())
			return dynamic_cast<T*>(*it);

		return nullptr;
	}
	template <typename T>
	T* findObjectByType()
	{
		for (auto& obj : m_objects)
			if (dynamic_cast<T*>(obj) != nullptr)
				return (T*)obj;
		return nullptr;
	}
	template <typename T>
	std::vector<T*> findObjectsByType()
	{
		std::vector<T*> objects;
		for (auto& obj : m_objects)
		{
			if (dynamic_cast<T*>(obj) != nullptr)
				objects.push_back((T*)obj);
		 
			auto objects_temp = obj->findObjectsByType<T>();
			if (!objects_temp.empty())
				objects.insert(objects.end(), objects_temp.begin(), objects_temp.end());
		}
		return objects;
	}
	template <typename T>
	T* castTo()
	{
		assert(dynamic_cast<T*>(this));
		return (T*)this;
	}
	template <typename T>
	bool isTypeOf() const
	{
		return (dynamic_cast<const T*>(this) != NULL);
	}
	void foreachObject(std::function<void(CGameObject*)> predicate);
	void foreachObject(std::function<void(CGameObject*, bool& need_break)> predicate);
	void removeObject(CGameObject* obj);
	void clear();
	static void invokePreupdateActions();
	virtual void start();
	virtual void update(int delta_time);
	virtual void events(const sf::Event& event) {};
	void enable();
	void disable();
	bool isEnabled() const;
	void hide();
	void show();
	bool isVisible() const;
	void turnOn();
	void turnOff();

	//Animator / Render Component
	virtual void draw(sf::RenderWindow* window);
	virtual void postDraw(sf::RenderWindow* window);

	//Transform component
	const Vector& getPosition() const;
	void setPosition(const Vector& point);
	void setPosition(float x, float y);
	void move(const Vector& point);
	void setDirection(const Vector& direction);
	Vector getDirection();

	//Solid Object component
	virtual Rect getBounds() const;
	virtual void setBounds(const Rect& rect);
	void setSize(const Vector& size);
 
protected:
	virtual void onPropertySet(const std::string& name);
	virtual void onPropertyGet(const std::string& name) const;
	virtual void onActivated() {};
private:
	std::string m_name;
	bool m_started = false;
	static std::vector<std::function<void()>> m_preupdate_actions;
	std::map<std::string, Property> m_properties;
	CGameObject* m_parent;
	std::list<CGameObject*> m_objects;
	Vector m_direction;
	bool m_enable;
	bool m_visible;
	Vector m_pos, m_size;
};


void destroyObject(CGameObject* gameObject);


class CEventManager
{
public:
	void pushEvent(const sf::Event& event);
	void subscribe(CGameObject* object);
	void unsubcribe(CGameObject* object);
private:
	std::list<CGameObject*> m_subcribes;
};

template <typename T>
class ResourceManager
{
public:
	ResourceManager() = default;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager operator=(const ResourceManager&) = delete;
	virtual void loadFromFile(const std::string& name, const std::string& file_path);
	T* get(const std::string& name);
	T* operator[](const std::string& name);
	const T* get(const std::string& name) const;
	~ResourceManager();
protected:
	std::map<std::string, T*> m_resources;
};

template <typename T>
void ResourceManager<T>::loadFromFile(const std::string& name, const std::string& file_path)
{
	assert(m_resources[name] == nullptr); // allready exist
	m_resources[name] = new T();
	if (!m_resources[name]->loadFromFile(file_path))
		throw std::runtime_error(("runtime error can't load resource: " + file_path).c_str());
}

template <typename T>
T* ResourceManager<T>::get(const std::string& name)
{
	assert(m_resources[name] != nullptr); //no such resource
	return m_resources[name];
}

template <typename T>
T* ResourceManager<T>::operator[](const std::string& name)
{
	return get(name);
}

template <typename T>
const T* ResourceManager<T>::get(const std::string& name) const
{
	assert(m_resources[name] != nullptr); //no such resource
	return m_resources[name];
}

template <typename T>
ResourceManager<T>::~ResourceManager()
{
	return;
	for (auto r : m_resources)
		if (r.second != NULL)
		{
			delete r.second;
			r.second = NULL;
		}
}


class CInputManager
{
private:
	std::map<sf::Keyboard::Key, bool> m_keys_prev, *m_keys_prev_ptr;
	std::map<sf::Keyboard::Key, bool> m_keys_now, *m_keys_now_ptr;
public:
	CInputManager();
	void registerKey(const sf::Keyboard::Key& key);
	void unregisterKey(const sf::Keyboard::Key& key);
	bool isKeyJustPressed(const sf::Keyboard::Key& key);
	bool isKeyJustReleased(const sf::Keyboard::Key& key);
	bool isKeyPressed(const sf::Keyboard::Key& key);
	void update(int delta_time);
};


using CTextureManager = ResourceManager<sf::Texture>;
using CFontManager = ResourceManager<sf::Font>;
using CSoundManager = ResourceManager<sf::SoundBuffer>;


class CGame
{
private:
	CGameObject* m_root_object = NULL;
	CTextureManager m_texture_manager;
	CFontManager m_font_manager;
	CSoundManager m_sound_manager;
	CEventManager m_event_manager;
	CInputManager m_input_manager;
	sf::Sound m_sounds_buf[32];
	sf::RenderWindow* m_window = NULL; 
	Vector m_screen_size;
	sf::Color m_clear_color = sf::Color::Black;
	void  draw(sf::RenderWindow* render_window);
protected:
	void virtual init();
	void virtual update(int delta_time);
	void setClearColor(const sf::Color& color);
public:
	CGame(const std::string& name, const Vector& screen_size);
	~CGame();
	void run();
	CGameObject*  getRootObject();
	CTextureManager&  textureManager();
	CFontManager&  fontManager();
	CSoundManager&  soundManager();
	CEventManager&  eventManager();
	CInputManager&  inputManager();
	void playSound(const std::string& name);
	Vector screenSize() const;
};




class CTimer : public CGameObject
{
public:
	CTimer();
	void update(int delta_time) override;
	void clear();
	template <typename T>
	void add(sf::Time time, T callable)
	{
		m_call_back_list.push_back(std::pair<sf::Time, std::function<void()>>(time, std::bind(callable)));
	}
	~CTimer();

private:
	std::list< std::pair<sf::Time, std::function<void()>>> m_call_back_list;
};


enum class AnimType { manual, forward, forward_stop, forward_cycle, forward_backward_cycle };

class CSpriteSheet : public CGameObject
{
public:
	CSpriteSheet();
	void load(const sf::Texture& texture, const std::vector<sf::IntRect>& rects);
	void load(const sf::Texture& texture, const Vector& off_set, const Vector& size, int cols, int rows);
	void draw(sf::RenderWindow* wnd) override;
	void update(int delta_time) override;
	void setAnimType(AnimType type);
	void setSpriteIndex(int index);
	sf::Sprite* currentSprite();
	void setPosition(sf::Vector2f pos);
	sf::Vector2f getPosition() const;
	void setSpeed(float speed);
	void setColor(const sf::Color& color);
	void setRotation(float angle);
	void scale(float fX, float fY);
	void invert_h();
	void flipX(bool isFliped);
	sf::Sprite& operator[](int index);
	void reset();
	bool empty() const;
	void setAnimOffset(float index);
	void setOrigin(const Vector& vector);
	void resize(const Vector& vector);
	AnimType animType() const;
private:
	AnimType m_anim_type;
	bool m_flipped = false;
	std::vector<sf::Sprite> m_sprites;
	sf::Sprite* m_current_sprite;
	float m_speed;
	sf::Vector2f m_position;
	sf::Vector2f m_torigin;
	float m_index = 0;
};



class Pallete
{
public:
	Pallete();
	void create(const std::initializer_list<sf::Color>& original_colors, const std::initializer_list<sf::Color>& swaped_colors);
	void apply();
	void cancel();
private:
	sf::Shader m_shader;
	int m_old_shader = 0;
};

class Animator : public CGameObject
{
public:
	~Animator();
	void create(const std::string& name, const sf::Texture& texture, const Vector& off_set, const Vector& size, int cols, int rows, float speed, AnimType anim_type = AnimType::forward_cycle);
	void create(const std::string& name, const sf::Texture& texture, const Rect& rect);
	void create(const std::string& name, const sf::Texture& texture, const std::vector<sf::IntRect>& rects, float speed);
	void play(const std::string& name);
	void update(int delta_time) override;
	void draw(sf::RenderWindow* wnd) override;
	void flipX(bool value);
	void setColor(const sf::Color& color);
	void setSpeed(const std::string& animation, float speed);
	void setSpriteOffset(const std::string& anim_name, int sprite_index, const Vector& value);
	void setAnimOffset(float index);
	void setPallete(Pallete* pallete);
	void scale(float fX, float fY);
	CSpriteSheet* get(const std::string& str);
private:
	Pallete* m_pallete = NULL;
	std::unordered_map<std::string, CSpriteSheet*> m_animations;
	CSpriteSheet* m_current_animation = NULL;
	std::string last_anim_name;
	bool m_flipped = false;
};


class CFlowText : public CGameObject
{
public:
	CFlowText(const sf::Font& font, bool self_remove = false);
	void setTextColor(const sf::Color& color);
	void setTextSize(int size);
	void splash(const Vector& pos, const std::string& text);
	virtual void update(int delta_time) override;
	virtual void draw(sf::RenderWindow* window) override;
	bool isFlashing() const;
	void setSplashVector(const Vector& vector);
	CFlowText* clone() const;
private:
	bool m_self_remove = false;
	sf::Text m_text;
	Vector m_offset;
	Vector m_splash_vector = {1,-1};
	float m_time;
	int m_color;
	bool m_flashing;
};


class CLabel : public CGameObject
{
public:
	enum { left, center };
	CLabel();
	CLabel(const std::string& str);
	void setSprite(const sf::Sprite& sprite);
	void setString(const std::string& str);
	void setTextAlign(int value);
	void setFontColor(const sf::Color& color);
	void setFontSize(int size);
	void setOutlineColor(const sf::Color& color);
	void setFillColor(const sf::Color& color);
	void setOutlineThickness(int value);
	void setBounds(int x, int y, int w, int h);
	Rect getBounds() const override;
	void setFontName(const sf::Font& font);
	void setFontStyle(sf::Uint32 style);
	bool contains(const Vector& point) const;
	CLabel* clone() const;
	virtual void draw(sf::RenderWindow* window) override;
protected:
	sf::RectangleShape m_shape;

private:
	void init();
	int m_text_align = center;
	sf::Sprite m_sprite;
	Rect m_rect;
	sf::Text m_text;
};



enum class ECollisionTag : int { none = 0, cell = 1, floor = 2, left = 4, right = 8, up = cell, down = floor };

inline ECollisionTag operator |= (ECollisionTag& one, const ECollisionTag& two)
{
	one = ECollisionTag(static_cast<int>(one) | static_cast<int>(two));
	return one;
}

inline bool operator & (ECollisionTag one, ECollisionTag  two)
{
	return  static_cast<int>(one) & static_cast<int>(two);
}


Vector  collsionResponse(const Rect& own_rect, const Vector& own_speed, const Rect& other_rect, const Vector& other_speed, const float delta_time, ECollisionTag& collision_tag);




class WaypointSystem : public CGameObject
{
private:
	std::vector<Vector> m_path;
	float m_length = 0;
	float m_speed = 0;

public:
	CGameObject* getObject();
	void addPath(const std::vector<Vector>& path, float speed, bool align = false);
	bool isMoving() const;
	void stop();
	void update(int delta_time) override;
};



#endif