#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QSharedPointer>

#include <iostream>
#include <stdexcept>
#include <vector>

using std::function;
using std::bind;

namespace utils
{

//
// основано на
// http://blog.emptycrate.com/node/288
//
//

template<typename ReturnType, typename T1, typename T2>
class Dispatcher
{
public:
    struct UnhandledDispatch : std::runtime_error
    {
        UnhandledDispatch()
            : std::runtime_error("Uknown dispatch for given arguments")
        {
        }
    };

    template<typename sharedI1, typename sharedI2>
    void addHandler(const function<ReturnType (sharedI1, sharedI2)> &h)
    {
        myHandlers.push_back(
                    std::shared_ptr<Handler>(
                        new HandlerSharedImpl<sharedI1, sharedI2>(h)
                        )
                    );
    }

    ReturnType operator()(const T1 &t1, const T2 &t2)
    {
        for (typename std::vector<std::shared_ptr<Handler> >::iterator itr = myHandlers.begin();
             itr != myHandlers.end();
             ++itr)
        {
            try {
                return (*itr)->go(t1, t2);
            } catch (std::bad_cast &) {
                //The current function did not match!
                //keep truckin and try the next one
            }
        }

        throw UnhandledDispatch();
    }

private:
    struct Handler
    {
        virtual ReturnType go(const T1 &, const T2 &) = 0;
    };

    template<typename sharedI1, typename sharedI2>
    struct HandlerSharedImpl : Handler
    {
        typedef typename sharedI1::value_type I1;
        typedef typename sharedI2::value_type I2;
        typedef function<ReturnType (sharedI1, sharedI2)> F;

        HandlerSharedImpl(const F &f)
            : myFunction(f)
        {
        }
        virtual ~HandlerSharedImpl() {}
        virtual ReturnType go(const T1 &t1, const T2 &t2)
        {
            // this only succeeds if the dynamic_cast succeeds. Otherwise bad_cast is thrown
            // and caught by Dispatcher::operator()
            sharedI1 i1 = qSharedPointerDynamicCast<I1>(t1);
            sharedI2 i2 = qSharedPointerDynamicCast<I2>(t2);
            if (!i1 || !i2) throw std::bad_cast();
            return myFunction(i1, i2);
        }

    private:
        F myFunction;
    };

    std::vector<std::shared_ptr<Handler> > myHandlers;
};

}

#endif // DISPATCHER_H
