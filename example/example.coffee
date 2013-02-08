mecab = require '../build/Release/mecab'

input  = '犬が猫を追いかけていました。'
parser = new mecab.MeCab()

parser.parse input, (error,result) ->
  throw error if error
  console.log result

