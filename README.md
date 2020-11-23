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

cmake .

make

make install
```
*Windows版本*
-
```
git clone https://github.com/johnsonyl/cpps.git

cmake .

使用Visual Studio打开cpps.sln进行编译即可。
```

是不是非常简单？

项目中demo文件夹 有如何嵌入CPPS的方法。支持 WINDOWS 与 LINUX。

example/cppsc 为嵌入CPPS实例
 
example/demo 为如何开发第一个CPPS库文件。

lib/curl 为libcurl封装库 需要安装编译libcurl

lib/json 为json解析封装库

lib/socket 为libevent封装库 需要安装编译libevent



*更新日志*：
-

2020-11-24 更新
-

1.增加hashlib库 md5 sha1 sha224 sha256 sha384 sha512

```
system("chcp 65001"); //use utf8 string

import "hashlib";


var md5 = new hashlib::md5();
md5.update("123456");
println(md5.hexdigest()); //e10adc3949ba59abbe56e057f20f883e


var sha1 = new hashlib::sha1();
sha1.update("123456");
println(sha1.hexdigest()); //7c4a8d09ca3762af61e59520943dc26494f8941b


var sha224 = new hashlib::sha224();
sha224.update("123456");
println(sha224.hexdigest());//f8cdb04495ded47615258f9dc6a3f4707fd2405434fefc3cbf4ef4e6


var sha256 = new hashlib::sha256();
sha256.update("123456");
println(sha256.hexdigest()); //8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92


var sha384 = new hashlib::sha384();
sha384.update("123456");
println(sha384.hexdigest());//0a989ebc4a77b56a6e2bb7b19d995d185ce44090c13e2984b7ecc6d446d4b61ea9991b76a4c2f04b1b4d244841449454


var sha512 = new hashlib::sha512();
sha512.update("123456");
println(sha512.hexdigest());//ba3253876aed6bc22d4a6ff53d8406c6ad864195ed144ab5c87621b6c233b548baeae6956df346ec8c17f5ea10f35ee3cbc514797ed7ddd3145464e2a0bab413
```

2020-11-23 更新
-

1.增加base64模块

2.增加mysql模块

3.优化cpps

4.修复字符串format 调用函数找不到变量的BUG.

```
//base64例子
system("chcp 65001"); //use utf8 string

import ["base64"];


var s = "abc";
println("source:{s}");
var encode_s = base64.encode(s);
println("encode_s:{encode_s}");
var deocde_s = base64.decode(encode_s);
println("deocde_s:{deocde_s}");

```

```
//mysql例子
import ["mysql"];


var option = new MysqlOption()
{
	db = "dbname",
	host = "127.0.0.1",
	user = "root",
	passwd = "*******",
	prot = 3306,
	isreconnect = true
}
var conn = mysql.connect(option);
if(conn == null)
{
	println("connect error");
	exit(0);
}

var results = conn.call("selectuser",["john"])
println(conn.error());
foreach(var result : results)
{
	foreach(var record : result.records())
	{
		println("----------------------------------------");
		foreach(var field: record.fields())
		{
			println("{field.first()}:{field.second()}");
		}
	}
}


var results = conn.execute("select * from users where username = 'john';")
println(conn.error());
foreach(var result : results)
{
	foreach(var record : result.records())
	{
		println("id:"..record.get("id"));
		println("username:"..record.get("username"));
		println("userpwd:"..record.get("userpwd"));
	}
}


```

2020-11-22 更新2
-

1.增加http模块 httprequest类

```
system("chcp 65001"); //use utf8 string

import ["http"];

var request = new http::httprequest();
request.setcookiesfile("cookies.txt");
//request.setproxy(httpproxy.SOCK5,"192.168.1.166:25455");
//request.setproxyaccount("johnsonyl","mima");
request.addheaders({User-Agent:"cppsrequest/1.1.0"});
var ret = request.get("http://192.168.1.166:8080/Home/test?kkk=100");


println("-------------------GET-------------------------");
println("get:{ret}");
var cookies = request.getcookies();
println("cookies:{cookies}");

