
#if !defined(DURATION_H)
#    define DURATION_H

class Duration;
class Instant;

class Duration
{
    public:
    private:
        Duration(unsigned int usecs);

        // Duration(Duration const &);
        // Duration operator=(Duration const &);

    public:
        Duration &operator+=(Duration const &b);
        Duration &operator-=(Duration const &b);

        Duration operator+(Duration const &b) const;
        Duration operator-(Duration const &b) const;

        bool operator==(Duration const &b) const;
        bool operator!=(Duration const &b) const;
        bool operator>(Duration const &b) const;
        bool operator<(Duration const &b) const;
        bool operator>=(Duration const &b) const;
        bool operator<=(Duration const &b) const;

    public:
        static Duration from_micros(unsigned int usecs);
        static Duration from_millis(unsigned int msecs);
        static Duration from(unsigned int secs, unsigned int usecs);
        static Duration from(float secs);
        static Duration from(double secs);

    public:
        unsigned int as_micros(void) const;
        unsigned int as_millis(void) const;
        unsigned int as_secs(void) const;
        float as_float(void) const;
        double as_double(void) const;

    private:
        unsigned int usecs_;
};

class Instant
{
    public:
        // Instant(Instant const &);
        // Instant operator=(Instant const &);

    private:
        Instant(unsigned int usecs);

    public:
        static Instant now(void);
        Duration elapsed_since(Instant const &tp);

    private:
        unsigned int usecs_;
};

#endif // defined(DURATION_H)
