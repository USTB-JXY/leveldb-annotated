LevelDB is a fast key-value storage library written at Google that provides an ordered mapping from string keys to string values.
LevelDB是一个在Google编写的快速键值存储库，它提供了从字符串键到字符串值的有序映射。

This repository is receiving very limited maintenance. We will only review the following types of changes.
此存储库的维护非常有限。我们只会审查以下类型的更改。

Fixes for critical bugs, such as data loss or memory corruption
修复了严重错误，例如数据丢失或内存损坏
Changes absolutely needed by internally supported leveldb clients. These typically fix breakage introduced by a language/standard library/OS update
内部支持的 leveldb 客户端绝对需要的更改。这些通常修复语言/标准库/操作系统更新引入的损坏
ci

Authors: Sanjay Ghemawat (sanjay@google.com) and Jeff Dean (jeff@google.com)
作者：Sanjay Ghemawat（sanjay@google.com）和Jeff Dean（jeff@google.com）

Features 特征
Keys and values are arbitrary byte arrays.
键和值是任意字节数组。
Data is stored sorted by key.
数据存储按键排序。
Callers can provide a custom comparison function to override the sort order.
调用方可以提供自定义比较函数来覆盖排序顺序。
The basic operations are Put(key,value), Get(key), Delete(key).
基本操作是 Put(key,value) 、 Get(key) 、 Delete(key) 。
Multiple changes can be made in one atomic batch.
可以在一个原子批次中进行多个更改。
Users can create a transient snapshot to get a consistent view of data.
用户可以创建瞬态快照以获得一致的数据视图。
Forward and backward iteration is supported over the data.
支持对数据进行向前和向后迭代。
Data is automatically compressed using the Snappy compression library, but Zstd compression is also supported.
数据使用 Snappy 压缩库自动压缩，但也支持 Zstd 压缩。
External activity (file system operations etc.) is relayed through a virtual interface so users can customize the operating system interactions.
外部活动（文件系统操作等）通过虚拟接口中继，因此用户可以自定义操作系统交互。
Documentation 文档
LevelDB library documentation is online and bundled with the source code.
LevelDB库文档是在线的，并与源代码捆绑在一起。

Limitations 局限性
This is not a SQL database. It does not have a relational data model, it does not support SQL queries, and it has no support for indexes.
这不是 SQL 数据库。它没有关系数据模型，不支持 SQL 查询，也不支持索引。
Only a single process (possibly multi-threaded) can access a particular database at a time.
一次只能有一个进程（可能是多线程）访问特定数据库。
There is no client-server support builtin to the library. An application that needs such support will have to wrap their own server around the library.
库中没有内置的客户端-服务器支持。需要此类支持的应用程序必须将自己的服务器包装在库周围。
Getting the Source 获取源
git clone --recurse-submodules https://github.com/google/leveldb.git
Building 建筑
This project supports CMake out of the box.
此项目支持开箱即用的 CMake。

Build for POSIX 为POSIX构建
Quick start: 快速入门：
git submodule update --init --recursive 
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON .. && cmake --build .
Building for Windows 为窗户而构建
First generate the Visual Studio 2017 project/solution files:
首先生成 Visual Studio 2017 项目/解决方案文件：

mkdir build
cd build
cmake -G "Visual Studio 15" ..
The default default will build for x86. For 64-bit run:
默认默认值将为 x86 生成。对于 64 位运行：

cmake -G "Visual Studio 15 Win64" ..
To compile the Windows solution from the command-line:
从命令行编译 Windows 解决方案：

devenv /build Debug leveldb.sln
or open leveldb.sln in Visual Studio and build from within.
或打开 leveldb.sln 在 Visual Studio 中并从内部构建。

Please see the CMake documentation and CMakeLists.txt for more advanced usage.
请参阅 CMake 文档和 CMakeLists.txt 更高级的用法。

Contributing to the leveldb Project
为 leveldb 项目做出贡献
This repository is receiving very limited maintenance. We will only review the following types of changes.
此存储库的维护非常有限。我们只会审查以下类型的更改。

Bug fixes 错误修复
Changes absolutely needed by internally supported leveldb clients. These typically fix breakage introduced by a language/standard library/OS update
内部支持的 leveldb 客户端绝对需要的更改。这些通常修复语言/标准库/操作系统更新引入的损坏
The leveldb project welcomes contributions. leveldb's primary goal is to be a reliable and fast key/value store. Changes that are in line with the features/limitations outlined above, and meet the requirements below, will be considered.
leveldb 项目欢迎贡献。leveldb 的主要目标是成为一个可靠且快速的键/值存储。将考虑符合上述功能/限制并满足以下要求的更改。

Contribution requirements:
供款要求：

Tested platforms only. We generally will only accept changes for platforms that are compiled and tested. This means POSIX (for Linux and macOS) or Windows. Very small changes will sometimes be accepted, but consider that more of an exception than the rule.
仅限经过测试的平台。我们通常只接受对经过编译和测试的平台的更改。这意味着POSIX（适用于Linux和macOS）或Windows。有时会接受非常小的更改，但认为这更像是例外而不是规则。

