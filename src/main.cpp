#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct LifeTimeProbe {
    explicit LifeTimeProbe(std::string name)
        :name_(std::move(name)){
        std::cout << "LifeTimeProbe" << name_<<std::endl;
    }

    ~LifeTimeProbe() {
        std::cout << "~LifeTimeProbe" << name_<<std::endl;
    }

    std::string name_;
};

int main() {
    LifeTimeProbe heap_probe("heap");
    auto probe = std::make_unique<LifeTimeProbe>("dad");
    std::cout << "main is ending" << std::endl;
    return 0;
}