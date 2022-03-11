#ifndef SOC_QUEUE_HPP
#define SOC_QUEUE_HPP

#include <deque>
#include <mutex>
#include <cstdint>


namespace soc {
    template<typename T>
    class TSQueue {
    public:
        TSQueue() = default;
        ~TSQueue() { clear(); }
        TSQueue(const TSQueue<T>&) = delete;

        const T& front() {
            std::scoped_lock lock(muxQueue_);
            return deq_.front();
        }

        const T& back() {
            std::scoped_lock lock(muxQueue_);
            return deq_.back();
        }

        void push_back(const T& item) {
            std::scoped_lock lock(muxQueue_);
            deq_.emplace_back(std::move(item));
        }

        void push_front(const T& item) {
            std::scoped_lock lock(muxQueue_);
            deq_.emplace_front(std::move(item));
        }

        bool empty() {
            std::scoped_lock lock(muxQueue_);
            return deq_.empty();
        }

        size_t count() {
            std::scoped_lock lock(muxQueue_);
            return deq_.size();
        }

        void clear() {
            std::scoped_lock lock(muxQueue_);
            deq_.clear();
        }

        T pop_front() {
            std::scoped_lock lock(muxQueue_);
            T t = std::move(deq_.front());
            deq_.pop_front();
            return t;
        }

        T pop_back() {
            std::scoped_lock lock(muxQueue_);
            T t = std::move(deq_.back());
            deq_.pop_back();
            return t;
        }

    protected:
        std::mutex muxQueue_;
        std::deque<T> deq_;
    private:

    };
}

#endif