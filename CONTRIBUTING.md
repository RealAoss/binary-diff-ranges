# Contributing

Use a C17 compiler named cc on PATH and Python 3.10+. Run the integration suite from the root:

```sh
python tests/check.py
```

The tests cover empty files, binary bytes, trailing data and ranges crossing read boundaries. For a bug report, include a minimal synthetic input and the expected output.
