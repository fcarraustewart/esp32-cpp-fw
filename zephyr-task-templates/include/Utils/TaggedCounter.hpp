#ifndef TAGGEDCOUNTER_HPP
#define TAGGEDCOUNTER_HPP

namespace Meta
{
    template <typename T> class Flag
    {
        struct Dummy
        {
            constexpr Dummy() {}
            friend constexpr void adl_flag(Dummy);
        };

        template <bool> struct Writer
        {
            friend constexpr void adl_flag(Dummy) {}
        };

        template <class Dummy, int = (adl_flag(Dummy{}),0)>
        static constexpr bool Check(int)
        {
            return true;
        }

        template <class Dummy>
        static constexpr bool Check(short)
        {
            return false;
        }

      public:
        template <class Dummy = Dummy, bool Value = Check<Dummy>(0), int = sizeof(Writer<Value && 0>)>
        static constexpr int ReadSet()
        {
            return Value;
        }

        template <class Dummy = Dummy, bool Value = Check<Dummy>(0)>
        static constexpr int Read()
        {
            return Value;
        }
    };
}

namespace Meta
{
        template <typename T,int I> struct Tag {

            constexpr int value() const noexcept{
                return I;
            }
        };

        template<typename T,int N,bool B>
        struct Checker{
            static constexpr int currentval() noexcept{
                return N;
            }
        };

        template<typename T,int N>
        struct CheckerWrapper{
        template<bool B=Flag<Tag<T,N>>::Read(),int M=Checker<T,N,B>::currentval()>
        static constexpr int currentval(){
            return M;
        }
        };

        template<typename T,int N>
        struct Checker<T,N,true>{
            template<int M=CheckerWrapper<T,N+1>::currentval()>
            static constexpr int currentval() noexcept{
                return M;
            }
        };

        template<typename T,int N,bool B=Flag<Tag<T,N>>::ReadSet()>
        struct Next{
            static constexpr int value() noexcept{
                return N;
            }
        };

    template <typename T> class TaggedCounter
    {
      public:
        template <int N=CheckerWrapper<T,0>::currentval()> static constexpr int Value(){
            return Next<T,N>::value();
        }
    };
}

#endif