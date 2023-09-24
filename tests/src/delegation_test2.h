#include <functional>
#include <iostream>
#include <memory>
#include <vector>

struct IDelegationTest
{

    virtual ~IDelegationTest() {}

    virtual bool getter() = 0;
    virtual bool setter(bool b) = 0;
    virtual void jobImpl(bool b, int i) = 0;
    virtual void jobImpl(bool b, int i, const std::string& s) = 0; 
    virtual int jobImpl2(bool b, int i) = 0;

};


struct DelegationTestImpl : public IDelegationTest
{
   int n = 0;

   DelegationTestImpl() = default;
   explicit DelegationTestImpl(int n) : n(n) {}

   virtual bool getter() override
   {
      std::cout << "DelegationTestImpl::getter(), n: " << n << "\n";
      return true;
   }

   virtual bool setter(bool b) override
   {
      std::cout << "DelegationTestImpl::setter(bool b), b: " << b << ", n: " << n << "\n";
      return true;
   }

   virtual void jobImpl(bool b, int i) override
   {
      std::cout << "DelegationTestImpl::jobImpl(bool b, int i), b: " << b << ", i: " << i << ", n: " << n << "\n";
   }

   virtual void jobImpl(bool b, int i, const std::string& s) override
   {
      std::cout << "DelegationTestImpl::jobImpl(bool b, int i), b: " << b << ", i: " << i << ", s: " << s << ", n: " << n << "\n";
   }

   virtual int jobImpl2(bool b, int i) override
   {
      std::cout << "DelegationTestImpl::jobImpl2(bool b, int i), b: " << b << ", i: " << i << ", n: " << n << "\n";
      return n;
   }

};

struct AtExit
{
    std::function<void()> guard;
    ~AtExit() { guard(); }
};

class Delegator : public IDelegationTest
{
   template <typename F>
   decltype(auto) doDelegate(F&& f)
   {
      const AtExit callTail {[&]
      {
         for (auto&& impl : m_tail)
         {
            f(*impl);
         }
      }};     
      return f(*m_head);
   }

public:

   // template <typename T, typename...X>
   // explicit Delegator(T&& head, X&&...tail)
   //    : m_head(std::forward<T>(head)), m_tail{ std::forward<X>(tail)...} {}

   bool getter() override                       { return doDelegate([&](IDelegationTest& t) { return t.getter(); }); }
   bool setter(bool b) override                 { return doDelegate([&](IDelegationTest& t) { return t.setter(b); }); }
   void jobImpl(bool b, int i) override         { return doDelegate([&](IDelegationTest& t) { return t.jobImpl(b, i); }); }
   void jobImpl( bool b, int i
               , const std::string& s) override { return doDelegate([&](IDelegationTest& t) { return t.jobImpl(b, i, s); }); }
   int jobImpl2(bool b, int i) override         { return doDelegate([&](IDelegationTest& t) { return t.jobImpl2(b, i); }); }

//private:
public:

   using ImplPtr = std::shared_ptr<IDelegationTest>;

   ImplPtr m_head;
   std::vector<ImplPtr> m_tail;
};

inline
std::shared_ptr<IDelegationTest> makeDelegator()
{
    std::shared_ptr<Delegator> ret = std::make_shared<Delegator>();
    ret->m_head            = std::make_shared<DelegationTestImpl>(1) ;
    ret->m_tail.emplace_back(std::make_shared<DelegationTestImpl>(2));

    return std::move(ret);
}


/*
int main()
{
   Delegator delegator {
      std::make_shared<DelegationTestImpl>(1),
      std::make_shared<DelegationTestImpl>(2),
      std::make_shared<DelegationTestImpl>(3),
   };

   delegator.getter();
   delegator.setter(true);
   delegator.jobImpl(false, 3);
   delegator.jobImpl2(true, 5);
}
*/
