{
  "targets": [
    {
      "target_name" : "mecab",
      "sources" : [ "src/mecab.cpp" ],
      'conditions' : [
        ['OS=="win"', {
          # no windows support
        }, {
          'libraries': [
            '<!@(mecab-config --libs)'
          ],
          'cflags' : [
            '<!@(mecab-config --cflags)'
          ]
        }
      ]
    ]
    }
  ]
}
