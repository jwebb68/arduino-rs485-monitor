// -*- mode: c++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
Something to test reading modbus messages correctly over rs485.

Modbus uses time gaps (i.e. no activity) to determine message start/stop
which makes it nasty (and slow).

So this is an investigation into if the arduino can detect these gaps and so split on them.
(spoiler: it can).

but beware of serial logging, this takes time and will stuff up the frame detection.

TODO:
1.  use an interrupt driven approach for reading, then, so long as no other
    interrupts cause interference then reading and idle/gap detection should be
    unaffected.
    May need a timer for inter-message and inter-byte time checking (or just check time since last).
*/

#include <ArduinoRS485.h>

#include "vector.h"
#include "duration.h"
#include "panic.h"
#include "defs.h"

size_t no_rx;

// RS485Class serial_us(HardwareSerial, pin_tx, pin_de, pin_re);
// tx = transmit pin ? only used to send breaks: ?, default PIN_SERIAL_us_TX, or 1
// de = initially set low, taken high before transmit, taken low to recv: ? data_enable: default A6
// re = initial set high, reset low, low on receive, high when not receive: receive_enable: default
// A5

RS485Class serial_us(SERIAL_US, -1, A6, A5);

#define RX_BUF_SIZE 256
uint8_t rx_buf[RX_BUF_SIZE];
char mon_buf[RX_BUF_SIZE * 3];

int to_hex(char outs[], size_t outs_len, uint8_t v)
{
    static char const hex[17] = "0123456789abcdef";
    if (outs_len < 2) { return -1; }
    outs[0] = hex[(v >> 4) & 0xf];
    outs[1] = hex[v & 0xf];
    return 2;
}

int to_hex(char outs[], size_t outs_len, uint8_t ins[], size_t ins_len)
{
    size_t o_idx = 0;
    for (size_t i_idx = 0; i_idx < ins_len; ++i_idx) {
        int n = to_hex(&outs[o_idx], outs_len - o_idx, ins[i_idx]);
        if (n < 0) { return n; }
        o_idx += n;
        if ((outs_len - o_idx) < 1) { break; }
        outs[o_idx] = ' ';
        o_idx += 1;
    }
    return o_idx;
}

enum State {
    Begin,
    Gap,
    CollectBegin,
    Collect,
};
State state;

// put your main code here, to run repeatedly:

// t35 for baud > 19200
Duration const t35 = Duration::from_micros(1750);
// t15 for baud > 19200
Duration const t15 = Duration::from_micros(750);

Instant wait_begin = Instant::now();

// have 2 io chans, one rx (the rs485, one serialusb)
// and running at same speed.
// when one is transmitting to usb, must be able to rx from rs485.
// so use 2 bufs.
typedef Vector<uint8_t, 256> Buf;
Buf bufs[2];
size_t buf_rx_idx;
size_t buf_tx_idx;

// unsigned int tx_start;
// unsigned int rx_start;
Instant rx_start = Instant::now();

void setup()
{
    // put your setup code here, to run once:

    CONSOLE.begin(115200);
    while (!CONSOLE)
        ;

    // SERIAL_US.begin(115200, SERIAL_8N1);
    // serial_us.begin(115200, SERIAL_8N1, 100, 100);
    SERIAL_US.begin(115200, SERIAL_8E1);
    serial_us.begin(115200, SERIAL_8E1, 100, 100);
    serial_us.receive();

    // SERIAL3.begin(115200, SERIAL_8E1);

    no_rx = 0;
    state = State::Begin;
    buf_rx_idx = 0;
    buf_tx_idx = 0;
    wait_begin = Instant::now();

    // tx_start = micros();
    // rx_start = micros();
    rx_start = Instant::now();

    CONSOLE.println(F("initialised.."));
}

