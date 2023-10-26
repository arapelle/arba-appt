#pragma once

#include <arba/core/debug/assert.hpp>
#include <arba/appt/util/duration.hpp>
#include <chrono>
#include <thread>
#include <cstdint>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{

template <typename module_base_type, typename module_type = void>
class loop;

template <typename module_base_type>
class loop<module_base_type> : public module_base_type
{
public:
    template <typename other_module_type>
    using rebind_t = loop<module_base_type, other_module_type>;

    using module_base_type::module_base_type;

protected:
    virtual ~loop() override = default;
};

template <typename module_base_type, typename module_type>
class loop : public loop<typename module_base_type::template rebind_t<module_type>>
{
private:
    using base_ = loop<typename module_base_type::template rebind_t<module_type>>;

public:
    using application_type = typename base_::application_type;
    using base_::base_;

    virtual ~loop() override = default;

    virtual void run() override;
    virtual void finish() {}
    virtual void stop() override { run_token_ = false; }

    [[nodiscard]] inline bool is_running() const noexcept { return run_token_; }

    inline uint16_t frequency() const { return frequency_; }
    inline void set_frequency(uint16_t times_per_second);
    inline const duration& delta_time() const { return delta_time_; }

private:
    inline std::chrono::nanoseconds compute_loop_duration_() const;

private:
    std::atomic_bool run_token_ = false;
    std::atomic_uint16_t frequency_ = 60;
    std::chrono::steady_clock::duration loop_duration_ = compute_loop_duration_();
    duration delta_time_;
};

template <typename module_base_type, typename module_type>
void loop<module_base_type, module_type>::run()
{
    run_token_ = true;
    std::chrono::steady_clock clock;
    std::chrono::steady_clock::duration run_loop_duration;
    delta_time_ = loop_duration_;
    std::chrono::steady_clock::time_point loop_start_tp;
    std::chrono::steady_clock::time_point loop_end_tp = clock.now();
    while (is_running())
    {
        loop_start_tp = loop_end_tp;
        this->self().run_loop(delta_time_);
        run_loop_duration = clock.now() - loop_start_tp;
        std::this_thread::sleep_for(loop_duration_ - run_loop_duration);
        loop_end_tp = clock.now();
        delta_time_ = loop_end_tp - loop_start_tp;
    }
    finish();
}

template <typename module_base_type, typename module_type>
void loop<module_base_type, module_type>::set_frequency(uint16_t times_per_second)
{
    ARBA_ASSERT(times_per_second > 0);
    frequency_ = times_per_second;
    loop_duration_ = compute_loop_duration_();
}

template <typename module_base_type, typename module_type>
std::chrono::nanoseconds loop<module_base_type, module_type>::compute_loop_duration_() const
{
    return std::chrono::nanoseconds(static_cast<intmax_t>(static_cast<long double>(1000000000.) / frequency_));
}

}
}
}
