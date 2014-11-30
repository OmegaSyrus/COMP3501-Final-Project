#pragma once

#include "Entity.h"

using namespace DirectX;

class WeaponCollection {
public:
	WeaponCollection(float delay);

	void AddWeapon(Entity& weapon);
	//T& ReturnWeapon();
	std::vector<Entity*>& GetWeapons();
	void FireWeapon();

private:
	std::vector<Entity> weapons;
	int current;
	float delay;
	int ammo;
};