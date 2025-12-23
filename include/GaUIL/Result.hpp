#ifndef GAUIL_RESULT_HPP
#define GAUIL_RESULT_HPP
#include <type_traits>
#include <string>
namespace gauil {
    template<typename T, std::enable_if_t<std::is_copy_assignable_v<T>&& std::is_copy_constructible_v<T>, bool> = true>
    class Result {
        T mValue;
        bool mValid;
        const std::string mMsg;
    public:
        T& get(){
            return mValue;
        }
        const T& get() const{
            return mValue;
        }
        const std::string& getMessage() const{
            return mMsg;
        }
        operator bool() const{
            return mValid;
        }
        
        Result() = delete;
        Result(T value, bool valid, const std::string& msg = "") : mValue(value), mValid(valid),mWhat(what){

        }
    };
}


#endif