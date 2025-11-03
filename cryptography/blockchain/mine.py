import hashlib
import sys

arg = sys.argv[1].encode('utf-8')
prefix = "0" * int(sys.argv[2])
sha256 = hashlib.sha256()
sha256.update(arg)

x = 0
while True:
    s = sha256.copy()
    s.update(f"{x}".encode('utf-8'))
    d = s.hexdigest()
    print(d)
    if d.startswith(prefix):
        print(x)
        break
    x += 1

