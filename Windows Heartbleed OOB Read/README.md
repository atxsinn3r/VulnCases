# Windows Heartbleed Example

This is a remote out-of-bound read example that mimics the [Heartbleed vulnerability](http://heartbleed.com/). Similar to Heartbleed, the format of your packet would look like this:

```
[Type][Size][Payload]
```

More specifically:

* Type = Unsigned char (1 byte)
* Size = Unsigned short (2 bytes)
* Payload = An array of string

The server is expected to spit back your payload. So for example, if your input looks like this:

```ruby
packet = ""
packet << "\x01"             # Type
packet << "\x00\x04"         # Size
packet << "\x41\x41\x41\x41" # Payload
```

Your are expected to receive this from the server:

```
AAAA
```

Since the size isn't properly checked. You can read more than you should. However, since this is just a small demo, there isn't any sensitive data to leak. But hey this example is open source, you can easily modify that to make it more exciting.
