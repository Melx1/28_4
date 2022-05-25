#include <iostream>
#include <queue>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>


std::mutex cookingMtx;
std::mutex deliveryMtx;
std::mutex orderCounterMtx;

int completedOrderCounter = 0;
const int orderLimit = 10;
enum Dish {pizza, soup, steak, salad, sushi};

template<typename Type = int>
Type timeSpan () {
    using namespace std::chrono;
    static steady_clock::time_point startTime = std::chrono::steady_clock::now();
    steady_clock::time_point nowTime = steady_clock::now();
    duration<Type> timeSpan = duration_cast<duration<Type>>(nowTime - startTime);
    return timeSpan.count();
}

std::string dishName (Dish& dish) {
    switch (dish) {
        case pizza: return "pizza";
        case soup:  return "soup";
        case steak: return "steak";
        case salad: return "salad";
        case sushi: return "sushi";
    }
}

template<typename Type = int>
Type myRand (Type min = 0, Type max = std::numeric_limits<Type>::max()) {
    //range [min, max]
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 randGenerator(seed);
    std::uniform_int_distribution<Type> distribution(min, max);
    return distribution(randGenerator);
}

void order (std::queue<Dish>& cooking) {
    while (completedOrderCounter < orderLimit) {
        int minTimeOrdering = 5; // 5 default
        int maxTimeOrdering = 10; // 10 default
        std::this_thread::sleep_for(std::chrono::seconds(myRand(minTimeOrdering, maxTimeOrdering)));
        Dish newDish = Dish(myRand(0, 4));
        cookingMtx.lock();
            cooking.push(newDish);
        cookingMtx.unlock();


        std::string message = std::to_string(timeSpan()) + "\t" + dishName(newDish) + " is ordered\n";
        std::cout << message;
    }
}

void cook (std::queue<Dish>& cooking, std::queue<Dish>& delivery) {
    while (completedOrderCounter < orderLimit) {
        if (cooking.empty()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        cookingMtx.lock();
            Dish cookingDish = cooking.front();
            cooking.pop();
        cookingMtx.unlock();
        int minTimeCook = 5; // 5 default
        int maxTimeCook = 15; // 15 default
        std::this_thread::sleep_for(std::chrono::seconds(myRand(minTimeCook, maxTimeCook)));
        deliveryMtx.lock();
            delivery.push(cookingDish);
        deliveryMtx.unlock();

        std::string message = std::to_string(timeSpan()) + "\t\t\t" + dishName(cookingDish) + " is cooked\n";
        std::cout << message;
    }
}

void deliver (std::queue<Dish>& delivery) {
    while (completedOrderCounter < orderLimit) {
        if (delivery.empty()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        deliveryMtx.lock();
            Dish deliveringDish = delivery.front();
            delivery.pop();
        deliveryMtx.unlock();
        int timeDeliver = 30; //30 default
        std::this_thread::sleep_for(std::chrono::seconds(timeDeliver));
        orderCounterMtx.lock();
            ++completedOrderCounter;
        orderCounterMtx.unlock();

        std::string message = std::to_string(timeSpan()) + "\t\t\t\t\t" + std::to_string(completedOrderCounter)
                + " " + dishName(deliveringDish) + " is delivered\n";
        std::cout << message;
    }
}

int main () {
    std::queue<Dish> cooking;
    std::queue<Dish> delivery;

    timeSpan();
    std::cout << "Restaurant open" << std::endl;
    std::thread waiter (order, std::ref(cooking));
    std::thread kitchener (cook, std::ref(cooking), std::ref(delivery));
    std::thread deliveryman (deliver, std::ref(delivery));
    waiter.join();
    kitchener.join();
    deliveryman.join();

    return 0;
}