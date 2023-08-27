Files 文件
The implementation of leveldb is similar in spirit to the representation of a single Bigtable tablet (section 5.3). However the organization of the files that make up the representation is somewhat different and is explained below.
leveldb 的实现在精神上类似于单个 Bigtable 平板电脑的表示（第 5.3 节）。但是，构成表示的文件的组织有些不同，下面将对此进行说明。

Each database is represented by a set of files stored in a directory. There are several different types of files as documented below:
每个数据库都由存储在目录中的一组文件表示。有几种不同类型的文件，如下所述：

Log files 日志文件
A log file (*.log) stores a sequence of recent updates. Each update is appended to the current log file. When the log file reaches a pre-determined size (approximately 4MB by default), it is converted to a sorted table (see below) and a new log file is created for future updates.
日志文件 （*.log） 存储一系列最近的更新。每个更新都会追加到当前日志文件中。当日志文件达到预定大小（默认约为 4MB）时，它将转换为排序表（见下文），并创建新的日志文件以供将来更新。

A copy of the current log file is kept in an in-memory structure (the memtable). This copy is consulted on every read so that read operations reflect all logged updates.
当前日志文件的副本保存在内存结构 （的 memtable ） 中。每次读取时都会查阅此副本，以便读取操作反映所有记录的更新。

Sorted tables 排序表
A sorted table (*.ldb) stores a sequence of entries sorted by key. Each entry is either a value for the key, or a deletion marker for the key. (Deletion markers are kept around to hide obsolete values present in older sorted tables).
排序表 （*.ldb） 存储按键排序的条目序列。每个条目要么是键的值，要么是键的删除标记。（保留删除标记以隐藏旧排序表中存在的过时值）。

The set of sorted tables are organized into a sequence of levels. The sorted table generated from a log file is placed in a special young level (also called level-0). When the number of young files exceeds a certain threshold (currently four), all of the young files are merged together with all of the overlapping level-1 files to produce a sequence of new level-1 files (we create a new level-1 file for every 2MB of data.)
排序表集被组织成一系列级别。从日志文件生成的排序表放置在特殊的年轻级别（也称为级别 0）中。当年轻文件的数量超过某个阈值（目前为四个）时，所有年轻文件将与所有重叠的 1 级文件合并在一起，以生成一系列新的 1 级文件（我们每 2MB 数据创建一个新的 1 级文件。

Files in the young level may contain overlapping keys. However files in other levels have distinct non-overlapping key ranges. Consider level number L where L >= 1. When the combined size of files in level-L exceeds (10^L) MB (i.e., 10MB for level-1, 100MB for level-2, ...), one file in level-L, and all of the overlapping files in level-(L+1) are merged to form a set of new files for level-(L+1). These merges have the effect of gradually migrating new updates from the young level to the largest level using only bulk reads and writes (i.e., minimizing expensive seeks).
年轻级别中的文件可能包含重叠的键。但是，其他级别的文件具有不同的非重叠键范围。考虑级别编号 L，其中 L >= 1。当级别 L 中的文件组合大小超过 （10^L） MB 时（即级别 1 为 10MB，级别 2 为 100MB，...），级别 L 中的一个文件和级别 （L+1） 中的所有重叠文件将合并为一组级别 （L+1） 的新文件。这些合并具有仅使用批量读取和写入（即最小化昂贵的搜索）将新更新从年轻级别逐渐迁移到最大级别的效果。

Manifest 清单
A MANIFEST file lists the set of sorted tables that make up each level, the corresponding key ranges, and other important metadata. A new MANIFEST file (with a new number embedded in the file name) is created whenever the database is reopened. The MANIFEST file is formatted as a log, and changes made to the serving state (as files are added or removed) are appended to this log.
MANIFEST 文件列出了构成每个级别的排序表集、相应的键范围和其他重要元数据。每当重新打开数据库时，都会创建一个新的清单文件（文件名中嵌入一个新编号）。MANIFEST 文件的格式设置为日志，对服务状态所做的更改（添加或删除文件时）将追加到此日志中。

Current 当前
CURRENT is a simple text file that contains the name of the latest MANIFEST file.
CURRENT 是一个简单的文本文件，其中包含最新清单文件的名称。

