/*-------------------------------------------------------------------------------

	BARONY
	File: actarrow.cpp
	Desc: behavior function for arrows/bolts

	Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "monster.hpp"
#include "engine/audio/sound.hpp"
#include "interface/interface.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "items.hpp"
#include "magic/magic.hpp"
#include "scores.hpp"
#include "player.hpp"
#include "prng.hpp"

/*-------------------------------------------------------------------------------

	act*

	The following function describes an entity behavior. The function
	takes a pointer to the entity that uses it as an argument.

-------------------------------------------------------------------------------*/

#define ARROW_STUCK my->skill[0]
#define ARROW_LIFE my->skill[1]
#define ARROW_VELX my->vel_x
#define ARROW_VELY my->vel_y
#define ARROW_VELZ my->vel_z
#define ARROW_OLDX my->fskill[2]
#define ARROW_OLDY my->fskill[3]
#define ARROW_MAXLIFE 600
#define ARROW_INIT my->skill[10]
#define ARROW_FLICKER my->skill[11]
#define ARROW_LIGHTING my->skill[12]
#define ARROW_STUCK_CLIENT my->skill[13]

enum ArrowSpriteTypes : int
{
	PROJECTILE_ROCK_SPRITE = 78,
	PROJECTILE_NORMAL_SPRITE = 166,
	PROJECTILE_BOLT_SPRITE = 167,
	PROJECTILE_SILVER_SPRITE = 924,
	PROJECTILE_PIERCE_SPRITE,
	PROJECTILE_SWIFT_SPRITE,
	PROJECTILE_FIRE_SPRITE,
	PROJECTILE_HEAVY_SPRITE,
	PROJECTILE_CRYSTAL_SPRITE,
	PROJECTILE_HUNTING_SPRITE,
	PROJECTILE_ARCANE_SPRITE
};

