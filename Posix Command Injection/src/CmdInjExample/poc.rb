#!/bin/env ruby

data = [0xDEADBEEF].pack("V*")
data << "/tmp/test.png; echo ph33r > /tmp/payload.txt"

File.write('bin/test.bin', data)
puts "test.bin created. Let the program load it."
