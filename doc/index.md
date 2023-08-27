leveldb 水平数据库
Jeff Dean, Sanjay Ghemawat
杰夫·迪恩，桑杰·盖马瓦特

The leveldb library provides a persistent key value store. Keys and values are arbitrary byte arrays. The keys are ordered within the key value store according to a user-specified comparator function.
leveldb 库提供了一个持久的键值存储。键和值是任意字节数组。键根据用户指定的比较器功能在键值存储中排序。

Opening A Database 打开数据库
A leveldb database has a name which corresponds to a file system directory. All of the contents of database are stored in this directory. The following example shows how to open a database, creating it if necessary:
leveldb 数据库的名称对应于文件系统目录。数据库的所有内容都存储在此目录中。下面的示例演示如何打开数据库，并在必要时创建数据库：

#include <cassert>
#include "leveldb/db.h"

leveldb::DB* db;
leveldb::Options options;
options.create_if_missing = true;
leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
assert(status.ok());
...
If you want to raise an error if the database already exists, add the following line before the leveldb::DB::Open call:
如果要在数据库已存在时引发错误，请在 leveldb::DB::Open 调用之前添加以下行：

options.error_if_exists = true;
Status 地位
You may have noticed the leveldb::Status type above. Values of this type are returned by most functions in leveldb that may encounter an error. You can check if such a result is ok, and also print an associated error message:
您可能已经注意到上面的 leveldb::Status 类型。这种类型的值由 leveldb 中可能遇到错误的大多数函数返回。您可以检查这样的结果是否正常，还可以打印相关的错误消息：

leveldb::Status s = ...;
if (!s.ok()) cerr << s.ToString() << endl;
Closing A Database 关闭数据库
When you are done with a database, just delete the database object. Example:
使用完数据库后，只需删除数据库对象即可。例：

... open the db as described above ...
... do something with db ...
delete db;
Reads And Writes 读取和写入
The database provides Put, Delete, and Get methods to modify/query the database. For example, the following code moves the value stored under key1 to key2.
数据库提供 Put、Delete 和 Get 方法来修改/查询数据库。例如，下面的代码将存储在 key1 下的值移动到 key2。

std::string value;
leveldb::Status s = db->Get(leveldb::ReadOptions(), key1, &value);
if (s.ok()) s = db->Put(leveldb::WriteOptions(), key2, value);
if (s.ok()) s = db->Delete(leveldb::WriteOptions(), key1);
Atomic Updates 原子更新
Note that if the process dies after the Put of key2 but before the delete of key1, the same value may be left stored under multiple keys. Such problems can be avoided by using the WriteBatch class to atomically apply a set of updates:
请注意，如果进程在 Put of key2 之后但在删除 key1 之前死亡，则相同的值可能会存储在多个键下。通过使用 WriteBatch 类以原子方式应用一组更新，可以避免此类问题：

#include "leveldb/write_batch.h"
...
std::string value;
leveldb::Status s = db->Get(leveldb::ReadOptions(), key1, &value);
if (s.ok()) {
  leveldb::WriteBatch batch;
  batch.Delete(key1);
  batch.Put(key2, value);
  s = db->Write(leveldb::WriteOptions(), &batch);
}
The WriteBatch holds a sequence of edits to be made to the database, and these edits within the batch are applied in order. Note that we called Delete before Put so that if key1 is identical to key2, we do not end up erroneously dropping the value entirely.
保存 WriteBatch 要对数据库进行的一系列编辑，批处理中的这些编辑将按顺序应用。请注意，我们在 Put 之前调用了 Delete，这样如果 key1 与 key2 相同，我们最终不会错误地完全删除该值。

Apart from its atomicity benefits, WriteBatch may also be used to speed up bulk updates by placing lots of individual mutations into the same batch.
除了原子性优势外， WriteBatch 还可用于通过将大量单个突变放入同一批次来加速批量更新。

Synchronous Writes 同步写入
By default, each write to leveldb is asynchronous: it returns after pushing the write from the process into the operating system. The transfer from operating system memory to the underlying persistent storage happens asynchronously. The sync flag can be turned on for a particular write to make the write operation not return until the data being written has been pushed all the way to persistent storage. (On Posix systems, this is implemented by calling either fsync(...) or fdatasync(...) or msync(..., MS_SYNC) before the write operation returns.)
默认情况下，每次对 leveldb 的写入都是异步的：它将写入从进程推送到操作系统后返回。从操作系统内存到基础持久存储的传输以异步方式进行。可以为特定写入打开同步标志，以使写入操作在写入的数据一直推送到持久存储之前不会返回。（在 Posix 系统上，这是通过在写入操作返回之前调用 fsync(...) or fdatasync(...) 或 来实现 msync(..., MS_SYNC) 的。

