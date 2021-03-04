#!/bin/env ruby

data = [0xDEADBEEF].pack("V*")
# I wonder what we can do in here?
data << "/tmp/test.png"

File.write('bin/test.bin', data)
puts "test.bin created. Let the program load it."
