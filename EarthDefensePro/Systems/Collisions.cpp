
#include "Collisions.h"

using namespace ESG;

bool ESG::Collisions::Init(std::shared_ptr<flecs::world> _game, std::weak_ptr<const GameConfig> _gameConfig)
{
	game = _game;
	gameConfig = _gameConfig;

	struct CollisionSystem {};
	withPlayerQ = game->query<CollideWithPlayer, ESG::ModelBoundry>();
	collidableQ = game->query<ESG::Collidable>();
	enemyQ = game->query<ESG::Enemy, ESG::ModelBoundry, ESG::Collidable>();
	bulletQ = game->query<ESG::Bullet, ESG::ModelBoundry>();
	missileQ = game->query<ESG::Missiles, ESG::ModelBoundry>();
	doubleQ = game->query<ESG::Double, ESG::ModelBoundry>();
	beamQ = game->query<ESG::Beams, ESG::ModelBoundry>();
	laserQ = game->query<ESG::Lasers, ESG::ModelBoundry>();
	projectileQ = game->query<ESG::Projectile, ESG::ModelBoundry>();
	game->entity("Collision Entity").add<CollisionSystem>();
	game->system<ESG::Collidable, ESG::ModelTransform>()
		.each([this](flecs::entity en, ESG::Collidable&, ESG::ModelTransform& t) {
		//make query variables

		//check count of possible queries to not throw an error

		//std::cout << game->count<ESG::Collidable>() << std::endl;

		/*std::cout << boundaryCheck->obb.center.x << "   " << boundaryCheck->obb.center.y << "   " << boundaryCheck->obb.center.z << std::endl;

		std::cout << boundaryCheck->obb.data->x << "   " << boundaryCheck->obb.center.y << "   " << boundaryCheck->obb.center.z << std::endl;*/
		/*if (b.name.find("EnemyShip") != b.name.npos) {
			auto m = en.get<ESG::ModelTransform>();
			std::cout << m->matrix.row1.x;
		}*/
		auto boundaryCheck = en.get_mut<ESG::ModelBoundry>();
		boundaryCheck->obb.center.x = t.matrix.row4.x;
		boundaryCheck->obb.center.y = t.matrix.row4.y;
		boundaryCheck->obb.center.z = 0;

		en.modified<ESG::ModelBoundry>();
		//std::cout << "colliable count = " << collidableQ.count() << std::endl;
		projectileQ.each([this](flecs::entity e, ESG::Projectile& p, ESG::ModelBoundry& b)
			{
				if (e.is_alive() && e.get_mut<Renderable>()->value)
				{


					collidableQ.each([&e, &b, &p](flecs::entity e2, ESG::Collidable&)
						{
							bool e2P = e2.has<Player>();
							bool e2E = e2.has<Enemy>();
							bool fr = p.friendly;
							if (e2.get_mut<Renderable>()->value && e2.has<ESG::ModelBoundry>() && ((e2P && !fr) || (e2E && fr)))//&&!e2.has<ESG::Player>() && e != e2 && !e2.has<ESG::Projectile>())
							{
								auto b2 = e2.get_mut<ESG::ModelBoundry>();
								/*std::cout << std::endl;

								std::cout << " Prefab e name is  " << e.get_mut<ESG::prefabName>()->name << std::endl;
								std::cout << std::endl;

								std::cout << " Prefab e2 name is  " << e2.get_mut<ESG::prefabName>()->name << std::endl;*/
								GW::MATH::GCollision::GCollisionCheck check;
								GW::MATH::GCollision::TestOBBToOBBF(b.obb, b2->obb, check);
								if (check == GW::MATH::GCollision::GCollisionCheck::COLLISION)
								{
									if (e2E && !e2.has<ESG::EnemyHit>())
									{
										e2.add<ESG::EnemyHit>();
									}
									else if (e2P && !e2.has<PlayerHit>() && !e2.has<ESG::Invulnerable>()) {
										e2.add<ESG::PlayerHit>();
										e2.set<ESG::Invulnerable>({ 0.1 });
									}
									if (!e.has<ESG::SlatedForDeath>())
									{
										e.add<ESG::SlatedForDeath>();
									}
								}
								check = GW::MATH::GCollision::GCollisionCheck::NO_COLLISION;
							}
						});
				}

			});


		if (game->count<ESG::Collidable>() > 0)
		{

			//	std::cout << game->count<ESG::Collidable>() << std::endl;

				//get player variables
			auto player1 = game->lookup("Spaceship_RaeTheRedPanda");

			auto player2 = game->lookup("Spaceship_RaeTheRedPanda");
			//for player 2 to stuff
			//auto player2 = game->lookup()

			//checks each enemy in game for collision and marks it
			if (game->count<ESG::Enemy>() > 0)
			{
				enemyQ.each([this, &player1, &player2](flecs::entity e, ESG::Enemy&, ESG::ModelBoundry& b, ESG::Collidable&)
					{
						if (e.is_alive() && e.get_mut<Renderable>()->value)
						{
							collidableQ.each([&e, b, &player1, &player2](flecs::entity e2, ESG::Collidable&)
								{
									if (!e2.has<Projectile>() && e != e2 && e2.has<ESG::ModelBoundry>() && !e2.has<ESG::Invulnerable>())
									{
										auto b2 = e2.get_mut<ESG::ModelBoundry>();
										GW::MATH::GCollision::GCollisionCheck check;
										GW::MATH::GCollision::TestOBBToOBBF(b.obb, b2->obb, check);
										if (check == GW::MATH::GCollision::GCollisionCheck::COLLISION)
										{
											//TODO Create invulnerable for player & reduce health
											if ((e2 == player1 || e2 == player2) && !e2.has<PlayerHit>()) {
												e2.add<ESG::PlayerHit>();
												e2.set<ESG::Invulnerable>({ 0.1 });
											}
											else if(!e2.has<Enemy>() && !e2.has<EnemyHit>() && e2.has<Enemy>()) {
												e2.add<EnemyHit>();
											}
											/*else if (!e.has<ESG::SlatedForDeath>()) {
												e.add<ESG::SlatedForDeath>();
											}*/
										}
										check = GW::MATH::GCollision::GCollisionCheck::NO_COLLISION;
									}
								});
						}
					});
			}
			//	//checks each bullet in game for collision and marks it
		//		if (game->count<ESG::Lasers>() > 0)
			//	{
			//		bulletQ.each([this](flecs::entity e, ESG::Bullet&, ESG::ModelBoundry& b)
			//			{
			//				if (e.is_alive())
			//				{
			//					collidableQ.each([&e, &b](flecs::entity e2, ESG::Collidable&)
			//						{
			//							if (e2.has<ESG::ModelBoundry>() && !e2.has<ESG::Player>() && e != e2)
			//							{
			//								auto b2 = e2.get_mut<ESG::ModelBoundry>();
			//								/*std::cout << std::endl;
			//								
			//								std::cout <<" Prefab e name is  " << e.get_mut<ESG::prefabName>()->name << std::endl;
			//								std::cout << std::endl;
			//						
			//								std::cout << " Prefab e2 name is  " << e2.get_mut<ESG::prefabName>()->name << std::endl;*/
			//								GW::MATH::GCollision::GCollisionCheck check;
			//								GW::MATH::GCollision::TestOBBToOBBF(b.obb, b2->obb, check);
			//								if (check == GW::MATH::GCollision::GCollisionCheck::COLLISION )
			//								{
			//									if (!e.has<ESG::SlatedForDeath>())
			//									{
			//										e.add<ESG::SlatedForDeath>();
			//									}
			//								}
			//								check = GW::MATH::GCollision::GCollisionCheck::NO_COLLISION;
			//							}
			//						});
			//				}
			//			});
			//	}
			//	//checks each missile in game and marks it
			//	if (game->count<ESG::Missiles>() > 0)
			//	{
			//		missileQ.each([this](flecs::entity e, ESG::Missiles&, ESG::ModelBoundry& b)
			//			{
			//				if (e.is_alive())
			//				{
			//					collidableQ.each([&e, &b](flecs::entity e2, ESG::Collidable&)
			//						{
			//							if (e2.has<ESG::ModelBoundry>() && !e2.has<ESG::Player>() && e != e2)
			//							{
			//								auto b2 = e2.get_mut<ESG::ModelBoundry>();
			//								/*std::cout << std::endl;

			//								std::cout << " Prefab e name is  " << e.get_mut<ESG::prefabName>()->name << std::endl;
			//								std::cout << std::endl;

			//								std::cout << " Prefab e2 name is  " << e2.get_mut<ESG::prefabName>()->name << std::endl;*/
			//								GW::MATH::GCollision::GCollisionCheck check;
			//								GW::MATH::GCollision::TestOBBToOBBF(b.obb, b2->obb, check);
			//								if (check == GW::MATH::GCollision::GCollisionCheck::COLLISION)
			//								{
			//									if (!e.has<ESG::SlatedForDeath>())
			//									{
			//										e.add<ESG::SlatedForDeath>();
			//									}
			//								}
			//								check = GW::MATH::GCollision::GCollisionCheck::NO_COLLISION;
			//							}
			//						});
			//				}
			//			});
			//	}
			//	if (game->count<ESG::Double>() > 0)
			//	{
			//		doubleQ.each([this](flecs::entity e, ESG::Double&, ESG::ModelBoundry& b)
			//			{
			//				if (e.is_alive())
			//				{
			//					collidableQ.each([&e, &b](flecs::entity e2, ESG::Collidable&)
			//						{
			//							if (e2.has<ESG::ModelBoundry>() && !e2.has<ESG::Player>() && e != e2)
			//							{
			//								auto b2 = e2.get_mut<ESG::ModelBoundry>();
			//								/*std::cout << std::endl;

			//								std::cout <<" Prefab e name is  " << e.get_mut<ESG::prefabName>()->name << std::endl;
			//								std::cout << std::endl;

			//								std::cout << " Prefab e2 name is  " << e2.get_mut<ESG::prefabName>()->name << std::endl;*/
			//								GW::MATH::GCollision::GCollisionCheck check;
			//								GW::MATH::GCollision::TestOBBToOBBF(b.obb, b2->obb, check);
		//									if (check == GW::MATH::GCollision::GCollisionCheck::COLLISION)
			//								{
			//									if (!e.has<ESG::SlatedForDeath>())
			//									{
			//										e.add<ESG::SlatedForDeath>();
			//									}
			//								}
			//								check = GW::MATH::GCollision::GCollisionCheck::NO_COLLISION;
			//							}
			//						});
			//				}
			//			});
			//	}
			//	if (game->count<ESG::Double>() > 0)
			//	{
			//		doubleQ.each([this](flecs::entity e, ESG::Double&, ESG::ModelBoundry& b)
			//			{
			//				if (e.is_alive())
			//				{
			//					collidableQ.each([&e, &b](flecs::entity e2, ESG::Collidable&)
			//						{
			//							if (e2.has<ESG::ModelBoundry>() && !e2.has<ESG::Player>() && e != e2)
			//							{
			//								auto b2 = e2.get_mut<ESG::ModelBoundry>();
			//								/*std::cout << std::endl;

			//								std::cout <<" Prefab e name is  " << e.get_mut<ESG::prefabName>()->name << std::endl;
			//								std::cout << std::endl;

			//								std::cout << " Prefab e2 name is  " << e2.get_mut<ESG::prefabName>()->name << std::endl;*/
			//								GW::MATH::GCollision::GCollisionCheck check;
			//								GW::MATH::GCollision::TestOBBToOBBF(b.obb, b2->obb, check);
			//								if (check == GW::MATH::GCollision::GCollisionCheck::COLLISION)
			//								{
			//									if (!e.has<ESG::SlatedForDeath>())
			//									{
			//										e.add<ESG::SlatedForDeath>();
			//									}
			//								}
			//								check = GW::MATH::GCollision::GCollisionCheck::NO_COLLISION;
			//							}
			//						});
			//				}
			//			});
			//	}
				//checks if player has collided and marks it
			/*if (game->count<CollideWithPlayer>() > 0)
			{
				withPlayerQ.each([&player1, &player1Bound](flecs::entity e, CollideWithPlayer&, ESG::ModelBoundry& b)
					{
						GW::MATH::GCollision::GCollisionCheck check;
						GW::MATH::GCollision::TestOBBToOBBF(player1Bound->obb, b.obb, check);
						if (check == GW::MATH::GCollision::GCollisionCheck::COLLISION)
						{
							if (!player1.has<PlayerHit>())
							{
								player1.add<PlayerHit>();
							}
						}
						check = GW::MATH::GCollision::GCollisionCheck::NO_COLLISION;

					});
			}*/
		}
		//cycles through all slated for death objects and destructs them
		if (game->count<ESG::SlatedForDeath>() > 0)
		{
			flecs::filter<ESG::SlatedForDeath> f = game->filter<ESG::SlatedForDeath>();
			f.each([](flecs::entity e, ESG::SlatedForDeath&)
				{
					
					//std::cout << "Collision Destroyed  " << e.get<ESG::BlenderName>()->name << std::endl;
					e.destruct();
				});
		}

		////do something in player logic to simulate player death.


			});

	return true;
}

bool ESG::Collisions::Shutdown()
{
	game->entity("Master Collision").destruct();
	game->entity("Collision Entity").destruct();
	game.reset();
	gameConfig.reset();
	return true;
}