leveldb::WriteOptions write_options;
write_options.sync = true;
db->Put(write_options, ...);
Asynchronous writes are often more than a thousand times as fast as synchronous writes. The downside of asynchronous writes is that a crash of the machine may cause the last few updates to be lost. Note that a crash of just the writing process (i.e., not a reboot) will not cause any loss since even when sync is false, an update is pushed from the process memory into the operating system before it is considered done.
异步写入的速度通常是同步写入的千倍以上。异步写入的缺点是计算机崩溃可能会导致最后几个更新丢失。请注意，仅写入进程的崩溃（即，不是重新启动）不会造成任何损失，因为即使同步为 false，更新也会在被视为完成之前从进程内存推送到操作系统。

Asynchronous writes can often be used safely. For example, when loading a large amount of data into the database you can handle lost updates by restarting the bulk load after a crash. A hybrid scheme is also possible where every Nth write is synchronous, and in the event of a crash, the bulk load is restarted just after the last synchronous write finished by the previous run. (The synchronous write can update a marker that describes where to restart on a crash.)
异步写入通常可以安全地使用。例如，将大量数据加载到数据库中时，可以通过在崩溃后重新启动批量加载来处理丢失的更新。也可以使用混合方案，其中每 N 次写入是同步的，并且在发生崩溃时，在上次运行完成最后一次同步写入后立即重新启动批量加载。（同步写入可以更新描述崩溃时重新启动位置的标记。

WriteBatch provides an alternative to asynchronous writes. Multiple updates may be placed in the same WriteBatch and applied together using a synchronous write (i.e., write_options.sync is set to true). The extra cost of the synchronous write will be amortized across all of the writes in the batch.
WriteBatch 提供异步写入的替代方法。可以将多个更新放置在同一个 WriteBatch 中，并使用同步写入（即设置为 write_options.sync true）一起应用。同步写入的额外成本将在批处理中的所有写入中摊销。

Concurrency 并发
A database may only be opened by one process at a time. The leveldb implementation acquires a lock from the operating system to prevent misuse. Within a single process, the same leveldb::DB object may be safely shared by multiple concurrent threads. I.e., different threads may write into or fetch iterators or call Get on the same database without any external synchronization (the leveldb implementation will automatically do the required synchronization). However other objects (like Iterator and WriteBatch) may require external synchronization. If two threads share such an object, they must protect access to it using their own locking protocol. More details are available in the public header files.
一个数据库一次只能由一个进程打开。leveldb 实现从操作系统获取锁以防止滥用。在单个进程中，同一 leveldb::DB 对象可以由多个并发线程安全地共享。即，不同的线程可以在没有任何外部同步的情况下写入或获取迭代器或调用同一个数据库的 Get（leveldb 实现将自动执行所需的同步）。但是，其他对象（如 Iterator 和 WriteBatch ）可能需要外部同步。如果两个线程共享此类对象，则必须使用自己的锁定协议保护对该对象的访问。公共头文件中提供了更多详细信息。

Iteration 迭 代
The following example demonstrates how to print all key,value pairs in a database.
下面的示例演示如何打印数据库中的所有键、值对。

leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
for (it->SeekToFirst(); it->Valid(); it->Next()) {
  cout << it->key().ToString() << ": "  << it->value().ToString() << endl;
}
assert(it->status().ok());  // Check for any errors found during the scan
delete it;
The following variation shows how to process just the keys in the range [start,limit):
以下变体显示了如何仅处理范围 [start，limit] 中的键：

for (it->Seek(start);
   it->Valid() && it->key().ToString() < limit;
   it->Next()) {
  ...
}
You can also process entries in reverse order. (Caveat: reverse iteration may be somewhat slower than forward iteration.)
您还可以按相反的顺序处理条目。（警告：反向迭代可能比正向迭代慢一些。

for (it->SeekToLast(); it->Valid(); it->Prev()) {
  ...
}
Snapshots 快照
Snapshots provide consistent read-only views over the entire state of the key-value store. ReadOptions::snapshot may be non-NULL to indicate that a read should operate on a particular version of the DB state. If ReadOptions::snapshot is NULL, the read will operate on an implicit snapshot of the current state.
快照为键值存储的整个状态提供一致的只读视图。 ReadOptions::snapshot 可能为非 NULL，以指示读取应对特定版本的数据库状态进行操作。如果为 NULL，则 ReadOptions::snapshot 读取将对当前状态的隐式快照进行操作。

