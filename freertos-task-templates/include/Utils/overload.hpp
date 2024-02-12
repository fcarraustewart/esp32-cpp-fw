#ifndef __OVERLOAD__H
#define __OVERLOAD__H
template <class... Ts>
struct overload : Ts...
{
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;
#endif