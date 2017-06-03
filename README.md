# Notice
* This is a course project.
* This project is built in Visual Studio, and only yacc file, lex file and source/header files are commited.


## 5.15 更新

* 重写了`frame.[ly]` 基本上完成了词法分析和语法分析的内容
  * 消除了manual中的左递归文法
  * 但是没有完成string的处理
  * 没有完成嵌套注释的处理
* 参考虎书中提供的`absyn.[ch]` `prabsyn.[ch]` `symbol.[ch]` 文件，实现了抽象语法树的生成与遍历。

## IR Tree

本部分的设计为根据需求逐步递增的开发，因此以下的模块需要随着支持语句的最多而扩展。

* `struct T_stm_` 需要增加新的支持的内容
* `struct T_exp_`
* 以及还需要增加一些解析的函数
