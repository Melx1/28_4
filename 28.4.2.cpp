#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>

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

struct Train {
    char name;
    unsigned travelTime;
};

std::mutex terminal;

void travel (const Train& train) {
    std::string outMessage;
    std::this_thread::sleep_for (std::chrono::seconds(train.travelTime));

    bool isFirstWaiting = true;
    while (true) {
        if (terminal.try_lock()) {
            outMessage = std::string() + train.name + " arrived\n";
            std::cout << outMessage;
            auto command = inputSomething<std::string> ("");
            while (command != "depart") {
                command = inputSomething<std::string> ("");
            }
            outMessage = std::string() + train.name + " departed\n";
            std::cout << outMessage;
            terminal.unlock();
            break;
        }
        else {
            if (isFirstWaiting) {
                outMessage = std::string() + train.name + " waiting\n";
                std::cout << outMessage;
                isFirstWaiting = false;
            }
        }
    }
}

int main() {
    Train trains[3];
    std::vector<std::thread> threads;

    char name = 'A';
    for (auto& it : trains) {
        it.name = name;
        std::string hint = std::string() + "Enter the travel time for " + name + " train (in seconds): ";
        it.travelTime = inputSomething<unsigned>(hint);
        ++name;
    }

    for (auto& it : trains) {
        threads.emplace_back(travel, it);
    }

    for (auto& it: threads) {
        it.join();
    }



    return 0;
}