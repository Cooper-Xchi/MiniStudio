#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct LifeTimeProbe {
    explicit LifeTimeProbe(std::string name)
        :name_(std::move(name)){
        std::cout << "construct: " << name_<<'\n';
    }

    ~LifeTimeProbe() {
        std::cout << "destroy: " << name_<<'\n';
    }

    std::string name_;
};

int main() {
    LifeTimeProbe stack_probe("stack");
    auto heap_probe = std::make_unique<LifeTimeProbe>("heap");
    std::cout << "main is ending" << std::endl;
    return 0;
}
