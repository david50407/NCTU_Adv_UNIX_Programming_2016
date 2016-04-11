# Advanced Unix Programming HW2

## Compile and Build

* Using the following command to build the library.

```
make
```

* Clean the object files and library.

```
make clean
```

* Test the library using `curl` (http).

```
make test
```

* Test the libray using `curl` with SSL (https).

```
make test_SSL
```

## Result

The result will be in the file named as `hijack.log`, and it will append the result to the tail of the file.
This will contain the function call, the hostname or IP, and the data transfered between the server and client.

### Function call

```
*** __libc_start_main(0x402300, 4, 0x7ffe791a88d8, 0x40f230, 0x40f2a0, 0x2aaf42c1a560, 0x7ffe791a88c8) ***
*** pipe({0, 0}) ***
*** close(4) ***
*** close(5) ***
*** curl_easy_init() = 0x1223740 ***
*** fopen(/home/david50407/.curlrc, r) = 0 ***
...
```

### Hostname or IP

```
...
*** Host(rubygems.nos.ncu.edu.tw) ***
...
*** IP(140.115.18.208) ***
...
```

### Transfered data

```
...
*** recv BEGIN ***
Server: nginx^M
Date: Sun, 10 Apr 2016 14:06:40 GMT^M
Content-Type: text/html^M
Content-Length: 3947^M
Last-Modified: Wed, 02 Dec 2015 11:10:43 GMT^M
Connection: keep-alive^M
ETag: "565ed1b3-f6b"^M
Accept-Ranges: bytes^M
^M
...
*** recv  END  ***
...
```
