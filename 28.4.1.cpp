#include <iostream>
#include <thread>
#include <vector>
#include <string>

const float finishDistance = 100; // measured in meters
const float checkPeriod = 1;      // measured in seconds

template<typename Type>
Type inputSomething (const std::string & previewText) {
    Type result;
    while (true) {
        std::cin.rdbuf()->pubsync();
        std::cin.clear();
        std::cout << previewText;
        std::cin >> result;
        if (!std::cin.fail()) {
            break;
        }
    }
    return result;
}

class Swimmer {
    std::string name = "Noname";
    float speed = 0;    // m/s
    float distance = 0; // m
    float finishTime = 0;  // m

public:
    Swimmer () {
        name = inputSomething<std::string>("Enter swimmer's name: ");
        while (speed <= 0) speed = inputSomething<float>("Enter swimmer's speed m/s: ");
    }

    void swim (float swimmerTimePeriod, float swimmerFinishDistance) {
        std::string outString;
        while (distance < swimmerFinishDistance) {
            distance += speed * swimmerTimePeriod;
            outString = name + " swam: " + std::to_string(distance) + "\n";
            std::cout << outString;

            std::this_thread::yield(); //slows down threads to ensure a roughly normal distribution
            //std::this_thread::sleep_for(std::chrono::milliseconds (1)); //stopped threads for 1 millisecond to ensure normal distribution
        }
        distance = swimmerFinishDistance;
        finishTime = swimmerFinishDistance / speed;
        outString = name + " finish time: " + std::to_string(finishTime) + "\n";
        std::cout << outString;
    }

    float get_finishTime () const {
        return finishTime;
    }

    std::string get_name () {
        return name;
    }

    friend void swap(Swimmer& first, Swimmer& second);
};

void swap (Swimmer& first, Swimmer& second) {
    std::swap(first.name, second.name);
    std::swap(first.speed, second.speed);
    std::swap(first.distance, second.distance);
    std::swap(first.finishTime, second.finishTime);
}

void insertionSort(std::vector<Swimmer>& swimmers) {
    Swimmer* leftPtr = &swimmers.front();
    Swimmer* rightPtr = &swimmers.back();
    for (Swimmer* swimmerPtr = leftPtr + 1; swimmerPtr <= rightPtr; swimmerPtr++) {
        Swimmer* currentPtr = swimmerPtr;
        while (currentPtr > leftPtr && (currentPtr - 1)->get_finishTime() > currentPtr->get_finishTime()) {
            swap(*(currentPtr - 1), *currentPtr);
            currentPtr--;
        }
    }
}

int main() {
    const int numberSwimmers = 6;
    std::vector<Swimmer> swimmers(numberSwimmers);
    std::vector<std::thread> threads;


    std::cout << std::endl;
    threads.reserve(swimmers.size());
    for (auto & swimmer : swimmers) {
        threads.emplace_back(&Swimmer::swim, std::ref(swimmer), checkPeriod, finishDistance);
    }
    for (auto& it : threads ) it.join();


    std::cout << std::endl;
    insertionSort(swimmers);
    for (auto& swimmer : swimmers) {
        std::cout << swimmer.get_name() << ", result time " << swimmer.get_finishTime() << std::endl;
    }

    return 0;
}