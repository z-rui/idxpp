# idxpp -- `makeindex`中文预处理程序。

本程序把`.idx`文件中出现的中文词汇自动转换成形如`zhong1 wen2@中文`的格式。方便接下来用`makeindex`编制按照拼音排序的索引。

## 用法

    c++ -o idxpp idxpp.cc cache.cc -licuuc -licuin # *
    latex document.ltx
    idxpp document.idx > document.idx~             # *
    cp document.idx~ document.idx                  # *
    makeindex document.idx
    latex document.ltx

未标记`*`的是原本使用LaTeX编写含索引的文档时的工作流程。为了实现预处理，首先要编译本程序。然后，在运行`makeindex`之前，使用本程序对`.idx`文件进行预处理。

## 已知问题

本程序仅为满足我个人的排版需求而编写，实现的功能有限，所以不能照顾到所有用户的需求。

### 多音字

ICU目前不能(以后也不太可能)对多音字做出正确的处理，“长度”会被转译为`zhang3 du4`。但是本程序提供了手动指定读音的方法，参看`cache.cc`。

可以尝试把`cache.cc`编译为动态链接库，这样修改该文件后无需重新编译主程序。

### 按其他方法排序

本程序依赖[ICU](http://icu-project.org/)将汉语转译为拼音。如果需要使用其他拉丁化方案，可以修改`translit`函数。

不太可能实现按笔画排序。这是`makeindex`自身的限制。可以考虑使用[`zhmakeindex`](https://github.com/leo-liu/zhmakeindex)。

### 多语言混排

英文单词会夹杂在中文词语中。如`change`会被排在“嫦娥”之后。

不太可能实现中英文分别排序。可以考虑将不同语言的索引分开，或者使用`zhmakeindex`。

## 版权

我，本程序的作者，将代码版权释放至公有领域(public domain)。
