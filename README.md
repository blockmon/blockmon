# BLOCKMON

Blockmon is software allowing construction of flexible and high performance
(rates in the 10Gb range) monitoring and data analysis nodes, where a node
can be for example a hardware probe or a PC. Blockmon is based around the
notion of blocks, which are small units of processing (e.g. packet counting).
Blocks are connected and communicate via gates, and the set of inter-connected
blocks represents a composition, where compositions are expressed in terms of
an XML file.

In order to control blockmon, a number of options exist:

1. CLI: python-based, run it with `python daemon/cli.py` (run it as root if
   for instance capturing from a network interface). At the prompt type
   `help` for a list of commands. Users are expected to write XML composition
   files by hand.

2. Daemon: a json-rpc, python-based daemon. Exposes the same functionality as
   the CLI, but allows for programmability. If using it, make sure to first
   edit the `daemon/config` file; run it with `python bmdaemon.py config` (as
   root if needed). Users are expected to write XML composition files by hand,
   or to write code that generates them. Also, make sure that `core/bmprocess.py`
   is executable (e.g., `chmod u+x core/bmprocess.py`).

## DIRECTORY STRUCTURE

```

├── bin       blockmon executable
├── blocks    blocks
├── core      core files
├── daemon    daemon files
├── doc       documentation
├── lib       libraries
├── messages  message files
├── tests     test files
└── usr       user blocks, messages, compositions
```

## DOCUMENTATION

The documentation is generated through doxygen. To generate, go to the doc
directory and run:

```
doxygen blockmondoc.cfg
```

This will generate HTML documentation for all the code; the index page can be
found under `doc/blockmon/html/index.html`

In addition to the code documentation, classes implementing blocks include
additional information regarding the block's functionality. To generate this
documentation, go to the doc directory and run:
```
python blocksdoc.py
```

The environment variable `PYTHONPATH` has to be set to Blockmon's daemon
directory (i.e., `node/daemon`) for this to work.


## COMPOSITION FILES

A very simple composition file consists of a set of blocks and their connections,
all in XML format. One such file is shown below:

```xml
<composition id="mysnifferctr" app_id="packet_count">
   <general>
      <clock type="wall"/>
   </general>
   
   <install>
      <threadpool id="sniffer_thread" num_threads="2">
         <core number="0"/>
      </threadpool>
      
      <block id="sniffer" type="PcapSource" invocation="async" threadpool="sniffer_thread">
         <params>
            <source type="live" name="eth0"/>
            <!--bpf_filter expression="!tcp"/-->
         </params>
      </block>
      
      <!-- NOTE: passive blocks shouldn't have a threadpool assigned to them -->
      <block id="counter" type="PacketCounter" invocation="direct">
         <params></params>
      </block>

      <connection src_block="sniffer" src_gate="source_out" dst_block="counter" dst_gate="in_pkt"/>
  </install>
</composition>
```

Compositions also allow configuration of how many threads are in a thread pool,
and how a threadpool is allocated to CPU cores. Some examples:

```xml
<!-- scheduler lets OS decide how to schedule threads over the specified cores -->
<threadpool id="pool1" num_threads="10" cores="0-2,4">
<!-- core-specific thread -->
<threadpool id="pool1" num_threads="1" cores="2">
<!-- scheduler assigns to cores as it sees fit -->
<threadpool id="pool1" num_threads="10">
```

In addition, it is possible to modify existing compositions, updating parts of
them, adding to them, or deleting from them:

```xml
<composition id="mysnifferctr">
  <update>

<!--
    These sections are optional
    <delete>
    </delete>

    <add>
    </add>-->

    <reconf>
     <!-- note: id, invocation and threadpool are MANDATORY even if they don't change -->
      <block id="sniffer" invocation="indirect" threadpool="sniffer_thread">
        <params>
          <source type="live" name="wlan0" />
        </params>
      </block>
    </reconf>

  </update>
</composition>
```

Note that blockmon does not support reconfiguration of connections. The user must delete and add
connections to emulate the equivalent of a connection reconfiguration.

## RUNNING A COMPOSITION

