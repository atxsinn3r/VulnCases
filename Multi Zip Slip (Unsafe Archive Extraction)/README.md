# Multi Zip Slip (Arbitrary Write in Archive Extraction)

The term "Zip Slip" comes from Snyk's [security research](https://snyk.io/research/zip-slip-vulnerability) in archive extraction. Basically what it means is, archive extraction libraries often don't care where the files are extracted to, which is a security problem because you could leverage this and write a file anywhere on the system. This tends to be a more serious issue for web applications that handle compressed files.

For example, for a PHP application, a malicious archive would contain a PHP payload (payload.php for example) with the following path:

```
../../../../../var/www/html/
```

Once extracted, the attacker could make a GET request to the PHP payload and get remote code execution:

```
$ curl http://example.com/payload.php
```

For a real world case, here's one that was found by Steven Seeley for [Cisco Prime Infrastructure's Health Monitor](https://srcincite.io/blog/2019/05/17/panic-at-the-cisco-unauthenticated-rce-in-prime-infrastructure.html) component, and I wrote a [Metasploit exploit](https://github.com/rapid7/metasploit-framework/blob/master/modules/exploits/linux/http/cpi_tararchive_upload.rb) for it.

It seems quite a few libraries still seem to overlook this so it is worth documented as a vulnerable case study. In my repository, there are a couple examples that demonstrate the problem. Since they are quite generic, they also serve as code patterns and maybe you can find more vulnerabilities on other applications that way.

I wrote a [Metasploit module](https://atxsinn3r.io/exploits.html) that allows you generate this type of malicious TAR file so you can quickly test Zip Slip vulns.

