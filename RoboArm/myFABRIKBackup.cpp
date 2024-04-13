#include <iostream>
#include <cmath>
#include <string>
#include <chrono>

#define pi 3.14159265359

struct vect{float x; float y; float z;};

vect origin = {0,0,0};

struct coord2D{float x; float y;};

#define l1 10
#define l2 10
#define l3 13

#define limit 10

vect p0 = origin;
vect p1 = {0,0,l1};
vect p2 = {0,0,l1+l2};
vect p3 = {0,0,l1+l2+l3};

coord2D P0 = {0,0};
coord2D P1,P2,P3;

void print(std::string str) {
    std::cout << str;
}

void println(std::string str) {
    std::cout << str;
    std::cout << "\n";
}

void printVect(vect v) {
    print("(");
    print(std::to_string(v.x));
    print(",");
    print(std::to_string(v.y));
    print(",");
    print(std::to_string(v.z));
    println(")");
}

vect vectorSub(vect a, vect b) {
    vect r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;

    return(r);
}

float vectLength (vect v) {
    float l = std::sqrt(pow(v.x, 2)+pow(v.y, 2)+pow(v.z,2));
    return(l);
}

vect unitVect (vect v) {
    vect r;
    r.x = v.x/vectLength(v);
    r.y = v.y/vectLength(v);
    r.z = v.z/vectLength(v);
    return(r);
}

vect scalar (vect v, float k) {
    vect r;
    r.x = k*v.x;
    r.y = k*v.y;
    r.z = k*v.z;
    return (r);
}

vect vectSum(vect a, vect b) {
    vect r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return(r);
}

void convertTo2D() {
    P1.x = std::sqrt(pow(p1.x, 2) + pow(p1.y, 2));
    P1.y = p1.z;
    P2.x = std::sqrt(pow(p2.x, 2) + pow(p2.y, 2));
    P2.y = p2.z;
    P3.x = std::sqrt(pow(p3.x, 2) + pow(p3.y, 2));
    P3.y = p3.z;
}

void iterate(vect target) {
    float reach = l1+l2+l3;
    if (reach < vectLength(target)) {
        std::cout << "Not Possible! Coordinate is too far!";
    } else {
        p3 = target;
        p2 = vectSum(scalar(unitVect(vectorSub(p2, p3)),l3),p3);
        p1 = vectSum(scalar(unitVect(vectorSub(p1, p2)),l2),p2);

        p1 = vectSum(scalar(unitVect(vectorSub(p1, p0)),l1), p0);
        p2 = vectSum(scalar(unitVect(vectorSub(p2, p1)),l2), p1);
        p3 = vectSum(scalar(unitVect(vectorSub(p3, p2)),l3), p2);
    }
}

void printOutputs(float a, float b, float c, float p) {
    println("Here Are The Results");
    std::cout << ("p0 : (" + std::to_string(p0.x) + "," + std::to_string(p0.y) + "," + std::to_string(p0.z) + ")\n");
    std::cout << ("p1 : (" + std::to_string(p1.x) + "," + std::to_string(p1.y) + "," + std::to_string(p1.z) + ")\n");
    std::cout << ("p2 : (" + std::to_string(p2.x) + "," + std::to_string(p2.y) + "," + std::to_string(p2.z) + ")\n");
    std::cout << ("p3 : (" + std::to_string(p3.x) + "," + std::to_string(p3.y) + "," + std::to_string(p3.z) + ")\n");

    print("a = ");
    print(std::to_string(a));
    print(", b = ");
    print(std::to_string(b));
    print(", c = ");
    print(std::to_string(c));
    print(", p = ");
    println(std::to_string(p));
}

float getA() {
    return(float(atan(P1.y/P1.x)));
}

float getB() {
    return(float(acos((pow(P2.x, 2) + pow(P2.y, 2) - pow(l1, 2) - pow(l2, 2))/(-2 * l1 * l2))));
}

float getC() {
    return(acos((pow(std::sqrt(pow(P3.x-P1.x, 2) + pow(P3.y-P1.y, 2)), 2) + -1*(pow((P2.x-P1.x), 2) + pow((P2.y-P1.y), 2) + pow((P3.x-P2.x), 2) + pow((P3.y-P2.y), 2)))/(-2*std::sqrt(pow((P2.x-P1.x), 2) + pow((P2.y-P1.y), 2))*std::sqrt(pow((P3.x-P2.x), 2) + pow((P3.y-P2.y), 2)))));
    //Well that's a magic formula if there ever was one...
    //This is acutally just cosine law but we don't actually know any of the sidelengths yet as they are stored in vectors
}

float getFinalP(vect v) {
    return(atan(v.y/v.x));
}

void interp(vect a, vect b, vect path[], int size) {
    //path[] must exist locally where function is called
    path[0] = a;
    path[size - 1] = b;
    //Set first/final coordinates to start/end point

    vect r = vectorSub(b, a);
    vect unit = unitVect(r);
    //Get the direction

    float scale = vectLength(r) / vectLength(unit);
    float multiplier = scale / size;
    //Find multiplier of unit vector to evenly fill line

    for(int t = 1; t < size - 1; t++) {
        path[t] = vectSum(scalar(unit, t*multiplier), a);
        //Populate the path with waypoints
    }
}

int main() {
    println("Welcome to FABRIK solver");

    vect start = {0, 0, 30};
    vect finish = {15, 15, 15};
    int pathLength = 100;
    vect path[pathLength];

    println("----------------------------------------");

    interp(start, finish, path, pathLength);

    println("----------------------------------------");

    for (int i = 0; i < pathLength; i++) {
        printVect(path[i]);
    }

    println("----------------------------------------");

    for (int n = 0; n < pathLength; n++) {
        println("----------------------------------------");
        for(int i = 0; i < limit; i++) {
            iterate(path[n]);
        }
        convertTo2D();
        float a = getA();
        float b = getB();
        float c = getC();
        float p = getFinalP(finish)/pathLength*n;

        float testY = sin(a) - sin(a + b) + sin(a + b + c);
        if (path[n].z - 1 > testY || path[n].z + 1 < testY) {
            println("Ambiguous Case");
            b = b > pi ? pi - abs(b - pi) : pi + abs(b - pi);
        }

        printOutputs(a,b,c,p);
    }

    return(0);
}