#if !defined(SLICE_H)
#    define SLICE_H

template<typename T>
class Slice;

template<typename T>
class Slice
{
    public:
        Slice(T *p, size_t len): p_(p), len_(len) {}

    public:
        T *ptr(void) const
        {
            return p_;
        }
        size_t len(void) const
        {
            return len_;
        }

    public:
        Slice slice(void) const
        {
            return Slice(p_, len_);
        }

        Slice slice(size_t b, size_t e) const
        {
            if (b >= e) { return Slice(nullptr, 0); }
            if (b >= len_) { return Slice(nullptr, 0); }
            return Slice(&p_[b], min(e - b, len_));
        }

    private:
        T *p_;
        size_t len_;
};

#endif // defined(SLICE_H)