var headers = request.getheaders();
println("headers:{headers}");


println("-------------------POST-------------------------");
ret = request.post("http://192.168.1.166:8080/Home/test","kkk=100");
println("post:{ret}");
var cookies = request.getcookies();
println("cookies:{cookies}");

var headers = request.getheaders();
println("headers:{headers}");
println("-------------------END-------------------------");

```

2020-11-22 更新
-

1.增加httpserver

2.增加cpps 一些对外函数

3.修复一些已知BUG

```
system("chcp 65001"); //use utf8 string

import ["socket"];

class Home
{
	var name;
	Home()
	{
		name = "I'm John ,32 years old."
	}
	//http://127.0.0.1:8080 or http://127.0.0.1:8080/Home or http://127.0.0.1:8080/Home/Index
	var Index(var request)
	{
		println("Home/Index..");
		println(name);
		request.addheader({
			Server:"John Server",
			Content-Type:"text/plain; charset=UTF-8",
			Connection:"close"
		});

		request.append(name.."\n");

		request.send(200,"OK");
	}
}

class Student
{
	var name;
	var age;
	Student()
	{
		name = "xiaoming";
		age = 18;
	}
	//http://127.0.0.1:8080/Student/ or http://127.0.0.1:8080/Student/Index
	var Index(var request)
	{
		
		request.addheader({
			Server:"John Server",
			Content-Type:"text/plain; charset=UTF-8",
			Connection:"close"
		});

		request.append("i'm {name}, {age} years old.");

		request.send(200,"OK");
	}
	//http://127.0.0.1:8080/Student/Name
	var Name(var request)
	{
		request.addheader({
			Server:"John Server",
			Content-Type:"text/plain; charset=UTF-8",
			Connection:"close"
		});

		request.append(name);

		request.send(200,"OK");
	}
	//http://127.0.0.1:8080/Student/Age
	var Age(var request)
	{
		request.addheader({
			Server:"John Server",
			Content-Type:"text/plain; charset=UTF-8",
			Connection:"close"
		});

		request.append(age);

		request.send(200,"OK");
	}
}
//http://127.0.0.1:8080/Home/test?kkk=100
var test(var request)
{
	request.addheader({
			Server:"John Server",
			Content-Type:"text/plain; charset=UTF-8",
			Connection:"close"
		});

	request.append("test page.\n");
	request.append("path:"..request.getpath().."\n");
	request.append("uri:"..request.geturi().."\n");
	request.append("kkk:"..request.getparam("kkk").."\n");
	request.append("User-Agent header:"..request.getheader("User-Agent").."\n");
	request.append("getbuffer():"..request.getbuffer().."\n");

	request.send(200,"OK");
}

var option = new HttpServerOption()
{
	exceptionfunc = [](var request,var errmsg)
	{
		println(errmsg);

		request.addheader({
			Server:"John Server",
			Content-Type:"text/plain; charset=UTF-8",
			Connection:"close"
		});

		request.append(errmsg);

		request.send(500,"SERVER ERROR");
	},
	notfoundfunc = [](var request)
	{

		request.addheader({
			Server:"John Server",
			Content-Type:"text/plain; charset=UTF-8",
			Connection:"close"
		});

		request.append("there's nothing here.");

		request.send(404,"Not Found");
	}
}

var http = new socket::httpserver().setoption(option).listen(8080);


http.register_controller_default(Home);
http.register_controller(Student);
http.register_handlefunc("/Home/test",test);

while (true){
	http.run();
}

```

2020-11-20 更新
-

1.修改项目结构

2.修改编译为cmake方式

2020-11-19 更新
-

1.增加第一版socket库基于libevent.
2.修复一些BUG

```
system("chcp 65001"); //use utf8 string

import ["socket"];

