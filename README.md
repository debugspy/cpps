# cpps script
#### **欢迎**

CPPS 是一个功能强大，运行速度快，容易上手的解释型脚本语言，非常容易学习和使用，并且可以很容易的嵌入到C++项目中。

CPPS 被设计的初衷是让那些和作者一样不喜欢LUA,PY,等等脚本语法的开发者使用，它可以对接各种项目，无论游戏，WEB，或者是人工智能方面。

它的语法极为像C++,所以C++开发者可以很快速的上手，无需太深入的学习它。

#### **下载与编译**

CPPS 的下载与安装非常简单，CPPS不依赖于任何第三方库，仅仅下载就可以编译。

*LINUX版本*：
-
```
git clone https://github.com/johnsonyl/cpps.git

make （编译可执行文件） 

或者 （so库文件）

 make lib 

 make install
```
*Windows版本*
-
```
git clone https://github.com/johnsonyl/cpps.git

使用Visual Studio打开cpps.sln进行编译即可。
```

是不是非常简单？

项目中demo文件夹 有如何嵌入CPPS的方法。支持 WINDOWS 与 LINUX。

Hello World 为嵌入CPPS实例
 
demo 为如何开发第一个CPPS库文件。


*更新日志*：
-

2020-11-15 更新

添加json支持库

增加 import 导入支持库功能

```
import ["json", "demo"];
//or
//import "json";
//import "demo";
```

2020-11-14 更新

1.增加linux 动态库功能

详情见demo/demo目录

脚本中使用方法

```
loadlibrary("demo");
demotest();
freelibrary("demo");
```

2.增加demo/hello world 对linux的编译支持

3.修改cpps make时编译文件目录到bin文件夹



2020-11-12 更新

1.支持类似py,php的字符串format处理功能

话不多说，上代码

```

class A
{
	var name = "john";
	var age = 32;
}
var a = new A();
var s = "my name is {a.name} , i'm {a.age} years old.";
println(s);

输出：
[Cpps 1.2.0 Copyright (C) 2015-2020 By Johnson, Pub-test]
my name is john , i'm 32 years old.
>

值得注意： { 与 } 需要使用 \{ 与 \} 来特殊处理了。
```

2020-11-10 更新2

1.修复父类构造函数执行顺序

2.增加调用父类虚函数的功能

3.增加父类对名空间的支持

4.foreach 还是修改成C++ 方式。 并且 : 后为表达式

```

foreach(var item : [1,2])
{
	println(item);
	foreach(var item2 : [3,4])
	{
		println(item2);
	}
}


foreach(var item : {a:1,b:"2"})
{
	println(item.first());
	println(item.second());
}

var b = {a:1,b:"2"};

foreach(var item : b)
{
	println(item.first());
	println(item.second());
}

```

```
namespace cpps
{
	class C
	{
		C()
		{
			println("C class");
		}
		var test()
		{
			println("C->test()");
			
		}
	}
}
class A : cpps::C
{
	var name;
	A(int a){
		println("A class:"..name);
	}
	
	var test()
	{
		println("A->test()");
				
		println(name);
	}
}

class B:A
{
	var age;
	B(){
		A::constructor(10); //如果构造函数非0参数时，需要手动调用，否则不会调用
		println("B class");
	}
	var test()
	{
		println("B->test()");
		println(age);
		cpps::C::test();
	}
}


var b = new B(){
	name = "john",
	age = 32
};

b.test();

输出：
[Cpps 1.2.0 Copyright (C) 2015-2020 By Johnson, Pub-test]
C class
A class:john
B class
B->test()
32
C->test()
>
```

2020-11-10 更新

重写foreach 

老版本使用lambda 方式，很不好。

修改成 

见上方新方法

增加 new 时设置变量功能类似c#

```
class A
{
    var name;
    var age;
}
var a = new A(){
    name = "john",
    age = 32
}
```

2020-1-28 更新

新年快乐~~ happy new year!

1.今天恢复了对linux gcc++ 的编译支持。

对linux实在不太熟悉。一堆警告。很糟心。

2.增加make lib 与 make install指令

3.cpps 默认工作目录为指定文件目录。

2020-1-21 更新

解决了&&与|| 未按照顺序执行导致逻辑错误的BUG。

2020-1-20 更新

增加const的支持。

const var abc = "10"; //不可修改。

2020-1-19 更新

1.增加调用函数参数支持到11个

2.增加支持构造函数。 c++ 部分需要注册一个名为constructor的函数当构造函数

3.并且修复多层名空间有错误的BUG。

4.增加类似js 当创建出匿名函数后并且调用的方法 例子：
```
[](var a){

	println(a);

}(123);
```


2019-12-7 更新

之前老版本是在函数结束后才调用GC，那么如果想在主函数写一个死循环执行逻辑就会造成内存永不会被GC释放，现在会在符合内存大小等限制情况下调用GC释放内存了。具体大小见CPPS_GEN0_CHECKSIZE ，等宏定义。

2019-11-01

修复lambda函数在传递时当前与偶发崩溃的BUG。

增加foreach 函数支持break操作

前面更新就不介绍了。乱七八糟的。

```
var i = toint(io.getc());

if(i == 10)
{
	println("i 等于 10");
}
else if(i > 10)
{
	println("i 大于 10");
}
else
{
	println("i 小于 10");
}

for(var i = 0;i < 10;i++)
{
	println(i);
}

class abc
{
	var a;
    var b = 10; //可以定义默认值
    var testfunc(var c)
    {
    	println(c);
        println(a);
        println(this.b);
    }
}
var obj = new abc;
obj.testfunc(10);

//继承关系
class abcd : abc
{
	var d = 10;
}

var obj2 = new abcd;
obj2.testfunc(20);
println(obj2.d);

var str = "hello world"; //字符串类型
var i = 10; //整数类型
var n = 3.1415926; //浮点类型
var b = true; //布尔类型
var g = new map; //对象类型
class abc
{
	var a = 10;
}
var a = new abc; //类对象类型
// 2-9日更新 可定义多个变量
var a1 = 0, a2 = 10,a3 = 20;
//8月31日更新
var b = 0xffff;

var str = "hello";
println(str.."world");

var o = str.."haha".."ooo"..str;
println(o);

var testfunc(var i)
{
	println(i);
}

testfunc(10);

更多例子，请查看脚本文档。。。


```

脚本文档在

http://www.kancloud.cn/johnsonyl/cpps/260361

QQ群：

CPPS脚本交流群 282081601

作者Q：88481106

有兴趣可以加群或者QQ聊聊。

脚本完全是自己的兴趣开发，不建议商用，可以当学习和娱乐使用。


