#include <arba/appt/util/integer_id_factory.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

namespace appt
{
template class integer_id_factory<>;
}

TEST(integer_id_factory_tests, test_new_id_simple)
{
    appt::integer_id_factory<> id_factory;
    ASSERT_EQ(id_factory.number_of_valid_id(), 0);
    std::size_t id = id_factory.new_id();
    ASSERT_EQ(id, 0);
    ASSERT_EQ(id_factory.number_of_valid_id(), 1);
}

TEST(integer_id_factory_tests, test_delete_id_simple)
{
    appt::integer_id_factory<> id_factory;
    std::size_t id = id_factory.new_id();
    ASSERT_EQ(id_factory.number_of_valid_id(), 1);
    id_factory.delete_id(id);
    ASSERT_EQ(id_factory.number_of_valid_id(), 0);
}

TEST(integer_id_factory_tests, test_new_id_queue)
{
    appt::integer_id_factory<> id_factory;
    std::size_t id0 = id_factory.new_id();
    id_factory.new_id();
    ASSERT_EQ(id_factory.number_of_valid_id(), 2);
    id_factory.delete_id(id0);
    ASSERT_EQ(id_factory.number_of_valid_id(), 1);
    std::size_t id0_bis = id_factory.new_id();
    ASSERT_EQ(id0_bis, 0);
    ASSERT_EQ(id_factory.number_of_valid_id(), 2);
}

TEST(integer_id_factory_tests, test_new_delete_id_mix)
{
    appt::integer_id_factory<> id_factory;
    id_factory.new_id();
    id_factory.new_id();
    id_factory.new_id();
    id_factory.new_id();
    id_factory.new_id();
    id_factory.delete_id(1);
    id_factory.delete_id(2);
    id_factory.delete_id(3);
    ASSERT_EQ(id_factory.number_of_valid_id(), 2);
    id_factory.delete_id(4);
    ASSERT_EQ(id_factory.number_of_valid_id(), 1);
    std::size_t id = id_factory.new_id();
    ASSERT_EQ(id, 1);
}

TEST(integer_id_factory_tests, test_reset)
{
    appt::integer_id_factory<int> id_factory;
    id_factory.new_id();
    id_factory.new_id();
    id_factory.reset(-2);
    int id = id_factory.new_id();
    ASSERT_EQ(id, -2);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