Stable API. We strive very hard to maintain a stable API. Changes that require changes for projects using leveldb might be rejected without sufficient benefit to the project.
稳定的 API。我们非常努力地维护一个稳定的API。需要使用 leveldb 对项目进行更改的更改可能会被拒绝，而不会给项目带来足够的好处。

Tests: All changes must be accompanied by a new (or changed) test, or a sufficient explanation as to why a new (or changed) test is not required.
测试：所有更改都必须附有新的（或更改的）测试，或充分解释为什么不需要新的（或更改的）测试。

Consistent Style: This project conforms to the Google C++ Style Guide. To ensure your changes are properly formatted please run:
风格一致：此项目符合 Google C++ 风格指南。要确保您的更改格式正确，请运行：

clang-format -i --style=file <file>
We are unlikely to accept contributions to the build configuration files, such as CMakeLists.txt. We are focused on maintaining a build configuration that allows us to test that the project works in a few supported configurations inside Google. We are not currently interested in supporting other requirements, such as different operating systems, compilers, or build systems.
我们不太可能接受对构建配置文件的贡献，例如 CMakeLists.txt .我们专注于维护构建配置，使我们能够测试该项目是否在 Google 内部的一些受支持的配置中工作。我们目前对支持其他需求不感兴趣，例如不同的操作系统、编译器或构建系统。

Submitting a Pull Request
提交拉取请求
Before any pull request will be accepted the author must first sign a Contributor License Agreement (CLA) at https://cla.developers.google.com/.
在接受任何拉取请求之前，作者必须首先在 https://cla.developers.google.com/ 签署贡献者许可协议 （CLA）。

In order to keep the commit timeline linear squash your changes down to a single commit and rebase on google/leveldb/main. This keeps the commit timeline linear and more easily sync'ed with the internal repository at Google. More information at GitHub's About Git rebase page.
为了保持提交时间线线性将您的更改压缩到单个提交，并在google/leveldb/main上重新定位。这使提交时间线保持线性，并且更容易与Google的内部存储库同步。更多信息，请访问 GitHub 的关于 Git 变基页面。

Performance 性能
Here is a performance report (with explanations) from the run of the included db_bench program. The results are somewhat noisy, but should be enough to get a ballpark performance estimate.
以下是包含的db_bench程序运行的性能报告（带有说明）。结果有些嘈杂，但应该足以获得大致的性能估计。

Setup 设置
We use a database with a million entries. Each entry has a 16 byte key, and a 100 byte value. Values used by the benchmark compress to about half their original size.
我们使用包含一百万个条目的数据库。每个条目都有一个 16 字节的密钥和一个 100 字节的值。基准测试使用的值压缩到其原始大小的一半左右。

LevelDB:    version 1.1
Date:       Sun May  1 12:11:26 2011
CPU:        4 x Intel(R) Core(TM)2 Quad CPU    Q6600  @ 2.40GHz
CPUCache:   4096 KB
Keys:       16 bytes each
Values:     100 bytes each (50 bytes after compression)
Entries:    1000000
Raw Size:   110.6 MB (estimated)
File Size:  62.9 MB (estimated)
Write performance 写入性能
The "fill" benchmarks create a brand new database, in either sequential, or random order. The "fillsync" benchmark flushes data from the operating system to the disk after every operation; the other write operations leave the data sitting in the operating system buffer cache for a while. The "overwrite" benchmark does random writes that update existing keys in the database.
“填充”基准以顺序或随机顺序创建一个全新的数据库。“fillsync”基准测试在每次操作后将数据从操作系统刷新到磁盘;其他写入操作将数据保留在操作系统缓冲区缓存中一段时间。“覆盖”基准测试执行随机写入，以更新数据库中的现有键。

fillseq      :       1.765 micros/op;   62.7 MB/s
fillsync     :     268.409 micros/op;    0.4 MB/s (10000 ops)
fillrandom   :       2.460 micros/op;   45.0 MB/s
overwrite    :       2.380 micros/op;   46.5 MB/s
Each "op" above corresponds to a write of a single key/value pair. I.e., a random write benchmark goes at approximately 400,000 writes per second.
上面的每个“op”对应于单个键/值对的写入。即，随机写入基准测试的速度约为每秒 400，000 次写入。

Each "fillsync" operation costs much less (0.3 millisecond) than a disk seek (typically 10 milliseconds). We suspect that this is because the hard disk itself is buffering the update in its memory and responding before the data has been written to the platter. This may or may not be safe based on whether or not the hard disk has enough power to save its memory in the event of a power failure.
每个“fillsync”操作的成本（0.3毫秒）比磁盘查找（通常为10毫秒）低得多。我们怀疑这是因为硬盘本身正在其内存中缓冲更新，并在数据写入盘片之前做出响应。根据硬盘是否有足够的电量在发生电源故障时保存其内存，这可能是安全的，也可能是不安全的。

