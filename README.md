## 常用的c++模板实现

###  <font color=pink>**一、用于RAII管理资源句柄AutoHandle**</font>


```c++
    template<class _T, class _FreeFunc>
    struct AutoHandle
```
#### <font color=pink> _T: 智能指针指向的类型</font>

#### <font color=pink>_FreeFunc: 析构并释放该类型的函数(需要用Functor<(函数地址)>来萃取为编译期的常量)</font>

---


###  <font color=pink>**二、用于反转比特位**</font>
```c++
    template<class _T>
    inline void reverse_bit(_T& val, _T local) noexcept requires std::is_pod_v<_T1>
```
#### <font color=pink> _T: 要反转的类型 </font>
#### <font color=pink>local: 要反转第几位</font>
