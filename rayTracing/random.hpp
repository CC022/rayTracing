//
//  random.hpp
//  rayTracing
//
//  Created by zzc on 3/3/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#ifndef random_hpp
#define random_hpp

#include <stdio.h>
inline double randomDouble() {return rand() / (RAND_MAX + 1.0);}
inline float randomFloat() {return rand() / (RAND_MAX + 1.0);}
inline int randomInt(int min, int max) {return static_cast<int>(min + (max-min+1)*randomFloat());}
#endif /* random_hpp */
