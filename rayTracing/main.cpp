//
//  main.cpp
//  rayTracing
//
//  Created by zzc on 3/2/20.
//  Copyright © 2020 zzc. All rights reserved.
//

#include <iostream>
#include <fstream>

int main(int argc, const char * argv[]) {
    using namespace std;
    ofstream imgFile("img.ppm");
//    imgFile.open("img.ppm");
    int nx = 200;
    int ny = 100;
    
    imgFile << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            float r = float(i) / float(nx);
            float g = float(j) / float(ny);
            float b = 0.2;
            int ir = int(255.99 * r);
            int ig = int(255.99 * g);
            int ib = int(255.99 * b);
            imgFile << ir << " " << ig << " " << ib << "\n";
        }
        
    }
    
    
    return 0;
}