Snapshots are created by the DB::GetSnapshot() method:
快照由以下 DB::GetSnapshot() 方法创建：

leveldb::ReadOptions options;
options.snapshot = db->GetSnapshot();
... apply some updates to db ...
leveldb::Iterator* iter = db->NewIterator(options);
... read using iter to view the state when the snapshot was created ...
delete iter;
db->ReleaseSnapshot(options.snapshot);
Note that when a snapshot is no longer needed, it should be released using the DB::ReleaseSnapshot interface. This allows the implementation to get rid of state that was being maintained just to support reading as of that snapshot.
请注意，当不再需要快照时，应使用 DB::ReleaseSnapshot 界面释放快照。这允许实现摆脱仅为了支持从该快照开始读取而维护的状态。

Slice 片
The return value of the it->key() and it->value() calls above are instances of the leveldb::Slice type. Slice is a simple structure that contains a length and a pointer to an external byte array. Returning a Slice is a cheaper alternative to returning a std::string since we do not need to copy potentially large keys and values. In addition, leveldb methods do not return null-terminated C-style strings since leveldb keys and values are allowed to contain '\0' bytes.
上面的 it->key() and it->value() 调用的返回值是该 leveldb::Slice 类型的实例。Slice 是一个简单的结构，包含一个长度和一个指向外部字节数组的指针。返回切片是返回 a std::string 的更便宜的替代方法，因为我们不需要复制潜在的大键和值。此外，leveldb 方法不返回以 null 结尾的 C 样式字符串，因为 leveldb 键和值允许包含 '\0' 字节。

C++ strings and null-terminated C-style strings can be easily converted to a Slice:
C++字符串和以 null 结尾的 C 样式字符串可以轻松转换为切片：

leveldb::Slice s1 = "hello";

std::string str("world");
leveldb::Slice s2 = str;
A Slice can be easily converted back to a C++ string:
切片可以很容易地转换回C++字符串：

std::string str = s1.ToString();
assert(str == std::string("hello"));
Be careful when using Slices since it is up to the caller to ensure that the external byte array into which the Slice points remains live while the Slice is in use. For example, the following is buggy:
使用切片时要小心，因为由调用方来确保切片指向的外部字节数组在使用切片时保持活动状态。例如，以下内容是错误的：

leveldb::Slice slice;
if (...) {
  std::string str = ...;
  slice = str;
}
Use(slice);
When the if statement goes out of scope, str will be destroyed and the backing storage for slice will disappear.
当 if 语句超出范围时，str 将被销毁，切片的后备存储将消失。

Comparators 比较
The preceding examples used the default ordering function for key, which orders bytes lexicographically. You can however supply a custom comparator when opening a database. For example, suppose each database key consists of two numbers and we should sort by the first number, breaking ties by the second number. First, define a proper subclass of leveldb::Comparator that expresses these rules:
前面的示例使用了 key 的默认排序函数，该函数按字典顺序对字节进行排序。但是，您可以在打开数据库时提供自定义比较器。例如，假设每个数据库键由两个数字组成，我们应该按第一个数字排序，按第二个数字断开连接。首先，定义一个表示这些规则的 leveldb::Comparator 适当子类：

class TwoPartComparator : public leveldb::Comparator {
 public:
  // Three-way comparison function:
  //   if a < b: negative result
  //   if a > b: positive result
  //   else: zero result
  int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const {
    int a1, a2, b1, b2;
    ParseKey(a, &a1, &a2);
    ParseKey(b, &b1, &b2);
    if (a1 < b1) return -1;
    if (a1 > b1) return +1;
    if (a2 < b2) return -1;
    if (a2 > b2) return +1;
    return 0;
  }

  // Ignore the following methods for now:
  const char* Name() const { return "TwoPartComparator"; }
  void FindShortestSeparator(std::string*, const leveldb::Slice&) const {}
  void FindShortSuccessor(std::string*) const {}
};
Now create a database using this custom comparator:
现在使用此自定义比较器创建一个数据库：

