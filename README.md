# VulnCases

I used to write vulnerable code snippets in order to understand how they work, how to reverse engineer, and how to exploit them. However, if you just want to learn how to spot vulnerabilities, an easy and fun way nowadays is practice with ChatGPT by setting up your test this way:

```
I want to practice some C/C++ vulnerabilities that are based on real world problems, or mimic them. Give me a code snippet with one of these vulnerability problems:

* Out-of-bound READ
* Out-of-bound WRITE
* Any buffer overflows
* Use after free
* Memroy leak
* Off by one problems
* Format string problems
* Integer overflow
* Integer underflow
* Type confusion

The difficulty of the tests ranges from 1 to 10. 1 being the easiest, 10 being the hardest. Start with level 1. If I get the answer right at first try for a few times in a row, gradually increase the level of difficulty for that type of vulnerability, and move on to the next test. If I get it wrong for three times in a row, tell me the answer, gradually decrease the level of difficulty, and move on to the next test. Don't tell me how difficult it is in the test, but if my level of difficulty has changed, let me know what it is.

In the test, ask me to explain what the problem is with some details. Don't give me any hints. No comments in the code snippet.

If I get the answer wrong, don't give me the answer. Just tell me it is wrong, and remind me to try again. However if I admit I don't know, you can give me the answer.
```

If you like my repository, you may also like:  

* [Modern Memory Safety: C/C++ Vulnerability Discovery, Exploitation, Hardening](https://github.com/struct/mms)
* [Carnegie Mellon University: SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
* [Carnegie Mellon University: SEI CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682)

# License

Please see [LICENSE](https://github.com/wchen-r7/VulnCases/blob/master/LICENSE)
