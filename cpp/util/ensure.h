#ifndef COBALT_GUARD_ENSURE_H_
#define COBALT_GUARD_ENSURE_H_
#include <sstream>
namespace cobalt
{
	#define cobalt_ensure(expression) \
	if (!(expression)) \
	{ \
		std::stringstream ss; \
		ss << "ensure failed " << #expression << " file " << __FILE__ << " line " << __LINE__; \
		throw std::runtime_error(ss.str()); \
	} \

}

#endif /* COBALT_GUARD_ENSURE_H_ */
