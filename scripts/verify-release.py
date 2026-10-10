#!/usr/bin/env python3
"""Verify packaged files without launching a game or Wine."""
from pathlib import Path
import hashlib
import sys
import zipfile

EXE = '6f1077aaceee9d2641cc4a89d951520b11d968c1bb2e0273cb6540e3a5169e9b'
DLL = '4282050beb55fcbfa6dc46e302299296552692d2cf8e5cac161b1597dedc2b2a'
root = Path(sys.argv[1])
for platform, filename, prefix in [
    ('Windows', 'RoadRash-Windows-1.9.7.zip', 'RoadRash-Windows-1.9.7/'),
    ('Mac', 'RoadRash-Mac-ARM64-1.9.7.zip', '暴力摩托 Mac.app/Contents/Resources/Game/'),
]:
    with zipfile.ZipFile(root / filename) as z:
        assert z.testzip() is None, filename
        for name in z.namelist():
            # ditto's Apple metadata entries are not ordinary app files.
            if name.startswith('__MACOSX/'):
                continue
            p = Path(name)
            assert p.suffix.lower() not in ('.rrs', '.log', '.dmp'), name
            assert p.name not in ('.DS_Store', 'user.reg', 'system.reg'), name
            assert not p.name.endswith('-check.exe'), name
        assert hashlib.sha256(z.read(prefix + 'Roadrash.exe')).hexdigest() == EXE
        assert hashlib.sha256(z.read(prefix + 'ddraw.dll')).hexdigest() == DLL
    print(platform + ': CRC, core hashes and clean contents passed')
