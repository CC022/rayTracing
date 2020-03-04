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
#endif /* random_hpp */