void actArrow(Entity* my)
{
	double dist = 0.0;
	node_t* node = nullptr;

	// lifespan
	ARROW_LIFE++;
	my->removeLightField();

	if ( ARROW_LIFE >= ARROW_MAXLIFE )
	{
		list_RemoveNode(my->mynode);
		return;
	}

	// falling out of the map
	if ( my->z > 32 )
	{
		list_RemoveNode(my->mynode);
		return;
	}

	if ( my->arrowQuiverType == QUIVER_FIRE || my->sprite == PROJECTILE_FIRE_SPRITE )
	{
		if ( ARROW_LIFE > 1 )
		{
			my->light = addLight(my->x / 16, my->y / 16, "fire_arrow");
			if ( flickerLights )
			{
				//Torches will never flicker if this setting is disabled.
				ARROW_FLICKER++;
			}
			if ( ARROW_FLICKER > 5 )
			{
				ARROW_LIGHTING = (ARROW_LIGHTING == 1) + 1;

				if ( ARROW_LIGHTING == 1 )
				{
					my->removeLightField();
					my->light = addLight(my->x / 16, my->y / 16, "fire_arrow");
				}
				else
				{
					my->removeLightField();
					my->light = addLight(my->x / 16, my->y / 16, "fire_arrow_flicker");
				}
				ARROW_FLICKER = 0;
			}

			if ( ARROW_STUCK != 0 )
			{
				if ( Entity* entity = spawnFlame(my, SPRITE_FLAME) )
				{
					if ( ARROW_STUCK == 1 )
					{
						entity->x += .5 * cos(my->yaw);
						entity->y += .5 * sin(my->yaw);
					}
					else
					{
						entity->x += 1.5 * cos(my->yaw);
						entity->y += 1.5 * sin(my->yaw);
					}
					entity->z = my->z;
				}
			}
			else
			{
				Entity* flame = spawnMagicParticleCustom(my, SPRITE_FLAME, 0.5, 4); // this looks nicer than the spawnFlame :)
				if ( flame )
				{
					flame->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
					flame->flags[SPRITE] = true;
                    flame->ditheringDisabled = true;
				}
			}
		}
	}
	else if ( my->arrowQuiverType == QUIVER_KNOCKBACK || my->sprite == PROJECTILE_HEAVY_SPRITE )
	{
		if ( ARROW_STUCK == 0 )
		{
			Entity* particle = spawnMagicParticleCustom(my, 159, 0.5, 4);
			if ( particle )
			{
				particle->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
				particle->flags[SPRITE] = true;
                particle->ditheringDisabled = true;
			}
		}
	}
	else if ( my->arrowQuiverType == QUIVER_SILVER || my->sprite == PROJECTILE_SILVER_SPRITE )
	{
		if ( ARROW_STUCK == 0 )
		{
			Entity* particle = spawnMagicParticleCustom(my, 160, 0.5, 4);
			if ( particle )
			{
				particle->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
				particle->flags[SPRITE] = true;
                particle->ditheringDisabled = true;
			}
		}
	}
	else if ( my->arrowQuiverType == QUIVER_CRYSTAL || my->sprite == PROJECTILE_CRYSTAL_SPRITE )
	{
		if ( ARROW_STUCK == 0 )
		{
			Entity* particle = spawnMagicParticleCustom(my, 155, 0.5, 4);
			if ( particle )
			{
				particle->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
				particle->flags[SPRITE] = true;
                particle->ditheringDisabled = true;
			}
		}
	}
	else if ( my->arrowQuiverType == QUIVER_PIERCE || my->sprite == PROJECTILE_PIERCE_SPRITE )
	{
		if ( ARROW_STUCK == 0 )
		{
			Entity* particle = spawnMagicParticleCustom(my, 158, 0.5, 4);
			if ( particle )
			{
				particle->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
				particle->flags[SPRITE] = true;
                particle->ditheringDisabled = true;
			}
		}
	}
	else if ( my->arrowQuiverType == QUIVER_LIGHTWEIGHT || my->sprite == PROJECTILE_SWIFT_SPRITE )
	{
		if ( ARROW_STUCK == 0 )
		{
			Entity* particle = spawnMagicParticleCustom(my, 156, 0.5, 4);
			if ( particle )
			{
				particle->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
				particle->flags[SPRITE] = true;
                particle->ditheringDisabled = true;
			}
		}
	}
	else if ( my->arrowQuiverType == QUIVER_HUNTING || my->sprite == PROJECTILE_HUNTING_SPRITE )
	{
		if ( ARROW_STUCK == 0 )
		{
			Entity* particle = spawnMagicParticleCustom(my, 157, 0.5, 4);
			if ( particle )
			{
				particle->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
				particle->flags[SPRITE] = true;
                particle->ditheringDisabled = true;
			}
		}
	}
	else if (my->arrowQuiverType == QUIVER_ARCANE || my->sprite == PROJECTILE_ARCANE_SPRITE)
	{
		if (ARROW_STUCK == 0)
		{
			Entity* particle = spawnMagicParticleCustom(my, 156, 0.5, 4);
			if (particle)
			{
				particle->lightBonus = vec4(0.5f, 0.5f, 0.5f, 0.f);
				particle->flags[SPRITE] = true;
				particle->ditheringDisabled = true;
			}
		}
	}

	if ( multiplayer != CLIENT )
	{
		Sint32 val = (1 << 31);
		val |= (Uint8)(17);
		val |= (((Uint16)(my->arrowShotByWeapon) & 0xFFF) << 8);
		val |= (my->arrowDropOffEquipmentModifier + 8) << 20;
		my->skill[2] = val;//-(1000 + my->arrowShotByWeapon); // invokes actArrow for clients.
		my->flags[INVISIBLE] = false;
	}

	if ( !ARROW_INIT )
	{
		if ( multiplayer == CLIENT )
		{
			if ( my->setArrowProjectileProperties(my->arrowShotByWeapon) )
			{
				ARROW_INIT = 1;
			}
			else
			{
				return;
			}
		}
		else
		{
			if ( my->arrowPower == 0 )
			{
				my->arrowPower = 10 + (my->sprite == PROJECTILE_BOLT_SPRITE);
			}
			if ( my->arrowShotByParent == 0 ) // shot by trap
			{
				my->arrowSpeed = 7;
			}
			ARROW_INIT = 1;
		}
	}

	if ( ARROW_STUCK == 0 )
	{
		if ( my->arrowFallSpeed > 0 )
		{
			real_t pitchChange = 0.02;
			if ( my->arrowShotByWeapon == LONGBOW )
			{
				pitchChange = 0.005;
			}
			if ( my->arrowBoltDropOffRange > 0 )
			{
				if ( my->ticks >= my->arrowBoltDropOffRange )
				{
					ARROW_VELZ += my->arrowFallSpeed;
					my->z += ARROW_VELZ;
					my->pitch = std::min(my->pitch + pitchChange, PI / 8);
				}
			}
			else
			{
				ARROW_VELZ += my->arrowFallSpeed;
				my->z += ARROW_VELZ;
				my->pitch = std::min(my->pitch + pitchChange, PI / 8);
			}
		}

		Entity* arrowSpawnedInsideEntity = nullptr;
		if ( ARROW_LIFE == 1 && my->arrowShotByParent == 0 && multiplayer != CLIENT ) // shot by trap
		{
			Entity* parent = uidToEntity(my->parent);
			if ( parent && parent->behavior == &actArrowTrap )
			{
				for ( node_t* node = map.creatures->first; node != nullptr; node = node->next )
				{
					Entity* entity = (Entity*)node->element;
					if ( entity && (entity->behavior == &actMonster || entity->behavior == &actPlayer) )
					{
						if ( entityInsideEntity(my, entity) )
						{
							arrowSpawnedInsideEntity = entity;
						}
						break;
					}
				}
			}
		}

		if ( multiplayer != CLIENT )
		{
			// horizontal motion
			ARROW_VELX = cos(my->yaw) * my->arrowSpeed;
			ARROW_VELY = sin(my->yaw) * my->arrowSpeed;
			ARROW_OLDX = my->x;
			ARROW_OLDY = my->y;
			dist = clipMove(&my->x, &my->y, ARROW_VELX, ARROW_VELY, my);
		}

		bool arrowInGround = false;
		int index = (int)(my->y / 16) * MAPLAYERS + (int)(my->x / 16) * MAPLAYERS * map.height;
		index = std::clamp(index, 0, (int)(MAPLAYERS * map.width * map.height) - 1);
		if ( map.tiles[index] )
		{
			if ( my->sprite == PROJECTILE_BOLT_SPRITE || my->sprite == PROJECTILE_ROCK_SPRITE ) // bolt/rock
			{
				if ( my->z >= 7 )
				{
					arrowInGround = true;
				}
			}
			else 
			{
				if ( my->pitch >= PI / 12 ) // heavily pitched
				{
					if ( my->z >= 6.5 )
					{
						arrowInGround = true;
					}
				}
				else
				{
					if ( my->z >= 7 ) // shallow pitch, make ground criteria lower.
					{
						arrowInGround = true;
					}
				}
			}

			if ( arrowInGround && (swimmingtiles[map.tiles[index]] || lavatiles[map.tiles[index]]) )
			{
				my->removeLightField();
				list_RemoveNode(my->mynode);
				return;
			}
		}

		if ( multiplayer == CLIENT )
		{
			if ( arrowInGround )
			{
				ARROW_STUCK = 2;
			}
			if ( fabs(ARROW_VELX) < 0.01 && fabs(ARROW_VELY) < 0.01 )
			{
				++ARROW_STUCK_CLIENT;
			}
			if ( ARROW_STUCK_CLIENT > 5 )
			{
				// sometimes the arrow may have 0 velocity during normal movement updates from the server.
				// let it hit 5 iterations before the client assumes it's stuck so z velocity updates no longer happen
				ARROW_STUCK = 1;
			}
			return;
		}

		// damage monsters
		if ( arrowSpawnedInsideEntity || dist != sqrt(ARROW_VELX * ARROW_VELX + ARROW_VELY * ARROW_VELY) || arrowInGround )
		{
			if ( arrowInGround )
			{
				ARROW_STUCK = 2;
			}
			else
			{
				ARROW_STUCK = 1;
			}
			serverUpdateEntitySkill(my, 0);
			my->x = ARROW_OLDX;
			my->y = ARROW_OLDY;

			if ( arrowSpawnedInsideEntity )
			{
				hit.entity = arrowSpawnedInsideEntity;
			}
			Entity* oentity = hit.entity;
			lineTrace(my, my->x, my->y, my->yaw, sqrt(ARROW_VELX * ARROW_VELX + ARROW_VELY * ARROW_VELY), 0, false);
			hit.entity = oentity;
			my->x = hit.x - cos(my->yaw);
			my->y = hit.y - sin(my->yaw);
			ARROW_VELX = 0;
			ARROW_VELY = 0;
			ARROW_VELZ = 0;
			my->entityCheckIfTriggeredBomb(true);
			if ( hit.entity != NULL )
			{
				Entity* parent = uidToEntity(my->parent);
				Stat* hitstats = hit.entity->getStats();
				playSoundEntity(my, 72 + local_rng.rand() % 3, 64);
				if ( hit.entity->behavior == &actChest || hit.entity->isInertMimic() )
				{
					playSoundEntity(hit.entity, 66, 64); //*tink*

					if ( parent )
					{
						if ( parent->behavior == &actPlayer )
						{
							messagePlayer(parent->skill[2], MESSAGE_COMBAT_BASIC, Language::get(667));
							messagePlayer(parent->skill[2], MESSAGE_COMBAT_BASIC, Language::get(447));
						}
						if ( hit.entity->behavior == &actMonster )
						{
							if ( hitstats )
							{
								updateEnemyBar(parent, hit.entity, Language::get(675), hitstats->HP, hitstats->MAXHP,
									false, DamageGib::DMG_WEAKEST);
							}
						}
						else
						{
							updateEnemyBar(parent, hit.entity, Language::get(675), hit.entity->chestHealth, hit.entity->chestMaxHealth,
								false, DamageGib::DMG_WEAKEST);
						}
					}
				}
				else if ( hitstats != NULL && hit.entity != parent )
				{
					if ( !(svFlags & SV_FLAG_FRIENDLYFIRE) )
					{
						// test for friendly fire
						if ( parent && parent->checkFriend(hit.entity) )
						{
							my->removeLightField();
							list_RemoveNode(my->mynode);
							return;
						}
					}

					bool silverDamage = false;
					bool huntingDamage = false;
					if ( my->arrowQuiverType == QUIVER_SILVER )
					{
						switch ( hitstats->type )
						{
							case SKELETON:
							case CREATURE_IMP:
							case GHOUL:
							case DEMON:
							case SUCCUBUS:
							case INCUBUS:
							case VAMPIRE:
							case LICH:
							case LICH_ICE:
							case LICH_FIRE:
							case DEVIL:
								// smite these creatures
								silverDamage = true;
								spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 981);
								playSoundEntity(hit.entity, 249, 64);
								break;
							default:
								silverDamage = false;
								break;
						}
					}
					else if ( my->arrowQuiverType == QUIVER_HUNTING )
					{
						switch ( hitstats->type )
						{
							case RAT:
							case SPIDER:
							case SCORPION:
							case SCARAB:
							case SLIME:
							case CREATURE_IMP:
							case DEMON:
							case MINOTAUR:
							case KOBOLD:
							case COCKATRICE:
							case TROLL:
								// more damage to these creatures
								huntingDamage = true;
								for ( int gibs = 0; gibs < 10; ++gibs )
								{
									Entity* gib = spawnGib(hit.entity);
									serverSpawnGibForClient(gib);
								}
								break;
							default:
								huntingDamage = false;
								break;
						}
					}

					int enemyAC = AC(hitstats);

					// do damage
					if ( my->arrowArmorPierce > 0 && enemyAC > 0 )
					{
						if ( my->arrowQuiverType == QUIVER_PIERCE )
						{
							bool oldDefend = hitstats->defending;
							hitstats->defending = false;
							enemyAC = AC(hitstats);
							enemyAC /= 2; // pierce half armor not caring about shield
							hitstats->defending = oldDefend;
						}
						else
						{
							enemyAC /= 2; // pierce half armor.
						}
					}
					else
					{
						// normal damage.
					}

					real_t targetACEffectiveness = Entity::getACEffectiveness(hit.entity, hitstats, hit.entity->behavior == &actPlayer, parent, parent ? parent->getStats() : nullptr);
					int attackAfterReductions = static_cast<int>(std::max(0.0, ((my->arrowPower * targetACEffectiveness - enemyAC))) + (1.0 - targetACEffectiveness) * my->arrowPower);
					int damage = attackAfterReductions;

					bool backstab = false;
					bool flanking = false;
					if ( parent && parent->getStats() )
					{
						Stat* parentStats = parent->getStats();
						if ( parentStats->helmet && parentStats->helmet->type == HAT_HOOD_WHISPERS 
							&& !monsterIsImmobileTurret(hit.entity, hitstats) && !(hitstats->type == MIMIC) )
						{
							real_t hitAngle = hit.entity->yawDifferenceFromEntity(my);
							if ( (hitAngle >= 0 && hitAngle <= 2 * PI / 3) ) // 120 degree arc
							{
								int stealthCapstoneBonus = 1;
								if ( parent->skillCapstoneUnlockedEntity(PRO_STEALTH) )
								{
									stealthCapstoneBonus = 2;
								}

								real_t equipmentModifier = 0.0;
								real_t bonusModifier = 1.0;
								if ( parentStats->helmet && parentStats->helmet->type == HAT_HOOD_WHISPERS )
								{
									if ( parentStats->helmet->beatitude >= 0 || shouldInvertEquipmentBeatitude(parentStats) )
									{
										equipmentModifier += (std::min(50 + (10 * abs(parentStats->helmet->beatitude)), 100)) / 100.0;
									}
									else
									{
										equipmentModifier = 0.5;
										bonusModifier = 0.5;
									}
								}

								if ( hit.entity->monsterState == MONSTER_STATE_WAIT
									|| hit.entity->monsterState == MONSTER_STATE_PATH
									|| (hit.entity->monsterState == MONSTER_STATE_HUNT && uidToEntity(hit.entity->monsterTarget) == nullptr) )
								{
									// unaware monster, get backstab damage.
									int bonus = (parentStats->getModifiedProficiency(PRO_STEALTH) / 20 + 2) * (2 * stealthCapstoneBonus);
									damage += ((bonus * equipmentModifier) * bonusModifier);
									if ( local_rng.rand() % 10 == 0 && hit.entity->behavior != &actPlayer )
									{
										parent->increaseSkill(PRO_STEALTH);
									}
									backstab = true;
								}
								else if ( local_rng.rand() % 2 == 0 )
								{
									// monster currently engaged in some form of combat maneuver
									// 1 in 2 chance to flank defenses.
									int bonus = (parentStats->getModifiedProficiency(PRO_STEALTH) / 20 + 1) * (stealthCapstoneBonus);
									damage += ((bonus * equipmentModifier) * bonusModifier);
									if ( local_rng.rand() % 20 == 0 && hit.entity->behavior != &actPlayer )
									{
										parent->increaseSkill(PRO_STEALTH);
									}
									flanking = true;
								}
							}
						}
					}

					damage = std::max(0, damage);

					if ( silverDamage || huntingDamage )
					{
						damage *= 1.5;
					}

					if ( parent )
					{
						real_t dist = entityDist(parent, hit.entity); // fskin note: sniper bonus
						if (hitstats->type == LICH
							|| hitstats->type == DEVIL
							|| hitstats->type == LICH_ICE
							|| hitstats->type == LICH_FIRE)
						{
							// bosses excluded
						}
						else
						{
							if (dist > 175)
							{
								damage *= parent->getPER() / 7;
								damage += hitstats->HP / 5;
								damage += dist/5;

								Uint32 color = makeColorRGB(0, 255, 225);
								messagePlayerMonsterEvent(parent->skill[1], color, *hitstats, Language::get(3732), Language::get(3732), MSG_COMBAT);
							}
						}
					}

					// fskin note: cause fear dependent on CHR - specific enemy interactions are hardcoded instead of using enemy CHR for the calculation.
					// this is because giving every enemy on every map and for every tileset of the monstercurve appropriate CHR stats would be incredibly annoying work.
					if (parent && parent->behavior == &actPlayer)
					{
						if (parent->getStats()->weapon)
						{
							if (damage > 0 && parent->getStats()->weapon->type == HEAVY_CROSSBOW)
							{
								if (parent->getStats()->CHR > 0)

									if (((hitstats->type == RAT && hitstats->MAXHP < parent->getStats()->CHR * 9 * (parent->getStats()->getProficiency(PRO_RANGED)/10))
										|| (hitstats->type == SKELETON && hitstats->MAXHP < parent->getStats()->CHR * 9)
										|| (hitstats->type == SPIDER && hitstats->MAXHP < parent->getStats()->CHR * 9)
										|| (hitstats->type == GNOME && hitstats->MAXHP < parent->getStats()->CHR * 9)
										|| (hitstats->type == TROLL && hitstats->MAXHP < parent->getStats()->CHR * 11)
										|| (hitstats->type == GHOUL && hitstats->MAXHP < parent->getStats()->CHR * 8)
										|| (hitstats->type == SLIME && hitstats->MAXHP < parent->getStats()->CHR * 8)
										|| (hitstats->type == GOBLIN && hitstats->MAXHP < parent->getStats()->CHR * 7.5)
										|| (hitstats->type == SCARAB && hitstats->MAXHP < parent->getStats()->CHR * 7)
										|| (hitstats->type == SCORPION && hitstats->MAXHP < parent->getStats()->CHR * 7)
										|| (hitstats->type == INSECTOID && hitstats->MAXHP < parent->getStats()->CHR * 9)
										|| (hitstats->type == CREATURE_IMP && hitstats->MAXHP < parent->getStats()->CHR * 7)
										|| (hitstats->type == DEMON && hitstats->MAXHP < parent->getStats()->CHR * 8)
										|| (hitstats->type == AUTOMATON && hitstats->MAXHP < parent->getStats()->CHR * 7)
										|| (hitstats->type == VAMPIRE && hitstats->MAXHP < parent->getStats()->CHR * 9)
										|| (hitstats->type == GOATMAN && hitstats->MAXHP < parent->getStats()->CHR * 9 )
										|| (hitstats->type == SUCCUBUS && hitstats->MAXHP < parent->getStats()->CHR * 10)
										|| (hitstats->type == INCUBUS && hitstats->MAXHP < parent->getStats()->CHR * 10)
										|| (hitstats->type == KOBOLD && hitstats->MAXHP < parent->getStats()->CHR * 10)
										|| (hitstats->type == COCKATRICE && hitstats->MAXHP < parent->getStats()->CHR * 10 && hitstats->HP < hitstats->MAXHP / 4)
										|| (hitstats->type == CRYSTALGOLEM && hitstats->MAXHP < parent->getStats()->CHR * 6)
										) || (currentlevel > 35 && parent->getStats()->CHR > currentlevel * 1.8)
										|| (currentlevel >= 25 && currentlevel <= 35 && parent->getStats()->CHR > currentlevel * 1.9)
										|| (currentlevel >= 10 && currentlevel <= 24 && parent->getStats()->CHR > currentlevel * 2.5))
									{
										{
											//int duration = parent->getStats()->CHR * 2.6 - ((hitstats->CHR * (hitstats->HP / hitstats->MAXHP)) + damage*0.8);
											int duration = (parent->getStats()->CHR * 2.5) + damage * 0.5;
											if (duration >= 200)
											{
												duration = 200;
											}
											if (parent->getStats()->MAXHP >= hitstats->MAXHP * 1.2)
										 	{
												duration += parent->getStats()->CHR;
											}
											if (backstab)
											{
												duration += parent->getStats()->getProficiency(PRO_STEALTH);
											}

											if (hitstats->HP > 0 && !hitstats->EFFECTS[EFF_FEAR] && !hitstats->EFFECTS[EFF_WITHDRAWAL])
											{
												hit.entity->setEffect(EFF_FEAR, true, duration, true);
												hit.entity->setEffect(EFF_WITHDRAWAL, true, 500, true);
											}
										}
									}
							}
						}
					}

					bool hitWeaklyOnTarget = false;
					int nominalDamage = damage;
					if ( parent )
					{
						if ( my->arrowFallSpeed > 0 )
						{
							if ( my->z >= 5.5 )
							{
								switch ( hitstats->type )
								{
									case RAT:
									case SPIDER:
									case SCORPION:
									case SCARAB:
									case GNOME:
									case KOBOLD:
										// small creatures, no penalty for low shot.
										hitWeaklyOnTarget = false;
										break;
									default:
										hitWeaklyOnTarget = true;
										break;
								}
							}
						}
					}
					real_t damageMultiplier = Entity::getDamageTableMultiplier(hit.entity, *hitstats, DAMAGE_TABLE_RANGED);
					if ( huntingDamage || silverDamage )
					{
						damageMultiplier = std::max(0.75, damageMultiplier);
					}

					if ( hitWeaklyOnTarget )
					{
						damage = damage * (std::max(0.1, damageMultiplier - 0.5));
					}
					else
					{
						damage *= damageMultiplier;
					}

					int trapResist = 0;
					if ( parent )
					{
						if ( parent->behavior == &actArrowTrap )
						{
							trapResist = hit.entity->getFollowerBonusTrapResist();
							if ( trapResist != 0 )
							{
								real_t mult = std::max(0.0, 1.0 - (trapResist / 100.0));
								damage *= mult;
							}
						}
					}

					/*messagePlayer(0, "My damage: %d, AC: %d, Pierce: %d", my->arrowPower, AC(hitstats), my->arrowArmorPierce);
					messagePlayer(0, "Resolved to %d damage.", damage);*/
					hit.entity->modHP(-damage);
					// write obituary
					if ( parent )
					{
						if ( parent->behavior == &actArrowTrap )
						{
							hit.entity->setObituary(Language::get(1503));
							hitstats->killer = KilledBy::TRAP_ARROW;
						}
						else
						{
							parent->killedByMonsterObituary(hit.entity);
						}

						if ( hit.entity->behavior == &actMonster && parent->behavior == &actPlayer )
						{
							if ( damage >= 80 && hitstats->type != HUMAN && !parent->checkFriend(hit.entity) )
							{
								achievementObserver.awardAchievement(parent->skill[2], AchievementObserver::BARONY_ACH_FELL_BEAST);
							}
							if ( my->arrowQuiverType == QUIVER_LIGHTWEIGHT
								&& my->arrowShotByWeapon == COMPOUND_BOW )
							{
								achievementObserver.updatePlayerAchievement(parent->skill[2], AchievementObserver::BARONY_ACH_STRUNG_OUT, AchievementObserver::ACH_EVENT_NONE);
							}
						}
					}

					if ( damage > 0 )
					{
						Entity* gib = spawnGib(hit.entity);
						serverSpawnGibForClient(gib);
						playSoundEntity(hit.entity, 28, 64);
						if ( hit.entity->behavior == &actPlayer )
						{
							if ( players[hit.entity->skill[2]]->isLocalPlayer() )
							{
								cameravars[hit.entity->skill[2]].shakex += .1;
								cameravars[hit.entity->skill[2]].shakey += 10;
							}
							else
							{
								if ( hit.entity->skill[2] > 0 )
								{
									strcpy((char*)net_packet->data, "SHAK");
									net_packet->data[4] = 10; // turns into .1
									net_packet->data[5] = 10;
									net_packet->address.host = net_clients[hit.entity->skill[2] - 1].host;
									net_packet->address.port = net_clients[hit.entity->skill[2] - 1].port;
									net_packet->len = 6;
									sendPacketSafe(net_sock, -1, net_packet, hit.entity->skill[2] - 1);
								}
							}
						}

						bool doSkillIncrease = true;
						if ( monsterIsImmobileTurret(hit.entity, hitstats) )
						{
							if ( hitstats->type == DUMMYBOT && hitstats->HP > 0 )
							{
								doSkillIncrease = true; // can train on dummybots.
							}
							else
							{
								doSkillIncrease = false; // no skill for killing/hurting other turrets.
							}
						}
						if ( hit.entity->behavior == &actPlayer && parent && parent->behavior == &actPlayer )
						{
							doSkillIncrease = false; // no skill for killing/hurting players
						}
						int chance = 35; // fskin note: increased from 10
						if ( doSkillIncrease && (local_rng.rand() % chance == 0) && parent && parent->getStats() )
						{
							if ( hitstats->type != DUMMYBOT 
								|| (hitstats->type == DUMMYBOT && parent->getStats()->getProficiency(PRO_RANGED) < SKILL_LEVEL_BASIC) )
							{
								parent->increaseSkill(PRO_RANGED);
							}
						}
					}
					else
					{
						// playSoundEntity(hit.entity, 66, 64); //*tink*
						if ( hit.entity->behavior == &actPlayer )
						{
							if ( players[hit.entity->skill[2]]->isLocalPlayer() )
							{
								cameravars[hit.entity->skill[2]].shakex += .05;
								cameravars[hit.entity->skill[2]].shakey += 5;
							}
							else
							{
								if ( hit.entity->skill[2] > 0 )
								{
									strcpy((char*)net_packet->data, "SHAK");
									net_packet->data[4] = 5; // turns into .05
									net_packet->data[5] = 5;
									net_packet->address.host = net_clients[hit.entity->skill[2] - 1].host;
									net_packet->address.port = net_clients[hit.entity->skill[2] - 1].port;
									net_packet->len = 6;
									sendPacketSafe(net_sock, -1, net_packet, hit.entity->skill[2] - 1);
								}
							}
						}
					}

					if ( hitstats->HP <= 0 && parent)
					{
						parent->awardXP( hit.entity, true, true );
						spawnBloodVialOnMonsterDeath(hit.entity, hitstats, parent);
					}

					// alert the monster
					if ( hit.entity->behavior == &actMonster && parent != nullptr && currentlevel != 42 && currentlevel != 43) //fskin note: hack to make monsters not beef on the arena level
					{
						bool alertTarget = true;
						if ( parent->behavior == &actMonster && parent->monsterAllyIndex != -1 )
						{
							if ( hit.entity->behavior == &actMonster && hit.entity->monsterAllyIndex != -1 )
							{
								// if a player ally + hit another ally, don't aggro back
								alertTarget = false;
							}
						}

						if ( alertTarget && hit.entity->monsterState != MONSTER_STATE_ATTACK && (hitstats->type < LICH || hitstats->type >= SHOPKEEPER) )
						{
							hit.entity->monsterAcquireAttackTarget(*parent, MONSTER_STATE_PATH, true);
						}

						bool alertAllies = true;
						if ( parent->behavior == &actPlayer || parent->monsterAllyIndex != -1 )
						{
							if ( hit.entity->behavior == &actPlayer || (hit.entity->behavior == &actMonster && hit.entity->monsterAllyIndex != -1) )
							{
								// if a player ally + hit another ally or player, don't alert other allies.
								alertAllies = false;
							}
						}

						// alert other monsters too
						if ( alertAllies )
						{
							hit.entity->alertAlliesOnBeingHit(parent);
						}
						hit.entity->updateEntityOnHit(parent, alertTarget);
						if ( parent->behavior == &actPlayer )
						{
							Uint32 color = makeColorRGB(0, 255, 0);
							if ( huntingDamage )
							{
								// plunges into the %s!
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(3750), Language::get(3751), MSG_COMBAT);
							}
							else if ( silverDamage )
							{
								// smites the %s!
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(3743), Language::get(3744), MSG_COMBAT);
							}
							else if ( backstab && hitstats->HP > 0 )
							{
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(6104), Language::get(6105), MSG_COMBAT);
							}
							else if ( damage <= (nominalDamage * .7) && hitstats->HP > 0 )
							{
								// weak shot.
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(3733), Language::get(3734), MSG_COMBAT);
							}
							else
							{
								// you shot the %s!
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(446), Language::get(448), MSG_COMBAT_BASIC);
							}
							if ( my->arrowArmorPierce > 0 && AC(hitstats) > 0 )
							{
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(2513), Language::get(2514), MSG_COMBAT);
							}
						}
					}
					if ( hit.entity->behavior == &actPlayer )
					{
						Uint32 color = makeColorRGB(255, 0, 0);
						if ( silverDamage )
						{
							messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT, color, Language::get(3745)); // you are smited!
						}
						else if ( huntingDamage )
						{
							messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT, color, Language::get(3752)); // arrow plunged into you!
						}
						else if ( my->arrowQuiverType == QUIVER_KNOCKBACK )
						{
							// no "hit by arrow!" message, let the knockback do the work.
						}
						else if ( my->arrowQuiverType == QUIVER_HUNTING && !(hitstats->amulet && hitstats->amulet->type == AMULET_POISONRESISTANCE)
							&& !(hitstats->type == INSECTOID) )
						{
							// no "hit by arrow!" message, let the hunting arrow effect do the work.
						}
						else
						{
							if ( my )
							{
								if ( my->sprite == PROJECTILE_ROCK_SPRITE )
								{
									// rock.
									messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT_BASIC, color, Language::get(2512));
								}
								else if (my->sprite == PROJECTILE_BOLT_SPRITE )
								{
									// bolt.
									messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT_BASIC, color, Language::get(2511));
								}
								else
								{
									// arrow.
									messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT_BASIC, color, Language::get(451));
								}
							}
							else
							{
								messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT_BASIC, color, Language::get(451));
							}
						}

						if ( my->arrowArmorPierce > 0 && AC(hitstats) > 0 )
						{
							messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT, color, Language::get(2515));
						}
					}

					bool statusEffectApplied = false;
					if (hitstats->HP > 0)
					{
						bool procEffect = true;
						if (trapResist > 0)
						{
							if (local_rng.rand() % 100 < trapResist)
							{
								procEffect = false;
							}
						}
						if (my->arrowQuiverType == QUIVER_FIRE && procEffect)
						{
							bool burning = hit.entity->flags[BURNING];
							hit.entity->SetEntityOnFire(my);
							if (hitstats)
							{
								hitstats->burningInflictedBy = static_cast<Sint32>(my->parent);
							}
							if (!burning && hit.entity->flags[BURNING])
							{
								if (parent && parent->behavior == &actPlayer)
								{
									Uint32 color = makeColorRGB(0, 255, 0);
									if (hitstats)
									{
										messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(3739), Language::get(3740), MSG_COMBAT);
										if (hit.entity->behavior == &actMonster)
										{
											achievementObserver.addEntityAchievementTimer(hit.entity, AchievementObserver::BARONY_ACH_PLEASE_HOLD, 150, true, 0);
										}
									}
								}
								if (hit.entity->behavior == &actPlayer)
								{
									Uint32 color = makeColorRGB(255, 0, 0);
									messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT, color, Language::get(3741));
								}
								statusEffectApplied = true;
							}
						}

						else if ( my->arrowQuiverType == QUIVER_ARCANE && procEffect) //fskin note: arcane arrow
						{
							hitstats->EFFECTS[EFF_SHADOW_TAGGED] = true;
							hitstats->EFFECTS_TIMERS[EFF_SHADOW_TAGGED] = 1500;
						}

						else if ( my->arrowQuiverType == QUIVER_KNOCKBACK && procEffect && hit.entity->setEffect(EFF_KNOCKBACK, true, 30, false) )
						{
							real_t pushbackMultiplier = 0.6;
							if ( !hit.entity->isMobile() )
							{
								pushbackMultiplier += 0.3;
							}
							/*if ( hitWeaklyOnTarget )
							{
								pushbackMultiplier -= 0.3;
							}*/

							if ( hit.entity->behavior == &actMonster )
							{
								if ( parent )
								{
									real_t tangent = atan2(hit.entity->y - parent->y, hit.entity->x - parent->x);
									hit.entity->vel_x = cos(tangent) * pushbackMultiplier;
									hit.entity->vel_y = sin(tangent) * pushbackMultiplier;
									hit.entity->monsterKnockbackVelocity = 0.01;
									hit.entity->monsterKnockbackUID = my->parent;
									hit.entity->monsterKnockbackTangentDir = tangent;
									//hit.entity->lookAtEntity(*parent);
								}
								else
								{
									real_t tangent = atan2(hit.entity->y - my->y, hit.entity->x - my->x);
									hit.entity->vel_x = cos(tangent) * pushbackMultiplier;
									hit.entity->vel_y = sin(tangent) * pushbackMultiplier;
									hit.entity->monsterKnockbackVelocity = 0.01;
									hit.entity->monsterKnockbackTangentDir = tangent;
									//hit.entity->lookAtEntity(*my);
								}
							}
							else if ( hit.entity->behavior == &actPlayer )
							{
								if ( parent )
								{
									real_t dist = entityDist(parent, hit.entity);
									if ( dist < TOUCHRANGE )
									{
										pushbackMultiplier += 0.5;
									}
								}
								if ( !players[hit.entity->skill[2]]->isLocalPlayer() )
								{
									hit.entity->monsterKnockbackVelocity = pushbackMultiplier;
									hit.entity->monsterKnockbackTangentDir = my->yaw;
									serverUpdateEntityFSkill(hit.entity, 11);
									serverUpdateEntityFSkill(hit.entity, 9);
								}
								else
								{
									hit.entity->monsterKnockbackVelocity = pushbackMultiplier;
									hit.entity->monsterKnockbackTangentDir = my->yaw;
								}
							}

							if ( parent && parent->behavior == &actPlayer )
							{
								Uint32 color = makeColorRGB(0, 255, 0);
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(3215), Language::get(3214), MSG_COMBAT);

								if ( hit.entity->behavior == &actMonster )
								{
									achievementObserver.awardAchievementIfActive(parent->skill[2], hit.entity, AchievementObserver::BARONY_ACH_PLEASE_HOLD);
								}
							}
							if ( hit.entity->behavior == &actPlayer )
							{
								Uint32 color = makeColorRGB(255, 0, 0);
								messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT, color, Language::get(3742));
							}

							if ( hit.entity->monsterAttack == 0 )
							{
								hit.entity->monsterHitTime = std::max(HITRATE - 12, hit.entity->monsterHitTime);
							}
							statusEffectApplied = true;
						}
						else if ( my->arrowQuiverType == QUIVER_HUNTING && !(hitstats->amulet && hitstats->amulet->type == AMULET_POISONRESISTANCE)
							&& !(hitstats->type == INSECTOID) && procEffect )
						{
							if ( !hitstats->EFFECTS[EFF_POISONED] )
							{
								hitstats->poisonKiller = my->parent;
								hitstats->EFFECTS[EFF_POISONED] = true;
								hitstats->EFFECTS[EFF_SLOW] = true;
								if ( my->arrowPoisonTime > 0 )
								{
									hitstats->EFFECTS_TIMERS[EFF_POISONED] = my->arrowPoisonTime;
									hitstats->EFFECTS_TIMERS[EFF_SLOW] = my->arrowPoisonTime;
								}
								else
								{
									hitstats->EFFECTS_TIMERS[EFF_POISONED] = 160;
									hitstats->EFFECTS_TIMERS[EFF_SLOW] = 160;
								}
								statusEffectApplied = true;
							}
							if ( statusEffectApplied )
							{
								serverUpdateEffects(hit.entity->skill[2]);
								if ( parent && parent->behavior == &actPlayer )
								{
									Uint32 color = makeColorRGB(0, 255, 0);
									messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(3747), Language::get(3748), MSG_COMBAT);

									achievementObserver.addEntityAchievementTimer(hit.entity, AchievementObserver::BARONY_ACH_PLEASE_HOLD, 150, true, 0);
								}
								if ( hit.entity->behavior == &actPlayer )
								{
									if ( local_rng.rand() % 8 == 0 && hit.entity->char_gonnavomit == 0 && !hitstats->EFFECTS[EFF_VOMITING] )
									{
										// maybe vomit
										messagePlayer(hit.entity->skill[2], MESSAGE_STATUS, Language::get(634));
										if ( hit.entity->entityCanVomit() )
										{
											hit.entity->char_gonnavomit = 140 + local_rng.rand() % 60;
										}
									}
									Uint32 color = makeColorRGB(255, 0, 0);
									messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT, color, Language::get(3749));
								}
							}
							else if ( hit.entity->behavior == &actPlayer )
							{
								Uint32 color = makeColorRGB(255, 0, 0);
								messagePlayerColor(hit.entity->skill[2], MESSAGE_COMBAT_BASIC, color, Language::get(451)); // you are hit by an arrow!
							}
						}
					}
					else
					{
						// HP <= 0
						if ( parent && parent->behavior == &actPlayer )
						{
							Uint32 color = makeColorRGB(0, 255, 0);
							if ( backstab )
							{
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(2547), Language::get(2548), MSG_COMBAT);
							}
							else
							{
								messagePlayerMonsterEvent(parent->skill[2], color, *hitstats, Language::get(692), Language::get(697), MSG_COMBAT);
							}
						}
					}

					if ( damage == 0 && !statusEffectApplied )
					{
						playSoundEntity(hit.entity, 66, 64); //*tink*
						if ( hit.entity->behavior == &actPlayer )
						{
							messagePlayer(hit.entity->skill[2], MESSAGE_COMBAT_BASIC, Language::get(452)); // player notified no damage.
						}
						if ( parent && parent->behavior == &actPlayer )
						{
							if ( hitstats->type == HUMAN )
							{
								if ( hitstats->sex )
								{
									messagePlayer(parent->skill[2], MESSAGE_COMBAT_BASIC, Language::get(449));
								}
								else
								{
									messagePlayer(parent->skill[2], MESSAGE_COMBAT_BASIC, Language::get(450));
								}
							}
							else
							{
								messagePlayer(parent->skill[2], MESSAGE_COMBAT_BASIC, Language::get(447));
							}
						}
					}
					else if ( damage == 0 && statusEffectApplied )
					{
						playSoundEntity(hit.entity, 28, 64);
					}

					// update enemy bar for attacker
					DamageGib dmgGib = DMG_DEFAULT;
					if ( damageMultiplier <= 0.75 )
					{
						dmgGib = DMG_WEAKEST;
					}
					else if ( damageMultiplier <= 0.85 )
					{
						dmgGib = DMG_WEAKER;
					}
					else if ( damageMultiplier >= 1.25 )
					{
						dmgGib = DMG_STRONGEST;
					}
					else if ( damageMultiplier >= 1.15 )
					{
						dmgGib = DMG_STRONGER;
					}
					if ( !strcmp(hitstats->name, "") )
					{
						updateEnemyBar(parent, hit.entity, getMonsterLocalizedName(hitstats->type).c_str(), hitstats->HP, hitstats->MAXHP, 
							false, dmgGib);
					}
					else
					{
						updateEnemyBar(parent, hit.entity, hitstats->name, hitstats->HP, hitstats->MAXHP,
							false, dmgGib);
					}

				}
				my->removeLightField();
				list_RemoveNode(my->mynode);
			}
			else if ( my->sprite == PROJECTILE_ROCK_SPRITE )
			{
				my->removeLightField();
				list_RemoveNode(my->mynode); // rocks don't stick to walls...
			}
			else
			{
				playSoundEntity(my, 72 + local_rng.rand() % 3, 64);
			}
		}
	}
}
