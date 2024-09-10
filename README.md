## 常用的c++模板实现

###  **一、用于RAII管理资源句柄AutoHandle**


```c++
    template<class _T, class _FreeFunc>
    struct AutoHandle
```
#### _T: 智能指针指向的类型

#### _FreeFunc: 析构并释放该类型的函数(需要用Functor<(函数地址)>来萃取为编译期的常量)</font>

---

###  **二、RAII文件标识符**
```c++
	template<class T = int, class FreeFunc = Functor<::close>>
	struct unique_fd
```
####  T: 文件标识符的类型一般为int 
#### FreeFunc: 关闭文件标识符的函数


---
###  **三、用于反转比特位**</font>
```c++
    template<class _T>
    inline void reverse_bit(_T& val, _T local) noexcept requires std::is_pod_v<_T1>
```
####  _T: 要反转的类型 
#### local: 要反转第几位

---
### 四、**循环队列**
```c++
	template<class _T, class _DeleteFunctionType = nullptr_t, bool _IsThreadSafe = true>
	class circular_queue
```
#### _T:队列元素的类型
#### _DeleteFunctionType: 析构并释放该类型的函数(如果没有默认delete)

---
### **五、有界队列**
```c++
	template<class _T>
	class bounded_queue
```
#### _T:队列元素的类型

