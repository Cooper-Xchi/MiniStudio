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

void observe(const LifeTimeProbe& probe) {
    std::cout << "observe: " << probe.name_<<'\n';
}


void consume(
    std::unique_ptr<LifeTimeProbe> probe,
    bool early_return
    ) {
    std::cout << "consume: " << probe->name_<<'\n';
    if (early_return) {
        return;
    }
}



int main() {
    auto heap_probe = std::make_unique<LifeTimeProbe>("heap");
    observe(*heap_probe);
    if (heap_probe.get() != nullptr) {
        consume(std::move(heap_probe), true);
    }
    std::cout << heap_probe.get()<<'\n';
    std::cout << "main is ending" << std::endl;
    return 0;
}
