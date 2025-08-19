#include <iostream>
#include <map>
#include <string>
#include <cstdint>

class MessageReceiver {
public:
    void receive(uint32_t id, const std::string& text) {
        // insert only if new id (avoid double counting)
        auto [it, inserted] = msgs.emplace(id, text);
        if (inserted) {
            stored++;
            if (!haveBase) {
                base = id;     // first id seen becomes batch base
                haveBase = true;
            }
        }

        if (stored % 5 == 0) {
            printBatch();
            base += 5;
        }
    }

private:
    void printBatch() {
        for (uint32_t i = 0; i < 5; ++i) {
            uint32_t cur = base + i;
            auto it = msgs.find(cur);
            if (it == msgs.end()) {
                std::cout << "[faulty message for ID " << cur << "]\n";
            } else {
                std::cout << "Message ID " << cur << ": " << it->second << "\n";
            }
        }
    }

    std::map<uint32_t, std::string> msgs;
    uint32_t base = 0;
    bool haveBase = false;
    std::size_t stored = 0;
};

int main() {
    MessageReceiver receiver;
    receiver.receive(1, "Hello");
    receiver.receive(2, "World");
    receiver.receive(4, "Missing ID 3");
    receiver.receive(5, "Another");
    receiver.receive(6, "End of batch");
    return 0;
}
