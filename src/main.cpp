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
    LifeTimeProbe* borrowed_probe = heap_probe.get();
    std::cout << "probe: " << borrowed_probe->name_<<'\n';
    heap_probe.reset();
    return 0;
}
