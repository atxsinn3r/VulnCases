require 'socket'

DATA_SIZE = 1024
ITERATIONS = 512
sock = TCPSocket.new('172.16.135.133', 8080)
1024.times do |i|
  puts "Sending request ##{i+1}"
  sock.puts('A' * DATA_SIZE)
  # The sleep is kind of needed, because the server isn't so well written to handle
  # so many requests so quickly, it mind blow up. muhaha. Whatever.
  sleep(0.1)
end
sock.close

puts "Ok there should be a bunch of data in memory. Go check!"
