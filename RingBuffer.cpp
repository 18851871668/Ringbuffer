#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iostream>

class RingBuffer {
private:
    void* buffer;
    unsigned int size;
    unsigned int in;
    unsigned int out;
    bool is_power_of_2(unsigned int n) {
        return (n != 0) && ((n & (n - 1)) == 0);
    }
    unsigned int aviailabe() {
        return size - (in - out);
    }

public:
    RingBuffer(unsigned int size)
        : size(size), in(0), out(0) {
        if (!is_power_of_2(size)) {
            throw std::runtime_error("Size must be a power of 2");
        }
        buffer = std::malloc(size);
        if (!buffer) {
            throw std::runtime_error("Failed to allocate memory");
        }
    }

    ~RingBuffer() {
        std::free(buffer);
    }

    void put (unsigned int no_sequence, unsigned int len, void* src) {
        if (len + 8 > aviailabe()) {
            std::cout << "There is no room to store the message";
            return;
        }
        __put(&no_sequence, sizeof(unsigned int));
        __put(&len, sizeof(unsigned int));
        __put(src, len);
    }

    void __put(void* src, unsigned int len) {
        unsigned int l;
        l = std::min(len, size - (in & (size - 1)));
        std::memcpy(static_cast<char*>(buffer) + (in & (size - 1)), src, l);
        std::memcpy(buffer, static_cast<char*>(src) + l, len - l);
        in += len;
    }

    void* get(unsigned int no_sequence, unsigned int len, void* data) {
        __get(&no_sequence, sizeof(unsigned int));
        __get(&len, sizeof(unsigned int));
        __get(data, len);
        return data;
    }

    void __get(void* data, unsigned int len) {
        unsigned int l;
        len = std::min(len, in - out);
        l = std::min(len, size - (out & (size - 1)));
        std::memcpy(data, static_cast<char*>(buffer) + (out & (size - 1)), l);
        std::memcpy(static_cast<char*>(data) + l, buffer, len - l);
        out += len;
        
    }

    void __get_peek(void* data, unsigned int len, unsigned int off) {//只拷贝不移动out指针，相当于没有把元素移出
        unsigned int l;
        len = std::min(len, in - (out + off));
        l = std::min(len, size - ((out + off) & (size - 1)));
        std::memcpy(data, static_cast<char*>(buffer) + ((out + off) & (size - 1)), l);
        std::memcpy(static_cast<char*>(data) + l, buffer, len - l);
    }

    void* search(unsigned int no_sequence, void* res) {
        unsigned int off = 0;
        while (out + off <= in) {
            unsigned int no, len_;
            __get_peek(&no, sizeof(unsigned int), off);
            off += sizeof(unsigned int);
            if (no == no_sequence) {
                __get_peek(&len_, sizeof(unsigned int), off);
                off += sizeof(unsigned int);
                __get_peek(res, len_, off);
                off += len_;
                return res;
            } else {
                __get_peek(&len_, sizeof(unsigned int), off);
                off += len_ + sizeof(unsigned int);
            }
        }
        return nullptr;
    }
};

int main() {
    int q[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char p[5] = {'a', 'b', 'c', 'd', 'e'};
    RingBuffer ringbuffer(4096);
    for (int i = 0; i < 10; i++) {
        ringbuffer.put(i, sizeof(int), &q[i]);
    }
    int res[100];
    for (int i = 0; i < 10; i++) {
        ringbuffer.get(i, sizeof(unsigned int), &res[i]);
    }
    for (int i = 0; i < 10; i++) {
        std::cout << res[i] << ' ';
    }
    std::cout << std::endl;
    char res2[100];
    for (int i = 10; i < 15; i++) {
        ringbuffer.put(i, sizeof(unsigned int), &p[i - 10]);
    }
    for (int i = 10; i < 15; i++) {
        ringbuffer.get(i, sizeof(int), &res2[i - 10]);
    }
    for (int i = 10; i < 15; i++) {
        std::cout << res2[i - 10] << ' ';
    }
    std::cout << std::endl;
    return 0;
}
