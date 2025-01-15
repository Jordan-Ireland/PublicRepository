#include "PhysicsLogic.h"
#include "../Components/Physics.h"
#include "../Components/Blender.h"
#include "../Components/Identification.h"
#include "../Components/Gameplay.h"

bool ESG::PhysicsLogic::Init(	std::shared_ptr<flecs::world> _game, 
								std::weak_ptr<const GameConfig> _gameConfig)
{
	// save a handle to the ECS & game settings
	game = _game;
	gameConfig = _gameConfig;
	// **** MOVEMENT ****
	// update velocity by acceleration
	game->system<Velocity, const Acceleration>("Acceleration System")
		.each([](flecs::entity e, Velocity& v, const Acceleration &a) {
		GW::MATH::GVECTORF accel;
		GW::MATH::GVector::ScaleF(a.value, e.delta_time(), accel);
		GW::MATH::GVector::AddVectorF(accel, v.value, v.value);
	});
	// update position by velocity
	game->system<ESG::ModelTransform,ESG::ForwardDirection, const Velocity>("Translation System")
		.each([](flecs::entity e, ESG::ModelTransform& p,ESG::ForwardDirection& f, const Velocity &v) {
	//	GW::MATH::GVECTORF speed;
		GW::MATH::GVECTORF modifiedDirection = v.value;
		

		GW::MATH::GMATRIXF rotationMatrix = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMatrix::RotateYLocalF(rotationMatrix, G_DEGREE_TO_RADIAN_F(f.degrees), rotationMatrix); // 45 degrees as an example
		

		GW::MATH::GMatrix::VectorXMatrixF(rotationMatrix, modifiedDirection, modifiedDirection);
		GW::MATH::GVECTORF worldDirection;
		GW::MATH::GVector::VectorXMatrixF(modifiedDirection, p.matrix, worldDirection);

		// 2. Scale the world direction by delta time
		GW::MATH::GVECTORF speed;
		GW::MATH::GVector::ScaleF(worldDirection, e.delta_time(), speed);

		// 3. Apply the scaled translation
		GW::MATH::GVector::AddVectorF(speed, p.matrix.row4, p.matrix.row4); // Assuming row4 is the position in row-major
		
	});

	//moves shield with player
	game->system<ShieldTag, ModelTransform>()
		.each([_game](flecs::entity e, ShieldTag&, ModelTransform& t) {
		auto player1 = _game->lookup("Spaceship_RaeTheRedPanda");
		auto playerTrans = player1.get<ModelTransform>();
		t.matrix.row4 = playerTrans->matrix.row4;
		e.modified<ModelTransform>();
			});
	//moves options with player
	game->system<Options, ModelTransform>()
		.each([_game](flecs::entity e, Options&, ModelTransform& t) {
		auto player1 = _game->lookup("Spaceship_RaeTheRedPanda");
		auto playerTrans = player1.get<ModelTransform>();
		t.matrix.row4 = playerTrans->matrix.row4;
		e.modified<ModelTransform>();
			});

	// **** CLEANUP ****
	// clean up any objects that end up offscreen
	game->system<const ESG::ModelTransform>("Cleanup System")
		.each([this](flecs::entity e, const ESG::ModelTransform& p) {
		auto player = game->lookup("Spaceship_RaeTheRedPanda");
		auto playerPos = player.get_mut<ESG::ModelTransform>();
		if (/*p.matrix.row4.x > (playerPos->matrix.row4.x + 100.0f) ||*/ p.matrix.row4.x < (playerPos->matrix.row4.x - 25.0f) ||
			p.matrix.row4.y > 40.0f || p.matrix.row4.y < -40.0f) {
			//std::cout << "CleanUp system   "  << std::endl;
				e.destruct();
		}
	});
	// **** COLLISIONS ****
	// due to wanting to loop through all collidables at once, we do this in two steps:
	// 1. A System will gather all collidables into a shared std::vector
	// 2. A second system will run after, testing/resolving all collidables against each other
	queryCache = game->query<Collidable, ESG::ModelTransform>();
	// only happens once per frame at the very start of the frame
	struct CollisionSystem {}; // local definition so we control iteration count (singular)
	game->entity("Detect-Collisions").add<CollisionSystem>();
	game->system<CollisionSystem>()
		.each([this](CollisionSystem& s) {
		// This the base shape all objects use & draw, this might normally be a component collider.(ex:sphere/box)
		constexpr GW::MATH::GVECTORF poly[polysize] = {
			{ -0.5f, -0.5f }, { 0, 0.5f }, { 0.5f, -0.5f }, { 0, -0.25f }
		};
		// collect any and all collidable objects
		queryCache.each([this, poly](flecs::entity e, Collidable& c, ESG::ModelTransform& p) {
			// create a 3x3 matrix for transformation
			GW::MATH::GMATRIXF matrix = p.matrix;
			
			SHAPE polygon; // compute buffer for this objects polygon
			// This is critical, if you want to store an entity handle it must be mutable
			polygon.owner = e; // allows later changes
			for (int i = 0; i < polysize; ++i) {
				GW::MATH::GVECTORF v = { poly[i].x, poly[i].y, 1 };
				GW::MATH::GMatrix::VectorXMatrixF(matrix, v, v);
				polygon.poly[i].x = v.x;
				polygon.poly[i].y = v.y;
			}
			// add to vector
			testCache.push_back(polygon);
		});
		// loop through the testCahe resolving all collisions
		//for (int i = 0; i < testCache.size(); ++i) {
		//	// the inner loop starts at the entity after you so you don't double check collisions
		//	for (int j = i + 1; j < testCache.size(); ++j) {

		//		// test the two world space polygons for collision
		//		// possibly make this cheaper by leaving one of them local and using an inverse matrix
		//		GW::MATH::GCollision::GCollisionCheck result;
		//		GW::MATH2D::GCollision2D::TestPolygonToPolygon2F(
		//			testCache[i].poly, polysize, testCache[j].poly, polysize, result);
		//		if (result == GW::MATH::GCollision::GCollisionCheck2D::COLLISION) {
		//			// Create an ECS relationship between the colliders
		//			// Each system can decide how to respond to this info independently
		//			testCache[j].owner.add<CollidedWith>(testCache[i].owner);
		//			testCache[i].owner.add<CollidedWith>(testCache[j].owner);
		//		}
		//	}
		//}
		// wipe the test cache for the next frame (keeps capacity intact)
		testCache.clear();
	});
	return true;
}

bool ESG::PhysicsLogic::Activate(bool runSystem)
{
	if (runSystem) {
		game->entity("Acceleration System").enable();
		game->entity("Translation System").enable();
		game->entity("Cleanup System").enable();
	}
	else {
		game->entity("Acceleration System").disable();
		game->entity("Translation System").disable();
		game->entity("Cleanup System").disable();
	}
	return true;
}

bool ESG::PhysicsLogic::Shutdown()
{
	queryCache.destruct(); // fixes crash on shutdown
	game->entity("Acceleration System").destruct();
	game->entity("Translation System").destruct();
	game->entity("Cleanup System").destruct();
	return true;
}
