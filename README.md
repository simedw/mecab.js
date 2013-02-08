mecab.js
========

Node.js bindings for mecab (japanese word boundaries)
The original project: http://mecab.googlecode.com/svn/trunk/mecab/doc/index.html

Installation
------------
If you read Japanese, follow the instructions on http://mecab.googlecode.com/svn/trunk/mecab/doc/index.html, do not forget to compile mecab-ipadic with unicode support.

For the rest of us:

Get the latest version of mecab from http://mecab.googlecode.com/svn/trunk/mecab/doc/index.html#download
```Bash
tar zxfv mecab-X.X.tar.gz
cd mecab-X.X
./configure 
make
make check
sudo make install
```

Get the latest version of mecab-ipadic (a dictionary) from the same site. There are other dictinaries, but I haven't tried them yet.

```Bash
tar zxfv mecab-ipadic-2.7.0-XXXX.tar.gz
mecab-ipadic-2.7.0-XXXX
./configure --with-charset=utf-8
make
sudo make install
```

Make sure you have a recent version of Node.js installed.
```Bash
git clone https://github.com/simedw/mecab.js.git
cd mecab.js
npm install
```

Usage
---

```CoffeeScript
mecab  = require 'mecab'
input  = '犬が猫を追いかけていました。'

parser = new mecab.MeCab()
parser.parse input, (error, result) ->
  throw error if error
  console.log result
```
Gives the following output
```CoffeeScript
[ '犬', 'が', '猫', 'を', '追いかけ', 'て', 'い', 'まし', 'た', '。' ]
```
Todo
----
+ Return information for each word segment (if it's an adjective, verb etc)
+ Make the result included the second best result as well



