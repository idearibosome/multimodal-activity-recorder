#include "irqmrandomhelper.h"

#include <limits>
#include <random>

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
IRQMRandomHelper::IRQMRandomHelper(QObject *parent) : QObject(parent) {

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int IRQMRandomHelper::intWithinBounds(int lowerBound, int upperBound) {
    if (lowerBound == upperBound) return lowerBound;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(lowerBound, upperBound);

    return dist(gen);
}
//------------------------------------------------------------------------------
int IRQMRandomHelper::positiveInt() {
    return intWithinBounds(0, std::numeric_limits<int>::max());
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
