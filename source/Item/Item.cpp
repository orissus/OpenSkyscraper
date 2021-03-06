/* Copyright (c) 2012-2015 Fabian Schuiki */
#include <cassert>
#include <string>
#include "../Game.h"
#include "Item.h"

using namespace OT::Item;
using std::string;
using OT::int2;
using OT::Person;
using OT::rectd;
using OT::recti;
using OT::Sprite;


Item::~Item()
{
	sprites.clear();
	for (People::iterator p = people.begin(); p != people.end(); p++) removePerson(*p);
}

void Item::setPosition(int2 p)
{
	if (position != p) {
		position.x = p.x/**8*/;
		position.y = p.y/**36*/;
	}
}

int2 Item::getPosition() const
{
	return position;
}

int2 Item::getPositionPixels() const
{
	return int2(position.x * 8, position.y * 36);
}

recti Item::getRect() const
{
	return recti(getPosition(), int2(getSize().x, getSize().y));
}

recti Item::getPixelRect() const
{
	return recti(getPositionPixels(), int2(getSizePixels().x, getSizePixels().y));
}

void Item::addSprite(Sprite * sprite)
{
	assert(sprite);
	sprites.insert(sprite);
}

void Item::removeSprite(Sprite * sprite)
{
	assert(sprite);
	sprites.erase(sprite);
}

void Item::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	render(target);
}

void Item::render(sf::RenderTarget & target) const
{
	for (SpriteSet::iterator s = sprites.begin(); s != sprites.end(); s++) {
		game->drawnSprites++;
		target.draw(**s);
	}

	if (!canHaulPeople() && position.y != 0 && prototype->icon != ICON_FLOOR && lobbyRoute.empty()) {
		Sprite noroute;
		noroute.SetImage(app->bitmaps["noroute.png"]);
		sf::Vector2u size = noroute.getTexture()->getSize();
		noroute.setOrigin(size.x/2, size.y/2);
		size = getSize();
		noroute.setPosition(size.x/2, -size.y/2);
		target.draw(noroute);
	}
}

void Item::encodeXML(tinyxml2::XMLPrinter & xml)
{
	xml.PushAttribute("type", prototype->id.c_str());
	xml.PushAttribute("x", position.x);
	xml.PushAttribute("y", position.y);
}

void Item::decodeXML(tinyxml2::XMLElement & xml)
{
}

void Item::addPerson(Person * p)
{
	assert(p);
	assert(!p->at);
	p->at = this;
	people.insert(p);
}

void Item::removePerson(Person * p)
{
	assert(p);
	assert(p->at == this);
	p->at = NULL;
	people.erase(p);
}

/*
 * Returns a rectangle that covers the same region
 * as this item. To be in-line with the rectd object,
 * the rectangle has its origin at the lower left
 * corner of the item on the game screen and its
 * other point "increases" upwards and to the right
 */

rectd Item::getMouseRegion()
{
	int2 p = getPositionPixels();
	sf::Vector2u s = getSizePixels();
	return rectd(p.x, p.y, s.x, s.y);
}

void Item::updateRoutes()
{
	if (!canHaulPeople() && position.y != 0) {
		lobbyRoute = game->findRoute(game->mainLobby, this);
	} else {
		lobbyRoute.clear();
	}
}