Info logs 信息日志
Informational messages are printed to files named LOG and LOG.old.
信息性消息将打印到名为 LOG 和 LOG.old 的文件中。

Others 别人
Other files used for miscellaneous purposes may also be present (LOCK, *.dbtmp).
其他用于杂项目的的文件也可能存在（LOCK，*.dbtmp）。

Level 0 级别 0
When the log file grows above a certain size (4MB by default): Create a brand new memtable and log file and direct future updates here.
当日志文件增长到一定大小以上（默认为 4MB）时：创建一个全新的内存表和日志文件，并在此处指示未来的更新。

In the background: 在后台：

Write the contents of the previous memtable to an sstable.
将上一个内存表的内容写入一个 sstable。
Discard the memtable. 丢弃记忆表。
Delete the old log file and the old memtable.
删除旧的日志文件和旧的内存表。
Add the new sstable to the young (level-0) level.
将新的马厩添加到年轻（0级）级别。
Compactions 压实
When the size of level L exceeds its limit, we compact it in a background thread. The compaction picks a file from level L and all overlapping files from the next level L+1. Note that if a level-L file overlaps only part of a level-(L+1) file, the entire file at level-(L+1) is used as an input to the compaction and will be discarded after the compaction. Aside: because level-0 is special (files in it may overlap each other), we treat compactions from level-0 to level-1 specially: a level-0 compaction may pick more than one level-0 file in case some of these files overlap each other.
当级别 L 的大小超过其限制时，我们将其压缩在后台线程中。压缩从级别 L 中选取一个文件，并从下一个级别 L+1 中选取所有重叠文件。请注意，如果 L 级文件仅与 L+1 级文件的一部分重叠，则级别 （L+1） 的整个文件将用作压缩的输入，并在压缩后被丢弃。旁白：因为 0 级是特殊的（其中的文件可能相互重叠），我们特别处理从 0 级到 1 级的压缩：0 级压缩可能会选择多个 0 级文件，以防其中一些文件相互重叠。

A compaction merges the contents of the picked files to produce a sequence of level-(L+1) files. We switch to producing a new level-(L+1) file after the current output file has reached the target file size (2MB). We also switch to a new output file when the key range of the current output file has grown enough to overlap more than ten level-(L+2) files. This last rule ensures that a later compaction of a level-(L+1) file will not pick up too much data from level-(L+2).
压缩合并拾取文件的内容以生成一系列级别 （L+1） 文件。在当前输出文件达到目标文件大小 （2MB） 后，我们切换到生成新的级别 （L+1） 文件。当当前输出文件的键范围增长到足以重叠十多个级别 （L+2） 文件时，我们也会切换到新的输出文件。最后一条规则可确保以后压缩级别 （L+1） 文件时不会从级别 （L+2） 中获取太多数据。

The old files are discarded and the new files are added to the serving state.
旧文件将被丢弃，新文件将添加到服务状态。

Compactions for a particular level rotate through the key space. In more detail, for each level L, we remember the ending key of the last compaction at level L. The next compaction for level L will pick the first file that starts after this key (wrapping around to the beginning of the key space if there is no such file).
特定级别的压缩在关键空间中旋转。更详细地说，对于每个级别 L，我们记住级别 L 处最后一个压缩的结束键。级别 L 的下一个压缩将选取在此键之后开始的第一个文件（如果没有此类文件，则绕到键空间的开头）。

Compactions drop overwritten values. They also drop deletion markers if there are no higher numbered levels that contain a file whose range overlaps the current key.
压缩会删除覆盖的值。如果没有更高编号的级别包含其范围与当前键重叠的文件，则它们还会删除删除标记。

Timing 定时
Level-0 compactions will read up to four 1MB files from level-0, and at worst all the level-1 files (10MB). I.e., we will read 14MB and write 14MB.
0 级压缩将从 0 级读取最多四个 1MB 文件，最坏的情况是读取所有 1 级文件 （10MB）。即，我们将读取 14MB 并写入 14MB。

