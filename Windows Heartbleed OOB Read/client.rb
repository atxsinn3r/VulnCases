require 'socket'
require 'hexdump'

type = 0x01
size = 0x0004
payload = "ABCD"

packet =
  [
    [type].pack("C"),
    [size].pack("s>"),
    payload
  ].join

sock = TCPSocket.new('192.168.7.129', 4444)
sock.write(packet)
resp = sock.recv(size)
sock.close
puts "Response (#{resp.length} bytes received):"
puts resp.hexdump
