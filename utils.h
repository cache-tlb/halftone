#pragma once
#include <cstdlib>

template <typename T>
inline T clamp(const T& value, const T& low, const T& high) {
    return value < low ? low : (value > high ? high : value);
}

template<typename T>
inline int round(T d) {
    return floor(d + 0.5);
}

class LBDebug {
public :
    LBDebug(){}
    LBDebug(const LBDebug &){}
    ~LBDebug(){
        std::cout << oss.str() << std::endl;
    }

    template<typename T>
    inline LBDebug & operator << ( const T & rhs) {
        oss << rhs ;
        oss << ' ' ;
        return (* this) ;
    }
private :
    std::ostringstream oss;
};

inline LBDebug info() {
    return LBDebug();
}

inline float rand_float() {
    return float(rand()) / float(RAND_MAX);
}
