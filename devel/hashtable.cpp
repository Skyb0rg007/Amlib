
#include <cstdlib>
#include <memory>
#include <vector>
#include <array>
#include <type_traits>

#include <immintrin.h> /* __m128i */

class Bitmask {
public:
    explicit Bitmask(uint32_t mask) : mask(mask) { }
    struct iterator {
        uint32_t mask;
        explicit iterator(uint32_t mask) : mask(mask) { }
        iterator &operator++() {
            mask &= (mask - 1);
            return *this;
        }
        explicit operator bool() const {
            return mask != 0;
        }
        int operator*() const {
            return __builtin_ctz(mask);
        }
    };
    iterator begin() const { return iterator(mask); }
    iterator end() const { return iterator(0); }
private:
    uint32_t mask;
};

template <class T, class Key>
struct dense_hash_set {
public:
    dense_hash_set() : groups(8) { }

    T *find(const Key &key, size_t hash) {
        size_t group_idx = H1(hash) % groups.size();
        while (true) {
            group g = groups[group_idx];
            T *val = g.find(key, hash);
            if (val)
                return val;
            if (g.match_empty())
                return nullptr;
            group_idx = group_idx % groups.size();
        }
    }

    void erase(T *it) {

    }

private:
    size_t H1(size_t hash) { return hash >> 7; }
    int8_t H2(size_t hash) { return hash & 0x7F; }

    enum class control_bit : int8_t {
        empty = -128, /* 0b10000000 */
        deleted = -2, /* 0b11111110 */
        sentinel = -1 /* 0b11111111 */
        /* full          0b0xxxxxxx */
    };

    struct control_byte {
        __m128i ctrl; /* 16 control bits */
        /* Return bitmask of matching slots */
        Bitmask match(int8_t h2_hash) const {
            __m128i hash_duped = _mm_set1_epi8(h2_hash);
            __m128i matching = _mm_cmpeq_epi8(hash_duped, this->ctrl);
            uint32_t bitmask = _mm_movemask_epi8(matching);
            return Bitmask(bitmask);
        }
        /* Return bitmask of empty slots */
        Bitmask match_empty() const {
            /* return match(control_bit::empty); */
            return Bitmask(_mm_movemask_epi8(_mm_sign_epi8(ctrl, ctrl)));
        }
        /* Return bitmask of empty/deleted slots */
        Bitmask match_empty_or_deleted() const {
            __m128i special = _mm_set1_epi8(control_bit::sentinel);
            /* Note that this may cause bugs in older GCC versions */
            __m128i matching = _mm_cmpgt_epi8(special, this->ctrl);
            return Bitmask(_mm_movemask_epi8(matching));
        }
    };

    struct group {
        control_byte ctrl;
        std::array<T, 16> elems;

        T *find(const Key &key, size_t hash) const {
            for (int i : this->ctrl.match(H2(hash))) {
                if (key == elems[i]) {
                    return &elems[i];
                }
            }
            return nullptr;
        }
    };

    std::vector<group> groups;

};

int main(void)
{
    return 0;
}

