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
    std::this_thread::sleep_for (std::chrono::seconds(train.travelTime));

    bool isFirstWaiting = true;
    while (true) {
        if (terminal.try_lock()) {
            std::cout << train.name << " arrived" << std::endl;
            auto command = inputSomething<std::string> ("");
            while (command != "depart") {
                command = inputSomething<std::string> ("");
            }
            std::cout << train.name << " departed\n" << std::endl;
            terminal.unlock();
            break;
        }
        else {
            if (isFirstWaiting) {
                std::cout << train.name << " waiting" << std::endl;
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

        std::string hint = "Enter the travel time for ";
        hint.push_back(name);
        hint += + " train (in seconds): ";
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