TwoPartComparator cmp;
leveldb::DB* db;
leveldb::Options options;
options.create_if_missing = true;
options.comparator = &cmp;
leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
...
Backwards compatibility 向后兼容性
The result of the comparator's Name method is attached to the database when it is created, and is checked on every subsequent database open. If the name changes, the leveldb::DB::Open call will fail. Therefore, change the name if and only if the new key format and comparison function are incompatible with existing databases, and it is ok to discard the contents of all existing databases.
比较器的 Name 方法的结果在创建数据库时附加到数据库，并在每次打开后续数据库时进行检查。如果名称更改， leveldb::DB::Open 调用将失败。因此，当且仅当新的密钥格式和比较功能与现有数据库不兼容时，更改名称，并且可以丢弃所有现有数据库的内容。

You can however still gradually evolve your key format over time with a little bit of pre-planning. For example, you could store a version number at the end of each key (one byte should suffice for most uses). When you wish to switch to a new key format (e.g., adding an optional third part to the keys processed by TwoPartComparator), (a) keep the same comparator name (b) increment the version number for new keys (c) change the comparator function so it uses the version numbers found in the keys to decide how to interpret them.
但是，您仍然可以通过一些预先规划来随着时间的推移逐渐发展您的密钥格式。例如，您可以在每个键的末尾存储一个版本号（一个字节应该足以满足大多数用途）。当您希望切换到新的键格式时（例如，向 处理的 TwoPartComparator 键添加可选的第三部分），（a） 保持相同的比较器名称 （b） 递增新键的版本号 （c） 更改比较器函数，使其使用在键中找到的版本号来决定如何解释它们。

Performance 性能
Performance can be tuned by changing the default values of the types defined in include/options.h.
可以通过更改 中 include/options.h 定义的类型的默认值来调整性能。

Block size 块大小
leveldb groups adjacent keys together into the same block and such a block is the unit of transfer to and from persistent storage. The default block size is approximately 4096 uncompressed bytes. Applications that mostly do bulk scans over the contents of the database may wish to increase this size. Applications that do a lot of point reads of small values may wish to switch to a smaller block size if performance measurements indicate an improvement. There isn't much benefit in using blocks smaller than one kilobyte, or larger than a few megabytes. Also note that compression will be more effective with larger block sizes.
leveldb 将相邻的密钥组合到同一个块中，这样的块是与持久存储之间的传输单元。默认块大小约为 4096 个未压缩字节。主要对数据库内容执行批量扫描的应用程序可能希望增加此大小。如果性能测量表明有所改进，则对小值进行大量点读取的应用程序可能希望切换到较小的块大小。使用小于一千字节或大于几兆字节的块没有太大好处。另请注意，对于较大的块大小，压缩将更有效。

Compression 压缩
Each block is individually compressed before being written to persistent storage. Compression is on by default since the default compression method is very fast, and is automatically disabled for uncompressible data. In rare cases, applications may want to disable compression entirely, but should only do so if benchmarks show a performance improvement:
每个块在写入持久存储之前都会单独压缩。默认情况下，压缩处于打开状态，因为默认压缩方法非常快，并且对于不可压缩的数据会自动禁用。在极少数情况下，应用程序可能希望完全禁用压缩，但只有在基准测试显示性能改进时才应这样做：

leveldb::Options options;
options.compression = leveldb::kNoCompression;
... leveldb::DB::Open(options, name, ...) ....
Cache 缓存
The contents of the database are stored in a set of files in the filesystem and each file stores a sequence of compressed blocks. If options.block_cache is non-NULL, it is used to cache frequently used uncompressed block contents.
数据库的内容存储在文件系统中的一组文件中，每个文件存储一系列压缩块。如果options.block_cache为非 NULL，则用于缓存常用的未压缩块内容。

#include "leveldb/cache.h"

leveldb::Options options;
options.block_cache = leveldb::NewLRUCache(100 * 1048576);  // 100MB cache
leveldb::DB* db;
leveldb::DB::Open(options, name, &db);
... use the db ...
delete db
delete options.block_cache;
Note that the cache holds uncompressed data, and therefore it should be sized according to application level data sizes, without any reduction from compression. (Caching of compressed blocks is left to the operating system buffer cache, or any custom Env implementation provided by the client.)
请注意，缓存保存未压缩的数据，因此应根据应用程序级别的数据大小调整其大小，而不会因压缩而减少任何内容。（压缩块的缓存留给操作系统缓冲区缓存或客户端提供的任何自定义 Env 实现。

When performing a bulk read, the application may wish to disable caching so that the data processed by the bulk read does not end up displacing most of the cached contents. A per-iterator option can be used to achieve this:
执行批量读取时，应用程序可能希望禁用缓存，以便批量读取处理的数据最终不会取代大多数缓存内容。每个迭代器选项可用于实现此目的：

