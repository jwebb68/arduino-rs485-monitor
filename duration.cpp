#include "duration.h"
#include <Arduino.h> // micros()

Duration::Duration(unsigned int usecs): usecs_(usecs) {}

Duration &Duration::operator+=(Duration const &b)
{
    usecs_ += b.usecs_;
    return *this;
}
Duration &Duration::operator-=(Duration const &b)
{
    usecs_ -= b.usecs_;
    return *this;
}

Duration Duration::operator+(Duration const &b) const
{
    return Duration(usecs_ + b.usecs_);
}
Duration Duration::operator-(Duration const &b) const
{
    return Duration(usecs_ - b.usecs_);
}

bool Duration::operator==(Duration const &b) const
{
    return usecs_ == b.usecs_;
}

bool Duration::operator!=(Duration const &b) const
{
    return usecs_ != b.usecs_;
}

bool Duration::operator>(Duration const &b) const
{
    return usecs_ > b.usecs_;
}

bool Duration::operator<(Duration const &b) const
{
    return usecs_ < b.usecs_;
}

bool Duration::operator>=(Duration const &b) const
{
    return usecs_ >= b.usecs_;
}

bool Duration::operator<=(Duration const &b) const
{
    return usecs_ <= b.usecs_;
}

Duration Duration::from_micros(unsigned int usecs)
{
    return Duration(usecs);
}
Duration Duration::from_millis(unsigned int msecs)
{
    return Duration(1000 * msecs);
}
Duration Duration::from(unsigned int secs, unsigned int usecs)
{
    return Duration(1000000 * secs + usecs);
}
Duration Duration::from(float secs)
{
    return Duration(1000000.0 * secs);
}
Duration Duration::from(double secs)
{
    return Duration(1000000.0 * secs);
}

unsigned int Duration::as_micros(void) const
{
    return usecs_;
}
unsigned int Duration::as_millis(void) const
{
    return usecs_ / 1000;
}
unsigned int Duration::as_secs(void) const
{
    return usecs_ / 1000000;
}
float Duration::as_float(void) const
{
    return usecs_ / 1000000.0;
}
double Duration::as_double(void) const
{
    return usecs_ / 1000000.0;
}
//###########################################################################

Instant::Instant(unsigned int usecs): usecs_(usecs) {};

Instant Instant::now(void)
{
    return Instant(micros());
}
Duration Instant::elapsed_since(Instant const &tp)
{
    return Duration::from_micros(now().usecs_ - tp.usecs_);
}
