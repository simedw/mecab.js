vows   = require 'vows'
assert = require 'assert'
libmecab  = require '../build/Release/mecab'

vows
  .describe('Parsing Sentences')
  .addBatch
    'mecab':
      topic: new libmecab.MeCab()
      'has a parse function': (mecab) ->
        assert.isFunction(mecab.parse)
      'japanese string':
        topic: (mecab) ->
          input  = '犬が猫を追いかけていました。'
          mecab.parse input, @callback
        'got results': (error, result) ->
          assert.isNull(error)
          assert.isArray(result)
        'same string as original': (error, result) ->
          input  = '犬が猫を追いかけていました。'
          assert.strictEqual(input,result.join(''))
      'empty string':
        topic: (mecab) ->
          input  = ''
          mecab.parse input, @callback
        'got results': (error, result) ->
          assert.isNull(error)
          assert.isArray(result)
        'same string as original': (error, result) ->
          assert.isEmpty(result)
  .export(module)
