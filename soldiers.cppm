export module soldiers;
import vlk;

namespace soldiers {
  export void foreach(auto fn) {
    fn(3, 1);
    fn(8, 12);
    fn(14, 14);
  }
}