leveldb::ReadOptions options;
options.fill_cache = false;
leveldb::Iterator* it = db->NewIterator(options);
for (it->SeekToFirst(); it->Valid(); it->Next()) {
  ...
}
delete it;
Key Layout 按键布局
Note that the unit of disk transfer and caching is a block. Adjacent keys (according to the database sort order) will usually be placed in the same block. Therefore the application can improve its performance by placing keys that are accessed together near each other and placing infrequently used keys in a separate region of the key space.
请注意，磁盘传输和缓存的单位是一个块。相邻键（根据数据库排序顺序）通常将放在同一个块中。因此，应用程序可以通过将一起访问的键放在彼此附近并将不常用的键放在键空间的单独区域中来提高其性能。

For example, suppose we are implementing a simple file system on top of leveldb. The types of entries we might wish to store are:
例如，假设我们在 leveldb 之上实现一个简单的文件系统。我们可能希望存储的条目类型是：

filename -> permission-bits, length, list of file_block_ids
file_block_id -> data
We might want to prefix filename keys with one letter (say '/') and the file_block_id keys with a different letter (say '0') so that scans over just the metadata do not force us to fetch and cache bulky file contents.
我们可能希望用一个字母（比如“/”）作为文件名键的前缀，用另一个字母（比如说'0'）作为 file_block_id 键的前缀，这样只扫描元数据就不会强迫我们获取和缓存庞大的文件内容。

Filters 过滤 器
Because of the way leveldb data is organized on disk, a single Get() call may involve multiple reads from disk. The optional FilterPolicy mechanism can be used to reduce the number of disk reads substantially.
由于 leveldb 数据在磁盘上的组织方式，单个 Get() 调用可能涉及从磁盘进行多次读取。可选的 FilterPolicy 机制可用于大幅减少磁盘读取次数。

leveldb::Options options;
options.filter_policy = NewBloomFilterPolicy(10);
leveldb::DB* db;
leveldb::DB::Open(options, "/tmp/testdb", &db);
... use the database ...
delete db;
delete options.filter_policy;
The preceding code associates a Bloom filter based filtering policy with the database. Bloom filter based filtering relies on keeping some number of bits of data in memory per key (in this case 10 bits per key since that is the argument we passed to NewBloomFilterPolicy). This filter will reduce the number of unnecessary disk reads needed for Get() calls by a factor of approximately a 100. Increasing the bits per key will lead to a larger reduction at the cost of more memory usage. We recommend that applications whose working set does not fit in memory and that do a lot of random reads set a filter policy.
上述代码将基于 Bloom 过滤器的过滤策略与数据库相关联。基于布隆过滤器的过滤依赖于在每个键的内存中保留一定数量的数据（在本例中为每个键 10 位，因为这是我们传递给的参数 NewBloomFilterPolicy ）。此筛选器会将 Get（） 调用所需的不必要的磁盘读取次数减少大约 100 倍。增加每个密钥的位数将导致更大的减少，但代价是内存使用量增加。我们建议其工作集不适合内存且执行大量随机读取的应用程序设置筛选策略。

If you are using a custom comparator, you should ensure that the filter policy you are using is compatible with your comparator. For example, consider a comparator that ignores trailing spaces when comparing keys. NewBloomFilterPolicy must not be used with such a comparator. Instead, the application should provide a custom filter policy that also ignores trailing spaces. For example:
如果使用自定义比较器，则应确保使用的过滤器策略与比较器兼容。例如，考虑一个比较器，它在比较键时忽略尾随空格。 NewBloomFilterPolicy 不得与此类比较器一起使用。相反，应用程序应提供也忽略尾随空格的自定义筛选器策略。例如：

class CustomFilterPolicy : public leveldb::FilterPolicy {
 private:
  leveldb::FilterPolicy* builtin_policy_;

 public:
  CustomFilterPolicy() : builtin_policy_(leveldb::NewBloomFilterPolicy(10)) {}
  ~CustomFilterPolicy() { delete builtin_policy_; }

  const char* Name() const { return "IgnoreTrailingSpacesFilter"; }