Other than the special level-0 compactions, we will pick one 2MB file from level L. In the worst case, this will overlap ~ 12 files from level L+1 (10 because level-(L+1) is ten times the size of level-L, and another two at the boundaries since the file ranges at level-L will usually not be aligned with the file ranges at level-L+1). The compaction will therefore read 26MB and write 26MB. Assuming a disk IO rate of 100MB/s (ballpark range for modern drives), the worst compaction cost will be approximately 0.5 second.
除了特殊的 0 级压缩之外，我们还将从 L 级中选择一个 2MB 的文件。在最坏的情况下，这将与级别 L+1 重叠 ~ 12 个文件（10 是因为级别 （L+1） 的大小是级别 L 的十倍，另外两个在边界，因为级别 L 的文件范围通常不会与级别 L+1 的文件范围对齐）。因此，压缩将读取 26MB 并写入 26MB。假设磁盘 IO 速率为 100MB/s（现代驱动器的大致范围），最差的压缩成本约为 0.5 秒。

If we throttle the background writing to something small, say 10% of the full 100MB/s speed, a compaction may take up to 5 seconds. If the user is writing at 10MB/s, we might build up lots of level-0 files (~50 to hold the 5*10MB). This may significantly increase the cost of reads due to the overhead of merging more files together on every read.
如果我们将后台写入限制为较小的内容，例如全速 100MB/s 的 10%，则压缩可能需要长达 5 秒的时间。如果用户以 10MB/s 的速度写入，我们可能会构建大量 0 级文件（~50 以容纳 5*10MB）。这可能会显著增加读取成本，因为在每次读取时将更多文件合并在一起会产生开销。

Solution 1: To reduce this problem, we might want to increase the log switching threshold when the number of level-0 files is large. Though the downside is that the larger this threshold, the more memory we will need to hold the corresponding memtable.
解决方案 1：为了减少此问题，我们可能需要在 0 级文件数量较大时增加日志切换阈值。虽然缺点是这个阈值越大，我们需要更多的内存来保存相应的内存表。

Solution 2: We might want to decrease write rate artificially when the number of level-0 files goes up.
解决方案 2：当 0 级文件的数量增加时，我们可能希望人为地降低写入速率。

Solution 3: We work on reducing the cost of very wide merges. Perhaps most of the level-0 files will have their blocks sitting uncompressed in the cache and we will only need to worry about the O(N) complexity in the merging iterator.
解决方案3：我们致力于降低非常广泛的合并的成本。也许大多数 0 级文件的块将未压缩地放在缓存中，我们只需要担心合并迭代器中的 O（N） 复杂性。

Number of files 文件数
Instead of always making 2MB files, we could make larger files for larger levels to reduce the total file count, though at the expense of more bursty compactions. Alternatively, we could shard the set of files into multiple directories.
我们可以为更大的级别制作更大的文件，而不是总是制作 2MB 的文件，以减少文件总数，但代价是更多的突发压缩。或者，我们可以将文件集分片到多个目录中。

An experiment on an ext3 filesystem on Feb 04, 2011 shows the following timings to do 100K file opens in directories with varying number of files:
2011 年 2 月 4 日对 ext3 文件系统的实验显示了在具有不同数量文件的目录中打开 100K 文件时，执行以下时间：

Files in directory 目录中的文件	Microseconds to open a file
打开文件的微秒
1000	9
10000	10
100000	16
So maybe even the sharding is not necessary on modern filesystems?
所以也许在现代文件系统上甚至不需要分片？

Recovery 恢复
Read CURRENT to find name of the latest committed MANIFEST
读取 CURRENT 以查找最新提交的清单的名称
Read the named MANIFEST file
读取命名的清单文件
Clean up stale files
清理过时的文件
We could open all sstables here, but it is probably better to be lazy...
我们可以在这里打开所有马厩，但最好是偷懒......
Convert log chunk to a new level-0 sstable
将日志块转换为新的 0 级稳定版
Start directing new writes to a new log file with recovered sequence#
使用恢复的序列开始将新写入定向到新的日志文件#
Garbage collection of files
文件的垃圾回收
RemoveObsoleteFiles() is called at the end of every compaction and at the end of recovery. It finds the names of all files in the database. It deletes all log files that are not the current log file. It deletes all table files that are not referenced from some level and are not the output of an active compaction.
RemoveObsoleteFiles() 在每次压缩结束时和恢复结束时调用。它查找数据库中所有文件的名称。它会删除所有不是当前日志文件的日志文件。它会删除所有未从某个级别引用且不是活动压缩输出的表文件。