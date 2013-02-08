#ifndef MECAB_HPP_
#define MECAB_HPP_

#include <node.h>
#include <mecab.h>

class MeCabJS : public node::ObjectWrap {
  public:
    static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Handle<v8::Object> target);
  protected:
    MeCabJS();
    ~MeCabJS();

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> Parse(const v8::Arguments& args);
    MeCab::Model *_model;
    MeCab::Tagger *_tagger;
};

#endif

