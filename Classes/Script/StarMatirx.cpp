#include "StarMatrix.h"
#include "../GameObject/DisplayNode.h"
#include "../GameObject/GameObject.h"

#include "LegacyStarMatrix.h"

struct StarMatrix::impl
{
	impl(){};
	~impl(){};

};

StarMatrix::StarMatrix(GameObject *game_object) : Script("StarMatrix", game_object), pimpl(new impl)
{
//	game_object->addComponent<DisplayNode>()->initAs<LegacyStarMatrix>();
}

StarMatrix::~StarMatrix()
{

}
