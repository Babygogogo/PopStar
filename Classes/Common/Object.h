#ifndef __OBJECT__
#define __OBJECT__

#include <string>

/*!
 * \class Object
 *
 * \brief Base class of all other class of the game.
 *
 * \details
 *	
 * \author Babygogogo
 * \date 2015.4
 */
class Object
{
public:
	virtual ~Object(){};

	const std::string & getName() const{
		return m_name;
	}

	int getInstanceId() const{
		return m_instance_id;
	}

protected:
	Object(std::string &&name) :m_name(std::move(name)), m_instance_id(instance_id_counter++){};

	const std::string m_name;
	const int m_instance_id;

private:
	static int instance_id_counter;
};

#endif // !__OBJECT__
