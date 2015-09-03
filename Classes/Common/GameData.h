#ifndef __GAME_DATA__
#define __GAME_DATA__

#include <memory>

/*!
 * \class GameData
 *
 * \brief The global game data.
 *
 *	TODO: This class should be replace by a script.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class GameData final
{
public:
	GameData();
	~GameData();

	//This should be removed.
	static std::unique_ptr<GameData> create();

	int getCurrentLevel() const;

	int getTargetScore() const;

	//Disable copy/move constructor and operator=.
	GameData(const GameData&) = delete;
	GameData(GameData&&) = delete;
	GameData& operator=(const GameData&) = delete;
	GameData& operator=(GameData&&) = delete;

private:
	struct GameDataImpl;
	std::shared_ptr<GameDataImpl> pimpl;
};

#endif
