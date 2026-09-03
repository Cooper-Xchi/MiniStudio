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
    auto heap_probe = std::make_unique<LifeTimeProbe>("heap");
    LifeTimeProbe* lt_probe = heap_probe.get();
    std::unique_ptr<LifeTimeProbe> moved_probe = std::move(heap_probe);
    std::cout << heap_probe.get()<<'\n'<<moved_probe.get()<<'\n'<<lt_probe<<'\n';
    std::cout << "main is ending" << std::endl;
    return 0;
}
