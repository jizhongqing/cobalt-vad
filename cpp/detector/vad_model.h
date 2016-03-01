#ifndef COBALT_GUARD_MODEL_H_
#define COBALT_GUARD_MODEL_H_

#include <boost/shared_ptr.hpp>
#include <string>
namespace cobalt
{
    class VadModel
    {
    public:
        typedef boost::shared_ptr<VadModel> Ptr;
        VadModel(const std::string& modelpath);

        //TODO: Fill out getters, like getFrontend.
    };
}
#endif
