import os
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as temp:
    directory = Path(temp)
    binary = directory / ("bytediff.exe" if os.name == "nt" else "bytediff")
    command = sys.argv[1:] or [str(binary)]
    if not sys.argv[1:]:
        subprocess.run([os.environ.get("CC", "cc"), "-std=c17", "-O2", "-Wall", "-Wextra", "-Werror",
                        ROOT / "src/bytediff.c", "-o", binary], check=True)
    left, right = directory / "left.bin", directory / "right.bin"

    def check(a, b, expected):
        left.write_bytes(a)
        right.write_bytes(b)
        result = subprocess.run([*command, left, right], capture_output=True, text=True)
        assert result.returncode == bool(expected), result.stderr
        assert result.stdout.splitlines() == [f"{start}\t{end}" for start, end in expected]
        assert f"{sum(end-start for start,end in expected)} differing bytes" in result.stderr

    check(b"", b"", [])
    check(b"a\0b\xff", b"a\0b\xff", [])
    check(b"abcdef", b"aXYdeZ!", [(1,3),(5,7)])
    check(b"a"*65535+b"BC"+b"z", b"a"*65535+b"12"+b"z", [(65535,65537)])
    check(b"123", b"", [(0,3)])
    check(b"", b"123", [(0,3)])
    result = subprocess.run([*command, left, directory/"missing"], capture_output=True)
    assert result.returncode == 2
