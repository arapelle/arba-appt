#pragma once

#include <arba/appt/util/delta_time.hpp>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <thread>

inline namespace arba
{
namespace appt
{
inline namespace mdec // module_decorator
{

class loop_base
{
public:
    using clock = std::chrono::steady_clock;
    using duration = typename clock::duration;
};

template <typename ModuleBase, typename SelfType = void>
class loop;

template <typename ModuleBase>
class loop<ModuleBase> : public ModuleBase, public loop_base

{
public:
    template <typename OtherType>
    using rebind_t = loop<ModuleBase, OtherType>;

    using ModuleBase::ModuleBase;

protected:
    virtual ~loop() override = default;
};

template <typename ModuleBase, typename SelfType>
class loop : public loop<typename ModuleBase::template rebind_t<SelfType>>
{
private:
    using base_ = loop<typename ModuleBase::template rebind_t<SelfType>>;

public:
    using application_type = typename base_::application_type;
    using base_::base_;
    using typename base_::clock;
    using typename base_::duration;

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
    duration loop_duration_ = compute_loop_duration_();
    duration delta_time_;
};

template <typename ModuleBase, typename SelfType>
void loop<ModuleBase, SelfType>::run()
{
    run_token_ = true;
    clock loop_clock;
    duration run_loop_duration;
    delta_time_ = loop_duration_;
    typename clock::time_point loop_start_tp;
    typename clock::time_point loop_end_tp = loop_clock.now();
    while (is_running())
    {
        loop_start_tp = loop_end_tp;
        this->self().run_loop(delta_time_);
        run_loop_duration = loop_clock.now() - loop_start_tp;
        std::this_thread::sleep_for(loop_duration_ - run_loop_duration);
        loop_end_tp = loop_clock.now();
        delta_time_ = loop_end_tp - loop_start_tp;
    }
    finish();
}

template <typename ModuleBase, typename SelfType>
void loop<ModuleBase, SelfType>::set_frequency(uint16_t times_per_second)
{
    assert(times_per_second > 0);
    frequency_ = times_per_second;
    loop_duration_ = compute_loop_duration_();
}

template <typename ModuleBase, typename SelfType>
std::chrono::nanoseconds loop<ModuleBase, SelfType>::compute_loop_duration_() const
{
    return std::chrono::nanoseconds(static_cast<intmax_t>(static_cast<long double>(1000000000.) / frequency_));
}

} // namespace mdec
} // namespace appt
} // namespace arba
