#include <cstdlib>
#include <string>
#include <iostream>

inline std::string get_starting_label_str() {
  char *ms_start_label_c_str = std::getenv("MS_START_LABEL");
  std::string ret = "start"; 
  if (ms_start_label_c_str != nullptr) {
    ret = ms_start_label_c_str;
  }
  return ret;
}

class Counter {
private:
    int count;
    std::string name;

public:
    // Constructor
    Counter(std::string counterName) : count(0), name(counterName) {}

    void reset() {
        count = 0;
    }

    // Method to increment counter
    void increment() {
        count++;
    }

    // Method to get the current count
    int getCount() const {
        return count;
    }

    // Overloaded ostream operator for printing
    friend std::ostream& operator<<(std::ostream& os, const Counter& c) {
        os << "Counter [" << c.name << "]: " << c.count;
        return os;
    }
};