To run a composition via the CLI simply run:
```
sudo python daemon/cli.py
```
Or, in Blockmon's shell:
```
start [composition file]
```
And to stop it in Blockmon's shell just type:
```
stop
```

Also note that to run the above, sudo is needed to access a local
interface.

## ADDING COMPOSITIONS

To create a new application, please add a subdirectory under `BLOCKMON_DIR/usr` with the name
```
app_[your_app_name]
```

Place any composition files in this directory. Also, under this directory
create a further subdirectory called blocks and another one called messages.

Finally, make sure to re-run `cmake .` in order to include the new files into
the project.

## CREATING BLOCKS

The easiest way is to start with an existing simple block such as
blocks/PktCounter.[hpp|cpp] and copy it. Please make sure to
document, at the top of the *cpp* file, what the block does; again, follow
the format in PktCounter.cpp.

If you are only going to have one file for your block, please make sure it is
a cpp file and not an hpp one.

For more details, please refer to the doxygen documentation of Block.hpp.

Once you're done creating the block, run
```
python core/blockinfoparser.py config
```

to let the CLI and daemon know about the new block.

## MESSAGE TYPES

Blockmon supports the ability for developers to add custom message types by
putting message files under `/usr/app_[your_app_name]/messages`. However, note
that in most cases the standard message types provided should be sufficient.

Packet: represents a packet captured from the wire and provides access to
fields of the parsed packet.

Flow: represents a flow keyed by five-tuple, counting bytes and packets.

PairMsg: represents a message carrying a generic (key, value) pair.

Each block can append a Tag to an existing Message (see documentation about
the TagRegistry class for details). Note that you must first register the tag
before creating a message or appending the tag to an existing message.

## QUEUEING TYPES

For indirectly invoked blocks, messages are queued at the InGate. The used
queues however have a finite capacity. If the messages are enqueued with a
higher rate than they are consumed by the block they are by default dropped
when the queue is full. This behavior can be changed on a per-block-
basis.
To enable this feature cmake must be run with the `-DBLOCKING_QUEUE=ON` parameter.
One can then configure the queuing behavior in the `<block>` tag within the
composition XML.
The following queue behaviors can be specified:
  1. drop (default):
     The default mode is "drop", in which messages are dropped if the InGate
     queue has no more free slots.
     Example:
     
   ```
   <block id="..." type="..." threadpool="..." invocation="indirect" blocking_mode="drop">
   ```
  2. sleep:
     In this mode, the enqueuing block (i.e., the thread running the block
     sending a message) sleeps for a configurable amount of time if the
     queue is full.
     Example:
     
   ```
   <block id="..." type="..." threadpool="..." invocation="indirect" blocking_mode="sleep" sleep_usec="1000">
   ```
  3. yield:
     This mode is very similar to the "sleep" mode. However, the the enqueuing
     thread simply yields.
     Example:
     
   ```
   <block id="..." type="..." threadpool="..." invocation="indirect" blocking_mode="yield">
   ```
  4. mutex:
     In this mode a mutex is used to block the enqueueing thread until a slot
     the queue becomes free.
     Example:
     
   ```
   <block id="..." type="..." threadpool="..." invocation="indirect" blocking_mode="mutex">
   ```

The above allows you to chose the best suited queueing strategy for your application.
Two important points:

1. The queueing behavior can currently only be configured **per block** but not per gate.

2. All methods (except "drop") may block the enqueueing block. This may lead to
   deadlocks if you are not careful with your composition design and the
   composition's block-to-threadpool and threads-per-threadpool configuration.
   E.g., assume you have two indirectly invoked blocks A and B, both assigned to
   the same threadpool. Also assume that the threadpool only has one thread
   assigned and that block A sends more messages than block B consumes. In this
   case the queue of block B will eventually get fully occupied. If any of the
   non-dropping methods above was specified, the thread will block when block A
   next tries to send a message. Since this thead is blocked it will not schedule
   any other block. Thus Blockmon's queue will never get free again, i.e., we
   are in a deadlock. (This limitation will be removed in a future version of blockmon)

## CODE COMMENTING

Please follow the format used in blocks/PktCounter.[hpp|cpp]
