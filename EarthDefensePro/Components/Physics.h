// define all ECS components related to movement & collision
#ifndef PHYSICS_H
#define PHYSICS_H

// example space game (avoid name collisions)
namespace ESG 
{
	// ECS component types should be *strongly* typed for proper queries
	// typedef is tempting but it does not help templates/functions resolve type
	struct Position { GW::MATH::GVECTORF value; };
	struct Velocity { GW::MATH::GVECTORF value; };
	struct Orientation { GW::MATH::GMATRIXF matrix; };
	struct Acceleration { GW::MATH::GVECTORF value; };
	struct ForwardDirection { float degrees; };

	// Individual TAGs
	struct Collidable {}; 
	
	// ECS Relationship tags
	struct CollidedWith {};
};

#endif