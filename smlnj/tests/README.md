This directory contains various simple test programs for checking
that the LLVM code generator is doing the right thing.

To regenerate the pickle files (*e.g.*, if the CFG IR has
changed), you can run the following shell command:

```bash
for f in tst-*sml ; do
  echo "" | $SML -Ccg.dump-cfg=true $f
done
```

where `$SML` expands is the path to a version of **SML/NJ** that
supports the desired CFG pickle format.
