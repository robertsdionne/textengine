#ifndef TEXTENGINE_BASE_H_
#define TEXTENGINE_BASE_H_

#define INTERFACE(interface)\
public:\
  virtual ~interface() {}\
  interface(interface const &) = delete;\
  interface &operator =(interface const &) = delete;\
protected:\
  interface() {}\
private:

#endif  // TEXTENGINE_BASE_H_
