# Unsafe DLL Loading

![Screenshot](https://raw.githubusercontent.com/wchen-r7/VulnCases/master/Windows%20Unsafe%20DLL%20Loading/Screenshot.png)

This is an example of unsafe DLL loading. The EXE attempts to load a non-existent named
HelloWorld.dll, and as you can see, Windows is looking for it in multiple places. This
is because LoadLibrary has this search order:

1. The directory from which the application loaded.
2. The system directory.
3. The 16-bit system directory.
4. The Windows directory.
5. The current directory.
6. The directories that are listed in the PATH environment variable.

If an attacker has control over one of these directories, for example: a directory
traversal that allows the person to write to anywhere on the file system, then he
can generate a DLL payload like this:

```
msfvenom -f dll -p windows/exec CMD="C:\windows\system32\calc.exe" -o runcalc.dll
```

And then hijack the DLL by placing it in a path that the EXE loads.

Also see:

https://github.com/rapid7/DLLHijackAuditKit

## WebDav Setup

You can do:

```
sudo pip install wsgidav cheroot
```

You might need to do:

```
sudo pip install six --upgrade --ignore-installed
```

Create a share folder:

```
/tmp/webdav/share
```

And finally, start wsgidav:

```
./start_wsgidav.sh
```
