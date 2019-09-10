# This is an example of unsafe archive extraction in Python
# The python version tested is: v2.7

import tarfile
t = tarfile.open('malicious_archive.tar')
t.extractall()
print "Done"