void loop()
{
#define METHOD 0
#if METHOD == 2
    if ((micros() - tx_start) > 10000) {
        char data[] = {0x01, 0x03, 0x05, 0x07};
        SERIAL3.write(data, sizeof(data));
        tx_start = micros();
        rx_start = micros();
    }

    size_t n_avail;
    if ((n_avail = SERIAL2.available()) > 0) {
        unsigned int rx_end = micros();
        CONSOLE.print("> ");
        CONSOLE.print(rx_end - tx_start);
        CONSOLE.print(" ");
        CONSOLE.print(SERIAL2.available());
        CONSOLE.print(" ");
        CONSOLE.print(rx_end - rx_start);
        CONSOLE.print(" ");
        CONSOLE.print(micros() - rx_start);
        CONSOLE.print(" ");
        CONSOLE.print("> ");
        char buf[2];
        for (; n_avail != 0; --n_avail) {
            int v = SERIAL2.read();
            to_hex(buf, 2, v);
            CONSOLE.write(buf, 2);
            CONSOLE.print(" ");
        }
        rx_start = micros();
        CONSOLE.println();
    }
    // delayMicroseconds(100);
#elif METHOD == 1

    size_t n_avail;
    if ((n_avail = serial_us.available()) > 0) {
        // unsigned int rx_end = micros();
        Instant rx_end = Instant::now();
        CONSOLE.print(F("> "));
        // CONSOLE.print(rx_end - tx_start);
        // CONSOLE.print(" ");
        CONSOLE.print(serial_us.available());
        CONSOLE.print(F(" "));
        // CONSOLE.print(rx_end - rx_start);
        CONSOLE.print((rx_end.elapsed_since(rx_start)).as_micros());
        CONSOLE.print(F(" "));
        // CONSOLE.print(micros() - rx_start);
        CONSOLE.print((Instant::now().elapsed_since(rx_start)).as_micros());
        CONSOLE.print(F(" "));
        CONSOLE.print(F("> "));
        char buf[2];
        for (; n_avail != 0; --n_avail) {
            int v = serial_us.read();
            to_hex(buf, 2, v);
            CONSOLE.write(buf, 2);
            CONSOLE.print(" ");
        }
        // rx_start = micros();
        rx_start = Instant::now();
        CONSOLE.println();
    }
    // delayMicroseconds(100);

#elif METHOD == 11

    switch (state) {
        case State::Begin:
            CONSOLE.print("1 ");
            CONSOLE.println();
            state = State::Gap;
            wait_begin = Instant::now();
            break;

        case State::Gap: {
            size_t n_avail = serial_us.available();
            if (n_avail > 0) {
                CONSOLE.print(F("2 "));
                // unsigned int rx_end = micros();
                Instant rx_end = Instant::now();
                CONSOLE.print(F("> "));
                // CONSOLE.print(rx_end - tx_start);
                // CONSOLE.print(" ");
                CONSOLE.print(serial_us.available());
                CONSOLE.print(F(" "));
                // CONSOLE.print(rx_end - rx_start);
                CONSOLE.print((rx_end.elapsed_since(rx_start)).as_micros());
                CONSOLE.print(F(" "));
                // CONSOLE.print(micros() - rx_start);
                CONSOLE.print((Instant::now().elapsed_since(rx_start)).as_micros());
                CONSOLE.print(F(" "));
                CONSOLE.print(F("> "));
                char buf[2];
                for (; n_avail != 0; --n_avail) {
                    int v = serial_us.read();
                    to_hex(buf, 2, v);
                    CONSOLE.write(buf, 2);
                    CONSOLE.print(F(" "));
                }
                // rx_start = micros();
                rx_start = Instant::now();
                wait_begin = Instant::now();
                CONSOLE.println();
            }
            if (n_avail == 0) {
                Instant rx_end = Instant::now();
                CONSOLE.print(F("2.2 "));
                CONSOLE.print((rx_end.elapsed_since(rx_start)).as_micros());
                CONSOLE.print(F(" "));
                // CONSOLE.print('\n');
                // CONSOLE.println();

                // } else if (Instant::now().elapsed_since(wait_begin) > t35) {
                //   CONSOLE.println(F("2.2 "));
                //   CONSOLE.print(F("> "));
                //   // CONSOLE.print(rx_end - tx_start);
                //   // CONSOLE.print(F(" "));
                //   CONSOLE.print(serial_us.available());
                //   CONSOLE.print(F(" "));
                //   //CONSOLE.print(rx_end - rx_start);
                //   CONSOLE.print((rx_end.elapsed_since(rx_start)).as_micros());
                //   CONSOLE.print(F(" "));
                //   //CONSOLE.print(micros() - rx_start);
                //   CONSOLE.print((Instant::now().elapsed_since(rx_start)).as_micros());
                //   CONSOLE.print(F(" "));
                //   CONSOLE.print(F("> "));
                //   //state = State::Collect;
                //   wait_begin = Instant::now();
            }
        } break;
        case State::Collect:
            break;
        default:
            panic0();
    }

#elif METHOD == 0
    switch (state) {
        case State::Begin:
            CONSOLE.println("1 ");
            state = State::Gap;
            wait_begin = Instant::now();
            break;

        case State::Gap: {
            size_t n_avail = serial_us.available();
            Instant tp = Instant::now();
            if (n_avail > 0) {
                CONSOLE.print(F("2.1 "));
                CONSOLE.print(n_avail);
                CONSOLE.print(' ');
                CONSOLE.print(tp.elapsed_since(wait_begin).as_micros());
                CONSOLE.print(' ');
                // got bytes in gap period, so gap not long enough
                // drain any rx and restart wait
                // block rx while draining
                // serial_us.noReceive();
                while (n_avail-- > 0) {
                    serial_us.read();
                }
                // serial_us.receive();
                // delayMicroseconds(100);

                wait_begin = Instant::now();
                CONSOLE.println();
            } else if (Instant::now().elapsed_since(wait_begin) > t35) {
                CONSOLE.print(F("2.2 "));
                CONSOLE.print(n_avail);
                CONSOLE.print(' ');
                CONSOLE.print(tp.elapsed_since(wait_begin).as_micros());
                CONSOLE.print(' ');
                // no chars, and waited long enough.

                // emit any chars collected so far though.
                // or queue for tx.
                Buf &buf_rx = bufs[buf_rx_idx];
                CONSOLE.print(buf_rx_idx);
                CONSOLE.print(' ');
                CONSOLE.print(buf_rx.is_empty());
                CONSOLE.print(' ');
                if (!buf_rx.is_empty()) {
                    buf_rx_idx += 1;
                    if (buf_rx_idx >= sizeof(bufs) / sizeof(bufs[0])) { buf_rx_idx = 0; }
                    if (buf_rx_idx == buf_tx_idx) { panic0(); }
                    bufs[buf_rx_idx].clear();
                }
                // and write any in the txt queue
                if (buf_tx_idx != buf_rx_idx) {
                    Buf &buf_tx = bufs[buf_tx_idx];
                    CONSOLE.print(buf_tx.len());
                    CONSOLE.print(' ');
                    if (!buf_tx.is_empty()) {
                        Buf::Iter it = buf_tx.iter();
                        for (uint8_t *p = nullptr; (p = it.next()) != nullptr;) {
                            char buf[2];
                            to_hex(buf, 2, *p);
                            CONSOLE.write(buf, 2);
                            CONSOLE.print(' ');
                        }
                        // CONSOLE.write(buf_tx.slice());
                        buf_tx.clear();
                        buf_tx_idx += 1;
                        if (buf_tx_idx >= sizeof(bufs) / sizeof(bufs[0])) { buf_tx_idx = 0; }
                    }
                }

                state = State::CollectBegin;
                wait_begin = Instant::now();
            }
        } break;

        case State::CollectBegin: {
            // start collecting, but might not start for some time, so t15 doesn't apply
            size_t n_avail = serial_us.available();
            if (n_avail > 0) {
                CONSOLE.print(F("3.1 "));
                CONSOLE.print(n_avail);
                CONSOLE.print(' ');
                Buf &buf_rx = bufs[buf_rx_idx];
                while (n_avail-- > 0) {
                    uint8_t v = serial_us.read();
                    CONSOLE.print(v, HEX);
                    CONSOLE.print(' ');
                    if (!buf_rx.push(move(v))) { panic0(); }
                }
                wait_begin = Instant::now();
                state = State::Collect;
                CONSOLE.println();
            }
        } break;

        case State::Collect: {
            size_t n_avail = serial_us.available();
            Instant tp = Instant::now();
            if (tp.elapsed_since(wait_begin) > t15) {
                CONSOLE.print(F("4.1 "));
                CONSOLE.print(n_avail);
                CONSOLE.print(' ');
                CONSOLE.print(tp.elapsed_since(wait_begin).as_micros());
                CONSOLE.print(' ');
                // waited too long for data.. even if it's present..
                // may be in the inter-frame gap now.. so continue the gap wait.
                state = State::Gap;
                //
            } else if (n_avail > 0) {
                CONSOLE.print(F("4.2 "));
                CONSOLE.print(n_avail);
                CONSOLE.print(' ');
                CONSOLE.print(Instant::now().elapsed_since(wait_begin).as_micros());
                CONSOLE.print(' ');
                // within t15 time, got data, collect into buffer
                Buf &buf_rx = bufs[buf_rx_idx];
                while (n_avail-- > 0) {
                    uint8_t v = serial_us.read();
                    CONSOLE.print(v, HEX);
                    CONSOLE.print(' ');
                    if (!buf_rx.push(move(v))) { panic0(); }
                }
                wait_begin = Instant::now();
                CONSOLE.println();
            }
        } break;

        default:
            panic0();
    }
#else
#    error Unknown method
#endif
}
