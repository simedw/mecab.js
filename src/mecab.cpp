#include "mecab.hpp"
#include <node.h>
#include <string>
#include <iostream>
#include <mecab.h>

using namespace v8;

// struct to pass around the parse request
struct parse_req {
  // callback (error, input)
  Persistent<Function> callback;
  // input is a utf8 japanese string
  std::string input;
  MeCab :: Lattice * lattice;
  MeCab :: Tagger * tagger;
  // true if an error occurred
  bool error;
  std::string error_msg;
};

Persistent<FunctionTemplate> MeCabJS::constructor;

void MeCabJS::Init(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  Local<String> name          = String::NewSymbol("MeCab");
  
  constructor = Persistent<FunctionTemplate>::New(tpl);
  constructor->InstanceTemplate()->SetInternalFieldCount(1); 
  constructor->SetClassName(name);

  NODE_SET_PROTOTYPE_METHOD(constructor, "parse", Parse);

  target->Set(name, constructor->GetFunction());
}

/* 
 * Here the actually parsing take place, this will be in a
 * separate thread from the rest of the program
 */
void ParseWork(uv_work_t* uv_req) {
  parse_req *req = static_cast<parse_req*>(uv_req->data);
  req->lattice->set_sentence(req->input.c_str());
  bool res      = req->tagger->parse(req->lattice);
  if(!res) {
    req->error     = true;
    req->error_msg = req->tagger->what();
  }
}

/*
 * If an error occurred
 */
void ParseError(parse_req *req) {
  const unsigned argc = 2;
  Local<Value> argv[argc] = {
    String::New(req->error_msg.c_str()),
    Local<Value>::New(Null())
  };

  TryCatch try_catch;
  req->callback->Call(Context::GetCurrent()->Global(),argc,argv);
  if(try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
}
/*
 * If the parsing when through fine
 */
void ParseSuccess(parse_req *req) {
  int len = 0;
  for(const MeCab::Node* node = req->lattice->bos_node(); node; node = node->next){
    len++;
  }
  len -= 2; // since the first and the last one isn't actually a word
  Local<v8::Array> arguments = Array::New(len);
  int pos = 0;
  for(const MeCab::Node* node = req->lattice->bos_node(); node; node = node->next){
    // skip the first and last node
    if(node->stat == MECAB_BOS_NODE || node->stat == MECAB_EOS_NODE)
      continue;
    char *tmp = new char[node->length+1];
    strncpy(tmp,node->surface,node->length);
    tmp[node->length] = '\0';
    arguments->Set(Number::New(pos), String::New(tmp));
    delete tmp;
    pos++;
  }

  const unsigned argc = 2;
  Local<Value> argv[argc] = {
     Local<Value>::New(Null()),
     arguments
  };

  TryCatch try_catch;
  req->callback->Call(Context::GetCurrent()->Global(),argc,argv);
  if(try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
}

/*
 * Called when ParserWork has finished
 */
void ParseAfter(uv_work_t *uv_req) {
  parse_req *req = static_cast<parse_req*>(uv_req->data);
  
  if(req->error) {
    ParseError(req);
  } else {
    ParseSuccess(req);
  }

  req->callback.Dispose();
  delete req->lattice;
  delete req;
  delete uv_req;
}

Handle<Value> MeCabJS::Parse(const Arguments& args) {
  HandleScope scope;

  if(args.Length() != 2) {
    return ThrowException(Exception::TypeError(
          String::New("The function takes two arguments, input and cb")
          ));
  }
  if(!args[1]->IsFunction()) {
    return ThrowException(Exception::TypeError(
          String::New("Second argument must be a callback function")
          ));
  }
  Local<Function> callback = Local<Function>::Cast(args[1]);

  MeCabJS * self    = ObjectWrap::Unwrap<MeCabJS>(args.This());
  parse_req *req    = new parse_req();
  req->callback     = Persistent<Function>::New(callback);
  req->error        = false;
  Local<String> str = args[0]->ToString();

  char *tmp = new char[str->Utf8Length()+1];
  str->WriteUtf8(tmp);
  req->input   = tmp;
  req->lattice = self->_model->createLattice();
  req->tagger  = self->_tagger;
  uv_work_t *r = new uv_work_t();
  r->data      = req;
  int status   = uv_queue_work(uv_default_loop(), r, ParseWork, ParseAfter);
  assert(status == 0);
  
  delete[] tmp;
  return Undefined(); 
}

MeCabJS::MeCabJS() : ObjectWrap() {
  this->_model  = MeCab :: createModel("");
  this->_tagger = this->_model->createTagger();  
}
MeCabJS::~MeCabJS() {
  if(this->_model){
    delete this->_tagger;
    delete this->_model;
  }
}

Handle<Value> MeCabJS::New(const Arguments& args) {
  HandleScope scope;
  
  if(args[0]->IsExternal())
    return args.This();

  if(!args.IsConstructCall()) {
    return ThrowException(Exception::TypeError(
          String::New("Use the new operator to create instances of this object.")
          ));
  }
  
  MeCabJS* obj = new MeCabJS;
  obj->Wrap(args.This());

  return args.This();
}


void RegisterModule(Handle<Object> target) {
  MeCabJS::Init(target);
}

NODE_MODULE(mecab, RegisterModule);