/*

//服务端部分
var socket_accept(var socketIndex)
{
	println("检测到有一个连接:"..socketIndex);
}
var socket_data(var socketIndex,var buffer) 
{
	var s = buffer.tostring();
	println(s);

	if(s == "e")
	{
		srv.closesocket(socketIndex);
	}
	
	var writer = new Buffer();
	writer.writeString(buffer.tostring());
	srv.send(socketIndex, writer);
}
var socket_close(var socketIndex,var err,var errstr) 
{
	println("检测到有一个关闭:{socketIndex},err:{err},errstr:{errstr}");
}
var socket_parser(var headerbuffer)
{

}
println("start server");
var srv = new socket::server().setoption(new ServerOption(){
							ip = "0.0.0.0",
							accept = socket_accept,
							data = socket_data,
							close = socket_close
							}).listen(4060);

// var srv = new socket::server().setOption(new ServerOption(){
// 							ip = "0.0.0.0",
// 							headersize = 8,//字节
// 							accept = socket_accept,
// 							data = socket_data,
// 							close = socket_close,
// 							parser = socket_parser
// 							}).listen(4060);

println("start over");
while (true)
	srv.run();
*/
//客户端部分
var socket_connected()
{
	println("连接成功啦");

	var writer = new Buffer();
	writer.writeString("这是一个乒乓消息");
	client.send(writer);
}
var socket_data(var buffer)
{
	var s = buffer.tostring();
	println(s);

	Sleep(5000);

	var writer = new Buffer();
	writer.writeString(buffer.tostring());
	client.send(writer);
}
var socket_close(var err,var errstr)
{
	println("检测到有一个关闭:err:{err},errstr:{errstr}");
}

var client = new socket::client();
client.setoption(new ClientOption(){
							connected = socket_connected,
							data = socket_data,
							close = socket_close
							});

var b = client.connect("192.168.1.166",4060);



while (true)
{
		client.run();
}
```

2020-11-16 更新
-

添加curl支持库

```
import ["json","demo","curl"];
//or
//import "json";
//import "demo";


var url = curl.curl_easy_init();

curl.curl_easy_setopt(url, curl.CURLOPT_URL, "http://blog.chinaunix.net");
curl.curl_easy_setopt(url, curl.CURLOPT_VERBOSE, 0);
var res = curl.curl_easy_perform(url);
println(res);
println(url.getdata());


curl.curl_easy_cleanup(url);
```

2020-11-15 更新
-

添加json支持库

简化json.encode默认支持为utf8格式

增加 import 导入支持库功能

```
import ["json", "demo"];
//or
//import "json";
//import "demo";

var arr = [1, 2, 3, 4, 5, 6, 7];
var obj = { a : "我是开发者", b : "hehe" };


println(arr);
println(obj);


var arr2 = json.encode(arr);
var obj2 = json.encode(obj);



println(arr2);
println(obj2);


println(json.decode(arr2));
println(json.decode(obj2));


```

2020-11-14 更新
-

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
-

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
-

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
-

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
-

新年快乐~~ happy new year!

1.今天恢复了对linux gcc++ 的编译支持。

对linux实在不太熟悉。一堆警告。很糟心。

2.增加make lib 与 make install指令

3.cpps 默认工作目录为指定文件目录。

2020-1-21 更新
-

解决了&&与|| 未按照顺序执行导致逻辑错误的BUG。

2020-1-20 更新
-

增加const的支持。

const var abc = "10"; //不可修改。

2020-1-19 更新
-

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
-

之前老版本是在函数结束后才调用GC，那么如果想在主函数写一个死循环执行逻辑就会造成内存永不会被GC释放，现在会在符合内存大小等限制情况下调用GC释放内存了。具体大小见CPPS_GEN0_CHECKSIZE ，等宏定义。

2019-11-01
-

修复lambda函数在传递时当前与偶发崩溃的BUG。

增加foreach 函数支持break操作

前面更新就不介绍了。乱七八糟的。

2017-8-31
-
1.新增CPPS内部函数resume() 在内部抛出thorw异常的时候使用resume可解决GC异常问题

2.新增assert函数

3.新增部分数学库

4.新增CPPS定义库变量 类似math.pi 

5.修复vector变量[] 括号内不能使用公式的BUG

6.新增string.format字符串格式化功能

7.修复三元表达式与namespace在某些情况下冲突的问题

8.增加16进制变量值

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