  void CreateFilter(const leveldb::Slice* keys, int n, std::string* dst) const {
    // Use builtin bloom filter code after removing trailing spaces
    std::vector<leveldb::Slice> trimmed(n);
    for (int i = 0; i < n; i++) {
      trimmed[i] = RemoveTrailingSpaces(keys[i]);
    }
    builtin_policy_->CreateFilter(trimmed.data(), n, dst);
  }
};
Advanced applications may provide a filter policy that does not use a bloom filter but uses some other mechanism for summarizing a set of keys. See leveldb/filter_policy.h for detail.
高级应用程序可能会提供不使用布隆过滤器但使用某种其他机制来汇总一组键的过滤器策略。有关详细信息，请参阅 leveldb/filter_policy.h 。

Checksums 校验
leveldb associates checksums with all data it stores in the file system. There are two separate controls provided over how aggressively these checksums are verified:
leveldb 将校验和与其存储在文件系统中的所有数据相关联。对于验证这些校验和的主动程度，提供了两个单独的控件：

ReadOptions::verify_checksums may be set to true to force checksum verification of all data that is read from the file system on behalf of a particular read. By default, no such verification is done.
ReadOptions::verify_checksums 可以设置为 true，以强制校验和验证代表特定读取从文件系统读取的所有数据。默认情况下，不执行此类验证。

Options::paranoid_checks may be set to true before opening a database to make the database implementation raise an error as soon as it detects an internal corruption. Depending on which portion of the database has been corrupted, the error may be raised when the database is opened, or later by another database operation. By default, paranoid checking is off so that the database can be used even if parts of its persistent storage have been corrupted.
Options::paranoid_checks 可以在打开数据库之前设置为 true，以使数据库实现在检测到内部损坏时立即引发错误。根据数据库的哪个部分已损坏，打开数据库时可能会引发错误，或者稍后由另一个数据库操作引发错误。默认情况下，偏执检查处于关闭状态，以便即使数据库的某些持久存储已损坏，也可以使用数据库。

If a database is corrupted (perhaps it cannot be opened when paranoid checking is turned on), the leveldb::RepairDB function may be used to recover as much of the data as possible
如果数据库已损坏（也许在打开偏执检查时无法打开），则可以使用该 leveldb::RepairDB 函数来恢复尽可能多的数据

Approximate Sizes 近似尺寸
The GetApproximateSizes method can used to get the approximate number of bytes of file system space used by one or more key ranges.
该方法 GetApproximateSizes 可用于获取一个或多个键范围使用的文件系统空间的近似字节数。

leveldb::Range ranges[2];
ranges[0] = leveldb::Range("a", "c");
ranges[1] = leveldb::Range("x", "z");
uint64_t sizes[2];
db->GetApproximateSizes(ranges, 2, sizes);
The preceding call will set sizes[0] to the approximate number of bytes of file system space used by the key range [a..c) and sizes[1] to the approximate number of bytes used by the key range [x..z).
前面的调用将设置为 sizes[0] 密钥范围使用的文件系统空间的近似字节数和 sizes[1] 密钥范围 [x..z) [a..c) 使用的近似字节数。

Environment 环境
All file operations (and other operating system calls) issued by the leveldb implementation are routed through a leveldb::Env object. Sophisticated clients may wish to provide their own Env implementation to get better control. For example, an application may introduce artificial delays in the file IO paths to limit the impact of leveldb on other activities in the system.
leveldb 实现发出的所有文件操作（和其他操作系统调用）都通过 leveldb::Env 对象路由。经验丰富的客户可能希望提供自己的 Env 实现以获得更好的控制。例如，应用程序可能会在文件 IO 路径中引入人为延迟，以限制 leveldb 对系统中其他活动的影响。

class SlowEnv : public leveldb::Env {
  ... implementation of the Env interface ...
};

SlowEnv env;
leveldb::Options options;
options.env = &env;
Status s = leveldb::DB::Open(options, ...);
Porting 移植
leveldb may be ported to a new platform by providing platform specific implementations of the types/methods/functions exported by leveldb/port/port.h. See leveldb/port/port_example.h for more details.
leveldb 可以通过提供 导出的类型 leveldb/port/port.h /方法/函数的平台特定实现来移植到新平台。有关更多详细信息，请参阅 leveldb/port/port_example.h 。

In addition, the new platform may need a new default leveldb::Env implementation. See leveldb/util/env_posix.h for an example.
此外，新平台可能需要新的默认 leveldb::Env 实现。有关示例，请参阅 leveldb/util/env_posix.h 。

Other Information 其他信息
Details about the leveldb implementation may be found in the following documents:
有关 leveldb 实现的详细信息，请参阅以下文档：

Implementation notes 实施说明
Format of an immutable Table file
不可变表文件的格式
Format of a log file
日志文件的格式