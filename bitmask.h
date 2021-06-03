#ifndef _BITMASK_H
#define _BITMASK_H

#include <vector>

class BitMask {
    public:
        BitMask(uint16_t length)
        : length_(length) {
            mask_ = new uint32_t[length/32 + 1]{};
        }
        BitMask(const BitMask&) = delete;
        BitMask& operator=(const BitMask&) = delete;

        ~BitMask() {
            delete [] mask_;
        }

        void setBit(uint16_t pos, bool value) {
            uint8_t index = pos / 32;
            mask_[index] |= value << (pos % 32);
        }

        bool readBit(uint16_t pos) const {
            uint8_t index = pos / 32;
            return (mask_[index] & (1 << (pos % 32)));
        }

        uint16_t count(bool value) const {
            uint16_t count = 0;
            for(uint16_t i = 0; i < 32*length_; ++i) {
                if(readBit(i) == value) {
                    count++;
                }
            }
            return count;
        }

        uint16_t maxLength() const {
            return length_*32;
        }

    private:
        uint16_t length_;
        uint32_t* mask_;
};

#endif // _BITMASK_H