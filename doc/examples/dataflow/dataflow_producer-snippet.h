template <typename T>
class Producer {
 public:
  explicit Producer(int seed) : seed_(seed), count_(4) {}
  bool Run(T& x) {
    // produce a new value x
    x = SimpleRand(seed_);
    count_--;
    return count_ >= 0;
  }

 private:
  int seed_;
  int count_;
};
