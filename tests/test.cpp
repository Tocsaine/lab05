#include <Account.h>
#include <Transaction.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}

    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(Account, Constructor) {
    Account acc(1, 500);

    EXPECT_EQ(acc.GetBalance(), 500);
    EXPECT_THROW(acc.ChangeBalance(-100), std::runtime_error);
    acc.Lock();
    acc.ChangeBalance(-100);
    EXPECT_EQ(acc.GetBalance(), 400);
    acc.Unlock();
}

TEST(Functions, transactions){
    MockAccount a1(1, 220);
    MockAccount a2(2, 100);
    Transaction test;

    EXPECT_CALL(a1, GetBalance())
    .WillOnce(testing::Return(220))
    .WillOnce(testing::Return(69))
    .WillOnce(testing::Return(69))
    .WillOnce(testing::Return(69));

    EXPECT_CALL(a2, GetBalance())
    .WillOnce(testing::Return(250))
    .WillOnce(testing::Return(250));

    EXPECT_CALL(a1, ChangeBalance(-151))
    .WillOnce(testing::Invoke(nullptr));

    EXPECT_CALL(a2, ChangeBalance(testing::_))
    .Times(3);

    EXPECT_CALL(a1, Lock())
    .Times(5)
    .WillOnce(testing::Throw(std::runtime_error("at first lock the account")))
    .WillOnce(nullptr)
    .WillOnce(testing::Throw(std::runtime_error("already locked")))
    .WillOnce(nullptr)
    .WillOnce(nullptr);

    EXPECT_CALL(a2, Lock())
    .Times(2);
    EXPECT_CALL(a1, Unlock())
    .Times(3);
    EXPECT_CALL(a2, Unlock())
    .Times(2);

    EXPECT_THROW(test.Make(a1, a1, 200), std::logic_error);
    EXPECT_THROW(test.Make(a1, a2, 50), std::logic_error);
    EXPECT_THROW(test.Make(a1, a2, -50), std::invalid_argument);

    test.set_fee(200);
    EXPECT_EQ(test.Make(a1, a2, 150), 0);
    EXPECT_EQ(test.fee(), 200);
    test.set_fee(1);

    EXPECT_THROW(test.Make(a1, a2, 150), std::runtime_error);
    a1.Lock();
    EXPECT_THROW(test.Make(a1, a2, 150), std::runtime_error);
    a1.Unlock();

    bool res = test.Make(a1, a2, 150);
    EXPECT_EQ(res, 1);

    res = test.Make(a1, a2, 150);
    EXPECT_EQ(res, 0);
}