Read performance 读取性能
We list the performance of reading sequentially in both the forward and reverse direction, and also the performance of a random lookup. Note that the database created by the benchmark is quite small. Therefore the report characterizes the performance of leveldb when the working set fits in memory. The cost of reading a piece of data that is not present in the operating system buffer cache will be dominated by the one or two disk seeks needed to fetch the data from disk. Write performance will be mostly unaffected by whether or not the working set fits in memory.
我们列出了在正向和反向方向上按顺序读取的性能，以及随机查找的性能。请注意，基准测试创建的数据库非常小。因此，当工作集适合内存时，该报告描述了 leveldb 的性能。读取操作系统缓冲区缓存中不存在的数据的成本将由从磁盘获取数据所需的一个或两个磁盘寻道主导。写入性能基本上不受工作集是否适合内存的影响。

readrandom  : 16.677 micros/op;  (approximately 60,000 reads per second)
readseq     :  0.476 micros/op;  232.3 MB/s
readreverse :  0.724 micros/op;  152.9 MB/s
LevelDB compacts its underlying storage data in the background to improve read performance. The results listed above were done immediately after a lot of random writes. The results after compactions (which are usually triggered automatically) are better.
LevelDB 在后台压缩其底层存储数据，以提高读取性能。上面列出的结果是在大量随机写入后立即完成的。压缩后的结果（通常是自动触发的）更好。

readrandom  : 11.602 micros/op;  (approximately 85,000 reads per second)
readseq     :  0.423 micros/op;  261.8 MB/s
readreverse :  0.663 micros/op;  166.9 MB/s
Some of the high cost of reads comes from repeated decompression of blocks read from disk. If we supply enough cache to the leveldb so it can hold the uncompressed blocks in memory, the read performance improves again:
读取的一些高成本来自从磁盘读取的块的重复解压缩。如果我们为 leveldb 提供足够的缓存，以便它可以将未压缩的块保存在内存中，则读取性能将再次提高：

readrandom  : 9.775 micros/op;  (approximately 100,000 reads per second before compaction)
readrandom  : 5.215 micros/op;  (approximately 190,000 reads per second after compaction)
Repository contents 存储库内容
See doc/index.md for more explanation. See doc/impl.md for a brief overview of the implementation.
有关更多说明，请参阅 doc/index.md。请参阅 doc/impl.md 以获取实现的简要概述。

The public interface is in include/leveldb/*.h. Callers should not include or rely on the details of any other header files in this package. Those internal APIs may be changed without warning.
公共接口位于 include/leveldb/*.h 中。调用方不应包含或依赖此包中任何其他头文件的详细信息。这些内部 API 可能会在不发出警告的情况下进行更改。

Guide to header files:
头文件指南：

include/leveldb/db.h: Main interface to the DB: Start here.
include/leveldb/db.h：数据库的主界面：从这里开始。

include/leveldb/options.h: Control over the behavior of an entire database, and also control over the behavior of individual reads and writes.
include/leveldb/options.h：控制整个数据库的行为，也控制单个读取和写入的行为。

include/leveldb/comparator.h: Abstraction for user-specified comparison function. If you want just bytewise comparison of keys, you can use the default comparator, but clients can write their own comparator implementations if they want custom ordering (e.g. to handle different character encodings, etc.).
include/leveldb/comparator.h：用户指定的比较函数的抽象。如果您只想按字节比较键，则可以使用默认比较器，但如果客户端想要自定义排序（例如，处理不同的字符编码等），则可以编写自己的比较器实现。

include/leveldb/iterator.h: Interface for iterating over data. You can get an iterator from a DB object.
include/leveldb/iterator.h：用于迭代数据的接口。您可以从数据库对象获取迭代器。

include/leveldb/write_batch.h: Interface for atomically applying multiple updates to a database.
include/leveldb/write_batch.h：用于以原子方式将多个更新应用于数据库的接口。

include/leveldb/slice.h: A simple module for maintaining a pointer and a length into some other byte array.
include/leveldb/slice.h：一个简单的模块，用于维护指针和长度到其他字节数组。

include/leveldb/status.h: Status is returned from many of the public interfaces and is used to report success and various kinds of errors.
include/leveldb/status.h：状态从许多公共接口返回，用于报告成功和各种错误。

include/leveldb/env.h: Abstraction of the OS environment. A posix implementation of this interface is in util/env_posix.cc.
include/leveldb/env.h：操作系统环境的抽象。此接口的 posix 实现位于 util/env_posix.cc 中。

include/leveldb/table.h, include/leveldb/table_builder.h: Lower-level modules that most clients probably won't use directly.
include/leveldb/table.h， include/leveldb/table_builder.h：大多数客户端可能不会直接使用的较低级别的模块。