【DLL】

【常规】
--->输出目录：$(SolutionDir)Lib\$(PlatformTarget)\$(Configuration)\
--->中间目录：$(SolutionDir)Tempout\$(PlatformTarget)\$(ProjectName)\$(Configuration)\

【C/C++】
--->常规--->附加包含目录：$(SolutionDir)Include;$(SolutionDir)$(ProjectName);$(SolutionDir)$(ProjectName)\src

【链接器】
--->常规--->附加库目录：$(SolutionDir)Lib\$(PlatformTarget)\$(Configuration)\


【Exe】
【常规】
--->输出目录：$(SolutionDir)bin\$(PlatformTarget)\$(Configuration)\
--->中间目录：$(SolutionDir)Tempout\$(PlatformTarget)\$(ProjectName)\$(Configuration)\




1) 要大胆的使用const，这将给你带来无尽的益处，但前提是你必须搞清楚原委； 
2) 要避免最一般的赋值操作错误，如将const变量赋值，具体可见思考题； 
3) 在参数中使用const应该使用引用或指针，而不是一般的对象实例，原因同上； 
4) const在成员函数中的三种用法（参数、返回值、函数）要很好的使用； 
5) 不要轻易的将函数的返回值类型定为const; 
6) 除了重载操作符外一般不要将返回值类型定为对某个对象的const引用;
