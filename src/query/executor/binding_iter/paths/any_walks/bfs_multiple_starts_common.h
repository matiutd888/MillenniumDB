#include <cstdlib>
#include <string>

inline std::string get_starting_label_str() {
  char *ms_start_label_c_str = std::getenv("MS_START_LABEL");
  std::string ret = "start"; 
  if (ms_start_label_c_str != nullptr) {
    ret = ms_start_label_c_str;
  }
  return ret;
}
