//
//  main.cpp
//  rayTracing
//
//  Created by zzc on 3/2/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "vec3.hpp"

int main(int argc, const char * argv[]) {
    using namespace std;
    ofstream imgFile("img.ppm");
//    imgFile.open("img.ppm");
    int nx = 200;
    int ny = 100;
    
    imgFile << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(float(i) / float(nx), float(j) / float(ny), 0.2);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            imgFile << ir << " " << ig << " " << ib << "\n";
        }
    }
    return 0;
}
