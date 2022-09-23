#if !defined(VECTOR_H)
#    define VECTOR_H

template<typename T, size_t const N>
class Vector;

#    include "slice.h"
#    include "defs.h" // move

template<typename T, size_t const N>
class Vector
{
    public:
        ~Vector(void)
        {
            for (size_t i = 0; i < pos_; ++i) {
                vals_[i].~T();
            }
        }
        Vector(): pos_(0) {}

    private:
        Vector(Vector const &);
        Vector operator=(Vector const &);

    public:
        bool is_empty(void) const
        {
            return pos_ == 0;
        }
        bool is_full(void) const
        {
            return pos_ == N;
        }
        size_t len(void) const
        {
            return pos_;
        }
        size_t capacity(void) const
        {
            return N;
        }

        Slice<T> slice(void)
        {
            return Slice<T>(vals_, pos_);
        }

    public:
        void clear(void)
        {
            for (size_t i = 0; i < pos_; ++i) {
                vals_[i].~T();
            }
            pos_ = 0;
        }

        bool push(T &&v)
        {
            if (is_full()) { return false; }
            vals_[pos_] = move(v);
            pos_ += 1;
            return true;
        }

        bool pop(T &v)
        {
            if (is_empty()) { return false; }
            pos_ -= 1;
            v = move(vals_[pos_]);
            return true;
        }

        class Iter
        {
            private:
                friend class Vector;
                Iter(T *const b, T *const e): b_(b), e_(e) {}

            public:
                T *next(void)
                {
                    return (e_ == b_) ? nullptr : b_++;
                }

            private:
                T *b_;
                T *e_;
        };
        Iter iter(void)
        {
            return Iter(&vals_[0], &vals_[pos_]);
        }

    private:
        T vals_[N];
        size_t pos_;
};

template<typename T>
class VectorIter
{
    public:
    private:
        T *b_;
        T *e_;
};

#endif // VECTOR_H
