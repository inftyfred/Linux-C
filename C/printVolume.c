#include <stdio.h>

typedef struct {
    double radius;
    double height;
} Cylinder;

typedef struct {
    double length;
    double width;
    double height;
} Rectangular;

typedef union {
    Cylinder cylinder;
    Rectangular rectangular;
} Shape;

double calculateCylinderVolume(Cylinder cylinder) {
    return 3.14 * cylinder.radius * cylinder.radius * cylinder.height;
}

double calculateRectangularVolume(Rectangular rectangular) {
    return rectangular.length * rectangular.width * rectangular.height;
}

double calculateShapeVolume(Shape shape, char type) {
    if (type == 'c') {
        ///////Begin///////////
        return calculateCylinderVolume(shape.cylinder);
        ////////End//////////// 
    } else if (type == 'r') {
        ///////Begin///////////
        return calculateRectangularVolume(shape.rectangular);
        ////////End//////////// 
    } else {
        printf("Invalid shape type.\n");
        return -1;
    }
}

int main() {
    Shape shape;
    char type;

    printf("Enter shape type (c for cylinder, r for rectangular): \n");
    scanf(" %c", &type);

    if (type == 'c') {
        printf("Enter radius and height of the cylinder: \n");
        scanf("%lf %lf", &shape.cylinder.radius, &shape.cylinder.height);
    } else if (type == 'r') {
        printf("Enter length, width and height of the rectangular: \n");
        scanf("%lf %lf %lf", &shape.rectangular.length, &shape.rectangular.width, &shape.rectangular.height);
    } else {
        printf("Invalid shape type.\n");
        return 1;
    }

    double volume = calculateShapeVolume(shape, type);

    if (volume != -1) {
        printf("The volume is %.2lf.\n", volume);
    }
    
    return 0;
}
