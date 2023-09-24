#pragma once

#include <iostream>
#include <memory>

// http://www.gotw.ca/publications/mill18.htm


#include <vector>

struct IDelegationTest
{
    virtual ~IDelegationTest() {}

    virtual bool getter() = 0;
    virtual bool setter(bool b) = 0;
    virtual void jobImpl(bool b, int i) = 0;
    virtual int jobImpl2(bool b, int i) = 0;
    virtual int jobImpl2(bool b, std::string s) = 0;

};

struct DelegationTestImpl : public IDelegationTest
{
    int n = 0;

    DelegationTestImpl(int p) : n{p} {}

    bool getter() override
    {
        std::cout << "DelegationTestImpl::getter(), n: " << n << "\n";
        return true;
    }

    bool setter(bool b) override
    {
        std::cout << "DelegationTestImpl::setter(bool b), b: " << b << ", n: " << n << "\n";
        return true;
    }

    void jobImpl(bool b, int i) override
    {
        std::cout << "DelegationTestImpl::jobImpl(bool b, int i), b: " << b << ", i: " << i << ", n: " << n << "\n";
    }

    int jobImpl2(bool b, int i) override
    {
        std::cout << "DelegationTestImpl::jobImpl2(bool b, int i), b: " << b << ", i: " << i << ", n: " << n << "\n";
        return n;
    }

    int jobImpl2(bool b, std::string s) override
    {
        std::cout << "DelegationTestImpl::jobImpl2(bool b, std::string s), b: " << b << ", s: " << s << ", n: " << n << "\n";
        return n;
    }


};

struct DelegatorImpl : public IDelegationTest
{
  using container_type = std::vector< std::shared_ptr<IDelegationTest> >;
    
    // data
    container_type impls;

    template <typename Return, typename... Args>
    Return delegateCall( Return(IDelegationTest::*func)(Args...), Args... args)
    {
        // !!! Тут assert на тему impls не должен быть пустым

        container_type::const_iterator it = impls.begin();

        // !!! Или assert тут по it!=impls.end();

        // Получаем результат
        auto res = ((**it).*func)(args...); ++it;

        for(; it!=impls.end(); ++it)
        {
            // Вызываем метод для всех экземпляров имплементаций
            ((**it).*func)(args...);
        }

        return res;
    }

    // 
    template <typename... Args>
    void delegateCall( void(IDelegationTest::*func)(Args...), Args... args)
    {
        container_type::const_iterator it = impls.begin();
        for(; it!=impls.end(); ++it)
        {
            ((**it).*func)(args...);
        }
    }

    bool getter() override
    {
        return delegateCall(&IDelegationTest::getter);
    }

    bool setter(bool b) override
    {
        return delegateCall(&IDelegationTest::setter, b);
    }

    void jobImpl(bool b, int i) override
    {
        delegateCall(&IDelegationTest::jobImpl, b, i);
    }

    int jobImpl2(bool b, int i) override
    {
        return delegateCall(static_cast<int(IDelegationTest::*)(bool, int)>(&IDelegationTest::jobImpl2), b, i);
    }

    int jobImpl2(bool b, std::string s) override
    {
        return delegateCall(static_cast<int(IDelegationTest::*)(bool, std::string)>(&IDelegationTest::jobImpl2), b, s);
    }

};


inline
std::shared_ptr<IDelegationTest> makeDelegator()
{
  std::shared_ptr<DelegatorImpl> ret = std::make_shared<DelegatorImpl>();
    ret->impls.emplace_back(std::make_shared<DelegationTestImpl>(1));
    ret->impls.emplace_back(std::make_shared<DelegationTestImpl>(2));

    return std::move(ret);
}

/*
int main()
{
    std::shared_ptr<IDelegationTest> pDelegator = makeDelegator();

    pDelegator->getter();
    pDelegator->setter(true);
    pDelegator->jobImpl(false, 3);
    pDelegator->jobImpl2(true, 5);
    pDelegator->jobImpl2(true, "str");
}
*/

