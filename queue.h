#if !defined(QUEUE_H)
#    define QUEUE_H

template<typename T, size_t const N>
class Queue;

#    include "defs.h" //move

template<typename T, size_t const N>
class Queue
{
    public:
        Queue(void): pos_r_(0), pos_w_(0), len_(0) {}
        ~Queue(void)
        {
            if (pos_r_ <= pos_w_) {
                for (size_t i = pos_r_; i != pos_w_r; ++i) {
                    vals_[i].~T();
                }
            } else {
                for (size_t i = pos_r_; i < N; ++i) {
                    vals_[i].~T();
                }
                for (size_t i = 0; i < pos_w_; ++i) {
                    vals_[i].~T();
                }
            }
        }

    private:
    public:
        size_t len(void) const
        {
            return len_;
        }
        size_t capacity(void) const
        {
            return N;
        }
        bool is_empty(void) const
        {
            return len_ == 0;
        }
        bool is_full(void) const
        {
            return len_ == N;
        }

    public:
        void clear(void)
        {
            if (pos_r_ <= pos_w_) {
                for (size_t i = pos_r_; i != pos_w_r; ++i) {
                    vals_[i].~T();
                }
            } else {
                for (size_t i = pos_r_; i < N; ++i) {
                    vals_[i].~T();
                }
                for (size_t i = 0; i < pos_w_; ++i) {
                    vals_[i].~T();
                }
            }
            pos_r_ = pos_w_ = len_ = 0;
        }
        bool push(T &&v)
        {
            if (is_full()) { return false; }
            vals_[pos_w_] = v;
            pos_w_ += 1;
            if (pos_w_ >= capacity()) { pos_w_ = 0; }
            len_ += 1;
            return true;
        }
        bool pop(T &v)
        {
            if (is_empty()) { return false; }
            v = vals_[pos_r_];
            pos_r_ += 1;
            if (pos_r_ >= capacity()) { pos_r_ = 0; }
            len_ -= 1;
            return true;
        }

    private:
        T vals_[N];
        size_t pos_r_;
        size_t pos_w_;
        size_t len_;
};

#endif // defined(QUEUE_H)
