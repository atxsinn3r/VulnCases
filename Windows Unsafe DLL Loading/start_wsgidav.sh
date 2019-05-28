#!/bin/sh

# Before you can run this script, you need to do:
# sudo pip install wsgidav cheroot
# Reference: https://github.com/mar10/wsgidav
# Might need to do this: sudo pip install six --upgrade --ignore-installed
# Next, on your server, host your EXE here:
# /tmp/webdav/share
# And then on your client machine (ie. win 10), open a network share/drive:
# \\server_ip\share
sudo wsgidav --host=0.0.0.0 --port=80 --root=/tmp/webdav --auth=anonymous